# muse-v2 witness: independent verification against the ORIGINAL upstream source

Verifier: a separate Rust program (`src/main.rs`, `Cargo.toml`) that links the published
crate `museair` 0.6.0 fetched from crates.io (`https://static.crates.io/crates/museair/museair-0.6.0.crate`,
sha256 `e07cb15b6e86f28d77f1c9a79ae9f1f12670941f8cc81bacefb4038ef987314e`; its `src/lib.rs`
sha256 `123772c6360a31ef29f5d525d87d8f7738c8e8d883b284a6b20dd29fb1d42b1c`, byte-identical to the
git worktree at tag `crate-0.6.0` = f3092ae, "Prepare release crate 0.6.0", 2026-07-14, origin
github.com/eternal-io/museair). The searcher's C port (`../museair2.h`) was NOT used. The crate's own
`cargo test --release stability_v2` passes on the same tree (hashverify vectors 0x7140CABC ... 0x9BAAAF63).

Seed protocol (same as the page's muse row): one uniformly random 64-bit `seed`, `museair::hash(bytes, seed)`;
`bfast::hash` same seed; `hash128`/`bfast::hash128` with `(seed_a, seed_b)` both uniform (independent draws).
RNG: xoshiro256** seeded per thread by splitmix64 from a master value (0x20260919 and 0x7A5E1D), i.e. a
different generator and different streams from the searcher's (splitmix64, 0x1001 / 0xC0FFEE / 0xBEEF).
Compute: Xeon 8375C, `nice -n 10 taskset -c 24-31`, 8 threads, `<xeon-work>/witness/muse-v2-verify/`;
total wall 2.6 min (23:20:25–23:23:00 UTC 2026-09-18). This Mac only edited files.

## Results (`logs/`)

Pair (32 bytes, L = 4):
```
M  = 0000000000000000000000000000000000000000000000000000000000000000
M2 = 00000000000000404a048402a910048a00000000000000a80000000000000000
```

| sample | master | `hash` | `bfast::hash` | `hash128` | `bfast::hash128` |
|---|---|---|---|---|---|
| 2^32 (fresh) | 0x20260919 | 23869 → 2^-17.457 [2^-17.476, 2^-17.439], cap 19.457 [19.439, 19.476] | 23869 | 24234 → 2^-17.435 | 24234 |
| 2^31 (fresh) | 0x7A5E1D | 12191 → 2^-17.426 [2^-17.452, 2^-17.401], cap 19.426 [19.401, 19.452] | 12191 | 12153 → 2^-17.431 | 12153 |
| pooled (mine) | | 36060 / 6442450944 = 2^-17.447 [2^-17.462, 2^-17.432], cap **19.447 bits [19.43, 19.46]** | | | |

Searcher's pooled: 42079 / 7516192768 = 2^-17.447 [2^-17.460, 2^-17.433]. Two-sample Poisson z = -0.03:
the independent measurement agrees exactly. All five streams pooled: 78139 / 13958643712 = 2^-17.447
[2^-17.457, 2^-17.437], cap 19.447 [19.437, 19.457]. Standard and BFast 64-bit collide on identical seeds
(same first colliding seed, 0xed1b9315431cefca); the 128-bit variants collide at the same rate on a different set.

Examples reproduced on the genuine crate: seed 0x040963434fe5e368 → hash 0xef46cda19c0bbc67 (both),
bfast 0x8d303f6a6db80174 (both); seed 0xacac861647d5f7f2 → 0xb1f37635e72a255f; seed 0 does NOT collide
(b5b9cc9d70e8f847 vs 8f8b1e9c2c5b13f0).

Runner-up (19 B, L = 3, M = 0^19, M2 = 48898050201582401100000000000000000015): 4019 / 2^30 = 2^-18.027
[2^-18.072, 2^-17.983], cap 19.61 [19.57, 19.66]; pooled with the searcher's two streams 12212 / 3·2^30 = 2^-18.009,
cap 19.59. Example seed 0xc191a8c344a44fa9 → 0x4080bbeb0f4cde82 reproduced. Weaker than the 32 B pair, as claimed.

Exact 40 B class (L = 5): 2^20 / 2^20 on `hash` and `bfast::hash` for seeds with `seed & 0xAAAA…AAAA = 0`
(VERIFY_MASK), 0 / 2^30 uniform (expected 0.25); `hash128` variants 0 / 2^20 in the class (the claim covers only
the 64-bit functions). Cap log2(5) + 32 = 34.3 bits, weaker; matches the searcher's class40 log.

Verdict: CONFIRMED. The 32 B pair is a genuine v2 witness at ε = 2^-17.45 under the row's uniform-64-bit-seed
model, cap 19.45 bits (point 19.447; 95% CI [19.43, 19.46]). Nothing in this pass beats it; no new search was run.

## Reproduce (Xeon, `<xeon-work>/witness/muse-v2-verify/`)

```
mkdir -p upstream && cd upstream && curl -O https://static.crates.io/crates/museair/museair-0.6.0.crate && sha256sum museair-0.6.0.crate && tar xzf museair-0.6.0.crate && cd ..
(cd upstream/museair-0.6.0 && cargo test --release stability_v2)      # crate self-test: ok
cd verify && cargo build --release                                     # Cargo.toml: museair = { path = "../upstream/museair-0.6.0", default-features = false }
./run_all.sh                                                           # writes logs/main_2p32.txt main_2p31.txt runnerup19_2p30.txt class40.txt
# single run: nice -n 10 taskset -c 24-31 ./target/release/verify <M_hex> <M2_hex> <log2N> <master_hex> <threads> [example seeds...]
# seed-class run: VERIFY_MASK=0xAAAAAAAAAAAAAAAA ./target/release/verify ...   (seed &= !mask)
```
CI: Poisson 95% on the count (Wilson–Hilferty), converted to log2 rate and to bits = log2(L) − log2(ε).
