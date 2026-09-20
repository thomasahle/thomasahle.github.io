// Exhaustive independent check of the new necessary congruence and fibres.
// Only small widths are enumerated; no extrapolation is used in the proof.
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <omp.h>

static unsigned val(unsigned x) { return __builtin_ctz(x); }

int main() {
    uint64_t pairs=0, low_equal=0, events=0, failures=0, instances=0;
    for(unsigned w=3; w<=6; ++w) {
        const unsigned q=1U<<w, p=(w==3?1U:(1U<<(w-3))-1U);
        uint64_t wp=0, wl=0, we=0, wf=0, wi=0;
        #pragma omp parallel for schedule(dynamic) reduction(+:wp,wl,we,wf,wi)
        for(unsigned delta=1; delta<q; ++delta) {
            const unsigned r=val(delta), R=1U<<r, Q=q/R;
            for(unsigned epsilon=0; epsilon<q; ++epsilon) {
                if(epsilon && val(epsilon)<r) continue;
                ++wi;
                for(unsigned A=0; A<q; ++A) {
                    unsigned Ap=(A+delta)&(q-1);
                    int d=(int(Ap)-int(A))/int(R);
                    // Tags include positive/negative gaps and high-word wrap.
                    for(unsigned profile=0; profile<3; ++profile) {
                        unsigned tag=profile==0?0:profile==1?q-1:q/2-1;
                        unsigned tagp=profile==0?0:profile==1?0:q/2;
                        unsigned M=profile==0?0:profile==1?q/3:q-1;
                        // Each (beta,m,t) may contain at most one B.
                        std::vector<int> seen(2*q*q,-1);
                        for(unsigned B=0; B<q; ++B) {
                            ++wp;
                            unsigned Bp=(B+epsilon)&(q-1);
                            unsigned N=A*B, Np=Ap*Bp;
                            if((N&(q-1))!=(Np&(q-1))) continue;
                            ++wl;
                            unsigned L=N&(q-1);
                            unsigned U=((N>>w)+tag)&(q-1);
                            unsigned Up=((Np>>w)+tagp)&(q-1);
                            unsigned X=M^L^U, Y=M^L^Up;
                            if(X%p!=Y%p) continue;
                            ++we;
                            unsigned m=X^Y, t=X&m, beta=(B+epsilon>=q);
                            unsigned index=(beta*q+m)*q+t;
                            if(seen[index]>=0 && seen[index]!=int(B)) ++wf;
                            seen[index]=B;
                            int e=(int(Bp)-int(B))/int(R);
                            int z=int((L&m)^(M&m)^t);
                            int gap=int(tagp)-int(tag);
                            int equation=d*int(B)+e*int(A)+int(R)*d*e
                                         -int(Q)*(int(m)-2*z-gap);
                            if(equation%int(q)!=0 || (d&1)==0 || z!=int(U&m)) ++wf;
                            unsigned candidate=0;
                            for(unsigned j=0;j<w;++j) {
                                int zj=int(((A*candidate)&m)^(M&m)^t);
                                int residual=d*int(candidate)+e*int(A)+int(R)*d*e
                                             -int(Q)*(int(m)-2*zj-gap);
                                if((uint32_t(residual)>>j)&1) candidate|=1U<<j;
                            }
                            if(candidate!=B) ++wf;
                        }
                    }
                }
            }
        }
        std::cerr << "width=" << w << " pairs=" << wp << " collisions=" << we
                  << " failures=" << wf << '\n';
        pairs+=wp; low_equal+=wl; events+=we; failures+=wf; instances+=wi;
    }
    std::cout << "{\"widths\":[3,4,5,6],\"all_minimum_valuation_increment_pairs\":"
              << instances << ",\"key_pair_profile_visits\":" << pairs
              << ",\"raw_low_equal_visits\":" << low_equal
              << ",\"projected_collision_visits\":" << events
              << ",\"failures\":" << failures << "}\n";
    return failures?1:0;
}
