// modebeam: heaviest value of the a5hash-64 initial state word S1_init for a given (len, v = v2(W*)),
// found by an EXACT histogram of the low k0 bits of the residual product followed by a beam
// extension over the remaining residual bits; then the dead-first-operand (len >= 17) or
// dead-tail-operand (len <= 16) pair is verified on the enumerated seed class.
//
//   X = K2^len ^ (seed & AA..), Y = K1^len ^ (seed & 55..),  S1_init = lo64(X*Y)
//   v2(X)=p, v2(Y)=q, v=p+q, m=64-v:  S1_init = 2^v * (X''*Y'' mod 2^m), X''=X>>p, Y''=Y>>q odd.
//   Every seed bit that matters (cost + free bits of X'' and Y'' below m) is enumerated; the class of
//   W* is exact; Pr[S1_init = W*] = weight / 2^S with S = max_k (cost_k + fx_k + fy_k) over feasible (p,q).
//
// usage: modebeam LEN V [-k K0] [-b BEAM] [-t THREADS] [-z ZBITS] [-x MAXVERIFY_LOG2] [-o pairfile]
//   -z ZBITS : residual bits >= ZBITS forced to 0 (W* < 2^(v+ZBITS)); needed for len <= 3 (A < 2^(8*len))
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include <stdint.h>
#include "a5.h"

static uint64_t rng(uint64_t *s){ uint64_t z=(*s+=0x9E3779B97F4A7C15ull); z=(z^(z>>30))*0xBF58476D1CE4E5B9ull; z=(z^(z>>27))*0x94D049BB133111EBull; return z^(z>>31); }
static int bit(uint64_t x,int i){ return (int)((x>>i)&1); }

typedef struct { int p,q,cost,fx,fy; int xpos[64], ypos[64]; uint64_t xfix, yfix; uint64_t seedfix_mask, seedfix_val; int e; } pq_t;
static int build(uint64_t len, int p, int q, int m, pq_t *o){
    uint64_t K2l=A5_K2^len, K1l=A5_K1^len;
    memset(o,0,sizeof *o); o->p=p; o->q=q;
    for (int i=0;i<p;i++){ if (i&1){ o->cost++; o->seedfix_mask|=1ull<<i; if (bit(K2l,i)) o->seedfix_val|=1ull<<i; }
                            else if (bit(K2l,i)) return 0; }
    if (p&1){ o->cost++; o->seedfix_mask|=1ull<<p; if (!bit(K2l,p)) o->seedfix_val|=1ull<<p; } else if (!bit(K2l,p)) return 0;
    for (int j=0;j<q;j++){ if (!(j&1)){ o->cost++; o->seedfix_mask|=1ull<<j; if (bit(K1l,j)) o->seedfix_val|=1ull<<j; }
                            else if (bit(K1l,j)) return 0; }
    if (!(q&1)){ o->cost++; o->seedfix_mask|=1ull<<q; if (!bit(K1l,q)) o->seedfix_val|=1ull<<q; } else if (!bit(K1l,q)) return 0;
    o->xfix=1; o->yfix=1;
    for (int t=1;t<m;t++){ int i=p+t; if (i>=64) break; if (i&1) o->xpos[o->fx++]=t; else if (bit(K2l,i)) o->xfix|=1ull<<t; }
    for (int t=1;t<m;t++){ int j=q+t; if (j>=64) break; if (!(j&1)) o->ypos[o->fy++]=t; else if (bit(K1l,j)) o->yfix|=1ull<<t; }
    o->e=o->cost+o->fx+o->fy;
    return 1;
}
static int xfree_at(const pq_t *o,int t){ return ((o->p+t)&1)!=0; }
static int yfree_at(const pq_t *o,int t){ return ((o->q+t)&1)==0; }
static int nfree_ge(const pq_t *o,int t0,int m){ int n=0; for(int t=t0;t<m;t++){ n+=xfree_at(o,t)+yfree_at(o,t);} return n; }

// ---------------- globals ----------------
static uint64_t LEN; static int V, M, K0, BEAM=65536, NT=8, ZBITS=-1, MAXVER=28; static const char *PAIRFILE=0;
static pq_t PQ[65]; static int NPQ=0; static int S;
static uint16_t *HIST; static uint64_t HMASK;
static uint64_t HW[65];           // histogram weight per pair
static uint64_t XV[65][1<<18], YV[65][1<<18]; static int NX[65], NY[65]; // enumerated low-k0 values
typedef struct { uint64_t X, Y; uint8_t k; } ent_t;
// nodes
typedef struct { uint64_t W; uint64_t wt; ent_t *e; size_t n, cap; } node_t;
static node_t *NODES; static size_t NNODES;

static void ent_push(node_t *nd, ent_t v){ if(nd->n==nd->cap){ nd->cap=nd->cap?nd->cap*2:8; nd->e=realloc(nd->e,nd->cap*sizeof(ent_t)); if(!nd->e){fprintf(stderr,"oom\n");exit(9);} } nd->e[nd->n++]=v; }

// ---- phase 1: histogram ----
typedef struct { int k; uint64_t a0,a1; } hjob_t;
static void *hist_thread(void *arg){ hjob_t *j=arg; int k=j->k; uint64_t w=HW[k];
    for (uint64_t ax=j->a0; ax<j->a1; ax++){ uint64_t X=XV[k][ax];
        for (int ay=0; ay<NY[k]; ay++){ uint64_t b=(X*YV[k][ay])&HMASK; __atomic_fetch_add(&HIST[b],(uint16_t)w,__ATOMIC_RELAXED); } }
    return 0; }

// ---- phase 2: collect entries for selected bins ----
typedef struct { uint64_t bin; uint32_t node; } hkey_t;
static hkey_t *HT; static uint64_t HTMASK;
static int ht_find(uint64_t bin, uint32_t *node){ uint64_t h=(bin*0x9E3779B97F4A7C15ull)>>20; for(;;){ h&=HTMASK; if(HT[h].node==0xffffffffu) return 0; if(HT[h].bin==bin){ *node=HT[h].node; return 1;} h++; } }
static void ht_ins(uint64_t bin, uint32_t node){ uint64_t h=(bin*0x9E3779B97F4A7C15ull)>>20; for(;;){ h&=HTMASK; if(HT[h].node==0xffffffffu){ HT[h].bin=bin; HT[h].node=node; return;} h++; } }
typedef struct { int k; uint64_t a0,a1; uint16_t thr; ent_t *buf; uint32_t *nid; size_t n, cap; } cjob_t;
static void *collect_thread(void *arg){ cjob_t *j=arg; int k=j->k;
    for (uint64_t ax=j->a0; ax<j->a1; ax++){ uint64_t X=XV[k][ax];
        for (int ay=0; ay<NY[k]; ay++){ uint64_t Y=YV[k][ay]; uint64_t b=(X*Y)&HMASK; if (HIST[b]<j->thr) continue; uint32_t nd; if(!ht_find(b,&nd)) continue;
            if(j->n==j->cap){ j->cap=j->cap?j->cap*2:1024; j->buf=realloc(j->buf,j->cap*sizeof(ent_t)); j->nid=realloc(j->nid,j->cap*sizeof(uint32_t)); }
            j->buf[j->n]=(ent_t){X,Y,(uint8_t)k}; j->nid[j->n]=nd; j->n++; } }
    return 0; }

// ---- phase 3: beam extension ----
typedef struct { size_t n0,n1; int t; node_t *out; } bjob_t;  // out has 2 slots per node: children 0/1
static void *beam_thread(void *arg){ bjob_t *j=arg; int t=j->t;
    for (size_t i=j->n0;i<j->n1;i++){ node_t *nd=&NODES[i]; node_t *c0=&j->out[2*i], *c1=&j->out[2*i+1];
        c0->W=nd->W; c1->W=nd->W|(1ull<<t); c0->wt=c1->wt=0; c0->n=c1->n=0;
        for (size_t a=0;a<nd->n;a++){ ent_t e=nd->e[a]; const pq_t *o=&PQ[e.k]; int xf=xfree_at(o,t), yf=yfree_at(o,t);
            for (int bx=0;bx<=xf;bx++) for(int by=0;by<=yf;by++){ ent_t c=e; if(bx) c.X|=1ull<<t; if(by) c.Y|=1ull<<t;
                int pb=(int)(((c.X*c.Y)>>t)&1); if (pb) ent_push(c1,c); else ent_push(c0,c); } }
        for (int s=0;s<2;s++){ node_t *c=s?c1:c0; uint64_t wt=0; for(size_t a=0;a<c->n;a++){ const pq_t *o=&PQ[c->e[a].k]; wt+= (1ull<<(S-o->e)) << nfree_ge(o,t+1,M); } c->wt=wt; }
    }
    return 0; }
static int cmp_wt_desc(const void *a,const void *b){ uint64_t x=*(const uint64_t*)a, y=*(const uint64_t*)b; return x<y?1:(x>y?-1:0); }

// ---- messages ----
static void make_pair(uint64_t W, uint8_t *m1, uint8_t *m2){
    for (uint64_t i=0;i<LEN;i++) m1[i]=(uint8_t)(i*7+3);
    if (LEN>16){ uint32_t hi=(uint32_t)(W>>32), lo=(uint32_t)W; memcpy(m1,&hi,4); memcpy(m1+4,&lo,4);
        memcpy(m2,m1,LEN); uint64_t w1=0x1122334455667788ull, w1b=0x8877665544332211ull; memcpy(m1+8,&w1,8); memcpy(m2+8,&w1b,8); }
    else if (LEN>=8){ uint32_t hi=(uint32_t)(W>>32), lo=(uint32_t)W; memcpy(m1,&hi,4); memcpy(m1+LEN-4,&lo,4); memcpy(m2,m1,LEN); m2[LEN-4]^=1; }
    else if (LEN<=3){ for (uint64_t i=0;i<LEN;i++) m1[i]=(uint8_t)(W>>(8*i)); memcpy(m2,m1,LEN); m2[0]^=1; }
    else { fprintf(stderr,"len 4..7: no valid pair construction (both tail operands are the same word)\n"); exit(5); }
}
// seed from an entry: fixed cost bits, enumerated X''/Y'' bits, random elsewhere
static void entry_seed_mask(const ent_t *e, uint64_t *fmask, uint64_t *fval){ const pq_t *o=&PQ[e->k]; uint64_t K2l=A5_K2^LEN, K1l=A5_K1^LEN;
    uint64_t fm=o->seedfix_mask, fv=o->seedfix_val;
    for (int t=1;t<M;t++){ if (xfree_at(o,t)){ int i=o->p+t; fm|=1ull<<i; if (bit(e->X,t)^bit(K2l,i)) fv|=1ull<<i; }
                           if (yfree_at(o,t)){ int j=o->q+t; fm|=1ull<<j; if (bit(e->Y,t)^bit(K1l,j)) fv|=1ull<<j; } }
    *fmask=fm; *fval=fv; }
typedef struct { node_t *nd; size_t a0,a1; uint64_t W; const uint8_t *m1,*m2; uint64_t nseed,ncoll,bad,exseed; int exhaustive; uint64_t per; uint64_t rs; } vjob_t;
static void *verify_thread(void *arg){ vjob_t *j=arg;
    for (size_t a=j->a0;a<j->a1;a++){ uint64_t fm,fv; entry_seed_mask(&j->nd->e[a],&fm,&fv); uint64_t dc=~fm; int ndc=__builtin_popcountll(dc);
        uint64_t reps = j->exhaustive ? (1ull<<ndc) : j->per;
        for (uint64_t r=0;r<reps;r++){ uint64_t seed;
            if (j->exhaustive){ uint64_t x=r, s=0; uint64_t d=dc; while(d){ int b=__builtin_ctzll(d); d&=d-1; if(x&1) s|=1ull<<b; x>>=1; } seed=fv|s; }
            else seed=fv|(rng(&j->rs)&dc);
            uint64_t s1,s2; a5_init(LEN,seed,&s1,&s2); if (s1!=j->W){ j->bad++; continue; }
            j->nseed++; if (a5hash64(j->m1,LEN,seed)==a5hash64(j->m2,LEN,seed)){ j->ncoll++; if(!j->exseed) j->exseed=seed; } } }
    return 0; }

int main(int argc, char **argv){
    if (argc<3){ fprintf(stderr,"usage: modebeam LEN V [-k K0] [-b BEAM] [-t THREADS] [-z ZBITS] [-x MAXVERIFY_LOG2] [-o pairfile]\n"); return 2; }
    LEN=strtoull(argv[1],0,0); V=atoi(argv[2]); M=64-V; K0=M<33?M:33;
    for (int i=3;i<argc;i++){ if(!strcmp(argv[i],"-k")) K0=atoi(argv[++i]); else if(!strcmp(argv[i],"-b")) BEAM=atoi(argv[++i]); else if(!strcmp(argv[i],"-t")) NT=atoi(argv[++i]);
        else if(!strcmp(argv[i],"-z")) ZBITS=atoi(argv[++i]); else if(!strcmp(argv[i],"-x")) MAXVER=atoi(argv[++i]); else if(!strcmp(argv[i],"-o")) PAIRFILE=argv[++i]; }
    if (K0>M) K0=M; if (K0>33){ fprintf(stderr,"K0 > 33 not supported\n"); return 2; }
    if (LEN<=3 && ZBITS<0){ ZBITS=(int)(8*LEN)-V; if (ZBITS<=0){ printf("len %llu v %d: W* < 2^%llu impossible\n",(unsigned long long)LEN,V,8ull*LEN); return 0; } }
    for (int p=0;p<=V;p++){ int q=V-p; if (p>63||q>63) continue; if (build(LEN,p,q,M,&PQ[NPQ])) NPQ++; }
    printf("len=%llu v=%d m=%d k0=%d beam=%d zbits=%d feasible (p,q) pairs: %d\n",(unsigned long long)LEN,V,M,K0,BEAM,ZBITS,NPQ);
    if (!NPQ) return 0;
    S=0; for (int k=0;k<NPQ;k++) if (PQ[k].e>S) S=PQ[k].e;
    // enumerate low-k0 values per pair
    int minexp=1<<30;
    for (int k=0;k<NPQ;k++){ pq_t *o=&PQ[k]; int fx0=0,fy0=0; for(int t=0;t<o->fx;t++) if(o->xpos[t]<K0) fx0++; for(int t=0;t<o->fy;t++) if(o->ypos[t]<K0) fy0++;
        if (fx0>18||fy0>18){ fprintf(stderr,"too many free bits below k0\n"); return 3; }
        NX[k]=1<<fx0; NY[k]=1<<fy0;
        for (int ax=0;ax<NX[k];ax++){ uint64_t X=o->xfix; for(int t=0;t<fx0;t++) if(ax>>t&1) X|=1ull<<o->xpos[t]; XV[k][ax]=X; }
        for (int ay=0;ay<NY[k];ay++){ uint64_t Y=o->yfix; for(int t=0;t<fy0;t++) if(ay>>t&1) Y|=1ull<<o->ypos[t]; YV[k][ay]=Y; }
        int ex=(S-o->e)+nfree_ge(o,K0,M); if (ex<minexp) minexp=ex;
        printf("  (p,q)=(%d,%d) cost=%d free=%d+%d e=%d (below k0: %d+%d)  weight 2^-%d per full assignment\n",o->p,o->q,o->cost,o->fx,o->fy,o->e,fx0,fy0,o->e); }
    for (int k=0;k<NPQ;k++){ int ex=(S-PQ[k].e)+nfree_ge(&PQ[k],K0,M)-minexp; HW[k]=1ull<<ex; }
    printf("S=%d determined seed bits; Pr[S1_init=W*] = weight/2^%d\n",S,S);
    // phase 1
    HMASK=(K0==64)?~0ull:((1ull<<K0)-1);
    HIST=calloc(1ull<<K0,sizeof(uint16_t)); if(!HIST){ fprintf(stderr,"alloc fail (2^%d x 2 bytes)\n",K0); return 4; }
    pthread_t th[64];
    for (int k=0;k<NPQ;k++){ hjob_t hj[64]; for(int i=0;i<NT;i++){ hj[i].k=k; hj[i].a0=(uint64_t)NX[k]*i/NT; hj[i].a1=(uint64_t)NX[k]*(i+1)/NT; pthread_create(&th[i],0,hist_thread,&hj[i]); } for(int i=0;i<NT;i++) pthread_join(th[i],0); }
    // frequency of counts, threshold
    static uint64_t freq[65536]; memset(freq,0,sizeof freq); uint64_t nb=1ull<<K0; uint64_t zmask = (ZBITS>=0 && ZBITS<K0) ? ~((1ull<<ZBITS)-1) : 0;
    uint64_t bestbin=0; uint16_t bw=0;
    for (uint64_t i=0;i<nb;i++){ if (zmask && (i&zmask)) continue; freq[HIST[i]]++; if (HIST[i]>bw){ bw=HIST[i]; bestbin=i; } }
    if (bw==65535) printf("WARNING: uint16 histogram saturated\n");
    printf("level %d histogram: max weight %u at residual 0x%llx (top-%d weight tail:", K0, bw, (unsigned long long)bestbin, BEAM);
    { uint64_t acc=0; int thr=65535; for (int c=65535;c>=1;c--){ acc+=freq[c]; if (acc>=(uint64_t)BEAM){ thr=c; break; } if(c==1) thr=1; }
      printf(" threshold %d, %llu bins >= it)\n",thr,(unsigned long long)acc);
      if (K0==M){ // exact answer at this level
          double pr=(double)bw/pow(2.0,S); /* K0==M: HW[k] = 2^(S-e_k) exactly */
          uint64_t W=bestbin<<V; uint64_t cnt=0; for(uint64_t i=0;i<nb;i++) if(HIST[i]==bw && !(zmask&&(i&zmask))) cnt++;
          int c2=0; for(int c=bw-1;c>=1;c--) if(freq[c]){c2=c;break;}
          printf("EXACT mode: W* = 0x%016llx  weight %u  Pr[S1_init = W*] = 2^%.3f  (bins at max: %llu, second-best weight %d = 2^%.3f)\n",(unsigned long long)W,bw,log2(pr),(unsigned long long)cnt,c2,log2((double)c2/pow(2.0,S)));
          double L=ceil((double)LEN/8.0); if(L<1)L=1; printf("score log2(L/eps) with L=%g words = %.2f bits\n",L,log2(L)-log2(pr));
          // build the node for verification
          NODES=calloc(1,sizeof(node_t)); NNODES=1; NODES[0].W=bestbin;
          for (int k=0;k<NPQ;k++) for(int ax=0;ax<NX[k];ax++) for(int ay=0;ay<NY[k];ay++) if(((XV[k][ax]*YV[k][ay])&HMASK)==bestbin) ent_push(&NODES[0],(ent_t){XV[k][ax],YV[k][ay],(uint8_t)k});
          NODES[0].wt=0; for(size_t a=0;a<NODES[0].n;a++) NODES[0].wt+=1ull<<(S-PQ[NODES[0].e[a].k].e);
          goto verify;
      }
      // select bins >= thr (cap at 4*BEAM)
      size_t cap=(size_t)4*BEAM; NODES=calloc(cap,sizeof(node_t)); NNODES=0;
      for (uint64_t i=0;i<nb && NNODES<cap;i++){ if (zmask && (i&zmask)) continue; if (HIST[i]>=thr && (HIST[i]>thr || NNODES<(size_t)BEAM)){ NODES[NNODES].W=i; NNODES++; } }
      uint64_t hs=1; while (hs<4*NNODES) hs<<=1; HTMASK=hs-1; HT=malloc(hs*sizeof(hkey_t)); for(uint64_t i=0;i<hs;i++) HT[i].node=0xffffffffu;
      for (size_t i=0;i<NNODES;i++) ht_ins(NODES[i].W,(uint32_t)i);
      printf("selected %zu level-%d bins for extension\n",NNODES,K0);
      // phase 2: collect
      cjob_t cj[64]; memset(cj,0,sizeof cj);
      for (int k=0;k<NPQ;k++){ for(int i=0;i<NT;i++){ cj[i].k=k; cj[i].a0=(uint64_t)NX[k]*i/NT; cj[i].a1=(uint64_t)NX[k]*(i+1)/NT; cj[i].thr=(uint16_t)thr; pthread_create(&th[i],0,collect_thread,&cj[i]); }
          for(int i=0;i<NT;i++){ pthread_join(th[i],0); for(size_t a=0;a<cj[i].n;a++) ent_push(&NODES[cj[i].nid[a]],cj[i].buf[a]); cj[i].n=0; } }
      for (int i=0;i<NT;i++){ free(cj[i].buf); free(cj[i].nid); }
      free(HIST); HIST=0; free(HT); HT=0;
      for (size_t i=0;i<NNODES;i++){ uint64_t wt=0; for(size_t a=0;a<NODES[i].n;a++){ const pq_t *o=&PQ[NODES[i].e[a].k]; wt+=(1ull<<(S-o->e))<<nfree_ge(o,K0,M);} NODES[i].wt=wt; }
    }
    // phase 3: beam levels K0..M-1
    for (int t=K0;t<M;t++){
        node_t *out=calloc(2*NNODES,sizeof(node_t)); bjob_t bj[64];
        for (int i=0;i<NT;i++){ bj[i].n0=NNODES*i/NT; bj[i].n1=NNODES*(i+1)/NT; bj[i].t=t; bj[i].out=out; pthread_create(&th[i],0,beam_thread,&bj[i]); }
        for (int i=0;i<NT;i++) pthread_join(th[i],0);
        for (size_t i=0;i<NNODES;i++) free(NODES[i].e);
        free(NODES);
        size_t nc=2*NNODES; if (ZBITS>=0 && t>=ZBITS){ for(size_t i=0;i<nc;i+=2){ out[i+1].wt=0; } }
        uint64_t *wts=malloc(nc*sizeof(uint64_t)); for(size_t i=0;i<nc;i++) wts[i]=out[i].wt; qsort(wts,nc,sizeof(uint64_t),cmp_wt_desc);
        uint64_t thr = nc>(size_t)BEAM ? wts[BEAM-1] : 0; if (thr==0) thr=1; free(wts);
        NODES=calloc(nc,sizeof(node_t)); NNODES=0; size_t kept_eq=0;
        for (size_t i=0;i<nc;i++){ if (out[i].wt>thr || (out[i].wt==thr && NNODES<(size_t)BEAM)){ NODES[NNODES++]=out[i]; if(out[i].wt==thr) kept_eq++; } else free(out[i].e); }
        free(out);
        uint64_t best=0; size_t bi=0; for(size_t i=0;i<NNODES;i++) if(NODES[i].wt>best){ best=NODES[i].wt; bi=i; }
        printf("level %2d: %zu nodes, best expected weight 2^%.3f (W prefix 0x%llx, %zu assignments)\n",t+1,NNODES,log2((double)best),(unsigned long long)NODES[bi].W,NODES[bi].n);
        if (!NNODES){ printf("beam empty\n"); return 0; }
    }
    { // final: pick best by exact weight
        size_t bi=0; uint64_t best=0; for(size_t i=0;i<NNODES;i++){ uint64_t wt=0; for(size_t a=0;a<NODES[i].n;a++) wt+=1ull<<(S-PQ[NODES[i].e[a].k].e); NODES[i].wt=wt; if(wt>best){best=wt;bi=i;} }
        node_t tmp=NODES[bi]; NODES[bi]=NODES[0]; NODES[0]=tmp;
        uint64_t second=0; for(size_t i=1;i<NNODES;i++) if(NODES[i].wt>second) second=NODES[i].wt;
        double pr=(double)best/pow(2.0,S); uint64_t W=NODES[0].W<<V;
        printf("BEAM mode: W* = 0x%016llx  weight %llu (%zu assignments)  Pr[S1_init = W*] = 2^%.3f  (second-best in beam %llu = 2^%.3f)\n",(unsigned long long)W,(unsigned long long)best,NODES[0].n,log2(pr),(unsigned long long)second,log2((double)second/pow(2.0,S)));
        double L=ceil((double)LEN/8.0); if(L<1)L=1; printf("score log2(L/eps) with L=%g words = %.2f bits\n",L,log2(L)-log2(pr));
    }
verify:
    {   node_t *nd=&NODES[0]; uint64_t W=nd->W<<V; uint64_t clsz=nd->wt<<(64-S);
        printf("class size = weight * 2^(64-%d) = %llu = 2^%.2f seeds\n",S,(unsigned long long)clsz,log2((double)clsz));
        if (LEN>=4 && LEN<=7){ printf("len 4..7: both tail operands are the same message word, no second message collides on this class; mode reported only\n"); return 0; }
        uint8_t *m1=calloc(LEN+16,1), *m2=calloc(LEN+16,1); make_pair(W,m1,m2);
        int exhaustive = clsz <= (1ull<<MAXVER); uint64_t per = exhaustive?0:((1ull<<MAXVER)/nd->n+1);
        vjob_t vj[64]; memset(vj,0,sizeof vj);
        for (int i=0;i<NT;i++){ vj[i].nd=nd; vj[i].a0=nd->n*i/NT; vj[i].a1=nd->n*(i+1)/NT; vj[i].W=W; vj[i].m1=m1; vj[i].m2=m2; vj[i].exhaustive=exhaustive; vj[i].per=per; vj[i].rs=0xC0FFEE00ull+i; pthread_create(&th[i],0,verify_thread,&vj[i]); }
        uint64_t nseed=0,ncoll=0,bad=0,ex=0; for(int i=0;i<NT;i++){ pthread_join(th[i],0); nseed+=vj[i].nseed; ncoll+=vj[i].ncoll; bad+=vj[i].bad; if(!ex) ex=vj[i].exseed; }
        printf("%s verification: class seeds tested %llu, full-hash collisions %llu, S1_init mismatches %llu (must be 0); example seed 0x%016llx\n", exhaustive?"EXHAUSTIVE":"SAMPLED", (unsigned long long)nseed,(unsigned long long)ncoll,(unsigned long long)bad,(unsigned long long)ex);
        if (ex){ printf("  h(m1)=%016llx h(m2)=%016llx\n",(unsigned long long)a5hash64(m1,LEN,ex),(unsigned long long)a5hash64(m2,LEN,ex)); }
        uint64_t rs=7; long ctrl=1L<<24; if (LEN>256) ctrl=(long)((1ull<<32)/(LEN+1)); if(ctrl<4096) ctrl=4096; uint64_t cs=0;
        for (long i=0;i<ctrl;i++){ uint64_t seed=rng(&rs); if (a5hash64(m1,LEN,seed)==a5hash64(m2,LEN,seed)) cs++; }
        printf("control: %llu collisions in %ld uniform random seeds (expected %.2e from this class)\n",(unsigned long long)cs,ctrl,(double)ctrl*(double)clsz/18446744073709551616.0);
        int show=LEN<24?(int)LEN:24; printf("pair: m1[0..%d) = ",show); for(int i=0;i<show;i++) printf("%02x",m1[i]); printf("  m2[0..%d) = ",show); for(int i=0;i<show;i++) printf("%02x",m2[i]);
        if (LEN>24) printf("  (rest identical: byte i = (7i+3)&0xff)"); printf("\n");
        if (PAIRFILE){ FILE *f=fopen(PAIRFILE,"w"); if(f){ for(uint64_t i=0;i<LEN;i++) fprintf(f,"%02x",m1[i]); fprintf(f,"\n"); for(uint64_t i=0;i<LEN;i++) fprintf(f,"%02x",m2[i]); fprintf(f,"\n"); fclose(f);} }
    }
    return 0;
}
