# rapidhash v3 under the random-secret key model

Row `rapid3` (rapidhash v3, SMHasher3 verification 0x1FDC65EE). Study date 2026-09-19.

## 1. Secret mechanism and key size

Upstream `rapidhash.h` (tag `rapidhash_v3`, sha256 807874ee…626e) exposes

```c
static inline uint64_t rapidhash_internal(const void *key, size_t len, uint64_t seed, const uint64_t *secret);
static inline uint64_t rapidhashMicro_internal(...same...);   /* secret[5], secret[6] unused */
static inline uint64_t rapidhashNano_internal(...same...);
```

The public wrappers `rapidhash(key,len)`, `rapidhash_withSeed(key,len,seed)` (and the Micro/Nano
variants) call `*_internal` with the shipped array `rapid_secret[8]`
(`{0x2d358dccaa6c78a5, 0x8bb84b93962eacc9, 0x4b33a62ed433d4a3, 0x4d5a2da51de1aa47,
0xa0761d6478bd642f, 0xe7037ed1a0b428db, 0x90ed1765281c388c, 0xaaaaaaaaaaaaaaaa}`).
A caller-supplied secret is therefore an array of **8 x 64 = 512 bits** passed to the header-visible
`*_internal` function (the doc comment still says "Triplet"; the code indexes `secret[0..7]`;
`secret[7]` only enters the final `rapid_mix`).  Together with the 64-bit seed the random-secret
key model has **576 bits** of uniform hidden key material.  There is no `make_secret`-style
generator in the header; SMHasher3's `rapidhash.cpp` also hard-codes the array.

Where the key enters (standard variant, lengths 17..112, `i` = length):

```
seed0 = seed ^ mix(seed ^ s2, s1)
seed1 = mix(w0 ^ s2, w1 ^ seed0)                    (i > 16)
seed2 = mix(w2 ^ s2, w3 ^ seed1)                    (i > 32)   ... s1/s2 alternate up to 112
a = load64(p + i - 16) ^ i ^ s1;  b = load64(p + i - 8) ^ seed_last;  (lo, hi) = a * b
h = mix(lo ^ s7, hi ^ s1 ^ i)
```

Every message word is XORed with a secret word or the seed-derived state before it reaches a
multiply, so under this model no message difference cancels before key material is mixed in.
The only fixed-pair mechanism that survives is a differential (dA, dB) on the two operands of one
`rapid_mix` whose inputs are otherwise unchanged: the pair collides iff
`fold(A, B) = fold(A ^ dA, B ^ dB)` for the uniform operands A, B, where `fold(A,B) = lo(AB) ^ hi(AB)`.

## 2. Files

- `rapid3_port.h` — the hash bodies (rapidhash v3 standard/micro/nano, unprotected, unrolled) from the
  blog's `verify/rapidhash-v3/rapidhash_v3_verify.c` (SMHasher3-validated); lines 1..410 of that file.
- `rs_rapid3.c` — measurement harness (OpenMP).  Modes: `pair`, `sweep`, `fold10`, `climb`, `foldrate`.
- `check_witness.c` — asserts the port reproduces the row's recorded default-secret witness.
- `ci.py` — exact (Garwood) 95 % Poisson interval -> log2 rate and bits.
- `run_all.sh`, `run_stage1b.sh`, `run_stage2.sh` — the exact command sequence; `logs/` — their outputs.

## 3. Reproduce

```sh
gcc -O3 -march=native -fopenmp -o rs_rapid3 rs_rapid3.c -lm
gcc -O2 -o check_witness check_witness.c && ./check_witness
A=9bd4604137366abec688a63706aa4a2188d35499de169df633e0964e8c04600c
A2=642b9fbec8c99541397759c8f955b5de88d35499de169df633e0964e8c04600c
P24=9bd4604137366abec688a63706aa4a2188d35499de169df6
P24b=642b9fbec8c99541c688a63706aa4a2188d35499de169df6
./rs_rapid3 pair 34 202 $A $A2 8            # row pair A, seed + 8 secret words uniform
./rs_rapid3 pair 34 204 $A $A2 8 default    # row pair A, seed uniform, shipped secret
./rs_rapid3 pair 34 201 $P24 $P24b 8        # 24-byte (M,0) pair, random secret
./rs_rapid3 pair 34 203 $P24 $P24b 8 default
./rs_rapid3 foldrate 64 ffffffffffffffff 0 38 102 8         # primitive P(M,0)
./rs_rapid3 foldrate 64 ffffffffffffffff ffffffffffffffff 38 101 8   # primitive P(M,M)
./rs_rapid3 fold10                          # exhaustive XOR differentials, 10-bit fold
./rs_rapid3 climb 32 22 31 6 8              # hill-climb at 32 bits from structured/random starts
./rs_rapid3 sweep 27 24 20 41 8             # structural sweep, lengths 1..64 + block boundaries
python3 ci.py COUNT LOG2TRIALS L
```

`pair` arguments: log2 keys, RNG seed, the two messages in hex, threads, optional `default`.
The RNG is xoshiro256** seeded per thread by splitmix64 from the RNG seed; every key draws the
64-bit seed and then the eight secret words.  Run on an x86-64 Xeon 8375C, 8 threads on shared cores.

## 4. Results

See `RESULTS.md` (tables with exact Poisson intervals) and `logs/`.  Summary: the row's pair
collides at the same ~2^-26.5 rate with the secret random as with the shipped secret (pooled
532 / 2^35.6, cap 28.56 [28.44, 28.69]); the best pair found is a 24-byte word-0 complement
(pooled 428 / 2^35.6 = 2^-26.87, cap 28.46 [28.32, 28.60]; primitive rate 2^-26.95 -> 28.53),
statistically tied with the current pair.  Row score under the random-secret model: 28.5 bits (cap).
No key-free or sub-2^-25 mechanism exists at L <= 2 among the candidates tried (all 0 / 2^27).

Run history: `run_all.sh` (stage 1; its untrimmed sweep was stopped after 99 candidates, kept as
`logs/08_sweep_untrimmed_partial.txt`, and replaced by `run_stage1b.sh`'s three-tier sweep),
`run_stage2.sh` (2^38 primitive rates and 2^34 confirmations; the redundant (0,M) primitive run was
skipped since fold(u,q) = fold(q,u) makes P(0,M) = P(M,0) exactly), `run_stage3.sh` (outlier
re-check, second fresh 2^35 samples, second primitive sample).
