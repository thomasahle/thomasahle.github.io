/* ChainHash adjacent pairs. Copyright 2026 Thomas Dybdahl Ahle. MIT.
 * C99 / C++11. The key, byte order and function are those of SPEC.md and
 * chainhash_x86.h. Default: 1024-byte blocks, S=1, exactly 137 key words.
 * Define CHAINHASH_X86_BLOCK_BYTES=256 for the separate 41-word definition.
 * Its batch1/2/4/8 entry points all return the SAME 256-byte-definition hash.
 * AArch64 GCC/Clang: enable crypto (Apple: -march=native plus +aes).
 * Other targets, or CHAINHASH_ARM_FORCE_PORTABLE, use the reference path.
 */
#ifndef CHAINHASH_ARM_H_INCLUDED
#define CHAINHASH_ARM_H_INCLUDED
#include "chainhash_x86.h"

typedef chainhash_x86_key chainhash_arm_key;
#define CHAINHASH_ARM_BLOCK_BYTES CHAINHASH_X86_BLOCK_BYTES
#define CHAINHASH_ARM_KEY_WORDS CHAINHASH_X86_KEY_WORDS
#define CHAINHASH_ARM_KEY_BYTES CHAINHASH_X86_KEY_BYTES
#define chainhash_arm_key_from_bytes chainhash_x86_key_from_bytes
#define chainhash_arm_key_from_words chainhash_x86_key_from_words
#define chainhash_arm_key_from_seed chainhash_x86_key_from_seed
#define chainhash_arm_portable chainhash_x86_portable

#if !defined(CHAINHASH_ARM_FORCE_PORTABLE) && defined(__aarch64__) && \
    !defined(__AARCH64EB__) && (defined(__GNUC__) || defined(__clang__)) && \
    (defined(__ARM_FEATURE_AES) || defined(__ARM_FEATURE_CRYPTO))
#define CHAINHASH_ARM_HAVE_NEON 1
#include <arm_neon.h>
#define CHA_INLINE static inline __attribute__((always_inline))
typedef uint64x2_t cha_vec;

CHA_INLINE cha_vec cha_zero(void) { return vdupq_n_u64(0); }
CHA_INLINE cha_vec cha_word(uint64_t x) {
    return vcombine_u64(vcreate_u64(x), vcreate_u64(0));
}
CHA_INLINE cha_vec cha_loadword(const void *p) {
    /* A lane load, without passing a field element through a GPR. */
    return vcombine_u64(vreinterpret_u64_u8(vld1_u8((const uint8_t *)p)), vcreate_u64(0));
}
CHA_INLINE cha_vec cha_xor3(cha_vec a, cha_vec b, cha_vec c) {
#if defined(__ARM_FEATURE_SHA3)
    return veor3q_u64(a,b,c);
#else
    return veorq_u64(veorq_u64(a,b),c);
#endif
}
/* Pin both products. In particular, never let clang replace the lane-0
 * state with DUP/PMULL2 or FMOV through a GPR. High lanes of field elements
 * are unspecified; ONLY lane 0 is consumed by the next field product. */
CHA_INLINE cha_vec cha_ll(cha_vec a, cha_vec b) {
    cha_vec r;
    __asm__("pmull %0.1q, %1.1d, %2.1d" : "=w"(r) : "w"(a), "w"(b));
    return r;
}
CHA_INLINE cha_vec cha_hh(cha_vec a, cha_vec b) {
    cha_vec r;
    __asm__("pmull2 %0.1q, %1.2d, %2.2d" : "=w"(r) : "w"(a), "w"(b));
    return r;
}
CHA_INLINE cha_vec cha_reduce_add(cha_vec p, cha_vec add) {
    const cha_vec poly=vdupq_n_u64(27);
    const cha_vec r=cha_hh(p,poly), s=cha_hh(r,poly);
    return cha_xor3(veorq_u64(p,add),r,s);
}
CHA_INLINE cha_vec cha_step(cha_vec q, cha_vec t) {
    /* Move the coefficient, not the state: EXT is outside the PH loop. */
    return cha_reduce_add(cha_ll(vextq_u64(t,t,1),q),t);
}

CHA_INLINE cha_vec cha_group(const uint64_t *k, const uint8_t *p, cha_vec acc) {
    /* LD2 .2D de-interleaves at the load: [w0,w2], [w1,w3], likewise
     * [k0,k2], [k1,k3]. No UZP/ZIP/EXT/TBL/DUP in the product loop. */
    const uint64x2x2_t m=vld2q_u64((const uint64_t *)(const void *)p);
    const uint64x2x2_t key=vld2q_u64(k);
    const cha_vec a=veorq_u64(m.val[0],key.val[0]);
    const cha_vec b=veorq_u64(m.val[1],key.val[1]);
    return cha_xor3(acc,cha_ll(a,b),cha_hh(a,b));
}
CHA_INLINE cha_vec cha_ph_full(const uint64_t *k, const uint8_t *p) {
    cha_vec a=cha_zero(),b=a;
    size_t off;
    /* Two independent accumulators; exactly 16 products per 256 bytes. */
#if defined(__clang__)
#pragma clang loop unroll(disable)
#elif defined(__GNUC__)
#pragma GCC unroll 1
#endif
    for (off=0;off<CHAINHASH_ARM_BLOCK_BYTES;off+=64) {
        a=cha_group(k+off/8,p+off,a);
        b=cha_group(k+off/8+4,p+off+32,b);
    }
    return veorq_u64(a,b);
}
CHA_INLINE cha_vec cha_pair(const uint64_t *k, const uint8_t *p) {
    return cha_ll(veorq_u64(cha_loadword(p),cha_loadword(k)),
                  veorq_u64(cha_loadword(p+8),cha_loadword(k+1)));
}
CHA_INLINE cha_vec cha_ph_tail(const uint64_t *k, const uint8_t *p, size_t n) {
    cha_vec a=cha_zero(),b=a;
    size_t off=0;
    for (;n>=64;n-=64,off+=64) {
        a=cha_group(k+off/8,p+off,a);
        b=cha_group(k+off/8+4,p+off+32,b);
    }
    if (n>=32) { a=cha_group(k+off/8,p+off,a);n-=32;off+=32; }
    if (n>=16) { b=veorq_u64(b,cha_pair(k+off/8,p+off));n-=16;off+=16; }
    if (n) {
        uint8_t tail[16]={0};
        memcpy(tail,p+off,n); /* Only the final active pair is padded. */
        b=veorq_u64(b,cha_pair(k+off/8,tail));
    }
    return veorq_u64(a,b);
}
CHA_INLINE uint64_t cha_finish(cha_vec p, const uint64_t *t) {
    const cha_vec v=vaddq_u64(p,cha_loadword(t+8)); /* integer twist */
    const cha_vec q=cha_reduce_add(cha_ll(v,v),cha_loadword(t+3));
    const cha_vec r=cha_xor3(v,q,veorq_u64(cha_loadword(t+3),cha_loadword(t+4)));
    const cha_vec s=cha_reduce_add(cha_ll(q,r),cha_loadword(t+6));
    return vgetq_lane_u64(cha_reduce_add(cha_ll(veorq_u64(v,cha_loadword(t+5)),s),cha_loadword(t+7)),0);
}

/* Q=P+u; a nonfinal step is F(Q)=A+B Q, A=a+u, B=b+y.
 * right(left(Q)) has A=Ar+Br Al, B=Br Bl. These coefficients depend
 * only on the message and key, never on Q. A balanced tree composes k
 * consecutive steps with log2(k) coefficient depth; then ONE step uses Q.
 * All products/reductions are exact in GF(2^64), including zero slopes.
 * The length-injected last block is peeled and is never in this tree.
 */
typedef struct cha_affine { cha_vec a,b; } cha_affine;
CHA_INLINE cha_affine cha_coeff(const uint64_t *k,const uint8_t *p,cha_vec uy) {
    cha_affine f;
    f.a=veorq_u64(cha_ph_full(k,p),uy);
    f.b=vextq_u64(f.a,f.a,1);
    return f;
}
CHA_INLINE cha_affine cha_compose(cha_affine left,cha_affine right) {
    cha_affine f;
    const cha_vec pa=cha_ll(right.b,left.a),pb=cha_ll(right.b,left.b);
    f.a=cha_reduce_add(pa,right.a);
    f.b=cha_reduce_add(pb,cha_zero());
    return f;
}
CHA_INLINE cha_affine cha_coeff2(const uint64_t *k,const uint8_t *p,cha_vec uy) {
    const cha_affine a=cha_coeff(k,p,uy),b=cha_coeff(k,p+CHAINHASH_ARM_BLOCK_BYTES,uy);
    return cha_compose(a,b);
}
CHA_INLINE cha_affine cha_coeff4(const uint64_t *k,const uint8_t *p,cha_vec uy) {
    const cha_affine a=cha_coeff2(k,p,uy),b=cha_coeff2(k,p+2*CHAINHASH_ARM_BLOCK_BYTES,uy);
    return cha_compose(a,b);
}
CHA_INLINE cha_affine cha_coeff8(const uint64_t *k,const uint8_t *p,cha_vec uy) {
    const cha_affine a=cha_coeff4(k,p,uy),b=cha_coeff4(k,p+4*CHAINHASH_ARM_BLOCK_BYTES,uy);
    return cha_compose(a,b);
}

#define CHA_DRIVER(NAME,BATCH,COEFF) \
static inline uint64_t NAME(const chainhash_arm_key *key,const void *data,size_t len) { \
    const uint64_t *k=key->words,*t=k+CHAINHASH_X86_PH_WORDS; \
    const uint8_t *p=(const uint8_t *)data; size_t rem=len; \
    const cha_vec uy=vld1q_u64(t); \
    cha_vec q=veorq_u64(cha_loadword(t+2),cha_loadword(t)); \
    if ((BATCH)>1) { \
        while (rem>(size_t)(BATCH)*CHAINHASH_ARM_BLOCK_BYTES) { \
            const cha_affine f=COEFF(k,p,uy); \
            q=cha_reduce_add(cha_ll(f.b,q),f.a); \
            p+=(BATCH)*CHAINHASH_ARM_BLOCK_BYTES;rem-=(BATCH)*CHAINHASH_ARM_BLOCK_BYTES; \
        } \
    } \
    while (rem>CHAINHASH_ARM_BLOCK_BYTES) { \
        q=cha_step(q,veorq_u64(cha_ph_full(k,p),uy)); \
        p+=CHAINHASH_ARM_BLOCK_BYTES;rem-=CHAINHASH_ARM_BLOCK_BYTES; \
    } \
    { \
        cha_vec a=(rem==CHAINHASH_ARM_BLOCK_BYTES)?cha_ph_full(k,p):cha_ph_tail(k,p,rem); \
        a=cha_xor3(a,vdupq_n_u64((uint64_t)len),veorq_u64(uy,cha_loadword(t))); \
        q=cha_step(q,a); \
    } \
    return cha_finish(q,t); \
}
CHA_DRIVER(chainhash_arm_batch1,1,cha_coeff)
#if CHAINHASH_ARM_BLOCK_BYTES == 256
CHA_DRIVER(chainhash_arm_batch2,2,cha_coeff2)
CHA_DRIVER(chainhash_arm_batch4,4,cha_coeff4)
CHA_DRIVER(chainhash_arm_batch8,8,cha_coeff8)
#endif
#undef CHA_DRIVER
#undef CHA_INLINE
#else
#define CHAINHASH_ARM_HAVE_NEON 0
#define chainhash_arm_batch1 chainhash_x86_portable
#if CHAINHASH_ARM_BLOCK_BYTES == 256
#define chainhash_arm_batch2 chainhash_x86_portable
#define chainhash_arm_batch4 chainhash_x86_portable
#define chainhash_arm_batch8 chainhash_x86_portable
#endif
#endif

/* Conservative default; choose a 256-byte batching knob explicitly. */
static inline uint64_t chainhash_arm(const chainhash_arm_key *key,const void *data,size_t len) {
    return chainhash_arm_batch1(key,data,len);
}
#endif
