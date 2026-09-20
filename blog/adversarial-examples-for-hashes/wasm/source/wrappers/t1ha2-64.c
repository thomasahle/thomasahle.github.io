/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/t1ha2-64/t1ha2_64_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0xc4,0xc0,0xcc,0x22,0x84,0xcd,0x23,0x9e,0xde,0x36,0xa1,0x8f,0xa6
};
static const uint8_t demo_message_1[] = {
0xfb,0xc3,0x18,0x66,0x04,0x9e,0xc0,0x2d,0xea,0x0c,0x4e,0x2b,0xe5,0x80,0xd3,0xcf
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{13,16},64,64,1,0,1,{UINT64_C(0x4c240c2749cd4915)},"c35b49165b1a4480"},
};

static int demo_source_validation(void) {
    demo_original_validate(); return 1;
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    out[0] = t1ha2_64(m,n,key[0]);
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    lmap_t map = lmap_init(demo_pairs[i].message[0], demo_pairs[i].length[0]);
    key[0] = lmap_seed(&map, (key[0] & ~PAIRS[0].lmask) | PAIRS[0].lval); return 1;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
