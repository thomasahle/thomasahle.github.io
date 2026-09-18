/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../verify/t1ha2-64/t1ha2_64_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x40,0x6b,0x68,0xc2,0x81,0xa5,0x5e,0x00,0x15,0x8e,0xd1,0x0a,0x0d,0x96,0xf8,0xff
};
static const uint8_t demo_message_1[] = {
0xd1,0xd8,0x62,0x41,0xd2,0x4d,0x9f,0x99,0x03,0x23,0xe0
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{16,11},64,64,1,0,1,{UINT64_C(0x3c805cc67a687f30)},"f2a1196d24fddaab"},
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
