# rapidhash v1.0 under the random-secret key model (row `rapid1`)

Date: 2026-09-19.  Hash: rapidhash v1.0, upstream header `rapidhash.h` at tag
`rapidhash_v1.0` (sha256 f57de5bf8eb6f0b86be9a6b3ea0893b56fd9a27709a903d21b235dbc8dc3b8b5),
default configuration (RAPIDHASH_FAST, RAPIDHASH_UNROLLED), called through
`rapidhash_internal`.  Machine: Intel Xeon 8375C, 8 cores, `nice -n 10 taskset -c 24-31`.

## 1. Secret mechanism and key size

`rapidhash.h` v1.0 exposes

    rapidhash_internal(const void* key, size_t len, uint64_t seed, const uint64_t* secret)

whose `secret` is a triplet of 64-bit words (the doc comment: "Triplet of 64-bit
secrets used to alter hash result predictably").  The public wrappers
`rapidhash_withSeed(key, len, seed)` and `rapidhash(key, len)` pass the compiled-in
constexpr array `rapid_secret[3] = {2d358dccaa6c78a5, 8bb84b93962eacc9, 4b33a62ed433d4a3}`.
A caller supplies a secret either by calling `rapidhash_internal` directly or by
compiling with a different `rapid_secret`.  Key material under the random-secret
model: 64-bit seed + 3 x 64-bit secret words = **256 bits**, all independent uniform.
(There is no `make_secret`; the only structural expectation upstream is that the
words are "chosen for performing 64-bit multiplications"; no oddness or popcount
constraint is enforced.  The paper's harness randomised the words as `r.next() | 1`;
this run uses fully uniform words, and an odd-word control is included.)

## 2. Where the secret enters (v1.0, lengths 17..48)

    seed' = seed ^ mix(seed ^ s0, s1) ^ len
    state = mix(w0 ^ s2, w1 ^ seed' ^ s1)                 // bytes 0..15
    if len > 32: state = mix(w2 ^ s2, w3 ^ state)          // bytes 16..31
    a = last16[0..8) ^ s1;  b = last16[8..16) ^ state;  (lo,hi) = a*b
    out = mix(lo ^ s0 ^ len, hi ^ s1)

with mix(A,B) = lo(A*B) ^ hi(A*B).  Every message word is XORed with hidden
uniform material before it meets any other word, so under the random-secret
model the only thing an attacker controls is the XOR difference (d0,d1) fed to a
fold whose operands are uniform.  The pair collides with probability

    P(d0,d1) = Pr_{A,B uniform}[ mix(A,B) = mix(A^d0, B^d1) ]   (+ ~2^-64)

whenever the difference sits in words that the rest of the hash does not re-read:
bytes 0..15 for len >= 32 (L = 4), bytes 0..7 for len = 24 (L = 3; bytes 8..23 are
re-read by the finalizer).  Differences confined to the finalizer (len <= 16) must
survive two folds in series (product of two such probabilities, ~2^-53 or worse).

## 3. Measurements (stages 1-2 complete; stages 3-8 still running when this snapshot was taken)

Pipeline: `run_all.sh` (logs in `logs/`, stage numbers below).  Keys: xoshiro256**
seeded by splitmix64(rngseed*2^32 + thread), 8 threads, seed and the three secret
words fully uniform.

| stage | pair | model | hits / keys | log2 rate [95% Garwood] | cap bits |
|---|---|---|---|---|---|
| 1 | row pair A (32 B, words 0,1 complemented) | seed + 3 uniform words | 38 / 2^32 | -26.75 [-27.25, -26.30] | 28.75 [28.30, 29.25] (L=4) |
| 2 | pair B24 (24 B, word 0 complemented) | seed + 3 uniform words | 32 / 2^32 | -27.00 [-27.55, -26.50] | 28.59 [28.09, 29.13] (L=3) |

Pair B24: M = 9bd4604137366abec688a63706aa4a2188d35499de169df6,
M' = 642b9fbec8c99541c688a63706aa4a2188d35499de169df6 (bytes 0..23 of the row's M, word 0
complemented).  Example key: seed 8f934f0d969d8c38, secret {b928485235f145d4, e19da6ded88cdbd0,
64eacf717f4fb9ce} -> both hash to e69e5ea677632e22.  Pair A example: seed 3879cdfddc782ad3,
secret {d0d81d65fd961dff, a9132a2f5b5d4f54, d72e7f4d4f7270f5} -> 4329ec0defb7f826.

Corroborating earlier data (paper harness, odd secret words, tools/bench/adversarial):
pair-A differential at 32 B, 24/2^31 = 2^-26.4; fold-level (M,M) 157/2^34 = 2^-26.71 and
(M,0) 147/2^34 = 2^-26.80 over uniform operands (results_fold.md).  The two pairs are
statistically tied; the L=3 pair wins on L by log2(4/3) = 0.415 bits only if its rate is
within that factor of the L=4 rate, which the pending stage-3 fold measurement (2^38
samples each) resolves to +-0.03 bits.

Pending in `run_all.sh`: stage 3 (fold (M,M), (M,0), (0,M) at 2^38), stage 4 (25 structural
differences at 2^35), stage 5 (128 single-bit neighbours of (M,M) and (M,0) at 2^33),
stage 6 (hash-level scan, lengths 1..64 and 65/80/95/96/97/112/128/143/144/145/160/192/193,
~150 structural + low-weight + random pairs per length, 2^26 keys each; floor 2^-24.4 per
candidate at 0 hits), stage 7 (controls: pair A default secret 2^30, odd secret 2^30, B24
default 2^30, 32-byte single-word complements 2^32), stage 8 (fresh 2^32 samples with
rngseed 2 for A and B24).

## 4. Reproduce

    g++ -O3 -std=c++17 -march=native -pthread rs_rapid1.cpp -o rs_rapid1
    ./rs_rapid1 selftest
    ./rs_rapid1 pair 9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c \
                     642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c 32 1 random 8
    ./rs_rapid1 pair 9bd4604137366abec688a63706aa4a2188d35499de169df6 \
                     642b9fbec8c99541c688a63706aa4a2188d35499de169df6 32 1 random 8
    ./run_all.sh          # all stages; CORES=0-7 NICE=0 ./run_all.sh on another machine
