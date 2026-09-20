// Independent literal finite checks for the new lemmas in PROOF2.md.
// No projection probability is extrapolated from these small widths.
#include <algorithm>
#include <array>
#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <numeric>
#include <vector>
#include <omp.h>

using u64 = uint64_t;

static unsigned pc(unsigned x) { return __builtin_popcount(x); }
static unsigned val(unsigned x) { assert(x); return __builtin_ctz(x); }

static u64 low_bound(unsigned w, unsigned r, unsigned e) {
    u64 R = 1ull << r;
    if (!e) return R;
    unsigned h = pc(e);
    return std::min({R * (1ull << (h - (e >> (w - 1)))),
                     (w + 1) * (1ull << (w - h)),
                     4 * R * (1ull << ((h + 1) / 2))});
}

static u64 high_bound(unsigned w, unsigned e) {
    unsigned h = pc(e);
    return std::min({2ull * (1ull << (h - (e >> (w - 1)))),
                     (4ull*w+20) * (1ull << (w - h)),
                     16ull * ((1ull << ((h + 1) / 2)) + 1)});
}

static u64 check_quadratic_patterns() {
    u64 cases = 0;
    for (unsigned w = 3; w <= 7; ++w) {
        unsigned q = 1u << w;
        u64 failures = 0, visited = 0;
        #pragma omp parallel for schedule(dynamic, 1) reduction(+:failures,visited)
        for (unsigned beta = 0; beta < q; ++beta) {
            for (unsigned gamma = 0; gamma < q; ++gamma) {
                if (!(beta & 1) && !(gamma & 1)) continue;
                std::vector<unsigned> tags;
                if (w <= 5) for (unsigned c = 0; c < q; ++c) tags.push_back(c);
                else tags = {0, 1, q/2, q-1};
                for (unsigned c : tags) {
                    std::vector<unsigned> ys(q);
                    for (unsigned a = 0; a < q; ++a)
                        ys[a] = (beta*a*a + gamma*a + c) & (q-1);
                    for (unsigned mask = 0; mask < q; ++mask) {
                        std::vector<unsigned> counts(q, 0);
                        for (unsigned y : ys) ++counts[y & mask];
                        unsigned actual = *std::max_element(counts.begin(), counts.end());
                        unsigned bound = std::min(q, (4*q) >> (pc(mask)/2));
                        if (actual > bound) ++failures;
                        ++visited;
                    }
                }
            }
        }
        assert(failures == 0);
        cases += visited;
    }
    return cases;
}

static std::array<u64, 4> check_nh() {
    const unsigned w = 8, q = 1u << w;
    const std::array<std::array<unsigned, 2>, 4> tags = {{{0,0},{q-1,0},{0,q-1},{q/3,q/3+7}}};
    u64 visits = 0, low_checks = 0, high_checks = 0, line_checks = 0, failures = 0;
    #pragma omp parallel for schedule(dynamic, 1) reduction(+:visits,low_checks,high_checks,line_checks,failures)
    for (unsigned delta = 2; delta < q; delta += 2) {
        for (unsigned epsilon = 2; epsilon < q; epsilon += 2) {
            unsigned r = std::min(val(delta), val(epsilon));
            std::array<u64, q> low{};
            std::array<std::array<u64, q>, 4> high{};
            for (unsigned a = 0; a < q; ++a) {
                unsigned ap = (a + delta) & (q-1);
                for (unsigned b = 0; b < q; ++b) {
                    unsigned bp = (b + epsilon) & (q-1);
                    unsigned n = a*b, np = ap*bp, l = n & (q-1), lp = np & (q-1);
                    ++low[l ^ lp]; ++visits;
                    if (l != lp) continue;
                    int da = int(ap) - int(a), db = int(bp) - int(b);
                    int gcd = std::gcd(std::abs(da), std::abs(db));
                    int step_a = da/gcd, step_b = -db/gcd;
                    assert(da && db && step_a && step_b);
                    assert(da*int(b) + db*int(a) + da*db == int(np) - int(n));
                    for (int t : {-2,-1,1,2}) {
                        int at = int(a) + step_a*t, bt = int(b) + step_b*t;
                        int polynomial = int(n) + (int(a)*step_b+int(b)*step_a)*t + step_a*step_b*t*t;
                        assert(at*bt == polynomial);
                        assert(da*bt + db*at == da*int(b) + db*int(a));
                        ++line_checks;
                    }
                    for (unsigned j = 0; j < tags.size(); ++j) {
                        unsigned u = ((n >> w) + tags[j][0]) & (q-1);
                        unsigned up = ((np >> w) + tags[j][1]) & (q-1);
                        ++high[j][u ^ up];
                    }
                }
            }
            assert(low[0] == u64(q)*(1u << r));
            for (unsigned e = 0; e < q; ++e) {
                if (e % (1u << r) == 0) {
                    if (low[e] > q*low_bound(w,r,e)) ++failures;
                    ++low_checks;
                } else assert(low[e] == 0);
                for (unsigned j = 0; j < tags.size(); ++j) {
                    if (high[j][e] > q*high_bound(w,e)) ++failures;
                    ++high_checks;
                }
            }
        }
    }
    assert(failures == 0);
    return {visits, low_checks, high_checks, line_checks};
}

int main() {
    omp_set_num_threads(8);
    u64 quadratic = check_quadratic_patterns();
    auto nh = check_nh();
    std::cout << "{\n  \"quadratic_pattern_cases\": " << quadratic
              << ",\n  \"nh_operand_pair_visits\": " << nh[0]
              << ",\n  \"low_xor_bound_checks\": " << nh[1]
              << ",\n  \"high_xor_bound_checks\": " << nh[2]
              << ",\n  \"integer_line_identity_checks\": " << nh[3]
              << ",\n  \"failures\": 0,\n  \"nh_word_bits\": 8,\n  \"nh_increment_scope\": \"all nonzero even increment pairs\",\n"
              << "  \"high_tag_profiles\": [[0,0],[255,0],[0,255],[85,92]]\n}\n";
}
