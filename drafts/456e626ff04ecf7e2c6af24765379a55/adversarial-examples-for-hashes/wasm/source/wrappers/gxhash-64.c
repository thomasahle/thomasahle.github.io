/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../verify/gxhash-64/gxhash64_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const uint8_t demo_message_1[] = {
0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff,0xff
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{15,16},64,64,1,0,0,{UINT64_C(0xf556ecbfcbfee3ad)},"43ec3783791d0fb8"},
    {{demo_message_0,demo_message_1},{15,16},128,64,1,1,0,{UINT64_C(0xf556ecbfcbfee3ad)},NULL},
};

static int demo_source_validation(void) {
    return smhasher3_verification(8) == 0x48F84240u && smhasher3_verification(16) == 0x64A77B47u;
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    blk h = gxhash128(m,n,key[0]); out[0] = demo_read64(h.b); if(demo_pairs[i].bits==128) out[1]=demo_read64(h.b+8);
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
