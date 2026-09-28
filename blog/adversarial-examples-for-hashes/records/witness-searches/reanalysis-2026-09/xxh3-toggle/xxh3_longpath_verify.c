/* Independent verifier for the XXH3 weak-key long-path multicollision.
 * Upstream xxHash v0.8.3 (xxhash.h) included verbatim; XXH_INLINE_ALL.
 * Mechanism (rederived here, not copied): in the bulk loop lane i of stripe n does
 *   acc[i^1] += data_val;  acc[i] += lo32(data_val^K[n+i]) * hi32(data_val^K[n+i])
 * with K[m] = LE64(secret + 8*m) and the same secret words reused every 1024-byte
 * block.  A "toggle" for lane-pair a=2t at (lane a, stripe j-a) and (lane a+1,
 * stripe j-a-1), both words = D, complemented together, changes acc[a] and acc[a+1]
 * each by dg+dD, dD=~D-D=M-2D, dg=M32(M32-l-h), l=lo32(D^K[j]),h=hi32(D^K[j]).
 * For D=0x7fffffff it vanishes iff (lo32(K[j])^0x7fffffff)+hi32(K[j])=0xfffffffe.
 * All toggles in all blocks share the ONE condition on K[j], so r toggles give 2^r
 * colliding messages at p = density = (2^32-1)/2^64 ~ 2^-32, not p^r.
 *
 * usage: xxh3_longpath <test> ...
 *   selfcheck
 *   single <bits64|128> <class|unif> <len> <j> <log2N> [seed]  -> per-toggle rate
 *   full16 <bits> <class|unif> <len> <numsecrets> [seed]       -> enumerate 2^16
 *   combo  <bits> <class|unif> <len> <ntoggles> <log2N> [seed] -> combined rate (p vs p^r)
 */
#define XXH_INLINE_ALL
#include "xxhash.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <math.h>
#include <pthread.h>

static inline uint64_t splitmix64(uint64_t *s){
    uint64_t z=(*s+=0x9E3779B97F4A7C15ULL);
    z=(z^(z>>30))*0xBF58476D1CE4E5B9ULL; z=(z^(z>>27))*0x94D049BB133111EBULL; return z^(z>>31);
}
typedef struct{uint64_t s[4];}xosh;
static inline uint64_t rol(uint64_t x,int k){return (x<<k)|(x>>(64-k));}
static inline uint64_t xn(xosh*r){uint64_t*s=r->s;uint64_t v=rol(s[1]*5,7)*9;uint64_t t=s[1]<<17;
 s[2]^=s[0];s[3]^=s[1];s[1]^=s[2];s[0]^=s[3];s[2]^=t;s[3]=rol(s[3],45);return v;}
static void xs(xosh*r,uint64_t sd){uint64_t m=sd;for(int i=0;i<4;i++)r->s[i]=splitmix64(&m);}

#define D_TOGGLE 0x000000007fffffffULL
#define MASK64 0xFFFFFFFFFFFFFFFFULL

/* toggle positions for (block b, lane-pair t, secret index j): fills 2 byte offsets */
static void tog_pos(int b,int t,int j,size_t*off1,size_t*off2){
    int a=2*t;
    int s1=b*16+(j-a),   l1=a;
    int s2=b*16+(j-a-1), l2=a+1;
    *off1=(size_t)s1*64 + (size_t)l1*8;
    *off2=(size_t)s2*64 + (size_t)l2*8;
}
static void wput(uint8_t*m,size_t off,uint64_t v){memcpy(m+off,&v,8);}
static uint64_t wget(const uint8_t*m,size_t off){uint64_t v;memcpy(&v,m+off,8);return v;}

/* make a class secret: force K[j]=LE64(secret+8j) to satisfy the condition */
static void make_class_secret(uint8_t*sec,int j,xosh*r){
    for(int i=0;i<192;i+=8){uint64_t w=xn(r);memcpy(sec+i,&w,8);}
    uint32_t lo,hi; uint64_t u;
    do{ lo=(uint32_t)xn(r); u=(uint64_t)(lo^0x7fffffffu); }while(u>0xfffffffeULL);
    hi=(uint32_t)(0xfffffffeULL-u);
    uint64_t K=((uint64_t)hi<<32)|lo;
    memcpy(sec+8*j,&K,8);
}
static void make_unif_secret(uint8_t*sec,xosh*r){
    for(int i=0;i<192;i+=8){uint64_t w=xn(r);memcpy(sec+i,&w,8);}
}

static int is128g;
static int heq(const uint8_t*m,size_t len,const uint8_t*sec,const uint8_t*m2){
    if(is128g){
        XXH128_hash_t a=XXH3_128bits_withSecret(m,len,sec,192);
        XXH128_hash_t b=XXH3_128bits_withSecret(m2,len,sec,192);
        return a.low64==b.low64 && a.high64==b.high64;
    } else {
        return XXH3_64bits_withSecret(m,len,sec,192)==XXH3_64bits_withSecret(m2,len,sec,192);
    }
}

typedef struct{uint64_t n,seed,hits;const uint8_t*base;const uint8_t*var;size_t len;const uint8_t*secbase;int j;}kj;
static void*krun(void*a){kj*p=(kj*)a;xosh r;xs(&r,p->seed);uint8_t sec[192];memcpy(sec,p->secbase,192);
    uint64_t h=0; for(uint64_t i=0;i<p->n;i++){uint64_t w=xn(&r);memcpy(sec+8*p->j,&w,8);
        if(is128g){XXH128_hash_t A=XXH3_128bits_withSecret(p->base,p->len,sec,192),B=XXH3_128bits_withSecret(p->var,p->len,sec,192);
            if(A.low64==B.low64&&A.high64==B.high64)h++;}
        else{if(XXH3_64bits_withSecret(p->base,p->len,sec,192)==XXH3_64bits_withSecret(p->var,p->len,sec,192))h++;}}
    p->hits=h;return NULL;}

int main(int argc,char**argv){
    /* selfcheck */
    XXH64_hash_t e=XXH3_64bits("",0);
    if(e!=0x2d06800538d394c2ULL){fprintf(stderr,"XXH3 vector mismatch\n");return 2;}
    if(argc<2){fprintf(stderr,"need test\n");return 1;}
    const char*test=argv[1];
    if(!strcmp(test,"selfcheck")){printf("XXH3_64(\"\")=%016llx OK\n",(unsigned long long)e);return 0;}

    if(!strcmp(test,"single")){
        is128g = atoi(argv[2])>=100;
        int useclass = !strcmp(argv[3],"class");
        size_t len=strtoull(argv[4],0,0);
        int j=atoi(argv[5]);
        int log2N=atoi(argv[6]);
        uint64_t seed=argc>7?strtoull(argv[7],0,0):0x1234;
        /* how many blocks -> how many toggle groups fit */
        size_t nb_blocks=(len-1)/1024;
        int tpb = (j-1)/2 + 1; /* toggles per block for this j (t=0..tpb-1) */
        /* build base message (FIXED before key) */
        uint8_t*base=malloc(len); uint8_t*var=malloc(len);
        xosh mr; xs(&mr,0xBA5E10C0DEULL);
        for(size_t i=0;i<len;i++)base[i]=(uint8_t)(xn(&mr)>>((i&7)*8));
        /* place D at every toggle position; cover full blocks, and block 0 as a
           partial block when there are none (short messages, e.g. 256 B / j=1) */
        size_t nplace = nb_blocks?nb_blocks:1;
        for(size_t b=0;b<nplace;b++) for(int t=0;t<tpb;t++){
            size_t o1,o2; tog_pos(b,t,j,&o1,&o2);
            if(o2+8<=len){wput(base,o1,D_TOGGLE);wput(base,o2,D_TOGGLE);}
        }
        uint64_t N=1ULL<<log2N;
        uint64_t hits=0; uint64_t trials=0;
        xosh sr; xs(&sr,seed);
        uint8_t sec[192];
        for(uint64_t it=0;it<N;it++){
            if(useclass) make_class_secret(sec,j,&sr); else make_unif_secret(sec,&sr);
            /* pick one random toggle group (block, t) each trial */
            size_t b = nb_blocks? (xn(&sr)%nb_blocks):0; (void)b;
            int t = (int)(xn(&sr)%tpb);
            size_t o1,o2; tog_pos(b,t,j,&o1,&o2);
            if(o2+8>len) continue;
            memcpy(var,base,len);
            wput(var,o1, wget(base,o1)^MASK64);
            wput(var,o2, wget(base,o2)^MASK64);
            if(heq(base,len,sec,var)) hits++;
            trials++;
        }
        double rate=(double)hits/(double)trials;
        printf("single bits=%s %s len=%zu j=%d nb_blocks=%zu tpb=%d N=%llu hits=%llu rate=%.4e = 2^-%.3f\n",
            is128g?"128":"64", useclass?"class":"unif", len,j,nb_blocks,tpb,
            (unsigned long long)trials,(unsigned long long)hits,rate, rate>0?-log2(rate):999.0);
        return 0;
    }

    if(!strcmp(test,"full16")){
        is128g=atoi(argv[2])>=100;
        int useclass=!strcmp(argv[3],"class");
        size_t len=strtoull(argv[4],0,0);
        int nsec=atoi(argv[5]);
        uint64_t seed=argc>6?strtoull(argv[6],0,0):0x77;
        int j=7;
        size_t nb_blocks=(len-1)/1024;
        /* need 16 groups: 4 full blocks x 4 toggles */
        if(nb_blocks<4){fprintf(stderr,"need >=4 full blocks (len>=5121)\n");return 3;}
        uint8_t*base=malloc(len);
        xosh mr; xs(&mr,0xBA5E10C0DEULL);
        for(size_t i=0;i<len;i++)base[i]=(uint8_t)(xn(&mr)>>((i&7)*8));
        /* 16 groups: block 0..3, t 0..3 */
        size_t g1[16],g2[16]; int ng=0;
        for(int b=0;b<4;b++)for(int t=0;t<4;t++){size_t o1,o2;tog_pos(b,t,j,&o1,&o2);g1[ng]=o1;g2[ng]=o2;
            wput(base,o1,D_TOGGLE);wput(base,o2,D_TOGGLE);ng++;}
        uint8_t*var=malloc(len);
        xosh sr; xs(&sr,seed);
        uint8_t sec[192];
        int allpass=1;
        for(int si=0;si<nsec;si++){
            if(useclass)make_class_secret(sec,j,&sr);else make_unif_secret(sec,&sr);
            /* enumerate all 2^16 subsets */
            uint64_t bad=0;
            /* base hash */
            for(uint32_t sub=0;sub<(1u<<16);sub++){
                memcpy(var,base,len);
                for(int g=0;g<16;g++) if(sub&(1u<<g)){
                    wput(var,g1[g],wget(base,g1[g])^MASK64);
                    wput(var,g2[g],wget(base,g2[g])^MASK64);
                }
                if(!heq(base,len,sec,var)) bad++;
            }
            if(bad!=0) allpass=0;
            if(si<4||bad!=0) printf("  secret %d: %u/65536 collide with base (bad=%llu)\n",
                si,65536-(unsigned)bad,(unsigned long long)bad);
        }
        printf("full16 bits=%s %s len=%zu nsec=%d : %s\n",is128g?"128":"64",
            useclass?"class":"unif",len,nsec, allpass?"ALL 65536 collide for every secret":"NOT all");
        return 0;
    }

    if(!strcmp(test,"combo")){
        is128g=atoi(argv[2])>=100;
        int useclass=!strcmp(argv[3],"class");
        size_t len=strtoull(argv[4],0,0);
        int ntog=atoi(argv[5]);   /* how many toggles active simultaneously */
        int log2N=atoi(argv[6]);
        uint64_t seed=argc>7?strtoull(argv[7],0,0):0x9;
        int j=7; size_t nb_blocks=(len-1)/1024;
        int tpb=(j-1)/2+1;
        int maxg=(int)nb_blocks*tpb;
        if(ntog>maxg){fprintf(stderr,"only %d groups fit\n",maxg);return 3;}
        uint8_t*base=malloc(len),*var=malloc(len);
        xosh mr; xs(&mr,0xBA5E10C0DEULL);
        for(size_t i=0;i<len;i++)base[i]=(uint8_t)(xn(&mr)>>((i&7)*8));
        size_t g1[64],g2[64];int ng=0;
        for(size_t b=0;b<nb_blocks&&ng<ntog;b++)for(int t=0;t<tpb&&ng<ntog;t++){
            size_t o1,o2;tog_pos(b,t,j,&o1,&o2);g1[ng]=o1;g2[ng]=o2;
            wput(base,o1,D_TOGGLE);wput(base,o2,D_TOGGLE);ng++;}
        memcpy(var,base,len);
        for(int g=0;g<ng;g++){wput(var,g1[g],wget(base,g1[g])^MASK64);wput(var,g2[g],wget(base,g2[g])^MASK64);}
        uint64_t N=1ULL<<log2N,hits=0; xosh sr;xs(&sr,seed);uint8_t sec[192];
        for(uint64_t it=0;it<N;it++){
            if(useclass)make_class_secret(sec,j,&sr);else make_unif_secret(sec,&sr);
            if(heq(base,len,sec,var))hits++;
        }
        double rate=(double)hits/(double)N;
        printf("combo bits=%s %s len=%zu ntog=%d N=2^%d hits=%llu rate=%.4e = 2^-%.3f\n",
            is128g?"128":"64",useclass?"class":"unif",len,ntog,log2N,
            (unsigned long long)hits,rate,rate>0?-log2(rate):999.0);
        return 0;
    }
    /* krate: multithreaded measurement of the per-toggle collision rate over a
       UNIFORM secret (varying only K[j]=secret[8j..8j+8], which is the only word
       the collision depends on).  ntog toggles active at once -> tests p vs p^r. */
    if(!strcmp(test,"krate")){
        is128g=atoi(argv[2])>=100;
        size_t len=strtoull(argv[3],0,0);
        int ntog=atoi(argv[4]);
        int log2N=atoi(argv[5]);
        int nth=argc>6?atoi(argv[6]):8;
        uint64_t seed0=argc>7?strtoull(argv[7],0,0):0xAA;
        int j=7; size_t nb_blocks=(len-1)/1024; int tpb=(j-1)/2+1;
        int maxg=(int)(nb_blocks?nb_blocks:1)*tpb;
        if(ntog>maxg){fprintf(stderr,"only %d groups fit\n",maxg);return 3;}
        static uint8_t base[1<<21], var[1<<21];
        if(len>(1<<21)){fprintf(stderr,"len too big\n");return 3;}
        xosh mr; xs(&mr,0xBA5E10C0DEULL);
        for(size_t i=0;i<len;i++)base[i]=(uint8_t)(xn(&mr)>>((i&7)*8));
        size_t g1[64],g2[64];int ng=0; size_t nplace=nb_blocks?nb_blocks:1;
        for(size_t b=0;b<nplace&&ng<ntog;b++)for(int t=0;t<tpb&&ng<ntog;t++){
            size_t o1,o2;tog_pos(b,t,j,&o1,&o2);g1[ng]=o1;g2[ng]=o2;
            wput(base,o1,D_TOGGLE);wput(base,o2,D_TOGGLE);ng++;}
        memcpy(var,base,len);
        for(int g=0;g<ng;g++){wput(var,g1[g],wget(base,g1[g])^MASK64);wput(var,g2[g],wget(base,g2[g])^MASK64);}
        /* a fixed base random secret; threads only vary K[j] */
        static uint8_t secbase[192]; xosh sb; xs(&sb,0xF00D); for(int i=0;i<192;i+=8){uint64_t w=xn(&sb);memcpy(secbase+i,&w,8);}
        uint64_t N=1ULL<<log2N,per=N/nth; pthread_t th[64];kj js[64];
        for(int i=0;i<nth;i++){js[i]=(kj){per,seed0+i*0x9E37+1,0,base,var,len,secbase,j};}
        for(int i=0;i<nth;i++)pthread_create(&th[i],0,krun,&js[i]);
        uint64_t hits=0;for(int i=0;i<nth;i++){pthread_join(th[i],0);hits+=js[i].hits;}
        uint64_t tot=per*(uint64_t)nth; double rate=(double)hits/(double)tot;
        printf("krate bits=%s len=%zu ntog=%d N=%llu hits=%llu rate=%.6e = 2^-%.3f\n",
            is128g?"128":"64",len,ntog,(unsigned long long)tot,(unsigned long long)hits,rate,rate>0?-log2(rate):999.0);
        return 0;
    }
    fprintf(stderr,"unknown test\n");return 1;
}
