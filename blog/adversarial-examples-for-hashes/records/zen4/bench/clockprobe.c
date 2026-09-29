// TSC rate vs CLOCK_MONOTONIC_RAW, core clock from a 1-cycle dependent add chain,
// and a user-mode perf_event_open cycles counter over the same loop.
#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <x86intrin.h>
#include <sys/syscall.h>
#include <sys/ioctl.h>
#include <linux/perf_event.h>
static double now(void){struct timespec t;clock_gettime(CLOCK_MONOTONIC_RAW,&t);return t.tv_sec+t.tv_nsec*1e-9;}
static long pe(uint64_t cfg){struct perf_event_attr a;memset(&a,0,sizeof a);a.size=sizeof a;a.type=PERF_TYPE_HARDWARE;a.config=cfg;a.disabled=1;a.exclude_kernel=1;a.exclude_hv=1;return syscall(SYS_perf_event_open,&a,0,-1,-1,0);}
int main(void){
  double t0=now();uint64_t c0=__rdtsc();while(now()-t0<1.0);uint64_t c1=__rdtsc();double t1=now();
  printf("tsc_ghz %.6f\n",(c1-c0)/(t1-t0)/1e9);
  long fd=pe(PERF_COUNT_HW_CPU_CYCLES);
  printf("perf_event_open cycles fd %ld\n",fd);
  for(int rep=0;rep<3;rep++){
    uint64_t n=2000000000ull,x=0;
    if(fd>=0){ioctl(fd,0x2403,0);ioctl(fd,0x2400,0);} // RESET, ENABLE
    double a=now();uint64_t ta=__rdtsc();
    for(uint64_t i=0;i<n;i++) __asm__ volatile("add $1,%0":"+r"(x));
    uint64_t tb=__rdtsc();double b=now();
    long long cyc=-1; if(fd>=0){ioctl(fd,0x2401,0); if(read(fd,&cyc,8)!=8) cyc=-1;}
    printf("add-chain core_ghz %.4f  tsc_ticks/iter %.4f  perf_cycles/iter %.4f\n", n/(b-a)/1e9, (double)(tb-ta)/n, cyc>0?(double)cyc/n:-1.0);
  }
  return 0;
}
