// Exact n-bit analogue of the wyhash loop fold differential: fold(X*Y) = lo ^ hi of the 2n-bit product.
// Site MM: (X,Y) vs (~X,~Y).  Site M0: (X,Y) vs (~X,Y).  For every X, count Y; report the spread over X.
#include <cstdio>
#include <cstdint>
#include <vector>
#include <algorithm>
#include <cmath>
int main(int argc, char** argv) {
    int n = argc > 1 ? atoi(argv[1]) : 16; uint64_t N = 1ull << n, M = N - 1;
    auto fold = [&](uint64_t a, uint64_t b) { unsigned __int128 r = (unsigned __int128)a * b; return (uint64_t)((r & M) ^ ((r >> n) & M)); };
    std::vector<uint32_t> cMM(N), cM0(N);
    for (uint64_t x = 0; x < N; x++) { uint32_t a = 0, b = 0; for (uint64_t y = 0; y < N; y++) { uint64_t f = fold(x, y); a += f == fold(x ^ M, y ^ M); b += f == fold(x ^ M, y); } cMM[x] = a; cM0[x] = b; }
    for (int s = 0; s < 2; s++) {
        auto& c = s ? cM0 : cMM; double tot = 0; for (auto v : c) tot += v; double mean = tot / N / N;
        std::vector<uint32_t> so(c); std::sort(so.begin(), so.end());
        double odd = 0, even = 0; for (uint64_t x = 0; x < N; x++) (x & 1 ? odd : even) += c[x];
        printf("n=%d site %s: mean P=2^%.3f (=2^(-%.4f n)); over X: min 2^%.2f p10 2^%.2f median 2^%.2f p90 2^%.2f max 2^%.2f; X odd mean 2^%.3f, X even mean 2^%.3f\n", n, s ? "M0" : "MM", log2(mean), -log2(mean) / n,
            log2((double)so[0] / N), log2((double)so[N / 10] / N), log2((double)so[N / 2] / N), log2((double)so[9 * N / 10] / N), log2((double)so[N - 1] / N), log2(odd / (N / 2) / N), log2(even / (N / 2) / N));
        uint64_t best = std::max_element(c.begin(), c.end()) - c.begin(); printf("   best X = 0x%llx (P=2^%.2f)\n", (unsigned long long)best, log2((double)c[best] / N));
    }
}
