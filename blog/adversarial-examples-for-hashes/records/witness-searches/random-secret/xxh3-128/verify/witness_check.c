/* witness_check.c -- recompute a random-secret witness through the public, non-inlined library API.
 * build: curl -sSL -o xxhash.c https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.c
 *        cc -O2 -o witness_check witness_check.c xxhash.c
 * usage: witness_check <len> <m_hex> <m'_hex> <secret_hex(192 B)> */
#include "xxhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static int hv(char c){ if(c>='0'&&c<='9')return c-'0'; c|=32; return c-'a'+10; }
static void unhex(const char*s,unsigned char*o,int n){ for(int i=0;i<n;i++) o[i]=(unsigned char)(hv(s[2*i])*16+hv(s[2*i+1])); }
int main(int argc,char**argv){
    if(argc!=5){fprintf(stderr,"usage\n");return 2;}
    int len=atoi(argv[1]); unsigned char m[1024],m2[1024],s[192];
    if(strlen(argv[2])!=(size_t)2*len||strlen(argv[3])!=(size_t)2*len||strlen(argv[4])!=384){fprintf(stderr,"bad lengths\n");return 2;}
    unhex(argv[2],m,len); unhex(argv[3],m2,len); unhex(argv[4],s,192);
    XXH128_hash_t a=XXH3_128bits_withSecret(m,len,s,192), b=XXH3_128bits_withSecret(m2,len,s,192);
    printf("library %u: H(m)=%016llx%016llx H(m')=%016llx%016llx %s\n",XXH_versionNumber(),(unsigned long long)a.high64,(unsigned long long)a.low64,(unsigned long long)b.high64,(unsigned long long)b.low64,(a.low64==b.low64&&a.high64==b.high64)?"COLLISION":"no collision");
    return 0;
}
