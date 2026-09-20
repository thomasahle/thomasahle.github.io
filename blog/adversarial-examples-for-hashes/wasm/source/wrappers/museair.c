/* Generated adapter. Rebuild with python3 build_demos.py. */
#define main demo_original_main
#define validate demo_original_validate
#define hash_pair demo_original_hash_pair
#define rng_init demo_original_rng_init
#include "../../../verify/museair/museair_verify.c"
#undef main
#undef validate
#undef hash_pair
#undef rng_init
#include "api-types.h"
static const uint8_t demo_message_0[] = {
0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00
};
static const uint8_t demo_message_1[] = {
0x80,0x79,0x76,0x3b,0xb1,0x9a,0x00,0x00,0x1a,0x9a,0x11,0x00,0x64,0x2d,0x3a,0x3f,0x01
};
static const DemoPair demo_pairs[] = {
    {{demo_message_0,demo_message_1},{17,17},64,64,1,0,0,{UINT64_C(0x2cb0f69f4abea221)},"d4ed417ecc529ae4"},
    {{demo_message_0,demo_message_1},{17,17},64,64,1,1,0,{UINT64_C(0x2cb0f69f4abea221)},NULL},
    {{demo_message_0,demo_message_1},{17,17},128,64,1,2,0,{UINT64_C(0x2cb0f69f4abea221)},NULL},
    {{demo_message_0,demo_message_1},{17,17},128,64,1,3,0,{UINT64_C(0x2cb0f69f4abea221)},NULL},
};

static int demo_source_validation(void) {
    for(int i=0;i<4;++i) if(smhasher3_verification(&VARIANTS[i]) != VARIANTS[i].verification) return 0; return 1;
}
static void demo_digest(int i,const uint8_t *m,size_t n,const uint64_t key[4],uint64_t out[4]) {
    uint8_t h[16]; const variant_t *v=&VARIANTS[demo_pairs[i].variant]; museair(m,n,key[0],v->bfast,v->b128,h); out[0]=demo_read64(h); if(v->b128) out[1]=demo_read64(h+8);
}
static int demo_class_key(int i,uint64_t state[4],uint64_t key[4]) {
    return 0;
}
static void demo_seed_key(uint64_t seed,uint64_t key[4]) { key[0]=seed; }
#include "api.h"
