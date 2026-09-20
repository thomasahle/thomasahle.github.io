// Exhaustive scaled checks of the new algebra. Integer arithmetic only.
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <set>
#include <vector>
using u64=uint64_t;
unsigned val(unsigned x){return x?__builtin_ctz(x):99;}
u64 clmul(u64 a,u64 b){u64 z=0;while(b){if(b&1)z^=a;b>>=1;a<<=1;}return z;}
unsigned gf_rank(std::vector<u64> v){u64 basis[64]={};unsigned rank=0;for(auto x:v)while(x){unsigned j=63-__builtin_clzll(x);if(basis[j])x^=basis[j];else{basis[j]=x;++rank;break;}}return rank;}
int main(){
 u64 visits=0,points=0,exact_targets=0,dense_checks=0;
 for(unsigned w=4;w<=8;++w){unsigned q=1u<<w;std::vector<std::pair<unsigned,unsigned>> cases;
  for(unsigned d=2;d<q;d+=2)if(val(d)<=3)for(unsigned e=0;e<q;e+=(1u<<val(d)))cases.emplace_back(d,e);
  u64 nv=0,np=0,ne=0,nd=0;
  #pragma omp parallel for schedule(dynamic) reduction(+:nv,np,ne,nd)
  for(size_t ci=0;ci<cases.size();++ci){auto [d,e]=cases[ci];unsigned r=val(d),R=1u<<r;std::vector<unsigned> hist(q);
   // Joint point counts are for literal product pairs, including all wraps.
   std::vector<unsigned> joint(q*q);
   for(unsigned A=0;A<q;++A)for(unsigned B=0;B<q;++B){unsigned Y=A*B&(q-1),Yp=((A+d)&(q-1))*((B+e)&(q-1))&(q-1);++hist[Y^Yp];++joint[Y*q+Yp];++nv;}
   assert(hist[0]==R*q);
   for(unsigned z=1;z<q;++z)if(val(z)==r){assert(hist[z]==R*q);++ne;}
   if(r==1){for(unsigned z=1;z<q;++z)if(val(z)==2){assert(hist[z]<=3*q);++np;}assert(hist[q-8]<=4*q);++nd;}
   if(r==2){assert(hist[q-8]<=6*q);++nd;}
   unsigned b=e/R;
   for(unsigned Y=0;Y<q;++Y)for(unsigned Yp=0;Yp<q;++Yp){unsigned z=Y^Yp,cnt=joint[Y*q+Yp];
    if(val(z)==r){unsigned bound=(b&1)?(Y&1?0:2*R):R;assert(cnt<=bound);++np;}
    if(r==1&&val(z)==2&&!(b&1)){unsigned bound=(b&2)?(Y%4?0:8):(Y%2?0:4);assert(cnt<=bound);++np;}
   }
  }
  visits+=nv;points+=np;exact_targets+=ne;dense_checks+=nd;
 }
 u64 rowchecks=0,rankchecks=0;
 for(unsigned w=4;w<=8;++w){unsigned q=1u<<w,p=(q>>3)-1;if(!p)p=1;std::set<unsigned> maskset;
  for(unsigned x=0;x<q;++x)for(unsigned y=x%p;y<q;y+=p)maskset.insert(x^y);
  for(unsigned t=0;t<=std::min(3u,(w-1)/2);++t){unsigned N=w-t;
   for(unsigned h=1;h<(1u<<(2*t+1));++h){if(val(h)>t)continue;
    int E=99,O=99,HE=-1,HO=-1;
    for(unsigned j=0;j<=2*t;++j)if(h>>j&1){if(j&1){O=std::min(O,int(j));HO=j;}else{E=std::min(E,int(j));HE=j;}}
    for(unsigned S=0;S<(1u<<(w+t));++S){unsigned v=S?val(S):w+t;int deg=S?31-__builtin_clz(S):-1;
     u64 actual[16]={};
     for(unsigned i=0;i<N;++i){u64 out=clmul(S,1u<<i)^clmul(h,u64(1)<<(2*i));for(unsigned n=0;n<2*w;++n)if(out>>n&1)actual[n]|=u64(1)<<i;}
     int lo[8],hi[8];
     for(unsigned n=0;n<w;++n){lo[n]=-1;if(n<N+v){int a=n>=v?int(n-v):-1;int e=n&1?O:E;int b=int(n)>=e?(int(n)-e)/2:-1;if(a!=b)lo[n]=std::max(a,b);}
      if(lo[n]>=0){assert(actual[n]);assert(63-__builtin_clzll(actual[n])==lo[n]);++rowchecks;}
      unsigned m=w+n;hi[n]=-1;
      if(int(m)>=deg){int a=(int(m)-deg<int(N))?int(m)-deg:99;int hh=m&1?HO:HE;int b=hh>=0&&int(m)-hh<int(2*N)?(int(m)-hh)/2:99;if(a!=b&&std::min(a,b)<99)hi[n]=std::min(a,b);}
      if(hi[n]>=0){assert(actual[m]);assert(__builtin_ctzll(actual[m])==hi[n]);++rowchecks;}
     }
     // The preceding literal row checks already verify every selected mask;
     // representative masks also directly check the resulting rank inequality.
     if(S%17==0)for(unsigned mask:maskset){u64 lp=0,hp=0;std::vector<u64> lm,hm;
      for(unsigned j=0;j<w;++j)if(mask>>j&1){lm.push_back(actual[j]);hm.push_back(actual[w+j]);if(lo[j]>=0)lp|=u64(1)<<lo[j];if(hi[j]>=0)hp|=u64(1)<<hi[j];}
      assert(gf_rank(lm)>=__builtin_popcountll(lp));assert(gf_rank(hm)>=__builtin_popcountll(hp));rankchecks+=2;
     }
    }
   }
  }
 }
 u64 wrapchecks=0;
 for(unsigned q=4;q<=64;q*=2)for(unsigned d=1;d<q;++d)for(unsigned e=1;e<q;++e)for(unsigned alpha=0;alpha<2;++alpha)for(unsigned beta=0;beta<2;++beta){
  long a=long(d)-alpha*long(q),b=long(e)-beta*long(q);unsigned na=q-std::abs(a),nb=q-std::abs(b);long span=std::abs(a)*(nb-1)+std::abs(b)*(na-1);assert(span<long(q*q));++wrapchecks;
 }
 std::cout<<"{\"low_operand_visits\":"<<visits<<",\"point_checks\":"<<points<<",\"exact_minimum_valuation_targets\":"<<exact_targets<<",\"dense_checks\":"<<dense_checks<<",\"literal_linearized_rows\":"<<rowchecks<<",\"selected_rank_checks\":"<<rankchecks<<",\"wrap_rectangles\":"<<wrapchecks<<",\"failures\":0}\n";
}
