#include "../../halftime-hash.hpp"
#include <cstdio>
#include <cstring>
using namespace halftime_hash::advanced;
template<unsigned B> void flips(){
 using W=RepeatWrapper<BlockWrapperScalar,B>;using Block=typename W::Block;
 unsigned counts[10]={};
 for(unsigned i=0;i<21;++i)for(unsigned lane=0;lane<B;++lane)for(unsigned bit=0;bit<64;++bit){
  Block a[9][3]={};uint64_t word=uint64_t(1)<<bit;
  memcpy((char*)&a[i/3][i%3]+lane*8,&word,8);Encode3(a);
  unsigned dist=0;for(unsigned j=0;j<9;++j){Block zero[3]={};dist+=memcmp(a[j],zero,sizeof(zero))!=0;}
  if(dist<3)__builtin_trap();++counts[dist];
 }
 printf("{\"b\":%u,\"single_bit_flips\":%u,\"distances\":{",B,21*B*64);
 for(unsigned d=3;d<=9;++d)printf("%s\"%u\":%u",d>3?",":"",d,counts[d]);printf("}}\n");
}
int main(){flips<1>();flips<2>();flips<4>();flips<8>();}
