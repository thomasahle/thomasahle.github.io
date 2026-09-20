// Literal finite checks of the NEW PH convolution and grouped ENH fibre count.
// No floating point, no sampling, no external libraries beyond OpenMP.
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <iostream>
#include <map>
#include <set>
#include <tuple>
#include <vector>
#include "prefix_weights.h"

using u64 = uint64_t;
static int val(unsigned x) { return x ? __builtin_ctz(x) : 99; }

int main() {
    u64 low_pairs=0, low_visits=0, point_checks=0, convolution_checks=0;
    for (unsigned s=4; s<=8; ++s) {
        unsigned m=1u<<s, mask=m-1;
        std::vector<std::pair<unsigned,unsigned>> cases;
        for (unsigned d=2; d<m; d+=2) {
            unsigned r=val(d);
            if (r>3) continue;
            for (unsigned e=0; e<m; e+=(1u<<r)) cases.emplace_back(d,e);
        }
        u64 visits_here=0, points_here=0, conv_here=0;
        #pragma omp parallel for schedule(dynamic) reduction(+:visits_here,points_here,conv_here)
        for (size_t ci=0; ci<cases.size(); ++ci) {
            auto [d,e]=cases[ci];
            unsigned r=val(d), R=1u<<r;
            std::vector<unsigned> hist(m);
            for (unsigned a=0; a<m; ++a) {
                unsigned ap=(a+d)&mask;
                for (unsigned b=0; b<m; ++b) {
                    unsigned bp=(b+e)&mask;
                    ++hist[((a*b)^(ap*bp))&mask];
                }
            }
            visits_here += u64(m)*m;
            assert(hist[0] == R*m);
            for (unsigned z=1; z<m; ++z) {
                if (val(z)==int(r)) { assert(hist[z] <= 2*R*m); ++points_here; }
                if (r==1 && val(z)==2) { assert(hist[z] <= 5*m); ++points_here; }
            }
            if (r<=2) { assert(hist[m-8] <= (s+12)*m); ++points_here; }
            u64 weighted=0;
            for (unsigned z=0; z<m; ++z) weighted += u64(prefix_weight[s][z])*hist[z];
            unsigned bound=(r==1 ? 2+184*4+62*5+s+12 :
                            r==2 ? 4+62*8+s+12 : 8+16);
            assert(weighted <= u64(bound)*m);
            ++conv_here;
        }
        low_pairs += cases.size(); low_visits += visits_here;
        point_checks += points_here; convolution_checks += conv_here;
    }

    u64 small_conv_cases=0, small_conv_visits=0;
    for (unsigned s=1; s<=3; ++s) {
        unsigned m=1u<<s, maximum=0;
        for (unsigned d=0; d<m; ++d) for (unsigned e=0; e<m; ++e) {
            unsigned total=0;
            for (unsigned a=0; a<m; ++a) for (unsigned b=0; b<m; ++b) {
                total+=prefix_weight[s][((a*b)^(((a+d)&(m-1))*((b+e)&(m-1))))&(m-1)];
                ++small_conv_visits;
            }
            maximum=std::max(maximum,total);
            assert(total<=852*m);
            ++small_conv_cases;
        }
        assert(maximum==852*m);
    }

    u64 nh_slices=0, nh_visits=0;
    for (unsigned w=3; w<=5; ++w) {
        unsigned q=1u<<w, mask=q-1;
        for (unsigned d=1; d<q; ++d) for (unsigned e=0; e<q; ++e)
        for (unsigned a=0; a<q; ++a) {
            std::set<unsigned> seen;
            unsigned ap=(a+d)&mask;
            for (unsigned b=0; b<q; ++b) {
                unsigned bp=(b+e)&mask;
                unsigned z=(ap*bp-a*b)&(q*q-1);
                assert(seen.insert(z).second);
                ++nh_visits;
            }
            ++nh_slices;
        }
    }

    u64 lift_cases=0, lift_visits=0, collision_visits=0, raw_zero_visits=0;
    for (unsigned w=5; w<=9; ++w) {
        unsigned q=1u<<w, mask=q-1, p=(q>>3)-1;
        std::set<std::pair<unsigned,unsigned>> patterns;
        for (unsigned x=0; x<q; ++x)
            for (unsigned y=x%p; y<q; y+=p)
                patterns.emplace(x^y, x&(x^y));
        std::vector<std::map<unsigned,unsigned>> classes(w);
        for (unsigned r=4; r<w; ++r) {
            unsigned R=1u<<r;
            for (auto [d,t]:patterns) {
                unsigned key=(d&(R/2-1))*R+((d-2*t)&(R-1));
                if (!classes[r].count(key)) classes[r][key]=classes[r].size();
                if (key==0) assert(d==0 && t==0);
            }
        }
        std::vector<std::tuple<unsigned,unsigned,unsigned>> cases;
        for (unsigned d=16; d<q; d+=16) {
            unsigned r=val(d);
            for (unsigned e=0; e<q; e+=(1u<<r))
                for (unsigned profile=0; profile<3; ++profile)
                    cases.emplace_back(d,e,profile);
        }
        u64 visits_here=0, collisions_here=0, zeros_here=0;
        #pragma omp parallel for schedule(dynamic) reduction(+:visits_here,collisions_here,zeros_here)
        for (size_t ci=0; ci<cases.size(); ++ci) {
            auto [d,e,profile]=cases[ci];
            unsigned r=val(d), R=1u<<r, smallq=q/R, aodd=d/R;
            unsigned inv=1;
            while ((aodd*inv)%smallq != 1) ++inv;
            unsigned tag= profile==0 ? 0 : profile==1 ? q-1 : q/2-1;
            unsigned tag2= profile==0 ? 0 : profile==1 ? 0 : q/2;
            unsigned common= profile==0 ? 0 : profile==1 ? q/3 : q-1;
            auto &ids=classes[r];
            unsigned F=ids.size();
            std::vector<int> seen(q*2*F,-1);
            unsigned count=0, zeros=0;
            for (unsigned a=0; a<q; ++a) {
                unsigned ap=(a+d)&mask;
                unsigned b0=(unsigned(-((e/R)*a+d*(e/R)))*inv)&(smallq-1);
                for (unsigned b=b0; b<q; b+=smallq) {
                    unsigned bp=(b+e)&mask;
                    unsigned n=a*b, np=ap*bp;
                    assert((n&mask)==(np&mask));
                    ++visits_here;
                    unsigned L=n&mask, U=((n>>w)+tag)&mask, Up=((np>>w)+tag2)&mask;
                    unsigned x=common^L^U, y=common^L^Up;
                    if (x%p != y%p) continue;
                    unsigned m=U^Up, t=x&m;
                    unsigned key=(m&(R/2-1))*R+((m-2*t)&(R-1));
                    auto it=ids.find(key); assert(it!=ids.end());
                    unsigned beta=(b+e)>=q;
                    unsigned slot=(a*2+beta)*F+it->second;
                    assert(seen[slot]==-1); seen[slot]=int(b);
                    ++count; ++collisions_here;
                    if (m==0) { ++zeros; ++zeros_here; }
                }
            }
            assert(zeros<=q);
            assert(count<=u64(2*F-1)*q);
        }
        lift_cases+=cases.size(); lift_visits+=visits_here;
        collision_visits+=collisions_here; raw_zero_visits+=zeros_here;
    }
    std::cout << "{\n"
      << "  \"low_xor_widths\": [4,5,6,7,8],\n"
      << "  \"low_increment_pairs\": "<<low_pairs<<",\n"
      << "  \"low_operand_pair_visits\": "<<low_visits<<",\n"
      << "  \"low_point_inequalities\": "<<point_checks<<",\n"
      << "  \"convolution_inequalities\": "<<convolution_checks<<",\n"
      << "  \"small_width_convolution_cases\": "<<small_conv_cases<<",\n"
      << "  \"small_width_convolution_pair_visits\": "<<small_conv_visits<<",\n"
      << "  \"nh_additive_slice_widths\": [3,4,5],\n"
      << "  \"nh_additive_slices\": "<<nh_slices<<",\n"
      << "  \"nh_additive_pair_visits\": "<<nh_visits<<",\n"
      << "  \"grouped_lift_widths\": [5,6,7,8,9],\n"
      << "  \"grouped_lift_increment_profile_cases\": "<<lift_cases<<",\n"
      << "  \"low_equal_operand_pair_profile_visits\": "<<lift_visits<<",\n"
      << "  \"projected_collision_visits\": "<<collision_visits<<",\n"
      << "  \"zero_mask_collision_visits\": "<<raw_zero_visits<<",\n"
      << "  \"failures\": 0\n}\n";
}
