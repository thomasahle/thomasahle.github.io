# muse-v2: witness search against MuseAir algorithm v2 (crate 0.6.0)

Target: the current upstream MuseAir, algorithm v2, crate `museair` 0.6.0 (tag `crate-0.6.0`
= f3092ae, 2026-07-13). Default seeding model of the crate: `museair::hash(bytes, seed)`,
64-bit output, one uniformly random 64-bit `seed`; `bfast::hash` is the BFast variant with the
same signature; `hash128(bytes, seed_a, seed_b)` takes two 64-bit seeds. The row on the page
(`muse`, MuseAir v0.3, 1.59-bit cap from a seed-free tail) does not transfer to v2: the v2 tail
products XOR the seed into the multiplicand, so the v0.3 pairs give 0/2^24 on v2 (fairness memo).

Compute: Xeon 8375C (`<xeon-host>`), `nice -n 10 taskset -c 24-31`, under
`<xeon-work>/witness/muse-v2/` (shared cores; other lanes' jobs ran on the same range). Total
wall time about 50 min (22:26–23:16 UTC, 2026-09-18). This Mac only edited files. The port
`museair2.h` was validated against the crate itself: `genvec` links the crate-0.6.0 worktree
and emits 200,000 vectors (lengths 0..511, all four functions, `UNROLL` parity asserted);
`check_vectors` reports 0 mismatches.

## Result

**Best pair (adopt as the v2 witness): 32 bytes, L = 4 words, collision rate 2^-17.45, cap
19.45 bits [19.43, 19.47].**

```
M  = 0000000000000000000000000000000000000000000000000000000000000000
M2 = 00000000000000404a048402a910048a00000000000000a80000000000000000
```

M2 differs from M in head word i (bytes 0..7, LE) by `0x4000000000000000`, in head word j
(bytes 8..15) by `0x8a0410a90284044a`, and in tail word u (bytes 16..23) by `0xa8 << 56`,
i.e. u bits 59, 61, 63.

| sample | RNG seed | `hash` | `bfast::hash` | `hash128` | `bfast::hash128` |
|---|---|---|---|---|---|
| screening 2^30 | 0x1001 | 6066 → 2^-17.433 [-17.470, -17.397] | 6066 | 6030 | 6030 |
| confirmation 2^32 (fresh) | 0xC0FFEE | 23952 → 2^-17.452 [-17.470, -17.434] | 23952 | 24150 | 24150 |
| confirmation 2^31 (fresh) | 0xBEEF | 12061 → 2^-17.442 [-17.468, -17.416] | 12061 | 12206 | 12206 |

Pooled `hash` over the three streams: 42079 / (2^30 + 2^32 + 2^31) = 2^-17.447
[2^-17.460, 2^-17.433]; cap = log2(4) + 17.447 = **19.447 bits [19.43, 19.46]**, displayed
upward as ≤ 19.45 (upper bound on the metric; L = 4 is the pair's own length). Both fresh
samples were drawn after the pair was fixed; nothing was selected on them.
The 64-bit standard and BFast variants collide on exactly the same seeds (the differential is
in the pre-finalizer state); the 128-bit variants collide at the same rate on a different seed
set (their tail multiplicand is `(C4 + seed_a) ^ u`, additive seed entry).

Example seed (screening stream): `seed = 0x040963434fe5e368`, `hash(M) = hash(M2) =
0xef46cda19c0bbc67`, `bfast::hash = 0x8d303f6a6db80174`. Confirmation stream:
`0xacac861647d5f7f2 → 0xb1f37635e72a255f`. Seed 0 does not collide.

Runner-up (L = 3): 19 bytes, `M = 0^19`, `M2 = 48898050201582401100000000000000000015`
(head i ^= `0x4082152050808948`, head j ^= `0x11`, u bits 0, 2, 4 = byte 18 ^= 0x15):
4096 / 2^30 = 2^-18.000 [-18.045, -17.956] (rng 0x1001) and, fresh, 4097 / 2^30 = 2^-18.000
(rng 0xC0FFEE; `hash128` 4011); cap log2(3) + 18.0 = 19.58 bits. Slightly weaker than the
32-byte pair, kept as the L = 3 record. Example: seed `0xc191a8c344a44fa9` →
`hash = 0x4080bbeb0f4cde82`, `bfast::hash = 0x81cd3d71251906c7`.

Exact secondary (long path): 40 bytes, class density exactly 2^-32 (cap 34.3 bits) — see
"Long path" below. Not adopted (weaker), but it is exact and explains the long-path floor.

## Mechanism (why 2^-17.4 and not 2^-64)

v2 short path, 17 ≤ len ≤ 32 (`hash_short_64`):

```
(i, j)  = read_short(bytes[0..16])                      # exact XOR of the head words
(i, j) ^= wmul(C2 ^ seed ^ len, C3 ^ len)                # same for both messages
(u, v)  = read_short(bytes[16..len])
(lo0, hi0) = wmul(C4 ^ seed ^ u, C5);  (lo1, hi1) = wmul(C6 ^ seed ^ v, C7)
i ^= lo0 ^ hi1;  j ^= lo1 ^ hi0;  then a seed-free finalizer
```

Flipping a set δ of bits of u changes X = C4 ^ seed ^ u by m = Σ_{k∈δ} ±2^k (each sign is one
seed bit), so the 128-bit product X·C5 changes by exactly ±m·C5 — an *additive* difference by
a public constant. Its XOR image equals a fixed pattern D with probability 2^-w, w = number of
carry mismatches in the most likely carry chain (a 2-state DP; ≈ NAF weight of m·C5, top bit
free). The head enters by XOR only, so `head' = head ^ D` completes a colliding pair with
probability 2^-((|δ|-1) + w). C5 = 8·odd and C7 is odd; the `read_short` windows overlap for
tail lengths < 16, which restricts which bits of u/v can be flipped alone.

`dpsearch.py` ranks (word, m, shift k, minimal length) by log2(L) + (naf(m)-1) + w over all odd
m < 2^20 with NAF weight ≤ 3 and m < 2^12 with NAF weight ≤ 4 (61k + 81k candidates). The
winner is m = 21 = 1 + 4 + 16 (21·C5 has carry weight 16-17, versus 21-22 for C5 itself):

| word | m | k | w | naf(m)−1 | len | L | predicted | measured (`hash`) |
|---|---|---|---|---|---|---|---|---|
| u | 21 | 59 | 16 | 2 | 32 | 4 | 2^-18 → 20.00 | 2^-17.45 → **19.45** |
| u | 21 | 0 | 17 | 2 | 19 | 3 | 2^-19 → 20.58 | 2^-18.00 → 19.58 |
| u | 21 | 48 | 17 | 2 | 18 | 3 | 2^-19 → 20.58 | 2^-18.93 → 20.52 |
| v | 33 | 58 | 17 | 1 | 25 | 4 | 2^-18 → 20.00 | 2^-18.35 → 20.35 |
| v | 33 | 0 | 18 | 1 | 19 | 3 | 2^-19 → 20.58 | 0 / 2^30 (DP tie-break incompatible with the forced parity of Y) |
| u | 1 | 63 | 21 | 0 | 32 | 4 | 2^-21 → 23.00 | 2^-21.19 → 23.19 |
| u | 1 | 0 | 22 | 0 | 19 | 3 | 2^-22 → 23.58 | 2^-21.11 → 22.70 |

The uniform-Y model is only approximate (Y = X·C5 is not uniform and the sign bits are
correlated with Y's top bits), so the top 135 DP candidates (predicted ≤ 21.7 bits) were
re-measured at 2^26 seeds each (`logs/screen_all.txt`); the 32-byte m = 21, k = 59 pair was
best there too (376 / 2^26).

Model-free check: `mode` takes every XOR value 1..255 of one tail byte and reports the modal
pre-finalizer state difference over 2^24 (len 32, byte 23) or 2^22 seeds (the other boundary
bytes). At len 32 byte 23 the best value is exactly 0xa8 with the DP's D (108 / 2^24, next best
0x54 at 57). At len 19 byte 18 the sweep cannot separate patterns at 2^-18 (16 expected hits);
its apparent leader (D with bit 63 set, 19 vs 16) measured 2122 / 2^30 against 4096 / 2^30 on
the same seed stream — noise, not adopted.

## Bounded empirical scan (lengths 1..64)

`scan`: for every message bit at every length, the modal XOR difference of the pre-finalizer
state over 2^22 seeds (len ≤ 32) / 2^18 seeds (len 33..64, the (i,j,k) triple before the final
three products). Summary (`logs/scan_short.txt`, `logs/scan_long.txt`):

- len ≤ 16: every bit flip gives a deterministic state difference (head-only XOR path); a
  fixed same-length pair cannot collide before the 128→64 fold (floor 2^-64). No witness.
- len 17..32, head bits: deterministic (same). Tail bits: the modal counts are 4-6 / 2^22
  for single-bit flips at len 18/19 (≈ 2^-20), consistent with the DP's single-bit
  predictions and below the m = 21 multi-bit pairs above. No zero-difference (exact) event.
- len 33..64 (12,416 cells, 2^18 seeds each): the maximal modal count is 1 in every cell,
  i.e. all 2^18 sampled state differences were distinct for every single-bit flip, and no
  cell produced a zero difference. No differential or exact structure below the 32-bit seed
  conditions of the zero-product classes.
- Runner-up C's event is not a low-bit residue class (`exact_c`: flipping random seed bits
  above bit 16..32 changes the indicator as often as for independent events), so its two
  counts of 4096 and 4097 per 2^30 are coincidence, not exactness.

## Long path (len > 32): exact 2^-32 class

Every initial state word carries exactly 32 seed bits (`state_seed64` masks the seed with
0xAAAA…/0x5555…). In `hash_loong_finalize` the tail product `wmul(state[4] ^ t0, state[5] ^ t1)`
vanishes when `state[4] ^ t0 = 0`, i.e. `seed & MASK_A = (C4 ^ t0) & MASK_A` with
`(C4 ^ t0) & MASK_B = 0`; then t1 and t2 may change by a common Δ (state[5] ^= t1 ^ t2 is
unchanged) without changing anything downstream. At len 40 the tail (bytes 8..39) does not
touch the first product's words, so:

```
M  = 0000000000000000 433b9f7c55b97c04 0000000000000000 0000000000000000 0000000000000000
M2 = 0000000000000000 433b9f7c55b97c04 efcdab8967452301 efcdab8967452301 0000000000000000
```

collides for exactly the 2^32 seeds with `seed & MASK_A = 0` (`class40`: 2^20 / 2^20
class-sampled seeds on `hash` and `bfast::hash`; 0 / 2^30 uniform, expected 0.25). Cap
log2(5) + 32 = 34.3 bits. The same trick on the first compress product needs len > 96
(L ≥ 13). No long-path event with fewer than 32 seed bits was found: every message word
enters at least one product whose co-multiplicand carries 32 seed bits.

## Not found / ruled out

- No seed-independent (every-seed) pair on v2 at any length ≤ 64: the v0.3 mechanism (public
  tail products) is gone, len ≤ 16 is head-XOR only, and the long path needs 32-bit seed
  conditions.
- BFast 64-bit: its finalizer `wmul(i^C8, j^C9)` is not injective (zero or swapped operands),
  but forcing either needs a full 64-bit seed condition for a fixed pair.
- Weak-seed classes above 2^-32 density: none found.

## Files and reproduction

All commands were run in `<xeon-work>/witness/muse-v2/` on the Xeon; sources are in this
directory, logs in `logs/`.

```
# 1. build the crate-linked vector generator (path dep on the crate-0.6.0 worktree) and validate the C port
(cd genvec && cargo build --release)
gcc -O2 -std=c11 -o check_vectors check_vectors.c
./genvec/target/release/genvec 200000 > vectors.txt          # sha256 77814203…f561a1
./check_vectors < vectors.txt                                # "vectors 200000, mismatches 0, max len 511 -> PASS"

# 2. DP candidate search and explicit pairs
python3 dpsearch.py 20 3 > logs/dp_a.txt; cp dp_top.json dp_top_a.json
mkdir -p dpb && (cd dpb && python3 ../dpsearch.py 12 4 > ../logs/dp_b.txt)
python3 - <<'EOF'
import json; json.dump(json.load(open("dp_top_a.json"))+json.load(open("dpb/dp_top.json")), open("dp_merged.json","w"))
EOF
python3 mkpairs.py dp_merged.json > logs/pairs.txt

# 3. measurements (8 threads)
gcc -O3 -march=native -std=c11 -pthread -o measure measure.c -lm
./screen.sh > logs/screen.txt                                  # 2^30 screening, rng 0x1001
python3 screen_all.py 21.7 26 > logs/screen_all.txt            # top-135 re-ranking at 2^26, rng 0x5eed
./measure 00…00 (64 hex zeros) 00000000000000404a048402a910048a00000000000000a80000000000000000 32 0xC0FFEE 8   # fresh 2^32
./measure 00…00 (64 hex zeros) 00000000000000404a048402a910048a00000000000000a80000000000000000 31 0xBEEF 8     # fresh 2^31
./measure 00…00 (38 hex zeros) 48898050201582401100000000000000000015 30 0xC0FFEE 8                            # runner-up, fresh

# 4. model-free sweeps, bounded scan, long-path class, examples
gcc -O3 -march=native -std=c11 -pthread -o mode mode.c && ./mode 32 23 24 8 > logs/mode_len32_byte23.txt; ./modes2.sh
gcc -O3 -march=native -std=c11 -pthread -o scan scan.c && ./scan 1 32 22 6 > logs/scan_short.txt; ./scan 33 64 18 6 > logs/scan_long.txt
gcc -O3 -march=native -std=c11 -o class40 class40.c -lm && ./class40 30 20 0x2026 > logs/class40.txt
gcc -O2 -std=c11 -o at_seed at_seed.c && ./at_seed <M> <M2> 0x040963434fe5e368 0x040963434fe5e368
```

`measure` reports a Poisson 95% CI on the count (Wilson-Hilferty). RNG: splitmix64 per thread,
stream `rngseed ^ (golden * (thread+1))`; seeds uniform 64-bit, `seed_b` an independent draw.
