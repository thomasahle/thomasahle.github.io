#include "../../halftime-hash.hpp"
#include <cstdio>
using namespace halftime_hash::advanced;
template<int K,int N,int D> void emit(){
  uint64_t a[N][3]={};
  for(int j=0;j<D*3;++j) a[j/3][j%3]=uint64_t(1)<<j;
  if(K==2) Encode2(a);
  if(K==3) Encode3(a);
  if(K==4) Encode4(a);
  if(K==5) Encode5(a);
  printf("{\"k\":%d,\"n\":%d,\"d\":%d,\"rows\":[",K,N,D);
  for(int j=0;j<N*3;++j)printf("%s%llu",j?",":"",(unsigned long long)a[j/3][j%3]);
  printf("]}\n");
}
int main(){emit<2,7,6>();emit<3,9,7>();emit<4,10,7>();emit<5,9,5>();}
