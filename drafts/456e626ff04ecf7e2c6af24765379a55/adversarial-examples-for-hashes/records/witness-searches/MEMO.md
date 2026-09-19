# Witness-search adoption memo (2026-09-19)

Scope: five witness searches run on the Xeon against the blog's heuristic-hash panel
(`~/repos/website/blog/adversarial-examples-for-hashes/data.json`). Adoption rule: only
witnesses confirmed by the independent verifier (upstream source, own harness, fresh RNG).
Numbers below are the verifier's upstream-linked figures unless marked (searcher).
Field-level patches are in `patches.json` next to this file (entries {id, field, new_value,
records_to_copy, reason}); `records_to_copy` lists the scratchpad directories to copy into
`records/witness-searches/` before the page cites them.

## Summary

| row | previous cap | new cap | pair | rate (upstream verifier) | verdict |
|---|---|---|---|---|---|
| t1ha (t1ha2_atonce-64 v2.1.4) | 31.0 | **29.19** | F60, 13/16 B, L=2 | class 2^-24 x cond 2^-4.193 (58,715,203/2^30) = 2^-28.19; fresh uniform 226+258 / 2x2^36, all in class | ADOPT |
| xxh3-64 (0.8.3) | 23.01 | **12.47** [12.473, 12.476] | NAF block-1 complement, 32 B, L=4 | 2,264,081 / 3x2^30 = 2^-10.4745 | ADOPT |
| ahash (0.8.12 AES) | 22.52 | **22.18** [22.16, 22.21] | A_2, 56 B, L=7 | 12,627 / 2^33 = 2^-19.376 (native crate) | ADOPT |
| muse-v2 (crate 0.6.0) | none (v0.3 row: 1.59) | **19.45** [19.43, 19.46] | 0^32 vs 00..40 4a04..8a 00..a8 00.., 32 B, L=4 | 36,060 / 1.5x2^32 = 2^-17.447 | ADOPT as NEW ROW |
| a5 (a5hash v5.21) | 47.81 | 47.81 | unchanged | 55 (len, v) points exhausted, best alternative 48.22 | KEEP (adopt=false, no verdict) |

## Per-row notes

### t1ha -> 29.19 bits (was 31.0)
Pair F60: m1 = c4c0cc2284cd239ede36a18fa6 (13 B), m2 = fbc31866049ec02dea0c4e2be580d3cf (16 B).
Seed class (L & a5845081f808f44a) == 2080000118003408 on L = lo(((seed ^ l0) + t) * P1), density
exactly 2^-24. Conditional rate against upstream (erthink/t1ha HEAD 00eb779, src/t1ha2.c
byte-identical to v2.1.4): 2^-4.1928 +- 0.0004 at 2^30 class samples, so contribution 2^-28.193 and
cap log2(2) + 28.19 = 29.19. Fresh uniform runs: 226/2^36 and 258/2^36 (verifier), 217/2^36
(searcher); 701 hits in 3x2^36 = 2^-28.13 [2^-28.24, 2^-28.02]; every hit inside the class. The 258
is a +2.3 sigma fluctuation; report 2^-28.19 / 29.2, not the pooled uniform point. Example seed
4c240c2749cd4915 -> c35b49165b1a4480. Found by exhaustive DP over all 224.9M 5-digit routes; 6-digit
routes not enumerated (gain <= 0.5-1 bit extrapolated).

### xxh3-64 -> 12.47 bits (was 23.01; fairness-memo pair would have been 17.16)
M = 0^16 || 51151210404400000000008204000105, M' = 0^16 || aeeaedefbfbbffffffffff7dfbfffefa. Bytes
16..31 are little-endian (P, N) = the +1/-1 digit sums of the NAF of D = K2+K3+1 =
0xfaff443b8e121551 (K2, K3 = default-secret words for block 1); M' complements the block. Mechanism:
each 16-byte block contributes fold(w0^(K[2i]+s), w1^(K[2i+1]-s)), avalanche bijective, so the pair
collides iff fold(A,B) = fold(~A,~B); the seed-shifted carry masks agree except at NAF digit
positions. Verifier (upstream xxhash.c v0.8.3 linked as a separate TU, own sampler): 755087, 754660,
754334 per 2^30 -> pooled 2^-10.4745, Clopper-Pearson [2^-10.4763, 2^-10.4726]; searcher pooled
2^-10.4746. Deterministic witness seed 2468b3bc26a44073 -> dd686b61e2f6dc69 (seed 0 does NOT collide
for this pair). Local optimum under 192 single moves (2^28) and 18,336 two-move neighbours (2^24);
search result, not a bound. Same-program controls: row pair 490/2^30, memo pair 29507/2^30.

### ahash -> 22.18 bits (was 22.52)
Pair A_2 (56 B): same 3-S-box trail {0,10}->{1,3}->bytes 32..35 as the shipped pair, with the
additive-lane carry loss minimised (2^-0.69 -> 2^-0.38). Native upstream crate 0.8.12
(RandomState::with_seeds, four uniform u64): 12627/2^33 = 2^-19.376 [2^-19.401, 2^-19.351], cap
22.183 [22.158, 22.208]; fresh 2^31: 3124 (2^-19.391); searcher's C port 3178/2^31 (2^-19.366).
Paired gain over the shipped pair on the same key stream: 0.349 bits, z = 22. Caveats from the
verifier: A_1 is statistically tied (pooled A_1 19154 vs A_2 18929 over 2^34, z = 1.15), so cite
~22.18 [22.16, 22.21] and do not claim A_2 > A_1; the 64-byte mirror B_0 reaches 2^-19.38 but L=8
puts it at 22.38; confirm_pairs.txt line 2 is mislabelled (stale 56-byte non-colliding pair). An
explicit colliding key for A_2 must be lifted from the verifier's logs before the row ships (the
shipped key 711096b4... belongs to the old pair).

### muse-v2 -> NEW ROW at 19.45 bits (v0.3 row stays at 1.59)
Algorithm v2 (crate 0.6.0) had no witness: the v0.3 every-seed pair gives 0/2^24 on v2. 32-byte
pair M = 0^32, M2 = 00000000000000404a048402a910048a00000000000000a80000000000000000 (head XORs
0x4000000000000000 / 0x8a0410a90284044a, tail bits 59/61/63). Mechanism: for 17..32 B the tail
enters as wmul(C4^seed^u, C5); flipping a signed-digit set of u changes the product by +-21*C5
(carry weight 16), whose XOR image equals a fixed pattern with probability ~2^-16; XORing that
pattern into the head completes the collision. Verifier (crates.io tarball museair-0.6.0.crate,
own Rust harness): 23869/2^32 and 12191/2^31, pooled 2^-17.447 [2^-17.462, 2^-17.432]; searcher
pooled 2^-17.447 (z = -0.03). Cap log2(4) + 17.447 = 19.447; display 19.45 (nearest; CI upper 19.46).
bfast::hash collides on identical seeds; hash128/bfast::hash128 at the same rate under a two-seed
model. Example seed 0x040963434fe5e368 -> 0xef46cda19c0bbc67; seed 0 does not collide. Exact
40-byte class (seed & 0xAAAA...AAAA = 0, 2^-32, cap 34.3) holds for the 64-bit functions only and is
not the adopted witness. Proposed as a separate row `muse-v2` (verdict: "a NEW v2 entry, not a
replacement of the v0.3 row"); chart_eligible false until a v2 speed is registered (SMHasher3 ships
v0.3 only). The v0.3 row gets text-only touch-ups (drop "current v2 untested").

### a5 -> no change (47.81)
Searcher exhausted all 55 feasible (len, v) dead-operand points (exact class enumeration, every
control 0): closest len 23 (168,320 seeds, 2^-46.64, L=3, cap 48.22), len 18 (48.67); memo's len
4567 settled exactly at 50.16; no heavy S2_init value at 2^-28 resolution. adopt=false, no verdict
requested. Records worth keeping: scratchpad/design/witness-searches/a5hash-lengths/ (summary_table.txt,
logs/) -> records/witness-searches/a5hash-lengths/ as a negative result, if the page cites searches.

## Follow-ups outside data.json (not in patches.json as field edits)
- Copy the four search + three verify directories into `records/witness-searches/` (paths in patches).
- verify/t1ha2-64: new pair, class mask/value, expected block (searcher's indep_check.c/verify_lib.c, verifier's vu.c).
- verify/xxh3-64: adopt xxh3_64_pair_check.c (sha256 1d5b05ee...), regenerate expected/CHECK_32B.md, note seed 0 is not a witness.
- verify/rust-ahash: add A_2 (and A_1/B_0 alternates) to ahash_pairs.c and native/; fix the mislabelled confirm_pairs.txt line.
- verify/museair-v2 (new): museair2.h + measure.c + at_seed.c or the verifier's Rust program; MANIFEST.json entry.
- Run figure/build.py after data.json + profiles.json change (chart pipeline rule); Appendix A figures/pseudo-code/wasm widgets for t1ha, xxh3-64, ahash need the new pairs; a muse-v2 appendix section is new.
- Disclosure: MuseAir #4, aHash #292, xxHash #1127 and the t1ha2 email draft quote the old figures; update before/at publication.
- Prose "bits" in index.html for these rows (table, hover, TOC) are generated from data.json in the content pass; do not hand-edit figure markup.
