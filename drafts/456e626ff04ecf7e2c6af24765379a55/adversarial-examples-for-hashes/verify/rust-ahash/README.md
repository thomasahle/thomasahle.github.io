# rust-ahash: fixed message pairs that collide with probability 2^-19.7 over a random key

Companion package for the blog post.  One file, `ahash_pairs.c`, C11, no dependencies
beyond libc/libm; it validates its aHash implementation against SMHasher3 at startup,
then hashes the pairs below under millions of random secrets.

## The hash

[aHash](https://github.com/tkaitchuck/aHash) (Tom Kaitchuck, MIT/Apache-2.0), crate
`ahash` version 0.8.12, the hasher behind `hashbrown`-based maps in many Rust programs.
Its secret is a `RandomState {k0,k1,k2,k3}` of four 64-bit words.  The crate has two
code paths, selected at compile time (`src/lib.rs`):

* **AES path** (`src/aes_hash.rs`): one inverse AES round (`aesdec`) plus a
  byte-shuffle-and-add per 16-byte block.  Compiled only when the `aes` target feature
  is on (x86-64: `-C target-feature=+aes` or `target-cpu=native`; aarch64 additionally
  needs the crate feature `nightly-arm-aes`).
* **fallback path** (`src/fallback_hash.rs`): one folded 64x64->128 multiply per block.
  These measurements cover the modeled folded-multiply path. Platform-specific
  alternatives and the ARM nightly AES path are outside the runtime checks.

The variant reproduced here is exactly what SMHasher3 (`hashes/rust-ahash.cpp`) tests as
`rust_ahash` (AES path, verification 0x3BF4383B) and `rust_ahash_fb` (fallback,
0x53C9F167): the 64-bit hash of a byte slice `&[u8]`, i.e. `write_usize(len)` followed
by `write(bytes)`, keyed directly by the four RandomState words.  SMHasher3 seeds it as
`k_j = PI2[j] ^ seed`, which is `RandomState::with_seeds(seed, seed, seed, seed)`.
The program was written from the crate source (constants `PI2` and `SHUFFLE_MASK` are
the crate's) and checked against the SMHasher3 port; it contains no third-party code.

Two secret models are sampled: **rs4**, four independent uniform words (an ideal
key model, not a proof of the initializer’s distribution), and **smh**, one uniform
64-bit seed expanded the SMHasher3 way. The AES rates are consistent; fallback
observations differ and remain unresolved.

## The pairs

**Pair A (AES path, 56 bytes, rate 2^-19.7, verifier-confirmed).**  A 56-byte message is
absorbed as four blocks v1=[0,16), v2=[16,32), v3=[24,40), v4=[40,56); each `hash_in`
does `enc <- aesdec(enc) ^ v` and `sum <- shuffle(sum) + v`.  The pair differs in bytes
0 and 10 of v1 (xor 7e, 65), which InvShiftRows places in a single column; the
inverse-S-box output differences (probability 4/256 each) are chosen so that
InvMixColumns leaves a column that v2 cancels except for one byte, which passes a third
S-box (2/256) and is cancelled by four bytes of v3, while the additive `sum` lane
cancels through the shuffle.  Total 4/256 * 4/256 * 2/256 * ~2^-0.7 (byte carries)
= approximately 2^-19.7 over the sampled keys. The selected historical run counted
2616/2^31 collisions; with L = 7 its score estimate is 22.454208..., displayed
as 22.45*. The search scripts and the reported 64-byte-pair experiment are not bundled.

**Pair B (fallback path, 16 bytes, rate ~2^-26.2, panel-measured, not independently
confirmed).**  A 16-byte message is one block (w0, w1); the only data mixing is
`fold((w0^k2) * (w1^k3))` with `fold(p) = lo64(p) ^ hi64(p)`.  Complementing both words
turns a*b into (~a)(~b) = a*b + (a+b+1) - 2^64 (a+b+2) (mod 2^128), so the low and high
halves of the product move by nearly opposite amounts and the xor-fold cancels for about
2^-26 of the keys (panel: 55/2^32 = 2^-26.22; a second run 12/2^30 = 2^-26.42).  These observations concern
the modeled folded-multiply path only.  It sits below the
resolution of the default 2^24 run; `./ahash_pairs 30 1 B` (about 15 s, fallback only)
resolves it, and this binary finds 7/2^30 = 2^-27.19 in both secret models, about half
the panel's figure (see the larger runs below).

## Build and run

```
# aarch64 (Apple silicon, Graviton, ...)
cc -O2 -march=armv8-a+crypto -o ahash_pairs ahash_pairs.c -lm
# x86-64
cc -O2 -maes -o ahash_pairs ahash_pairs.c -lm
# any CPU: portable software AES, about 10x slower.  With no flags at all the compiler may
# still pick the hardware path (Apple clang on arm64 defines __ARM_FEATURE_AES by default,
# and the first line of the output says which backend was chosen); -DAHASH_SOFT_AES forces
# the software path.
cc -O2 -DAHASH_SOFT_AES -o ahash_pairs ahash_pairs.c -lm

./ahash_pairs            # 2^24 secrets per model, rng seed 1
./ahash_pairs 26 7       # 2^26 secrets, rng seed 7
./ahash_pairs 30 1 B     # pair B only
```

The program aborts unless both SMHasher3 verification values are reproduced, prints
the pairs, checks the explicit colliding secret from the records, and then samples N
secrets per model.  The RNG is its own splitmix64-seeded xoshiro256**, so a run is
reproducible from (log2 N, seed).  Single-threaded; the default run takes ~2 s with
hardware AES.  An unknown pair tag or a non-numeric argument is rejected with exit
status 2.

## Expected output (`./ahash_pairs`, Apple M2 Pro, clang 17; only the `s` timings vary)

```
rust-ahash: aHash 0.8.12 as tested by SMHasher3 (hashes/rust-ahash.cpp); AES backend: ARMv8 AES instructions
  rust_ahash     SMHasher3 verification 0x3BF4383B (registered 0x3BF4383B) OK
  rust_ahash_fb  SMHasher3 verification 0x53C9F167 (registered 0x53C9F167) OK
N = 2^24 random secrets per model, rng seed 1

== pair A: rust_ahash (AES path), 56-byte messages
  m1 = 40313233343536373839253b3c3d3e3f408142094445464748494a4b4c4d4e4f8d896d065c5d5e5f606162636465666768696a6b6c6d6e6f
  m2 = 3e313233343536373839403b3c3d3e3f405e42204445464748494a4b4c4d4e4f727290085c5d5e5f606162636465666768696a6b6c6d6e6f
  xor= 7e00000000000000000065000000000000df0029000000000000000000000000fffbfd0e0000000000000000000000000000000000000000
  mechanism: 3-S-box differential trail through hash_in: the byte differences in block 1 (bytes 0,10)
  are chosen so that InvShiftRows puts them in one column and InvMixColumns of the inverse-S-box
  outputs (DDT 4/256 each) leaves one byte, which block 2 cancels except for one byte (DDT 2/256)
  that block 3 cancels; the sum lane cancels additively through the shuffle.  Predicted
  4/256 * 4/256 * 2/256 * (carry factor ~2^-0.7) = 2^-19.7; unconditional over the keys.
  explicit colliding secret: k0..k3 = 711096b41e1b7d99 59af3b7ae3b7d595 d2fb75eb5658a771 5a0f91d8ed1ffe38
    H(m1) = 45ef7682d72b19b6  H(m2) = 45ef7682d72b19b6  COLLIDE
  model rs4 (four uniform keys k0..k3)      : collisions/N = 26/2^24 = 2^-19.30
    0.8 s;  first colliding secret: k0..k3 = 26f10121bf2cf5bb 3d32a8e232ec9e5e 6a6f9182eaa86b6d 0d8f8a1098f9d71f
    H(m1) = H(m2) = b06f3670babedffb
  model smh (one uniform seed, k_j = PI2[j]^seed): collisions/N = 11/2^24 = 2^-20.54
    0.7 s;  first colliding secret: seed = f04f16d5122a6a3a, k0..k3 = b56737332afa794d 4e1b701a26c36656 30e33f62db563ae7 cfcbc360a76d632d
    H(m1) = H(m2) = 2dc0adbf4570e056

== pair B: rust_ahash_fb (fallback path), 16-byte messages
  m1 = 0123456789abcdef0011223344556677
  m2 = fedcba9876543210ffeeddccbbaa9988
  xor= ffffffffffffffffffffffffffffffff
  mechanism: complement both words of a one-block message: the single folded multiply sees (~a)(~b) =
  a*b + (a+b+1) - 2^64*(a+b+2) mod 2^128 with a = w0^k2, b = w1^k3, so the low and high halves
  of the product move by nearly opposite amounts and lo^hi cancels for about 2^-26 of the keys
  (panel: 55/2^32 = 2^-26.2; below the resolution of a 2^24 run, see README).
  explicit colliding secret: k0..k3 = 64721c4a2c381d36 6b5648524261eaab b59e91f82c7d1e12 94c815ea45f3d238
    H(m1) = 82d7a0f7495678e3  H(m2) = 82d7a0f7495678e3  COLLIDE
  model rs4 (four uniform keys k0..k3)      : collisions/N = 0/2^24  (none observed in 2^24 trials; no population bound)
    0.1 s;  no collision found in this run
  model smh (one uniform seed, k_j = PI2[j]^seed): collisions/N = 0/2^24  (none observed in 2^24 trials; no population bound)
    0.1 s;  no collision found in this run
```

Poisson noise at these counts is +-20-30%; the panel's 2^30-trial figures for pair A are
1227/2^30 = 2^-19.74 (rs4) and the verifier's independent implementation gave 2616/2^31
= 2^-19.65 (rs4) and 1260/2^30 = 2^-19.70 (smh).

Larger local runs of the same binary (`./ahash_pairs 26 <seed>`):

```
pair A, rng seed 1:  rs4 94/2^26 = 2^-19.45      smh 68/2^26 = 2^-19.91
pair B, rng seeds 1..4 pooled (4 x 2^26 per model):
                     rs4  2/2^28 = 2^-27.0       smh  1/2^28 = 2^-28.0
pair B, ./ahash_pairs 30 1 B (2^30 per model, 13 s):
                     rs4  7/2^30 = 2^-27.19      smh  7/2^30 = 2^-27.19
```

Pooled over those runs this binary sees 17 pair-B collisions in 2^31.3 trials, a rate of
about 2^-27.2, where the panel's 2^-26.22 predicts 34.8 (Poisson P(X <= 17) about 0.0006).
The mechanism is not in doubt (the explicit secret collides, and 2^-27 is 2^37 above the
2^-64 of an ideal hash), but the rate of pair B should be read as "between 2^-26 and 2^-27"
until the discrepancy between the two samplers is understood.

## License

`ahash_pairs.c` and this README: MIT, Copyright (c) 2026 Thomas Dybdahl Ahle.
aHash itself is (c) Tom Kaitchuck, MIT OR Apache-2.0; the two constants copied from it
are listed in the source.  Pair data: the panel records (`pairs_heur.json`) for this
hash; the verification procedure follows SMHasher3 `lib/Hashinfo.cpp`.

## Native Rust check

The `native/` Cargo program hashes byte slices against aHash 0.8.12. Run on an x86 host:

```sh
cd native
RUSTFLAGS="-C target-feature=+aes" cargo run --release -- 34 3 7
```

To reproduce an explicit internal key k0..k3, call `RandomState::with_seeds(k_i ^ PI2[i])`; independent uniform arguments give independent uniform internal words. The recorded native run gives 19,964/2^34, cap 22.52 bits. The same stream against master a9d649d agrees; the independent program gives 19,884/2^34, recorded as two 2^33 runs in ../../records/fairness-pass/ahash-verify/. The default C reproduction takes about 2 seconds on Apple M2 Pro and about 6 seconds per model on a loaded Xeon 8375C.

The 0.8.12 release additionally gates ARM AES on `nightly-arm-aes`. Master dropped that gate in PR #268 (2025-05-06), before the release, but the 0.8.12 release merge did not carry it.

The recorded seven-thread run takes floor(2^34 / 7) samples per thread (17,179,869,182 trials); the log rounds this to 2^34. The two-sample difference does not affect the displayed score.
