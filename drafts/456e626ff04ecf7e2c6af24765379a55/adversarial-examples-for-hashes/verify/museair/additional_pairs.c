/* Supplement adapted from the supplied reviewer witness driver; MIT. */
#define main package_main
#include "museair_verify.c"
#undef main
int main(void) {
 const uint32_t expected[4]={0xF89F1683,0xC61BEE56,0xD3DFE238,0x27939BF1};
 for(int v=0;v<4;v++) if(smhasher3_verification(&VARIANTS[v])!=expected[v]) { fprintf(stderr,"verification mismatch %d\n",v);return 2; }

 const char *a[]={"101112131415161718191a1b1c1d1e1f3edfaa2763ccd3e8f8e5d00dc73236e4","9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad3650583dc3e0351f69bc5","5072a91bf37d672e9635eb86faaca478c2b800a93946a2b944b8d7eca4d5beede4981ab1371302717cdc4ef66698a882"};
 const char *b[]={"101112131415161718191a1b1c1d1e1f99b0519651a489b815f5132911d93af1","9d21a7fafac0e24f428e3c0124de5f8c54c1de8bef8535a636cfd97be5aad3656d83dc3e0351f69bad","5072a91bf37d672e9635eb86faaca47879d722e85be12cb6ffd7f5adc67230e2e4981ab1371302717cdc4ef66698a882"};
 uint64_t seeds[]={0,0x5f642f87d5e23888ULL,0x869c4388c903a121ULL};
 for(int p=0;p<3;p++) {uint8_t m[64],n[64],h[16],g[16]; size_t len=strlen(a[p])/2; unhex(a[p],m); unhex(b[p],n);
  for(int v=0;v<4;v++){museair(m,len,seeds[p],VARIANTS[v].bfast,VARIANTS[v].b128,h);museair(n,len,seeds[p],VARIANTS[v].bfast,VARIANTS[v].b128,g);printf("pair %c %s: ",'C'+p,VARIANTS[v].name);print_hex(h,hashbytes(&VARIANTS[v]));printf(" / ");print_hex(g,hashbytes(&VARIANTS[v]));puts(""); if((p==0 || VARIANTS[v].bfast) && memcmp(h,g,hashbytes(&VARIANTS[v]))) return 1;}
 }
 uint8_t m[64],n[64],h[16],g[16]; rng_t rng; rng_init(&rng,1);
 const uint64_t mask=UINT64_C(0x21233494220c8459), value=UINT64_C(0x0000008000008001), target=UINT64_C(0x5b9234ff04f56f3d);
 for(int p=1;p<3;p++) {size_t len=strlen(a[p])/2;unhex(a[p],m);unhex(b[p],n);
   uint64_t count=0, N=UINT64_C(1)<<20;
   for(uint64_t i=0;i<N;i++) {uint64_t seed=rng_next(&rng);if(p==2){seed=(seed&~mask)|value;if(((K[2]^seed)^(K[3]+seed))!=target)return 3;}
     int equal_all=1;
     for(int v=0;v<4;v++) if(VARIANTS[v].bfast){museair(m,len,seed,1,VARIANTS[v].b128,h);museair(n,len,seed,1,VARIANTS[v].b128,g);equal_all &= !memcmp(h,g,hashbytes(&VARIANTS[v]));}
     count+=equal_all;
   }
   printf("pair %c BFast both widths: %llu/%llu (%s)\n",'C'+p,(unsigned long long)count,(unsigned long long)N,p==1?"uniform seeds":"class-conditioned seeds");if(count!=N)return 4;
 }
 return 0;
}
