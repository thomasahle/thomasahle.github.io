/* check_vectors.c: compare the C port against crate-generated vectors on stdin */
#include <stdio.h>
#include <stdlib.h>
#include "museair2.h"
static int hexv(char c){ return c<='9'? c-'0' : (c|32)-'a'+10; }
static ma_u128 p128(const char*s){ ma_u128 x=0; for(int i=0;i<32;i++) x=(x<<4)|hexv(s[i]); return x; }
int main(void){
    char line[8192]; static uint8_t buf[4096]; long n=0, bad=0; size_t maxlen=0;
    while(fgets(line,sizeof line,stdin)){
        unsigned long len; unsigned long long seed,sa,sb,h,hb; char hex[2048], h2s[40], h2bs[40];
        if(sscanf(line,"%lu %llx %llx %llx %2047s %llx %llx %39s %39s",&len,&seed,&sa,&sb,hex,&h,&hb,h2s,h2bs)!=9){ fprintf(stderr,"parse error\n"); return 2; }
        if(len>0) for(size_t i=0;i<len;i++) buf[i]=(uint8_t)(hexv(hex[2*i])*16+hexv(hex[2*i+1]));
        uint64_t c0=museair2_hash(buf,len,seed,0), c1=museair2_hash(buf,len,seed,1);
        ma_u128 d0=museair2_hash128(buf,len,sa,sb,0), d1=museair2_hash128(buf,len,sa,sb,1);
        int ok = c0==h && c1==hb && d0==p128(h2s) && d1==p128(h2bs);
        if(!ok){ bad++; if(bad<10) fprintf(stderr,"MISMATCH len=%lu seed=%016llx: %016llx/%016llx %016llx/%016llx\n",len,seed,(unsigned long long)c0,h,(unsigned long long)c1,hb); }
        n++; if(len>maxlen) maxlen=len;
    }
    printf("vectors %ld, mismatches %ld, max len %zu -> %s\n", n, bad, maxlen, bad? "FAIL":"PASS");
    return bad?1:0;
}
