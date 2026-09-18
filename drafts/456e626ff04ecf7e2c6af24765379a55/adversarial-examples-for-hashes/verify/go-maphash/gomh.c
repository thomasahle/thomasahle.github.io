// gomh.c -- independent transcription of Go 1.27.1 internal/runtime/maps/memhash_amd64.s (memHashAES) into AES-NI
// intrinsics, written from the assembly (not from the earlier software port).  Modes:
//   ./gomh vectors <v_file>          check every M / E / B line of a vectors.go dump against this transcription
//   ./gomh delta <L>                 derive the cross-length pair (a = L zero bytes, b = pad16(a) ^ Delta*) from the
//                                    DDT of the AES S-box, using only this implementation's round function
//   ./gomh measure <L> <log2N> [thr] draw a fresh 128-byte key + 64-bit seed per sample (xoshiro256**, /dev/urandom
//                                    seeded), hash a and b, count h(a)==h(b); prints count, rate, 95% CI (Garwood)
//   ./gomh seedfree <L> <log2N>      find one colliding key, then check the pair under 2^log2N random seeds
// build: gcc -O2 -maes -msse4.1 -mssse3 -fopenmp -o gomh gomh.c -lm
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <wmmintrin.h>
#include <smmintrin.h>
#include <omp.h>

typedef __m128i v128;
static uint8_t aeskeysched[128];

#define R(x) _mm_aesenc_si128((x), (x))     // AESENC X,X : MC(SR(SB(x))) ^ x

static inline v128 ld(const uint8_t *p) { return _mm_loadu_si128((const v128 *)p); }
static inline uint64_t lo64(v128 x) { return (uint64_t)_mm_cvtsi128_si64(x); }

// memHashAES(p, seed, len) -- the amd64 assembly, class by class.
static uint64_t memhash_aes(const uint8_t *p, uint64_t seed, size_t len) {
    v128 X0 = _mm_cvtsi64_si128((long long)seed);        // MOVQ BX, X0
    X0 = _mm_insert_epi16(X0, (int)(len & 0xffff), 4);     // PINSRW $4, CX, X0
    X0 = _mm_shufflehi_epi16(X0, 0);                       // PSHUFHW $0, X0, X0
    v128 X1 = X0;                                          // unscrambled seed
    X0 = _mm_xor_si128(X0, ld(aeskeysched));               // PXOR aeskeysched, X0
    X0 = R(X0);                                            // AESENC X0, X0
    if (len < 16) {
        if (len == 0) { X0 = R(X0); return lo64(X0); }     // aes0
        uint8_t buf[16] = {0};                             // masks<>: keep the first len bytes (or the pshufb path)
        memcpy(buf, p, len);
        X1 = ld(buf);
    final1:
        X1 = _mm_xor_si128(X1, X0);
        X1 = R(X1); X1 = R(X1); X1 = R(X1);
        return lo64(X1);
    }
    if (len == 16) { X1 = ld(p); goto final1; }
    if (len <= 32) {
        X1 = R(_mm_xor_si128(X1, ld(aeskeysched + 16)));
        v128 X2 = _mm_xor_si128(ld(p), X0), X3 = _mm_xor_si128(ld(p + len - 16), X1);
        for (int i = 0; i < 3; i++) { X2 = R(X2); X3 = R(X3); }
        return lo64(_mm_xor_si128(X2, X3));
    }
    if (len <= 64) {
        v128 X2 = X1, X3 = X1;
        X1 = R(_mm_xor_si128(X1, ld(aeskeysched + 16)));
        X2 = R(_mm_xor_si128(X2, ld(aeskeysched + 32)));
        X3 = R(_mm_xor_si128(X3, ld(aeskeysched + 48)));
        v128 X4 = _mm_xor_si128(ld(p), X0), X5 = _mm_xor_si128(ld(p + 16), X1);
        v128 X6 = _mm_xor_si128(ld(p + len - 32), X2), X7 = _mm_xor_si128(ld(p + len - 16), X3);
        for (int i = 0; i < 3; i++) { X4 = R(X4); X5 = R(X5); X6 = R(X6); X7 = R(X7); }
        X4 = _mm_xor_si128(X4, X6); X5 = _mm_xor_si128(X5, X7);
        return lo64(_mm_xor_si128(X4, X5));
    }
    v128 S[8];                                             // X0..X7 lane seeds
    S[0] = X0;
    for (int i = 1; i < 8; i++) S[i] = R(_mm_xor_si128(X1, ld(aeskeysched + 16 * i)));
    v128 Y[8];
    if (len <= 128) {
        Y[0] = ld(p); Y[1] = ld(p + 16); Y[2] = ld(p + 32); Y[3] = ld(p + 48);
        Y[4] = ld(p + len - 64); Y[5] = ld(p + len - 48); Y[6] = ld(p + len - 32); Y[7] = ld(p + len - 16);
        for (int i = 0; i < 8; i++) Y[i] = _mm_xor_si128(Y[i], S[i]);
    } else {
        for (int i = 0; i < 8; i++) Y[i] = _mm_xor_si128(ld(p + len - 128 + 16 * i), S[i]);
        size_t blocks = (len - 1) >> 7;
        const uint8_t *q = p;
        do {
            for (int i = 0; i < 8; i++) Y[i] = R(Y[i]);
            for (int i = 0; i < 8; i++) Y[i] = _mm_aesenc_si128(Y[i], ld(q + 16 * i)); // AESENC block, X_i
            q += 128;
        } while (--blocks);
    }
    for (int r = 0; r < 3; r++) for (int i = 0; i < 8; i++) Y[i] = R(Y[i]);
    Y[0] = _mm_xor_si128(Y[0], Y[4]); Y[1] = _mm_xor_si128(Y[1], Y[5]);
    Y[2] = _mm_xor_si128(Y[2], Y[6]); Y[3] = _mm_xor_si128(Y[3], Y[7]);
    Y[0] = _mm_xor_si128(Y[0], Y[2]); Y[1] = _mm_xor_si128(Y[1], Y[3]);
    return lo64(_mm_xor_si128(Y[0], Y[1]));
}

// hash/maphash.Bytes: 128-byte chunks chained through memhash; empty final chunk returns the state.
static uint64_t maphash_bytes(const uint8_t *p, uint64_t seed, size_t len) {
    uint64_t st = seed;
    while (len > 128) { st = memhash_aes(p, st, 128); p += 128; len -= 128; }
    return len ? memhash_aes(p, st, len) : st;
}

// ---- vectors.go reproduction ----
static uint64_t sm;
static uint64_t splitmix(void) {
    sm += 0x9E3779B97F4A7C15ULL; uint64_t z = sm;
    z = (z ^ (z >> 30)) * 0xBF58476D1CE4E5B9ULL; z = (z ^ (z >> 27)) * 0x94D049BB133111EBULL; return z ^ (z >> 31);
}
static int hexval(char c) { return c <= '9' ? c - '0' : (c | 32) - 'a' + 10; }

static int do_vectors(const char *path) {
    FILE *f = fopen(path, "r"); if (!f) { perror(path); return 2; }
    static uint8_t data[8192 + 4096];
    sm = 12345;
    for (int i = 0; i < 8192; i += 8) { uint64_t v = splitmix(); for (int j = 0; j < 8; j++) data[i + j] = (uint8_t)(v >> (8 * j)); }
    char line[1024]; long n = 0, bad = 0, skipped = 0; int aes = -1; char arch[32] = "";
    while (fgets(line, sizeof line, f)) {
        if (!strncmp(line, "KS ", 3)) { for (int i = 0; i < 128; i++) aeskeysched[i] = (uint8_t)(hexval(line[3 + 2 * i]) * 16 + hexval(line[4 + 2 * i])); continue; }
        if (!strncmp(line, "AES ", 4)) { aes = line[4] - '0'; continue; }
        if (!strncmp(line, "ARCH ", 5)) { sscanf(line + 5, "%31s", arch); continue; }
        char tag[8]; unsigned long long a, s, h;
        if (sscanf(line, "%7s %llu %llx %llx", tag, &a, &s, &h) != 4) continue;
        uint64_t mine;
        if (!strcmp(tag, "M")) mine = memhash_aes(data, s, a);
        else if (!strcmp(tag, "E")) mine = memhash_aes(data + 4096 - a, s, a);
        else if (!strcmp(tag, "B")) mine = maphash_bytes(data, s, a);
        else { skipped++; continue; }
        n++;
        if (mine != h) { bad++; if (bad <= 5) printf("MISMATCH %s len=%llu seed=%016llx go=%016llx mine=%016llx\n", tag, a, s, h, (unsigned long long)mine); }
    }
    fclose(f);
    printf("%s: arch=%s aes=%d checked=%ld (M/E/B) mismatches=%ld skipped(H64/H32/C*/S)=%ld -> %s\n", path, arch, aes, n, bad, skipped,
           (aes == 1 && !strcmp(arch, "amd64")) ? (bad ? "FAIL" : "OK") : "n/a (not the amd64 AES path)");
    return bad != 0;
}

// ---- pair construction from the S-box DDT, using only this implementation's round function ----
// F(x) = R(x).  For L<16 vs 16 the two seed states entering F differ by Dlen = (L^16) in bytes 8,10,12,14.
// The S-box output difference w with the largest DDT count (4) at input v = L^16 occurs at each of the four
// bytes with probability 4/256; then F(y)^F(y^Dlen) is a constant Delta*.  Recover it by brute force.
static int sbox_ddt_best(uint8_t v, uint8_t *w_out, uint8_t *y_out) {
    // S-box via R on a single-byte state is awkward; use the standard AES S-box computed from the intrinsic:
    // AESENCLAST(x, 0) = SR(SB(x)); for a state with only byte 0 set, SR keeps byte 0.
    static uint8_t S[256]; static int init = 0;
    if (!init) { for (int x = 0; x < 256; x++) { uint8_t b[16] = {0}; b[0] = (uint8_t)x; v128 t = _mm_aesenclast_si128(ld(b), _mm_setzero_si128()); uint8_t o[16]; _mm_storeu_si128((v128 *)o, t); S[x] = o[0]; } init = 1; }
    int cnt[256] = {0}; int best = 0, bw = 0;
    for (int y = 0; y < 256; y++) cnt[S[y] ^ S[y ^ v]]++;
    for (int w = 0; w < 256; w++) if (cnt[w] > best) { best = cnt[w]; bw = w; }
    *w_out = (uint8_t)bw;
    for (int y = 0; y < 256; y++) if ((S[y] ^ S[y ^ v]) == bw) { *y_out = (uint8_t)y; break; }
    return best;
}
static void make_pair(int L, uint8_t a[16], uint8_t b[16], int verbose) {
    uint8_t v = (uint8_t)(L ^ 16), w, y0; int ddt = sbox_ddt_best(v, &w, &y0);
    uint8_t y[16] = {0}; y[8] = y[10] = y[12] = y[14] = y0;            // any y with the DDT-4 event at all four bytes
    uint8_t d[16] = {0}; d[8] = d[10] = d[12] = d[14] = v;
    v128 Y = ld(y), D = ld(d);
    v128 Delta = _mm_xor_si128(R(Y), R(_mm_xor_si128(Y, D)));
    memset(a, 0, 16); _mm_storeu_si128((v128 *)b, Delta);              // b = pad16(a) ^ Delta*, a = zeros
    if (verbose) {
        printf("L=%d: v=0x%02x best DDT count=%d (w=0x%02x) -> predicted eps=(%d/256)^4=2^%.2f\nDelta*=", L, v, ddt, w, ddt, 4 * log2(ddt / 256.0));
        for (int i = 0; i < 16; i++) printf("%02x", b[i]);
        printf("\na (len %d) = ", L); for (int i = 0; i < L; i++) printf("00"); printf("\nb (len 16) = "); for (int i = 0; i < 16; i++) printf("%02x", b[i]); printf("\n");
    }
}

// ---- sampling ----
static inline uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
typedef struct { uint64_t s[4]; } xo;
static uint64_t xo_next(xo *r) {
    uint64_t *s = r->s, res = rotl(s[1] * 5, 7) * 9, t = s[1] << 17;
    s[2] ^= s[0]; s[3] ^= s[1]; s[1] ^= s[2]; s[0] ^= s[3]; s[2] ^= t; s[3] = rotl(s[3], 45); return res;
}
static void xo_seed(xo *r) { FILE *u = fopen("/dev/urandom", "r"); if (!u || fread(r->s, 8, 4, u) != 4) { fprintf(stderr, "urandom\n"); exit(1); } fclose(u); }

// per-thread key: memhash_aes reads the global aeskeysched, so make it thread-local for the sampler
static uint64_t memhash_aes_key(const uint8_t *key, const uint8_t *p, uint64_t seed, size_t len);

static void garwood(long c, double N, double *lo, double *hi) {
    // exact Poisson 95% interval via chi-square quantiles; simple bisection on the Poisson cdf
    double a = 0.025;
    // cdf(k, mu)
    #define CDF(k, mu) ({ double s_ = 0, t_ = exp(-(mu)); for (long i_ = 0; i_ <= (k); i_++) { s_ += t_; t_ *= (mu) / (i_ + 1); } s_; })
    double l = 0, h = 4.0 * c + 30;
    if (c > 0) { double x = 0, yv = 4.0 * c + 10; for (int i = 0; i < 200; i++) { double m = (x + yv) / 2; if (1 - CDF(c - 1, m) < a) x = m; else yv = m; } l = x; }
    { double x = 0, yv = h; for (int i = 0; i < 200; i++) { double m = (x + yv) / 2; if (CDF(c, m) > a) x = m; else yv = m; } h = x; }
    *lo = l / N; *hi = h / N;
}

int main(int argc, char **argv) {
    if (argc < 2) { fprintf(stderr, "usage: see header\n"); return 1; }
    if (!strcmp(argv[1], "vectors")) return do_vectors(argv[2]);
    int L = atoi(argv[2]);
    uint8_t a[16], b[16]; make_pair(L, a, b, 1);
    if (!strcmp(argv[1], "delta")) return 0;
    int lg = atoi(argv[3]); uint64_t N = 1ULL << lg;
    if (!strcmp(argv[1], "seedfree")) {
        xo r; xo_seed(&r); uint64_t tries = 0;
        for (;;) { for (int i = 0; i < 16; i++) ((uint64_t *)aeskeysched)[i] = xo_next(&r); uint64_t sd = xo_next(&r); tries++;
            if (memhash_aes(a, sd, L) == memhash_aes(b, sd, 16)) break; }
        printf("colliding key found after %llu draws: key[8,10,12,14]=%02x %02x %02x %02x\n", (unsigned long long)tries, aeskeysched[8], aeskeysched[10], aeskeysched[12], aeskeysched[14]);
        uint64_t hits = 0; for (uint64_t i = 0; i < N; i++) { uint64_t sd = xo_next(&r); hits += memhash_aes(a, sd, L) == memhash_aes(b, sd, 16); }
        printf("that key, %llu random seeds: %llu collide (seed-free: %s)\n", (unsigned long long)N, (unsigned long long)hits, hits == N ? "yes" : "NO");
        // and: other 124 key bytes re-randomised, the 4 bytes kept
        uint8_t k8 = aeskeysched[8], k10 = aeskeysched[10], k12 = aeskeysched[12], k14 = aeskeysched[14]; hits = 0; uint64_t M = N > 65536 ? 65536 : N;
        for (uint64_t i = 0; i < M; i++) { for (int j = 0; j < 16; j++) ((uint64_t *)aeskeysched)[j] = xo_next(&r); aeskeysched[8] = k8; aeskeysched[10] = k10; aeskeysched[12] = k12; aeskeysched[14] = k14; uint64_t sd = xo_next(&r); hits += memhash_aes(a, sd, L) == memhash_aes(b, sd, 16); }
        printf("same 4 bytes, other 124 key bytes + seed random, %llu draws: %llu collide\n", (unsigned long long)M, (unsigned long long)hits);
        return 0;
    }
    if (!strcmp(argv[1], "measure")) {
        int thr = argc > 4 ? atoi(argv[4]) : 8; omp_set_num_threads(thr);
        double t0 = omp_get_wtime(); uint64_t total = 0;
        #pragma omp parallel reduction(+:total)
        {
            xo r; xo_seed(&r); uint64_t per = N / omp_get_num_threads(), c = 0; uint8_t key[128];
            for (uint64_t i = 0; i < per; i++) {
                for (int j = 0; j < 16; j++) ((uint64_t *)key)[j] = xo_next(&r);      // fresh 128-byte process key
                uint64_t sd = xo_next(&r);                                             // fresh 64-bit seed
                c += memhash_aes_key(key, a, sd, L) == memhash_aes_key(key, b, sd, 16);
            }
            total = c;
        }
        double lo, hi; garwood((long)total, (double)N, &lo, &hi);
        printf("measure L=%d vs 16: collisions=%llu N=2^%d rate=%.4g=2^%.3f  95%% CI [2^%.3f, 2^%.3f]  (%.1f s, %d threads)\n",
               L, (unsigned long long)total, lg, (double)total / N, log2((double)total / N), log2(lo), log2(hi), omp_get_wtime() - t0, thr);
        return 0;
    }
    return 1;
}

// keyed variant: identical to memhash_aes but reading the key from a pointer (for the multi-threaded sampler).
static uint64_t memhash_aes_key(const uint8_t *key, const uint8_t *p, uint64_t seed, size_t len) {
    v128 X0 = _mm_cvtsi64_si128((long long)seed);
    X0 = _mm_insert_epi16(X0, (int)(len & 0xffff), 4);
    X0 = _mm_shufflehi_epi16(X0, 0);
    X0 = R(_mm_xor_si128(X0, ld(key)));
    uint8_t buf[16] = {0}; memcpy(buf, p, len < 16 ? len : 16);
    v128 X1 = _mm_xor_si128(ld(buf), X0);
    X1 = R(X1); X1 = R(X1); X1 = R(X1);
    return lo64(X1);                                       // only the len<=16 class is needed by the sampler
}
