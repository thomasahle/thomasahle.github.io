/* Instruction throughput and loop-carried recurrence in SMHasher3 timer units. */
#include "Platform.h"
#include "Timing.h"
#include "chainhash128.h"
#include <stdio.h>
#include <algorithm>
double cycle_timer_mult=0;
static volatile uint64_t sink;
#if defined(__aarch64__)
#define KEEP(v) __asm__ volatile("":"+w"(v))
#else
#define KEEP(v) __asm__ volatile("":"+x"(v))
#endif
static double measure(int mode) {
    const int n=131072;double best=1e100;
    for(int rep=0;rep<9;rep++) {
        ch128_vec a=ch128_vword(ch128_make(0xabc123,0x13579)),b=ch128_vword(ch128_make(0x9abcdef,0x192837465));
        ch128_vec s0=a,s1=ch128_vxor(a,b),s2=b,s3=ch128_swap(a),s4=ch128_swap(b),s5=ch128_vxor(s1,s3),s6=ch128_vxor(s2,s4),s7=ch128_vxor(s5,s6);
        uint64_t t=cycle_timer_start();
        if(mode==0) for(int i=0;i<n;i++) {
#define STEP s0=ch128_ll(s0,b);
            STEP STEP STEP STEP STEP STEP STEP STEP
#undef STEP
            KEEP(s0);
        }
        if(mode==1) for(int i=0;i<n;i++) {
            s0=ch128_ll(s0,b);s1=ch128_ll(s1,b);s2=ch128_ll(s2,b);s3=ch128_ll(s3,b);
            s4=ch128_ll(s4,b);s5=ch128_ll(s5,b);s6=ch128_ll(s6,b);s7=ch128_ll(s7,b);
            KEEP(s0);KEEP(s1);KEEP(s2);KEEP(s3);KEEP(s4);KEEP(s5);KEEP(s6);KEEP(s7);
        }
        if(mode==2) for(int i=0;i<n;i++) {
#define STEP s0=ch128_vxor(a,ch128_vmul(s0,b));
            STEP STEP STEP STEP
#undef STEP
            KEEP(s0);
        }
        if(mode==3) for(int i=0;i<n;i++) {
#define STEP s0=ch128_vmul(s0,b);
            STEP STEP STEP STEP
#undef STEP
            KEEP(s0);
        }
        uint64_t dt=cycle_timer_end()-t;
        sink=ch128_wordv(ch128_vxor(ch128_vxor(ch128_vxor(s0,s1),ch128_vxor(s2,s3)),ch128_vxor(ch128_vxor(s4,s5),ch128_vxor(s6,s7)))).lo;
        best=std::min(best,(double)dt/(n*(mode<2?8:4)));
    }
    return best;
}
#ifdef CHAINHASH128_VP
static double vpmeasure() {
    double best=1e100; const int n=131072;
    for(int rep=0;rep<9;rep++) {
        __m256i b=_mm256_set_epi64x(99,97,95,93),a0=b,a1=_mm256_set1_epi64x(13),a2=_mm256_set1_epi64x(15),a3=_mm256_set1_epi64x(17),a4=_mm256_set1_epi64x(19),a5=_mm256_set1_epi64x(21),a6=_mm256_set1_epi64x(23),a7=_mm256_set1_epi64x(25);
        uint64_t t=cycle_timer_start();
        for(int i=0;i<n;i++) {
#define V(x) x=_mm256_clmulepi64_epi128(x,b,0);KEEP(x);
            V(a0) V(a1) V(a2) V(a3) V(a4) V(a5) V(a6) V(a7)
#undef V
        }
        uint64_t dt=cycle_timer_end()-t;
        sink=(uint64_t)_mm256_extract_epi64(a0,0);best=std::min(best,(double)dt/(n*8));
    }
    return best;
}
#endif
int main() {
    cycle_timer_init();
    printf("{\"timer_mult\":%.9f,\"clmul_latency\":%.6f,\"clmul_reciprocal_throughput\":%.6f,\"recurrence_latency\":%.6f,\"gfmul_latency\":%.6f",cycle_timer_mult,measure(0),measure(1),measure(2),measure(3));
#ifdef CHAINHASH128_VP
    printf(",\"vp256_reciprocal_throughput\":%.6f",vpmeasure());
#endif
    puts("}");return 0;
}
