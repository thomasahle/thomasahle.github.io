#include "rng.hpp"
#include <omp.h>
#include <sys/random.h>
#include <atomic>
#include <chrono>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <algorithm>
#include <vector>
extern "C" void hh24(unsigned,const uint64_t*,const char*,size_t,uint64_t*);
int main(int argc,char**argv){
 if(argc!=5 || !AesCtr::self_test())return 2;
 unsigned b=atoi(argv[1]),threads=atoi(argv[3]);uint64_t count=strtoull(argv[2],0,0);int cond=atoi(argv[4]);
 unsigned char seed[16]; if(getrandom(seed,16,0)!=16)return 3;
 char hs[33];for(int i=0;i<16;++i)sprintf(hs+2*i,"%02x",seed[i]);
 printf("{\"event\":\"start\",\"b\":%u,\"requested\":%llu,\"threads\":%u,\"condition\":%d,\"seed\":\"%s\",\"rng_kat\":true,\"live_words\":%u,\"live_ranges\":[[0,27],[216,%u],[%u,%u]]}\n",b,(unsigned long long)count,threads,cond,hs,28+6*b,216+3*b,216+212*b,217+215*b);fflush(stdout);
 std::atomic<uint64_t> done{0},hits{0};
 auto start=std::chrono::steady_clock::now();
 const uint64_t chunk=1<<20, chunks=(count+chunk-1)/chunk;
 #pragma omp parallel num_threads(threads)
 {
  int tid=omp_get_thread_num(); AesCtr rng(seed,tid);
  alignas(64) uint64_t key[2048]={},random[80];
  std::vector<char>x(168*b,0),y=x;y[48*b]=1;
  uint64_t a[3],c[3];
  #pragma omp for schedule(static)
  for(uint64_t job=0;job<chunks;++job){
   uint64_t n=std::min(chunk,count-job*chunk),localhits=0;
   for(uint64_t i=0;i<n;++i){
    rng.fill(random,28+6*b);
    memcpy(key,random,27*8);memcpy(key+216,random+27,3*b*8);
    memcpy(key+216+212*b,random+27+3*b,(3*b+1)*8);
    if(cond==1)key[6]&=0xffffffffULL;
    if(cond==2)while(!(key[6]>>32))key[6]=rng.next_word();
    hh24(b,key,x.data(),x.size(),a);hh24(b,key,y.data(),y.size(),c);
    localhits+=!((a[0]^c[0])|(a[1]^c[1])|(a[2]^c[2]));
   }
   hits.fetch_add(localhits);
   uint64_t d=done.fetch_add(n)+n;
   if(d%(1ULL<<28)==0){
    #pragma omp critical
    {printf("{\"event\":\"progress\",\"completed\":%llu,\"collisions\":%llu,\"seconds\":%.3f}\n",(unsigned long long)d,(unsigned long long)hits.load(),std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count());fflush(stdout);}
   }
  }
 }
 printf("{\"event\":\"result\",\"b\":%u,\"condition\":%d,\"completed\":%llu,\"collisions\":%llu,\"seconds\":%.3f}\n",b,cond,(unsigned long long)done.load(),(unsigned long long)hits.load(),std::chrono::duration<double>(std::chrono::steady_clock::now()-start).count());
 return done!=count||hits!=0;
}
