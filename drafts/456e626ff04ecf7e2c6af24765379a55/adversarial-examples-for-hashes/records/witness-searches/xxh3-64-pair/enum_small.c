/* Enumerate the collision set {(A,b): fold(A,~b) == fold(~A,b)} for A,b < 2^n. */
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
static inline uint64_t fold(uint64_t a, uint64_t b){unsigned __int128 p=(unsigned __int128)a*b;return (uint64_t)p^(uint64_t)(p>>64);}
int main(int argc,char**argv){int n=argc>1?atoi(argv[1]):10; uint64_t N=1ULL<<n, c=0, diag=0;
 for(uint64_t A=0;A<N;A++)for(uint64_t b=0;b<N;b++){ if(fold(A,~b)==fold(~A,b)){c++; if(A==b)diag++; else if(c-diag<=60) printf("A=%llx b=%llx\n",(unsigned long long)A,(unsigned long long)b);} }
 printf("n=%d pairs=%llu collisions=%llu diagonal=%llu\n",n,(unsigned long long)(N*N),(unsigned long long)c,(unsigned long long)diag); return 0;}
