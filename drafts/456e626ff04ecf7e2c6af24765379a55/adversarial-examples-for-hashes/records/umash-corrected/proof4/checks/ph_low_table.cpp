#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <limits>
#include <set>
#include <vector>
#include "masks.h"
using u64=uint64_t;using u128=__uint128_t;
const u128 SCALE=u128(1)<<64;
std::string str(u128 x){if(!x)return "0";std::string s;while(x){s.push_back('0'+x%10);x/=10;}std::reverse(s.begin(),s.end());return s;}
unsigned rank(u64 m,unsigned N,unsigned v,unsigned E,unsigned O){
 u64 used=0;
 while(m){unsigned n=__builtin_ctzll(m);m&=m-1;
  if(n>=N+v)continue;
  int mul=n>=v ? int(n-v):-1;
  unsigned h=n&1?O:E;
  int sq=n>=h?int((n-h)/2):-1;
  if(mul==sq)continue;
  int pivot=std::max(mul,sq);
  if(pivot>=0){assert(pivot<int(N));used|=u64(1)<<pivot;}
 }
 return __builtin_popcountll(used);
}
u128 walk(const std::vector<unsigned>& ix,const std::array<std::array<u128,64>,852>& ws,unsigned k){
 if(ix.empty())return 0;
 if(k==64)return SCALE;
 std::vector<unsigned> left,right;
 u128 costleft=0,costright=0;
 for(auto i:ix){if((rows[i].mask>>k)&1){right.push_back(i);costleft+=ws[i][k];}else{left.push_back(i);costright+=ws[i][k];}}
 return std::max(costleft+walk(left,ws,k+1),costright+walk(right,ws,k+1));
}
int main(){
 std::cout<<"{\"scale\":\""<<str(SCALE)<<"\",\"rows\":[\n";
 bool first=true;
 for(unsigned s=1;s<=3;++s)for(unsigned t=0;t<=11;++t){
  unsigned N=64-t;
  std::vector<std::pair<unsigned,unsigned>> shapes;
  for(unsigned E=0;E<=2*t;E+=2){for(unsigned O=1;O<=2*t;O+=2)if(std::min(E,O)<=t)shapes.emplace_back(E,O);if(E<=t)shapes.emplace_back(E,129);}
  for(unsigned O=1;O<=t;O+=2)shapes.emplace_back(128,O);
  std::array<std::array<u128,64>,852> ws;
  #pragma omp parallel for schedule(dynamic)
  for(unsigned i=0;i<852;++i){
   auto &row=rows[i];
   for(unsigned k=s;k<64;++k){
    unsigned vt=k-s, mr=64;
    for(auto [E,O]:shapes){
     unsigned base=s+std::min(E,O);
     if(vt==base){for(unsigned v=base+1;v<=64+t;++v)mr=std::min(mr,rank(row.mask,N,v,E,O));}
     else mr=std::min(mr,rank(row.mask,N,std::min(vt,base),E,O));
    }
    ws[i][k]=std::min(SCALE,u128(row.np)<<(64-mr));
   }
  }
  std::vector<u128> vals;
  for(unsigned a=0;a<(1u<<s);++a){std::vector<unsigned> ix;for(unsigned i=0;i<852;++i)if(rows[i].mask% (1u<<s)==a)ix.push_back(i);vals.push_back(walk(ix,ws,s)*(1u<<s));}
  if(!first)std::cout<<",\n";first=false;
  u128 mx=*std::max_element(vals.begin(),vals.end());
  std::cout<<"{\"s\":"<<s<<",\"t\":"<<t<<",\"by_prefix\":[";
  for(unsigned a=0;a<vals.size();++a){if(a)std::cout<<",";std::cout<<"\""<<str(vals[a])<<"\"";}
  std::cout<<"],\"maximum_numerator\":\""<<str(mx)<<"\",\"ceiling\":"<<str((mx+SCALE-1)/SCALE)<<"}"<<std::flush;
 }
 std::cout<<"\n]}\n";
}
