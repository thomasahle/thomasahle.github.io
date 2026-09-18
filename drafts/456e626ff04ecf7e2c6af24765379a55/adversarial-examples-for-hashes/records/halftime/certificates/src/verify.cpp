#include <cstdio>
#include <cstdlib>
#include <cstdint>
#include <vector>
#include <random>
#include <cstring>
#include <sys/mman.h>
#include <sys/wait.h>
#include <unistd.h>
#ifdef HH_SCALAR
#undef __SSE2__
#undef __AVX2__
#undef __AVX512F__
#undef __ARM_NEON
#undef __ARM_NEON__
#endif
#ifdef HH_BASE
#include "../../base-neon.hpp"
#else
#include "../../halftime-hash.hpp"
#endif
using namespace halftime_hash;
using namespace halftime_hash::advanced;
static volatile uint64_t sink;
template<unsigned K> void hash(unsigned b,const uint64_t*k,const char*x,size_t n,uint64_t*y){
 switch(b){case 1:V1<K>(k,x,n,y);break;case 2:V2<K>(k,x,n,y);break;case 4:V3<K>(k,x,n,y);break;case 8:V4<K>(k,x,n,y);break;default:abort();}
}
void hash(unsigned b,unsigned k,const uint64_t*e,const char*x,size_t n,uint64_t*y){
 switch(k){case 2:hash<2>(b,e,x,n,y);break;case 3:hash<3>(b,e,x,n,y);break;case 4:hash<4>(b,e,x,n,y);break;case 5:hash<5>(b,e,x,n,y);break;}
}
template<unsigned B>size_t needed(unsigned k,size_t n){
 using W=RepeatWrapper<BlockWrapperScalar,B>;
 switch(k){case 2:return GetEntropyBytesNeeded<W,2>(n);case 3:return GetEntropyBytesNeeded<W,3>(n);case 4:return GetEntropyBytesNeeded<W,4>(n);default:return GetEntropyBytesNeeded<W,5>(n);}
}
size_t needed(unsigned b,unsigned k,size_t n){switch(b){case 1:return needed<1>(k,n);case 2:return needed<2>(k,n);case 4:return needed<4>(k,n);default:return needed<8>(k,n);}}
int guardcall(unsigned b,unsigned k,size_t n,size_t bytes){
 pid_t child=fork();if(child<0)abort();
 if(!child){
  const size_t page=sysconf(_SC_PAGESIZE),span=((bytes+page-1)/page+1)*page;
  char*p=(char*)mmap(0,span,PROT_READ|PROT_WRITE,MAP_PRIVATE|MAP_ANONYMOUS,-1,0);if(p==MAP_FAILED)_exit(2);
  if(mprotect(p+span-page,page,PROT_NONE))_exit(3);
  uint64_t*key=(uint64_t*)(p+span-page-bytes);
  for(size_t j=0;j<bytes/8;++j)key[j]=0xe51dab919713fc5bULL*j;
  std::vector<char>x(n+1,0x53);uint64_t out[5]={};hash(b,k,key,x.data(),n,out);sink=out[0];_exit(0);
 }
 int status;waitpid(child,&status,0);return WIFSIGNALED(status)?-WTERMSIG(status):WEXITSTATUS(status);
}
int main(int argc,char**argv){
 if(argc>1&&!strcmp(argv[1],"guard")){
  unsigned tests=0;
  for(unsigned b:{1,2,4,8})for(unsigned k:{2,3,4,5}){
   size_t group=24*b*(k==2?6:k==5?5:7);
   std::vector<size_t> lens={0,1,7,8,8*b-1,8*b,8*b+1,group-1,group,group+1,group+8*b-1,group+8*b,2*group-1};
   for(size_t roots:{8,9,64,65,72,73,512,513})for(int off:{-1,0,1})lens.push_back(roots*group+off);
   for(auto n:lens){size_t bytes=needed(b,k,n);int ok=guardcall(b,k,n,bytes),bad=guardcall(b,k,n,bytes-8);
    printf("{\"b\":%u,\"k\":%u,\"bytes\":%zu,\"key_words\":%zu,\"exact_status\":%d,\"minus_one_word_status\":%d}\n",b,k,n,bytes/8,ok,bad);fflush(stdout);
    if(ok||bad!=-11)return 1;++tests;
   }
  }
  fprintf(stderr,"guard passed: %u exact extents and %u one-word-short faults\n",tests,tests);return 0;
 }
 std::mt19937_64 r(0x3314114720185279ULL);
 FILE*f=argc>1?fopen(argv[1],"wb"):nullptr;
 if(argc>1&&!f)return 3;
 for(unsigned t=0;t<10000;++t){
  size_t n=r()%32768;
  if(t<160){const unsigned sizes[]={8,16,32,64,144,168,288,336,576,672,1152,1344,9216,10752,73728,86016};n=sizes[t%16]*(t/48+1);if(t%3==0)--n;if(t%3==1)++n;}
  std::vector<char>x(n+16);for(size_t j=0;j<x.size();j+=8){uint64_t w=r();memcpy(x.data()+j,&w,std::min(size_t(8),x.size()-j));}
  std::vector<uint64_t>key(9000);for(auto&v:key)v=r();
  for(unsigned b:{1,2,4,8})for(unsigned k:{2,3,4,5}){
   uint64_t out[7]={0,0,0,0,0,0xb73f2c845221710aULL,0x8265f790271835baULL};
   hash(b,k,key.data(),x.data()+t%8,n,out);if(out[5]!=0xb73f2c845221710aULL||out[6]!=0x8265f790271835baULL)return 4;
   sink=out[0];if(f)fwrite(out,8,k,f);
  }
  uint64_t s[4]={HalftimeHashStyle64(key.data(),x.data()+t%8,n),HalftimeHashStyle128(key.data(),x.data()+t%8,n),HalftimeHashStyle256(key.data(),x.data()+t%8,n),HalftimeHashStyle512(key.data(),x.data()+t%8,n)};
  if(f)fwrite(s,8,4,f);
 }
 if(f)fclose(f);
 printf("{\"random_inputs\":10000,\"widths\":[1,2,4,8],\"output_words\":[2,3,4,5],\"style_wrappers\":4,\"core_calls\":160000,\"style_calls\":40000,\"unaligned_inputs\":true,\"status\":\"passed\"}\n");
}
