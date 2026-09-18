#include "../../halftime-hash.hpp"
using namespace halftime_hash::advanced;
extern "C" __attribute__((noinline)) void hh24(unsigned b,const uint64_t*k,const char*x,size_t n,uint64_t*y){
 switch(b){case 1:V1<3>(k,x,n,y);break;case 2:V2<3>(k,x,n,y);break;case 4:V3<3>(k,x,n,y);break;case 8:V4<3>(k,x,n,y);break;default:__builtin_trap();}
}
