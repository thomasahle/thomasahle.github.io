# ahash-trail: independent native verification of the proposed witness A_2 (2026-09-19)

Verifier: my own Rust program (src/main.rs) linking the UPSTREAM crate `ahash = "=0.8.12"` from
crates.io (Cargo.lock checksum 5a15f179cd60c4584b8a8c596927aadc462e27f2ca70c04e0071964a73ba7a75), built
with RUSTFLAGS="-C target-feature=+aes,+ssse3" (program prints cfg aes=true, ssse3=true => AES path).
Not the searcher's C port.  Seed protocol = the row's rs4 model: RandomState::with_seeds(a,b,c,d) with four
uniform u64 arguments (= four uniform internal words, since k_i = arg_i ^ PI2[i]); hash = hash_one(&[u8]).
RNG: per-thread xoshiro256** seeded by splitmix64 from (rngseed*0x9E3779B97F4A7C15) ^ (t*0xD1B54A32D192ED03),
a seeding scheme distinct from the searcher's sampler and from the row's native program => fresh keys.
Sanity: the row's explicit key 711096b4... collides the shipped pair (H = 45ef7682d72b19b6) in this build.

Host: Xeon 8375C, rustc 1.92.0, 8 threads, nice -n 10 taskset -c 24-31, <xeon-work>/witness/ahash-trail/native-verify/.
All pairs were hashed on the SAME key stream in each run (paired comparison).

## Fresh 2^31 sample (rngseed 77, 47.7 s)          [records/confirm_31_seed77.log]
  A_2 (proposed)  3124/2^31 = 2^-19.391  95% CI [2^-19.442, 2^-19.341]   cap log2(7)+19.391 = 22.198 [22.148, 22.250]
  A_1 (runner-up) 3241/2^31 = 2^-19.338  95% CI [2^-19.388, 2^-19.289]   cap 22.145 [22.096, 22.196]
  shipped         2501/2^31 = 2^-19.712  95% CI [2^-19.769, 2^-19.656]   cap 22.519 [22.464, 22.577]   (row: 2^-19.715, consistent)
  B_0 64 B mirror 3105/2^31 = 2^-19.400  95% CI [2^-19.451, 2^-19.350]   cap log2(8)+19.400 = 22.400 [22.350, 22.451]

## Larger 2^33 sample (rngseed 78, 206 s)           [records/confirm_33_seed78.log]
  A_2            12627/2^33 = 2^-19.376  95% CI [2^-19.401, 2^-19.351]   cap 22.183 [22.158, 22.209]
  A_1            12778/2^33 = 2^-19.359  95% CI [2^-19.384, 2^-19.334]   cap 22.166 [22.141, 22.191]
  shipped         9895/2^33 = 2^-19.728  95% CI [2^-19.756, 2^-19.699]   cap 22.535 [22.507, 22.564]
  B_0 64 B       12580/2^33 = 2^-19.381  95% CI [2^-19.407, 2^-19.356]   cap 22.381 [22.356, 22.407]

## Pooled (my 2^31 + 2^33 = 1.25 * 2^33 keys)
  A_2 15751 = 2^-19.379 [2^-19.401, 2^-19.356], cap 22.186;  with the searcher's 2^31 added: 18929/2^33.58 = 2^-19.377, cap 22.184.
  A_1 16019 = 2^-19.354 [2^-19.377, 2^-19.332], cap 22.162;  pooled with searcher: 19154/2^33.58 = 2^-19.360, cap 22.167.
  A_2 vs shipped: gain 0.349 bits, z = 22 (paired stream).  A_1 vs A_2: 225 counts apart over 2^34, z = 1.15 -> indistinguishable.

## Verdict
CONFIRMED.  A_2 reproduces at 2^-19.38 against the upstream crate on fresh keys (proposal: 2^-19.366 [-19.417,-19.317]);
cap 22.18-22.20 bits vs the current row's 22.52; the shipped pair reproduces at the row's rate as an in-run control.
The 64-byte mirror B_0 confirms at ~2^-19.38 too but with L = 8 its cap is 22.38, so it does not beat A_2.
Note A_1 is statistically tied with A_2 (both ~2^-19.36..-19.38); the proposal's A_2 > A_1 ordering is within noise.

## Reproduce
  ssh <xeon-host>; cd <xeon-work>/witness/ahash-trail/native-verify
  RUSTFLAGS="-C target-feature=+aes,+ssse3" cargo build --release
  nice -n 10 taskset -c 24-31 ./target/release/ahash_trail_verify pairs_confirm.txt 31 77 8   # 2^31, ~50 s
  nice -n 10 taskset -c 24-31 ./target/release/ahash_trail_verify pairs_confirm.txt 33 78 8   # 2^33, ~3.5 min
  (./run_confirm.sh does both.)  Sources: Cargo.toml, src/main.rs, pairs_confirm.txt here; sha256 in records/sha256_and_env.txt.
  pairs_confirm.txt = the searcher's confirm_pairs.txt with line 2 relabelled B_0_STALE56B (see issues) plus the 64 B B_0 from pairs_B.txt.
