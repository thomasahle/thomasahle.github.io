#include "../../halftime-hash.hpp"
#include <cstdio>
#include <cstring>
#include <random>
#include <vector>
using namespace halftime_hash::advanced;
extern "C" void hh24(unsigned,const uint64_t*,const char*,size_t,uint64_t*);
uint64_t nh(uint64_t x,uint64_t k){uint64_t a=(uint32_t(x)+uint32_t(k))&0xffffffffULL;uint64_t b=(uint32_t(x>>32)+uint32_t(k>>32))&0xffffffffULL;return a*b;}
int main(){
 std::mt19937_64 rng(0x35eba134507ad221ULL);unsigned unequal=0,live=0,reference=0;
 for(unsigned b:{1,2,4,8}){
  std::vector<size_t> lens={0,1,7,8,8*b-1,8*b,168*b-1,168*b,168*b+1,1344*b,1344*b+1};
  for(unsigned trial=0;trial<10000;++trial){
   uint64_t k[2048],k2[2048];for(auto&v:k)v=rng();for(auto&v:k2)v=rng();
   size_t n=lens[trial%lens.size()],m=lens[(trial+1)%lens.size()];if(n==m)++m;
   std::vector<char>x(std::max(n,m)+1,0);uint64_t a[3],c[3];hh24(b,k,x.data(),n,a);hh24(b,k,x.data(),m,c);
   if(!memcmp(a,c,24))return 1;++unequal;
   // Read-set test: re-randomize every hole, retaining only the one-leaf live words.
   memcpy(k2,k,27*8);memcpy(k2+216,k+216,3*b*8);memcpy(k2+216+212*b,k+216+212*b,(3*b+1)*8);
   x.resize(168*b+1);x[48*b]=1;hh24(b,k,x.data(),168*b,a);hh24(b,k2,x.data(),168*b,c);
   if(memcmp(a,c,24))return 2;++live;
   // Independent scalar formula for a short, random byte string plus terminal length.
   n=rng()%(168*b);x.resize(n+1);for(auto&v:x)v=rng();hh24(b,k,x.data(),n,a);
   size_t blocks=n/(8*b)+1,start=216+192*b+(21-blocks)*b;
   for(unsigned j=0;j<3;++j){
    uint64_t expect=nh(n,k[216+(213+j)*b]);
    for(size_t i=0;i<blocks*b;++i){uint64_t word=0;if(8*i<n)memcpy(&word,x.data()+8*i,std::min(size_t(8),n-8*i));expect+=nh(word,k[start+i+j*b]);}
    if(expect!=a[j])return 3;
   }++reference;
  }
 }
 printf("{\"unequal_length_zero_message_pairs\":%u,\"unequal_length_collisions\":0,\"one_leaf_live_readset_checks\":%u,\"independent_short_tail_formula_checks\":%u,\"status\":\"passed\"}\n",unequal,live,reference);
}
