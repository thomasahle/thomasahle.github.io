// Independent check of a PolymurHash 2.0 pair under the seeded constructor
// polymur_init_params(k_seed, s_seed) with uniform 64-bit seeds.
// Upstream header: github.com/orlp/polymur-hash @ a7cc6b0 (polymur-hash.h, unmodified).
// Build: cc -O2 -std=c11 v_polymur.c -o v_polymur
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <stdlib.h>
#include "polymur-hash.h"

static uint64_t sm_state;
static uint64_t splitmix64(void) {
    uint64_t z = (sm_state += 0x9e3779b97f4a7c15ULL);
    z = (z ^ (z >> 30)) * 0xbf58476d1ce4e5b9ULL;
    z = (z ^ (z >> 27)) * 0x94d049bb133111ebULL;
    return z ^ (z >> 31);
}
static uint64_t xs[4];
static uint64_t rotl(uint64_t x, int k) { return (x << k) | (x >> (64 - k)); }
static uint64_t xoshiro(void) {
    uint64_t r = rotl(xs[1] * 5, 7) * 9, t = xs[1] << 17;
    xs[2] ^= xs[0]; xs[3] ^= xs[1]; xs[1] ^= xs[2]; xs[0] ^= xs[3]; xs[2] ^= t; xs[3] = rotl(xs[3], 45);
    return r;
}
static void rng_seed(uint64_t s) { sm_state = s; for (int i = 0; i < 4; i++) xs[i] = splitmix64(); }

// SMHasher3 verification (polymurhash: init_params_from_seed, tweak 0, LE output).
static void smh_hash(const uint8_t *in, size_t len, uint64_t seed, uint8_t out[8]) {
    PolymurHashParams p; polymur_init_params_from_seed(&p, seed);
    uint64_t h = polymur_hash(in, len, &p, 0);
    for (int i = 0; i < 8; i++) out[i] = (uint8_t)(h >> (8 * i));
}
static uint32_t smh_verify(void) {
    uint8_t key[256], hashes[8 * 256], total[8];
    memset(key, 0, sizeof key);
    for (int i = 0; i < 256; i++) { smh_hash(key, i, 256 - i, &hashes[8 * i]); key[i] = (uint8_t)i; }
    smh_hash(hashes, sizeof hashes, 0, total);
    return total[0] | (total[1] << 8) | (total[2] << 16) | ((uint32_t)total[3] << 24);
}

// One iteration of the upstream rejection loop, applied to an already
// incremented k_seed value v.  Copied from polymur_init_params; returns 1 and
// the key k if v is accepted.
static uint64_t POW37[64];
static void init_pow(void) {
    POW37[0] = 37; POW37[32] = 559096694736811184ULL;
    for (int i = 0; i < 31; ++i) {
        POW37[i+ 1] = polymur_extrared611(polymur_red611(polymur_mul128(POW37[i],    POW37[i])));
        POW37[i+33] = polymur_extrared611(polymur_red611(polymur_mul128(POW37[i+32], POW37[i+32])));
    }
}
static int accept_value(uint64_t v, uint64_t *kout) {
    uint64_t e = (v >> 3) | 1;
    if (e % 3 == 0) return 0;
    if (!(e % 5 && e % 7)) return 0;
    if (!(e % 11 && e % 13 && e % 31)) return 0;
    if (!(e % 41 && e % 61 && e % 151 && e % 331 && e % 1321)) return 0;
    uint64_t ka = 1, kb = 1;
    for (int i = 0; e; i += 2, e >>= 2) {
        if (e & 1) ka = polymur_extrared611(polymur_red611(polymur_mul128(ka, POW37[i])));
        if (e & 2) kb = polymur_extrared611(polymur_red611(polymur_mul128(kb, POW37[i+1])));
    }
    uint64_t k = polymur_extrared611(polymur_red611(polymur_mul128(ka, kb)));
    uint64_t pk = polymur_extrared611(k);
    uint64_t k2 = polymur_extrared611(polymur_red611(polymur_mul128(pk, pk)));
    uint64_t k3 = polymur_red611(polymur_mul128(pk, k2));
    uint64_t k4 = polymur_red611(polymur_mul128(k2, k2));
    uint64_t k7 = polymur_extrared611(polymur_red611(polymur_mul128(k3, k4)));
    if (k7 < (1ULL << 60) - (1ULL << 56)) { *kout = pk; return 1; }
    return 0;
}

int main(void) {
    uint32_t v = smh_verify();
    printf("SMHasher3 verification polymurhash LE: 0x%08X (expected 0x0722B1A7)\n", v);
    if (v != 0x0722B1A7u) return 1;
    init_pow();

    const uint8_t M1[8] = {0x2c,0x33,0x6f,0x94,0x8f,0x6f,0x6c,0x99};
    const uint8_t M2[7] = {0xaa,0x37,0x3a,0x8e,0xef,0xf8,0x5a};
    const uint64_t KS = 0x8545715fd36ce9e9ULL, SS = 0xd95d3b0f3d7ebc54ULL;
    PolymurHashParams P; polymur_init_params(&P, KS, SS);
    uint64_t h1 = polymur_hash(M1, 8, &P, 0), h2 = polymur_hash(M2, 7, &P, 0);
    printf("explicit key init_params(0x%016llx, 0x%016llx): k=%llu\n  H(M1,8B)=0x%016llx H(M2,7B)=0x%016llx %s\n",
           (unsigned long long)KS, (unsigned long long)SS, (unsigned long long)P.k,
           (unsigned long long)h1, (unsigned long long)h2, h1 == h2 ? "COLLIDE" : "differ");
    const uint64_t kstar = P.k;

    // Preimages of kstar: final accepted values x with the same exponent e,
    // then backward rejection runs.  Find e* by replaying the loop from KS.
    uint64_t x = KS, kk;
    unsigned steps = 0;
    do { x += POLYMUR_ARBITRARY2; steps++; } while (!accept_value(x, &kk));
    uint64_t estar = (x >> 3) | 1;
    printf("replay: accepted after %u steps, e*=0x%016llx, k=%llu (match %d)\n", steps,
           (unsigned long long)estar, (unsigned long long)kk, kk == kstar);
    // All v with (v>>3)|1 == e*: v>>3 in {e*-1, e*}.
    uint64_t W = 0; int nfinal = 0; uint64_t total_checked = 0, total_coll = 0;
    rng_seed(20260923);
    for (uint64_t hi = estar - 1; hi <= estar; hi++)
        for (uint64_t lo = 0; lo < 8; lo++) {
            uint64_t xv = (hi << 3) | lo;
            if (!accept_value(xv, &kk)) { printf("final %016llx not accepted?\n", (unsigned long long)xv); continue; }
            if (kk != kstar) { printf("final value gives other k!\n"); return 2; }
            nfinal++;
            uint64_t t = 1; // start = xv - t*A2
            for (;;) {
                uint64_t start = xv - t * POLYMUR_ARBITRARY2;
                // verify with the real upstream constructor and a random s_seed
                PolymurHashParams Q; uint64_t sseed = xoshiro();
                polymur_init_params(&Q, start, sseed);
                uint64_t a = polymur_hash(M1, 8, &Q, 0), b = polymur_hash(M2, 7, &Q, 0);
                total_checked++;
                if (Q.k != kstar) { printf("  preimage walk ended: start=xv-%llu*A2 gives other key\n", (unsigned long long)t); break; }
                total_coll += (a == b);
                W++;
                uint64_t prev = xv - t * POLYMUR_ARBITRARY2; // this value itself must be rejected for t+1 to continue
                uint64_t dummy;
                if (accept_value(prev, &dummy)) { t++; // next start would stop at prev
                    // confirm: start xv-(t)*A2 ends at prev with another key (checked on next loop iteration)
                    continue; }
                t++;
            }
        }
    printf("final values: %d; W(k*) = %llu k_seed preimages; colliding among them %llu/%llu (random s_seed each)\n",
           nfinal, (unsigned long long)W, (unsigned long long)total_coll, (unsigned long long)W);

    // Uniform control.
    uint64_t hits = 0, N = 2000000;
    for (uint64_t i = 0; i < N; i++) {
        PolymurHashParams Q; polymur_init_params(&Q, xoshiro(), xoshiro());
        hits += polymur_hash(M1, 8, &Q, 0) == polymur_hash(M2, 7, &Q, 0);
    }
    printf("uniform (k_seed,s_seed) control: %llu/%llu\n", (unsigned long long)hits, (unsigned long long)N);
    return 0;
}
