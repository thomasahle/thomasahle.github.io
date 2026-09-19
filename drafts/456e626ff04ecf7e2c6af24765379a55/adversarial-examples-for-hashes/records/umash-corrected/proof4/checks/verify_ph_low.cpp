// Independent low-table evaluator: literal canonical rows and explicit centres.
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <set>
#include <vector>
#include "masks.h"
using u64=uint64_t;using u128=__uint128_t;
const u128 SCALE=u128(1)<<64;
std::string str(u128 x){if(!x)return "0";std::string s;while(x){s.push_back('0'+x%10);x/=10;}std::reverse(s.begin(),s.end());return s;}
unsigned literal_lower_rank(u64 mask,unsigned N,unsigned v,unsigned E,unsigned O){
 u64 row[64]={};
 for(unsigned i=0;i<N;++i){
  if(v+i<64)row[v+i]^=u64(1)<<i;
  if(E+2*i<64)row[E+2*i]^=u64(1)<<i;
  if(O+2*i<64)row[O+2*i]^=u64(1)<<i;
 }
 u64 leads=0;
 while(mask){unsigned n=__builtin_ctzll(mask);mask&=mask-1;
  if(n>=N+v)continue;
  // Canonical rows vanish at a cancellation of the two known leading terms.
  if(row[n])leads|=u64(1)<<(63-__builtin_clzll(row[n]));
 }
 return __builtin_popcountll(leads);
}
int main(){bool first=true;std::cout<<"{\"scale\":\""<<str(SCALE)<<"\",\"rows\":[\n";
 for(unsigned s=1;s<=3;++s)for(unsigned t=0;t<=11;++t){
  std::array<std::array<u128,64>,852> weight;
  #pragma omp parallel for schedule(dynamic)
  for(unsigned i=0;i<852;++i){if(rows[i].mask&1)continue;
   for(unsigned k=s;k<64;++k){unsigned rr=64;
    for(unsigned E=0;E<=2*t+2;E+=2)for(unsigned O=1;O<=2*t+2;O+=2){
     unsigned e=E>2*t?128:E,o=O>2*t?129:O;
     if(std::min(e,o)>t)continue;
     unsigned base=s+std::min(e,o),vt=k-s;
     if(vt==base){for(unsigned v=base+1;v<=64+t;++v)rr=std::min(rr,literal_lower_rank(rows[i].mask,64-t,v,e,o));}
     else rr=std::min(rr,literal_lower_rank(rows[i].mask,64-t,std::min(vt,base),e,o));
    }
    weight[i][k]=std::min(SCALE,u128(rows[i].np)<<(64-rr));
   }
  }
  for(unsigned a=0;a<(1u<<s);a+=2){
   std::vector<unsigned> ix;std::vector<u64> centers;
   for(unsigned i=0;i<852;++i)if(rows[i].mask% (1u<<s)==a){ix.push_back(i);centers.push_back(rows[i].mask);for(unsigned k=s;k<64;++k)centers.push_back(rows[i].mask^(u64(1)<<k));}
   std::sort(centers.begin(),centers.end());centers.erase(std::unique(centers.begin(),centers.end()),centers.end());
   u128 best=0;
   for(auto c:centers){u128 sum=0;for(auto i:ix){u64 d=c^rows[i].mask;sum+=d?weight[i][__builtin_ctzll(d)]:SCALE;}best=std::max(best,sum);}
   best<<=s;
   if(!first)std::cout<<",\n";first=false;
   std::cout<<"{\"s\":"<<s<<",\"t\":"<<t<<",\"prefix\":"<<a<<",\"numerator\":\""<<str(best)<<"\",\"centres\":"<<centers.size()<<"}"<<std::flush;
  }
 }
 std::cout<<"\n]}\n";
}
