// Exact scaled ENH-only fibres on the raw-low-equality grid.
// This is a search family, not an all-pairs extremality certificate.
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <omp.h>
#include <vector>
using u64 = uint64_t;

int main() {
    const unsigned w=12, q=1u<<w, p=(1u<<(w-3))-1, stride=1u<<8;
    const std::vector<unsigned> tags={0,1,7,255,p,q/2,q-1};
    const std::vector<unsigned> offsets={0,q-1,0x555,0xaaa,p};
    u64 visits=0, max_raw=0, max_primary=0;
    std::array<unsigned,5> raw_witness{}, primary_witness{};
    omp_set_num_threads(8);
    #pragma omp parallel for schedule(dynamic,1) reduction(+:visits)
    for(unsigned delta=stride;delta<q;delta+=stride) {
      for(unsigned epsilon=stride;epsilon<q;epsilon+=stride) {
        unsigned da=delta,db=epsilon;
        if(__builtin_ctz(da)>__builtin_ctz(db)) std::swap(da,db);
        unsigned r=__builtin_ctz(da),R=1u<<r,Q=q/R;
        unsigned inv=1; while(((da/R)*inv)%Q!=1) inv+=2;
        std::vector<std::vector<u64>> hist(tags.size(),std::vector<u64>(q));
        std::vector<std::vector<u64>> col(tags.size(),std::vector<u64>(offsets.size()));
        for(unsigned a=0;a<q;++a) {
          unsigned ap=(a+da)%q;
          unsigned b0=(Q-((u64(db/R)*a+u64(da)*db/R)%Q))*inv%Q;
          for(unsigned b=b0;b<q;b+=Q) {
            unsigned bp=(b+db)%q, n=a*b,np=ap*bp,l=n%q,lp=np%q;
            assert(l==lp); ++visits;
            for(unsigned t=0;t<tags.size();++t) {
              unsigned u=(n/q+tags[t])%q,up=(np/q+tags[t])%q;
              ++hist[t][u^up];
              for(unsigned m=0;m<offsets.size();++m)
                col[t][m]+=((u^l^offsets[m])%p==(up^l^offsets[m])%p);
            }
          }
        }
        #pragma omp critical
        {
          for(unsigned t=0;t<tags.size();++t) {
            for(unsigned e=0;e<q;++e) if(hist[t][e]>max_raw) {
              max_raw=hist[t][e];raw_witness={da,db,tags[t],e,r};
            }
            for(unsigned m=0;m<offsets.size();++m) if(col[t][m]>max_primary) {
              max_primary=col[t][m];primary_witness={da,db,tags[t],offsets[m],r};
            }
          }
        }
      }
    }
    auto arr=[](const auto& a) {std::cout<<'[';for(size_t i=0;i<a.size();++i)std::cout<<(i?",":"")<<a[i];std::cout<<']';};
    std::cout<<"{\n\"width\":"<<w<<",\"p\":"<<p<<",\"denominator\":"<<u64(q)*q
             <<",\n\"scope\":\"all nonzero increments divisible by 256, seven common tags, five fixed high offsets; low equality grid only\",\n"
             <<"\"grid_visits\":"<<visits<<",\n\"raw_high_max_count\":"<<max_raw<<",\"raw_witness_delta_epsilon_tag_mask_r\":";
    arr(raw_witness);std::cout<<",\n\"primary_max_count\":"<<max_primary<<",\"primary_witness_delta_epsilon_tag_offset_r\":";
    arr(primary_witness);std::cout<<"\n}\n";
}
