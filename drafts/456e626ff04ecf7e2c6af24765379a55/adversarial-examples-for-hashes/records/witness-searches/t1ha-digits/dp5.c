// dp5.c -- exhaustive signed-digit route search for t1ha2_atonce (64-bit) fixed pairs
// of 9..16 bytes (L = 2 words), extending the panel's dp.c:
//   * exhaustive 3+T, 4, 4+T, 5 and 5+T digit routes (dp.c did 1..3 digits, T with <= 2
//     digits, and 40M random 4-digit routes, keeping only the 60 best lines per run);
//   * every DP candidate at or below a total-cost bound is realised in C (explicit
//     message pair + L class), and each realised pair gets a direct 2^16-sample
//     conditional check against the hash itself, so the printed estimate is measured.
// Model (see prior README / trail_dp.py): x = len + w0, (l,h) = x*P2, a = seed ^ l,
// B = len + h; u = a + t, (L,H) = u*P1; state (a + H, B ^ L).  Pair differs by a
// signed-digit Da = l* - l (each non-top digit costs 1 sign bit), du = u* - u chosen so
// hi(du*P1) + c64 - wrapu*P1 = -Da, and E = B ^ B* = L ^ (L + D), D = du*P1 mod 2^64,
// a carry pattern whose weight = number of fixed L bits (DP).  tot = wt(E) + ndig - log2 pw.
//
// Usage: dp5 <mode> <threads> <bound> [part/nparts]
//   mode: 3T | 4 | 4T | 5 | 5T ; bound: print candidates with tot <= bound
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <math.h>
#include <pthread.h>
#include "t1ha2.h"

typedef unsigned __int128 u128;
static const uint64_t TOP = 1ULL << 63;
static uint64_t P2inv, P1inv;
static uint64_t inv64(uint64_t p) { uint64_t x = p; for (int i = 0; i < 6; i++) x *= 2 - p * x; return x; }

static inline uint64_t splitmix64(uint64_t *s) {
    uint64_t z = (*s += UINT64_C(0x9E3779B97F4A7C15));
    z = (z ^ (z >> 30)) * UINT64_C(0xBF58476D1CE4E5B9);
    z = (z ^ (z >> 27)) * UINT64_C(0x94D049BB133111EB);
    return z ^ (z >> 31);
}

// ---- carry-pattern DP (as dp.c) with early abort above maxcost ----
static int dp(uint64_t D, uint64_t K, int c64, int maxcost, uint64_t *Eout) {
    int cost[4], ncost[4]; unsigned char bp[64][4];
    for (int s = 0; s < 4; s++) cost[s] = 1 << 20; cost[0] = 0;
    for (int i = 0; i < 64; i++) {
        int Di = (D >> i) & 1, Ki = (K >> i) & 1;
        for (int s = 0; s < 4; s++) ncost[s] = 1 << 20;
        int mn = 1 << 20;
        for (int s = 0; s < 4; s++) {
            if (cost[s] >= (1 << 20)) continue;
            int C = s & 1, Cp = s >> 1; int Ei = C ^ Di; if (Ei != (Cp ^ Ki)) continue;
            int c1lo = (C == Di) ? Di : 0, c1hi = (C == Di) ? Di : 1;
            int p1lo = (Cp == Ki) ? Ki : 0, p1hi = (Cp == Ki) ? Ki : 1;
            for (int c1 = c1lo; c1 <= c1hi; c1++) { if (i == 63 && c1 != c64) continue;
                for (int p1 = p1lo; p1 <= p1hi; p1++) { int ns = c1 | (p1 << 1); int v = cost[s] + Ei;
                    if (v < ncost[ns]) { ncost[ns] = v; bp[i][ns] = (unsigned char)s; if (v < mn) mn = v; } } }
        }
        if (mn > maxcost) return -1;
        memcpy(cost, ncost, sizeof cost);
    }
    int best = -1, bs = 0; for (int s = 0; s < 4; s++) if (cost[s] < (1 << 20) && (best < 0 || cost[s] < best)) { best = cost[s]; bs = s; }
    if (best < 0) return -1;
    uint64_t E = 0; int s = bs;
    for (int i = 63; i >= 0; i--) { s = bp[i][s]; int C = s & 1; E |= (uint64_t)(C ^ ((D >> i) & 1)) << i; }
    *Eout = E; return best;
}

// ---- route description ----
typedef struct { int ndig; int pos[6]; int sgn[6]; int hasT; } route_t;

typedef struct {
    double tot, est; int len1, len2; uint8_t m1[16], m2[16]; uint64_t Lmask, Lval;
    uint64_t D, K, E, du, Da, d0, w0, t, w0s, ts; int c64, wrapu, wrapx, cT, dlen, dpcost, fixed;
    uint64_t condN, condHits; uint64_t exseed;
} cand_t;

static int realize(const route_t *r, uint64_t Da, uint64_t d0, uint64_t du, uint64_t D, uint64_t K, uint64_t E,
                   int c64, int wrapx, int cT, int dlen, uint64_t *rng, cand_t *o) {
    int len1, len2;
    if (dlen <= 0) { len1 = 16; len2 = 16 + dlen; } else { len1 = 16 - dlen; len2 = 16; }
    uint64_t fixedmask = E & ~TOP;                      // B_63 is free even when E_63 = 1
    uint64_t fixedval = ((E ^ K) >> 1) & fixedmask;     // B_i = Cp_{i+1} = E_{i+1} ^ K_{i+1}
    for (int tries = 0; tries < 40000; tries++) {
        uint64_t B = (splitmix64(rng) & ~fixedmask) | fixedval;
        uint64_t h = B - (uint64_t)len1;
        if (h >= P2) continue;
        u128 lo = (((u128)h << 64) + P2 - 1) / P2, hi = ((((u128)h + 1) << 64) - 1) / P2;
        for (u128 xx = lo; xx <= hi; xx++) {
            if (xx >> 64) break;
            uint64_t x = (uint64_t)xx; uint64_t l = x * P2;
            int ok = 1;
            for (int k = 0; k < r->ndig; k++) { int b = (l >> r->pos[k]) & 1; if (r->sgn[k] > 0 ? b != 0 : b != 1) { ok = 0; break; } }
            if (!ok) continue;
            if (r->hasT && (int)(l >> 63) != cT) continue;
            int wr = ((u128)x + d0) >> 64 ? 1 : 0; if (wr != wrapx) continue;
            uint64_t xs = x + d0; uint64_t ls = xs * P2, hs = (uint64_t)(((u128)xs * P2) >> 64);
            uint64_t Bs = (uint64_t)len2 + hs;
            if (Bs - B != K) continue; if ((B ^ Bs) != E) continue; if (ls != l + Da) continue;
            uint64_t w0 = x - (uint64_t)len1, w0s = xs - (uint64_t)len2, dt = du - Da, t, ts;
            if (len2 < len1) { ts = splitmix64(rng) & ((1ULL << (8 * (len2 - 8))) - 1); t = ts - dt; }
            else if (len1 < len2) { t = splitmix64(rng) & ((1ULL << (8 * (len1 - 8))) - 1); ts = t + dt; }
            else { t = splitmix64(rng); ts = t + dt; }
            memset(o->m1, 0, 16); memset(o->m2, 0, 16);
            memcpy(o->m1, &w0, 8); memcpy(o->m1 + 8, &t, len1 - 8);
            memcpy(o->m2, &w0s, 8); memcpy(o->m2 + 8, &ts, len2 - 8);
            o->len1 = len1; o->len2 = len2; o->w0 = w0; o->t = t; o->w0s = w0s; o->ts = ts;
            // L class: bit i fixed where E_i = 1, to C_{i+1} = E_{i+1} ^ D_{i+1} (i < 63), c64 for i = 63
            uint64_t Cfull = ((E ^ D) >> 1) | ((uint64_t)c64 << 63);
            o->Lmask = E; o->Lval = Cfull & E; o->fixed = __builtin_popcountll(E);
            return 1;
        }
    }
    return 0;
}

// direct conditional check: sample L in the class, derive the seed from m1, hash both
static void condcheck(cand_t *o, int lg, uint64_t *rng) {
    uint64_t W0, T1, LO; memcpy(&W0, o->m1, 8); T1 = tail64(o->m1 + 8, o->len1 - 8); LO = ((uint64_t)o->len1 + W0) * P2;
    uint64_t n = 1ULL << lg, hits = 0, ex = 0;
    for (uint64_t i = 0; i < n; i++) {
        uint64_t L = (splitmix64(rng) & ~o->Lmask) | o->Lval; uint64_t u = L * P1inv; uint64_t a = u - T1; uint64_t seed = a ^ LO;
        if (t1ha2_atonce(o->m1, o->len1, seed) == t1ha2_atonce(o->m2, o->len2, seed)) { if (!hits) ex = seed; hits++; }
    }
    o->condN = n; o->condHits = hits; o->exseed = ex;
    o->est = hits ? o->fixed - log2((double)hits / n) : 99.0;
}

// ---- search ----
static int MODE_NDIG, MODE_T; static double BOUND; static int NTHREADS;
static pthread_mutex_t outmx = PTHREAD_MUTEX_INITIALIZER;
static volatile long work_next = 0; static long work_total = 0;
static int PART = 0, NPARTS = 1;

typedef struct { long routes, dpruns, cands, realized; double best; uint64_t rng; } stats_t;

static void hexs(char *dst, const uint8_t *p, int n) { for (int i = 0; i < n; i++) sprintf(dst + 2 * i, "%02x", p[i]); dst[2 * n] = 0; }

static void process_route(const route_t *r, stats_t *st) {
    st->routes++;
    uint64_t Da = 0; for (int k = 0; k < r->ndig; k++) Da += (uint64_t)(int64_t)r->sgn[k] << r->pos[k];
    if (r->hasT) Da += TOP;
    int negative = !r->hasT && (int64_t)Da < 0;
    uint64_t need = -Da;
    uint64_t d0 = Da * P2inv; uint64_t q = (uint64_t)(((u128)d0 * P2) >> 64);
    int signcost = r->ndig;
    for (int wrapu = 0; wrapu < 2; wrapu++) for (int c64 = 0; c64 < 2; c64++) {
        uint64_t target = need - c64 + (wrapu ? P1 : 0);
        u128 lo = (((u128)target << 64) + P1 - 1) / P1, hi = ((((u128)target + 1) << 64) - 1) / P1;
        for (u128 duu = lo; duu <= hi; duu++) { if (duu >> 64) break; uint64_t du = (uint64_t)duu;
            if ((uint64_t)(((u128)du * P1) >> 64) != target) continue;
            double pw = wrapu ? du / 18446744073709551616.0 : 1 - du / 18446744073709551616.0;
            if (pw < 0.01) continue;
            double wcost = -log2(pw);
            int maxdp = (int)floor(BOUND - signcost - wcost + 1e-9);
            if (maxdp < 0) continue;
            uint64_t D = du * P1;
            int nKv = r->hasT ? 2 : 1;
            for (int kv = 0; kv < nKv; kv++) for (int wrapx = 0; wrapx < 2; wrapx++) for (int dlen = -7; dlen <= 7; dlen++) {
                uint64_t K = q + (r->hasT ? kv : negative) - (wrapx ? P2 : 0) + (uint64_t)(int64_t)dlen;
                uint64_t E; int cost = dp(D, K, c64, maxdp, &E); st->dpruns++;
                if (cost < 0) continue;
                double tot = cost + signcost + wcost;
                if (tot > BOUND + 1e-9) continue;
                st->cands++;
                cand_t c; memset(&c, 0, sizeof c);
                c.tot = tot; c.D = D; c.K = K; c.E = E; c.du = du; c.Da = Da; c.d0 = d0; c.c64 = c64; c.wrapu = wrapu; c.wrapx = wrapx; c.cT = kv; c.dlen = dlen; c.dpcost = cost;
                int ok = realize(r, Da, d0, du, D, K, E, c64, wrapx, kv, dlen, &st->rng, &c);
                char digs[128]; digs[0] = 0;
                for (int k = 0; k < r->ndig; k++) sprintf(digs + strlen(digs), "%s%+d*%d", k ? "," : "", r->sgn[k], r->pos[k]);
                if (r->hasT) strcat(digs, ",T");
                if (!ok) {
                    pthread_mutex_lock(&outmx);
                    printf("UNREAL tot=%.2f dp=%d ndig=%d digs=%s Da=%016llx du=%016llx c64=%d wrapu=%d d0=%016llx K=%016llx wrapx=%d cT=%d dlen=%d E=%016llx\n",
                        tot, cost, r->ndig, digs, (unsigned long long)Da, (unsigned long long)du, c64, wrapu, (unsigned long long)d0, (unsigned long long)K, wrapx, kv, dlen, (unsigned long long)E);
                    fflush(stdout); pthread_mutex_unlock(&outmx);
                    continue;
                }
                st->realized++;
                condcheck(&c, 16, &st->rng);
                if (c.est < st->best) st->best = c.est;
                char h1[40], h2[40]; hexs(h1, c.m1, c.len1); hexs(h2, c.m2, c.len2);
                pthread_mutex_lock(&outmx);
                printf("REAL tot=%.2f est=%.2f dp=%d ndig=%d digs=%s len1=%d len2=%d m1=%s m2=%s Lmask=%016llx Lval=%016llx cond=%llu/%llu Da=%016llx du=%016llx c64=%d wrapu=%d d0=%016llx K=%016llx D=%016llx wrapx=%d cT=%d dlen=%d E=%016llx exseed=%016llx\n",
                    tot, c.est, cost, r->ndig, digs, c.len1, c.len2, h1, h2, (unsigned long long)c.Lmask, (unsigned long long)c.Lval,
                    (unsigned long long)c.condHits, (unsigned long long)c.condN, (unsigned long long)Da, (unsigned long long)du, c64, wrapu,
                    (unsigned long long)d0, (unsigned long long)K, (unsigned long long)D, wrapx, kv, dlen, (unsigned long long)E, (unsigned long long)c.exseed);
                fflush(stdout); pthread_mutex_unlock(&outmx);
            }
        }
    }
}

// work units: for ndig>=4 the pair (p0,p1) of the two highest positions; for ndig=3 the highest position
static void do_unit(long unit, stats_t *st) {
    route_t r; r.ndig = MODE_NDIG; r.hasT = MODE_T;
    if (MODE_NDIG == 3) {
        int p0 = (int)unit;
        for (int p1 = 0; p1 < p0; p1++) for (int p2 = 0; p2 < p1; p2++) for (int sg = 0; sg < 8; sg++) {
            r.pos[0] = p0; r.pos[1] = p1; r.pos[2] = p2;
            for (int k = 0; k < 3; k++) r.sgn[k] = (sg >> k) & 1 ? -1 : 1;
            process_route(&r, st);
        }
    } else {
        // decode unit -> (p0, p1) with 63 > p0 > p1 >= 0
        long u = unit; int p0 = 1; while (u >= p0) { u -= p0; p0++; } int p1 = (int)u;
        r.pos[0] = p0; r.pos[1] = p1;
        if (MODE_NDIG == 4) {
            for (int p2 = 0; p2 < p1; p2++) for (int p3 = 0; p3 < p2; p3++) for (int sg = 0; sg < 16; sg++) {
                r.pos[2] = p2; r.pos[3] = p3;
                for (int k = 0; k < 4; k++) r.sgn[k] = (sg >> k) & 1 ? -1 : 1;
                process_route(&r, st);
            }
        } else {
            for (int p2 = 0; p2 < p1; p2++) for (int p3 = 0; p3 < p2; p3++) for (int p4 = 0; p4 < p3; p4++) for (int sg = 0; sg < 32; sg++) {
                r.pos[2] = p2; r.pos[3] = p3; r.pos[4] = p4;
                for (int k = 0; k < 5; k++) r.sgn[k] = (sg >> k) & 1 ? -1 : 1;
                process_route(&r, st);
            }
        }
    }
}

static void *worker(void *arg) {
    stats_t *st = arg; st->best = 99;
    for (;;) {
        long u = __sync_fetch_and_add(&work_next, 1);
        if (u >= work_total) break;
        if (u % NPARTS != PART) continue;
        do_unit(u, st);
    }
    return NULL;
}

static void selftest(void) {
    // SMHasher3 verification value for t1ha2_64 (0x8F16C948)
    uint8_t key[256] = { 0 }, hashes[8 * 256];
    for (int i = 0; i < 256; i++) { uint64_t h = t1ha2_atonce(key, (size_t)i, (uint64_t)(256 - i)); memcpy(hashes + 8 * i, &h, 8); key[i] = (uint8_t)i; }
    uint32_t v = (uint32_t)t1ha2_atonce(hashes, sizeof hashes, 0);
    if (v != 0x8F16C948u) { fprintf(stderr, "selftest FAILED: %08x\n", v); exit(1); }
    fprintf(stderr, "selftest ok (SMHasher3 verification 0x8F16C948)\n");
}

int main(int argc, char **argv) {
    if (argc < 4) { fprintf(stderr, "usage: dp5 <3T|4|4T|5|5T> <threads> <bound> [part/nparts]\n"); return 1; }
    P2inv = inv64(P2); P1inv = inv64(P1);
    selftest();
    const char *m = argv[1]; MODE_NDIG = m[0] - '0'; MODE_T = m[1] == 'T';
    NTHREADS = atoi(argv[2]); BOUND = atof(argv[3]);
    if (argc > 4) sscanf(argv[4], "%d/%d", &PART, &NPARTS);
    if (MODE_NDIG == 3) work_total = 63; else work_total = 63 * 62 / 2;
    fprintf(stderr, "mode ndig=%d T=%d threads=%d bound=%.2f units=%ld part=%d/%d\n", MODE_NDIG, MODE_T, NTHREADS, BOUND, work_total, PART, NPARTS);
    pthread_t th[64]; stats_t st[64]; memset(st, 0, sizeof st);
    for (int i = 0; i < NTHREADS; i++) { st[i].rng = 0x1234567ULL * (i + 1) ^ 0xABCDEFULL; pthread_create(&th[i], NULL, worker, &st[i]); }
    stats_t tot; memset(&tot, 0, sizeof tot); tot.best = 99;
    for (int i = 0; i < NTHREADS; i++) { pthread_join(th[i], NULL); tot.routes += st[i].routes; tot.dpruns += st[i].dpruns; tot.cands += st[i].cands; tot.realized += st[i].realized; if (st[i].best < tot.best) tot.best = st[i].best; }
    printf("DONE mode=%s routes=%ld dpruns=%ld cands<=bound=%ld realized=%ld best_est=%.2f\n", m, tot.routes, tot.dpruns, tot.cands, tot.realized, tot.best);
    return 0;
}
