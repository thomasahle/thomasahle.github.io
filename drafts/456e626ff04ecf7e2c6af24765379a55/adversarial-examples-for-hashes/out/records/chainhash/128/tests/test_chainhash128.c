#include "../chainhash128.h"
#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>
#if defined(__unix__) || defined(__APPLE__)
#include <sys/mman.h>
#include <unistd.h>
#endif
static uint64_t rng=UINT64_C(0x517cc1b727220a95);
static uint64_t rnd(void) { return ch128_splitmix(&rng); }
static ch128_word rw(void) { ch128_word a; a.lo=rnd(); a.hi=rnd(); return a; }
static void fail(const char *s,size_t n) { fprintf(stderr,"FAIL %s %zu\n",s,n); exit(1); }
static void check(const chainhash128_key *k,const uint8_t *p,size_t n) {
    ch128_word a=chainhash128(k,p,n),b=chainhash128_portable(k,p,n);
    if(!ch128_equal(a,b)) { fprintf(stderr,"%016" PRIx64 "%016" PRIx64 " != %016" PRIx64 "%016" PRIx64 "\n",a.hi,a.lo,b.hi,b.lo); fail("hash",n); }
}
/* Separate polynomial long-division reduction for arbitrary 256 bits. */
static ch128_word reduce_serial(ch128_raw p) {
    uint64_t t[4]={p.lo.lo,p.lo.hi,p.hi.lo,p.hi.hi}; int i; unsigned j;
    static const unsigned bits[]={0,1,2,7,128};
    for(i=255;i>=128;i--) if((t[i/64]>>(i%64))&1)
        for(j=0;j<5;j++) { unsigned pos=(unsigned)i-128+bits[j]; t[pos/64]^=UINT64_C(1)<<(pos%64); }
    return ch128_make(t[0],t[1]);
}
static void arithmetic(void) {
    size_t i;
    for(i=0;i<20000;i++) {
        ch128_word a=rw(),b=rw(); ch128_raw p=ch128_clmul_ref(a,b);
        if(!ch128_equal(reduce_serial(p),ch128_mul_ref(a,b))) fail("reference mul",i);
#if CHAINHASH128_HARDWARE
        ch128_vraw v=ch128_vclmul(ch128_vword(a),ch128_vword(b));
        if(!ch128_equal(p.lo,ch128_wordv(v.lo))||!ch128_equal(p.hi,ch128_wordv(v.hi))) fail("raw product",i);
        if(!ch128_equal(ch128_mul_ref(a,b),ch128_wordv(ch128_vmul(ch128_vword(a),ch128_vword(b))))) fail("mul",i);
        if(!ch128_equal(ch128_mul_ref(a,a),ch128_wordv(ch128_vsquare(ch128_vword(a))))) fail("square",i);
        p.lo=rw();p.hi=rw();v.lo=ch128_vword(p.lo);v.hi=ch128_vword(p.hi);
        if(!ch128_equal(reduce_serial(p),ch128_wordv(ch128_reduce(v)))) fail("reduce",i);
#endif
    }
    /* Every monomial product, including X^254. */
    for(i=0;i<128*128;i++) {
        unsigned x=(unsigned)i/128,y=(unsigned)i%128;
        ch128_word a=x<64?ch128_make(UINT64_C(1)<<x,0):ch128_make(0,UINT64_C(1)<<(x-64));
        ch128_word b=y<64?ch128_make(UINT64_C(1)<<y,0):ch128_make(0,UINT64_C(1)<<(y-64));
if(!ch128_equal(ch128_mul_ref(a,b),reduce_serial(ch128_clmul_ref(a,b)))) fail("reference monomial",i);
#if CHAINHASH128_HARDWARE
        if(!ch128_equal(ch128_mul_ref(a,b),ch128_wordv(ch128_vmul(ch128_vword(a),ch128_vword(b))))) fail("monomial",i);
#endif
    }
    if(!ch128_equal(ch128_addint(ch128_make(UINT64_MAX,UINT64_MAX),ch128_make(1,0)),ch128_make(0,0))) fail("twist carry",0);
}
static void vectors(void) {
    uint8_t keybytes[CHAINHASH128_KEY_BYTES],msg[8193],out[16]; size_t i,j;
    static const size_t lens[]={0,1,15,16,17,31,32,33,63,64,65,127,128,129,255,256,257,511,512,513,1024,4096,8193};
    chainhash128_key k;
    for(i=0;i<sizeof keybytes;i++) keybytes[i]=(uint8_t)(i*73+19);
    for(i=0;i<sizeof msg;i++) msg[i]=(uint8_t)(i*137+29);
    k=chainhash128_key_from_ideal_bytes(keybytes);
    for(i=0;i<sizeof lens/sizeof *lens;i++) {
        chainhash128_store(out,chainhash128(&k,msg,lens[i])); printf("%zu ",lens[i]);
        for(j=0;j<16;j++) printf("%02x",out[j]);
        puts("");
    }
}
int main(int argc,char **argv) {
    size_t i,j; uint8_t *buf; chainhash128_key k;
    (void)argv;
    if(argc>1) { vectors(); return 0; }
    if(!chainhash128_selftest()) fail("selftest",0);
    arithmetic();
    buf=(uint8_t *)malloc(4*1024*1024+64); if(!buf) return 2;
    for(i=0;i<12000;i++) {
        size_t n=i<4097?i:rnd()%4097,offset=rnd()%32;
        for(j=0;j<CHAINHASH128_KEY_WORDS;j++) k.words[j]=rw();
        for(j=0;j<n;j++) buf[offset+j]=(uint8_t)rnd();
        check(&k,buf+offset,n);
    }
    for(i=0;i<32;i++) {
        size_t n=i<28?4097+rnd()%131072:1024*1024+(i-28)*333331;
        for(j=0;j<CHAINHASH128_KEY_WORDS;j++) k.words[j]=rw();
        for(j=0;j<n;j++) buf[j]=(uint8_t)rnd();
        check(&k,buf,n);
    }
    /* Zero/ones keys, zero/ones messages, all tails and model-A expansion. */
    for(i=0;i<4;i++) {
        uint8_t material[160];
        memset(&k,(i&1)?255:0,sizeof k);memset(buf,(i&2)?255:0,4097);
        for(j=0;j<=1025;j++) check(&k,buf,j);
        for(j=0;j<sizeof material;j++) material[j]=(uint8_t)rnd();
        k=chainhash128_key_from_bytes(material);check(&k,buf,4097);
        for(j=1;j<CHAINHASH128_BLOCK_WORDS;j++)
            if(!ch128_equal(k.words[j],ch128_mul_ref(k.words[j-1],k.words[0]))) fail("schedule",j);
    }
#if defined(__unix__) || defined(__APPLE__)
    {
        size_t page=(size_t)sysconf(_SC_PAGESIZE);
        uint8_t *m=(uint8_t *)mmap(NULL,2*page,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANON,-1,0);
        if(m==MAP_FAILED||mprotect(m+page,page,PROT_NONE)) fail("mmap",0);
        for(i=0;i<page;i++) m[i]=(uint8_t)rnd();
        for(i=0;i<=1025;i++) check(&k,m+page-i,i);
        munmap(m,2*page);
    }
#endif
    check(&k,NULL,0); free(buf);
    printf("PASS backend=%s block=%d: 12000 random lengths 0..4096, 32 long, 4104 patterns, 1026 guarded tails, 20000 arithmetic, 16384 monomials\n",CHAINHASH128_BACKEND,CHAINHASH128_BLOCK_BYTES);
    return 0;
}
