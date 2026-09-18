# foldhash 0.2.0 — collision analysis, synthesis for the write-up

Source: workflow `wf_cc37597f-017`, journal
`/Users/ahle/.claude/projects/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd/subagents/workflows/wf_cc37597f-017/journal.jsonl`.
Four lens agents returned (`short-fold`, `long-lanes`, `key-free`, `quality-and-seeds`);
eight of twelve verifiers returned. **The workflow was stopped before all verifiers
finished**, so everything below is split into VERIFIED (a verifier result with
`reproduced=true` exists) and UNVERIFIED.

Verifiers that returned: `key-free:0/1/2`, `quality-and-seeds:0/1/2`,
`long-lanes:0`, `long-lanes:2`.
Verifiers that never returned: **`short-fold:0/1/2` and `long-lanes:1`** — the whole
`short-fold` lens is therefore unverified in its own right.

Bookkeeping caveat, stated so the table can be audited: **every** returned verifier
reported `pair_index: 0`, but four of them quote messages/claims belonging to a later
pair in their lens's list (`key-free:2` measures the quality claim, `quality-and-seeds:1`
the quality claim, `quality-and-seeds:2` the UTF-8 String pair, `long-lanes:2` the
32-byte negative result). Below, a pair is treated as verified when a verifier states
that pair's messages verbatim; under the strict index rule the same set of lenses is
covered, only the row attribution inside a lens shifts.

---

## 1. Headline rows

### 1a. foldhash-fast (`foldhash::fast`, = `hashbrown` 0.16.1 `DefaultHashBuilder`)

| field | value |
|---|---|
| m1 (hex) | `0000000000000000` |
| m2 (hex) | `ffffffffffffffff` |
| len1 / len2 | 8 / 8 bytes |
| L = ceil(max len / 8) | 1 word |
| verifier count | 2757 collisions in 2^38 uniform hidden seeds |
| ε | 2^-26.5711 (= 1.00299e-8) |
| exact Poisson 95% CI on ε | [2^-26.6255, 2^-26.5173] (Garwood, λ ∈ [2655.04, 2861.87]) |
| **bits = log2(L/ε)** | **26.5711**, CI [26.5173, 26.6255] |
| key-free | no |
| label | **EXTENSION** |
| verifier program | `/private/tmp/claude-501/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd/scratchpad/vkf1/foldhash_verify.c` (`verify:key-free:1`) |

Independent confirmations of the same pair (all `reproduced=true`, all with their own
C11 harness cross-checked bit-exactly against the unmodified crates.io crate
`foldhash "=0.2.0"`):

| verifier | hits / seeds | ε | bits | exact 95% CI on ε | program |
|---|---|---|---|---|---|
| key-free:1 | 2757 / 2^38 | 2^-26.5711 | 26.5711 | [2^-26.6255, 2^-26.5173] | `…/scratchpad/vkf1/foldhash_verify.c` |
| key-free:0 | 2712 / 2^38 | 2^-26.5949 | 26.595 | [2^-26.6497, 2^-26.5406] | `…/scratchpad/fhverify-kf0/fhverify_kf0.c` |
| key-free:2 | 696 / 2^36 | 2^-26.5571 | 26.557 | [2^-26.6663, 2^-26.4499] | `…/scratchpad/fh_verify_kf2/foldhash_verify.c` |
| quality-and-seeds:0 | 179 / 2^34 | 2^-26.5162 | 26.52 | [2^-26.7357, 2^-26.3049] | `/home/thomas-ahle/agents/foldhash-verify-quality-and-seeds-0/foldhash_verify.c` |
| quality-and-seeds:1 | 178 / 2^34 | 2^-26.524 | 26.52 | [2^-26.744, 2^-26.312] | `/home/thomas-ahle/agents/foldhash-verify-quality-and-seeds-1/verify_foldhash.c` |

(Local scratchpad prefix elided above is
`/private/tmp/claude-501/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd`.)

A second VERIFIED pair at the same length, both sides valid UTF-8, so it works as a
`String`/`&str` key: m1 = `2141214121412141` ("!A!A!A!A"), m2 = `debedebedebedebe`
("\u{7be}\u{7be}\u{7be}\u{7be}"), len 8/8, L = 1. Verifier `quality-and-seeds:2`:
2677 hits in 2^38, ε = 2^-26.6136, exact Poisson 95% CI [2^-26.6688, 2^-26.5589],
bits = 26.61 [26.559, 26.669]; program
`/home/thomas-ahle/agents/foldhash-verify-quality-and-seeds-2/verify.c`. That verifier
corrects the lens's claimed 26.48 to **26.61** and shows the choice of base word is free:
any 8-byte word and its bitwise complement works (2^63 such pairs), and the control
`00`*8 vs `ff`*8 in the same harness gives 674 hits / 2^36 = 2^-26.603.

**Label: EXTENSION.** The differential itself is prior art — it is the paper's own
wyhash/rapidhash *complement-both-words* fold differential, and the folded-multiply
family was attacked by Orson Peters, "Breaking CityHash64, MurmurHash2/3, wyhash, and
more" (2024), who is also foldhash's author. (The key-free lens notes that Peters 2024
targets the known/fixed-seed setting and the zero-absorption property, and does not
itself cover the complement-both-operands differential or analyse foldhash; the
long-lanes lens cites it as prior art for the family. Credit both, and label the
differential a reproduction.) What is new here, and what makes this an extension rather
than a reproduction, is that `hash_bytes_short` *aliases the two fold operands at
len == 8*, so the known differential lands at L = 1 word instead of L = 2 — a full bit
cheaper than the same attack on wyhash/rapidhash — plus the measured foldhash numbers.

Three-sentence mechanism (appendix-ready):

> For inputs of 1..16 bytes foldhash takes the `hash_bytes_short` path, and at length
> exactly 8 the two 8-byte windows it reads — `bytes[0..8]` and `bytes[len-8..len]` —
> are the same word `w`, so the entire 64-bit hash is the single folded multiply
> `folded_multiply(rotate_right(per_hasher_seed, 8) ^ w, seeds[1] ^ w)`, whose two
> operands are independent and uniform over the hidden seed. Replacing `w` by its
> bitwise complement therefore complements *both* operands at once, reducing the pair to
> the fold differential `fold(u,v)` versus `fold(~u,~v)`: since `~u * ~v` adds
> `t = u+v+1` to the low half of the 128-bit product and subtracts the same `t` from the
> high half, the XOR fold `lo ^ hi` is preserved exactly when the carry chain of
> `lo + t` and the borrow chain of `hi - t` agree at every bit position, which costs a
> factor 3/4 per bit plus a side condition. The attack needs no queries, no observation
> of any output and no knowledge of the seed: a single fixed pair of 8-byte keys
> collides for about 1 foldhash instance in 2^26.6 rather than the 1 in 2^64 a 64-bit
> output suggests, and because the base word cancels, all 2^63 complementary 8-byte
> pairs work equally well.

### 1b. foldhash-quality (`foldhash::quality`)

| field | value |
|---|---|
| m1 (hex) | `0000000000000000` |
| m2 (hex) | `ffffffffffffffff` |
| len1 / len2 | 8 / 8 bytes |
| L = ceil(max len / 8) | 1 word |
| verifier count | 696 collisions in 2^36 uniform hidden seeds (quality counter) |
| ε | 2^-26.5571 |
| exact Poisson 95% CI on ε | [2^-26.6663, 2^-26.4499] |
| **bits = log2(L/ε)** | **26.5571**, CI [26.4499, 26.6663] |
| key-free | no |
| label | **EXTENSION** |
| verifier program | `/private/tmp/claude-501/-Users-ahle-repos-fast-polynomials/624f2aa7-83b9-480b-aeba-96fbb5117dcd/scratchpad/fh_verify_kf2/foldhash_verify.c` (`verify:key-free:2`) |

That verifier measured `fast_write = quality_write = fast_vec = quality_vec = 696` in
every run (4 independent chunks of 2^34: 168/198/170/160). Corroboration from the other
verifiers, each on its own seed stream: `key-free:1` reports five counters all **exactly
2757** at 2^38 (bare accumulator, fast raw, fast `Vec<u8>`, fast `str`, quality);
`quality-and-seeds:0` reports 179/179/179/179/179/179 at 2^34 across all six
fast/quality × raw/`Vec<u8>`/`str` framings; `quality-and-seeds:1` reports
`quality_hits == fast_hits` in every cell and **0** quality-only collisions in ~1.48e11
(seed, key-model) trials; `quality-and-seeds:2` reports 2677 in all six framings at 2^38;
`key-free:2` reports quality-only = 0 over 2^36 + 2^33 + 2^30 draws.

**Label: EXTENSION**, same prior-art credit as 1a.

Three-sentence mechanism (appendix-ready):

> `quality::FoldHasher::finish()` is literally `folded_multiply(self.inner.finish(),
> ARBITRARY0)` — a deterministic, seed-independent post-map applied to the fast
> variant's 64-bit output, reading none of the seed material. Equal fast outputs
> therefore force equal quality outputs, so the quality collision set is a superset of
> the fast one and the 8-byte complement pair carries over verbatim at the same rate;
> the map is not injective, so quality can only ever *add* collisions, at a rate of about
> 2^-64, and none were observed. The "statistical quality" variant, which the crate
> documents as the right choice for HyperLogLog and MinHash, therefore buys exactly
> nothing adversarially: it scores the same 26.6 bits at L = 1 as `foldhash::fast`.

---

## 2. Other lens results by length

`ε` and `bits` are as reported by the measuring agent; `bits = log2(L/ε)` with
L = ceil(len/8). "V" = a verifier with `reproduced=true` covers this row; "U" = no
verifier returned for it.

| len | lens | difference | hits / seeds | ε | bits | status |
|---|---|---|---|---|---|---|
| 1..3, 4..7 | short-fold | complements (2-byte, 4-byte, 7-byte) | 0 / 2^35 each | < 2^-33.1 (95%) | — | U |
| 8 | key-free | (M,M) full complement, raw write+finish | 1367 / 2^37 | 2^-26.5832 | 26.5832 | **V** (2757/2^38, 2^-26.5711, 26.5711) |
| 8 | key-free | same pair through `HashMap<Vec<u8>>` | 1367 / 2^37 | 2^-26.5832 | 26.5832 | **V** |
| 8 | key-free | same pair, `foldhash::quality` | 172 / 2^34 | 2^-26.5737 | 26.5737 | **V** (696/2^36, 2^-26.5571) |
| 8 | quality-and-seeds | (M,M), byte-string key — lens headline | 2722 / 2^38 | 2^-26.5895 | 26.59 | **V** |
| 8 | quality-and-seeds | (M,M), `quality::RandomState` | 83 / 2^33 | 2^-26.625 | 26.63 | **V** |
| 8 | quality-and-seeds | (M,M), UTF-8 `String` pair `2141…`/`debe…` | 92 / 2^33 | 2^-26.4764 | 26.48 | **V**, corrected to 2677/2^38 = 2^-26.6136, **26.61** |
| 8 | quality-and-seeds | (M,M), foldhash's own `gen_per_hasher_seed` + real global `SharedSeed`, 8 processes | 337 / 2^35 | 2^-26.603 | 26.60 | U |
| 8 | quality-and-seeds | one-sided (M,0), `HashMap<u64>` keys 0 / `u64::MAX` | 2116 / 2^38 | 2^-26.9529 | 26.95 | U |
| 8 | short-fold | (M,M), uniform seeds — lens headline | 2689 / 2^38 | 2^-26.607 | 26.61 | U |
| 8 | short-fold | (M,M), real `SharedSeed::from_u64` seed model | 688 / 2^36 | 2^-26.574 | 26.57 | U |
| 8 | short-fold | (M,M) as `HashMap<Vec<u8>>` key | 355 / 2^35 | 2^-26.528 | 26.53 | U |
| 8 | short-fold | (M,M) as `String`/`&str` key | 298 / 2^35 | 2^-26.781 | 26.78 | U |
| 8 | short-fold | (M,M), quality variant | 320 / 2^35 | 2^-26.678 | 26.68 | U |
| 8 | short-fold | (M,M), ASCII base `password`/`8f9e8c8c88908d9b` | 359 / 2^35 | 2^-26.512 | 26.51 | U |
| 8 | short-fold | runner-up delta `~0 ^ 2`: `0000…00`/`fdffffffffffffff` | 129 / 2^35 | 2^-27.989 | 27.99 | U |
| 8 | short-fold | *outside lens*: `HashMap<u64>` (0, `u64::MAX`), real from_u64 seeds | 983 / 2^36 | 2^-26.059 | 26.06 | U |
| 8 | short-fold | *outside lens*: `HashMap<u64>`, uniform seeds | 580 / 2^36 | 2^-26.82 | 26.82 | U |
| 8 | short-fold | *outside lens*: `HashMap<u64>` keys 1000000 / !1000000 | 278 / 2^35 | 2^-26.881 | 26.88 | U |
| 9 | short-fold | (M,M) full complement (max window overlap) | 637 / 2^36 | 2^-26.685 | 27.68 | U |
| 12 | short-fold | (M,M) full complement | 675 / 2^36 | 2^-26.601 | 27.60 | U |
| 15 | short-fold | (M,M) full complement | 668 / 2^36 | 2^-26.616 | 27.62 | U |
| 16 | key-free | (M,M) full complement (also `(u64,u64)`, `u128`, `(i64,i64)` keys) | 636 / 2^36 | 2^-26.6871 | 27.6871 | U |
| 16 | quality-and-seeds | (M,M) diagonal complement | 73 / 2^33 | 2^-26.8102 | 27.81 | U |
| 16 | quality-and-seeds | one-sided, last 8 bytes complemented | 65 / 2^33 | 2^-26.9776 | 27.98 | U |
| 16 | short-fold | (M,M) full complement | 682 / 2^36 | 2^-26.586 | 27.59 | U |
| 16 | short-fold | one-sided (M,0), **real** `from_u64` seeds (FORCED_ONES helps the attacker) | 965 / 2^36 | 2^-26.086 | 27.09 | U |
| 16 | short-fold | one-sided (M,0), uniform seeds (contrast) | 572 / 2^36 | 2^-26.84 | 27.84 | U |
| 17..23 | long-lanes | no clean single-fold word complement exists (len 20 controls) | 0 / 2^35 each | — | — | U |
| 24 | long-lanes | one-sided (M,0) on lane 0's only private word — lens best | 255 / 2^35 | 2^-27.006 | 28.59 | **V** (1105/2^37, 2^-26.890, CI [2^-26.977, 2^-26.805], **28.475** [28.390, 28.562]) |
| 24 | long-lanes | same difference, valid-UTF-8 `String` pair | 269 / 2^35 | 2^-26.929 | 28.51 | U |
| **32** | long-lanes | **negative result**: complement bytes 0..8 — every word is read by two loads in the same lane, so the difference must survive two folds (predicted ~2^-53.7) | **0 / 2^37** | **bound** ≤ 2^-35.415 | ≥ 37.42 | **V** (0/2^37; exact two-sided Poisson bound ε ≤ 2^-35.117, **bits ≥ 37.117**) |
| 32 | long-lanes | (M,M) on both of lane 0's folds (bytes 0..8 and 16..24) | 0 / 2^36 | — | — | U |
| 40 | long-lanes | one-sided (M,0); shortest length in the 33..47 band that admits the attack | 283 / 2^35 | 2^-26.855 | 29.18 | U (verifier `long-lanes:2` ran an independent len-40 control: 123 / 2^34 = 2^-27.06, bits 29.38) |
| 48 | long-lanes | (M,M) via the self-overlapping fold (bytes 16..24 feed both operands) | 342 / 2^35 | 2^-26.582 | 29.17 | U |
| 64 | long-lanes | (M,M) on lane 0's first fold (bytes 0..8 and 48..56) | 309 / 2^35 | 2^-26.729 | 29.73 | U |
| 128 | long-lanes | one-sided (M,0); the one tail length with fully disjoint loads | 277 / 2^35 | 2^-26.886 | 30.89 | U |
| 129 | long-lanes | one-sided (M,0) on the 4-lane loop's first iteration | 265 / 2^35 | 2^-26.95 | 31.04 | U |
| 200 | long-lanes | (M,M) on the first 4-lane iteration (bytes 0..8 and 32..40) | 352 / 2^35 | 2^-26.541 | 31.18 | U |
| 256 | long-lanes | one-sided (M,0), top of the 4-lane band | 131 / 2^34 | 2^-26.967 | 31.97 | U |

Structural summary (unverified, from the `long-lanes` structural scan): a fold owning a
word that no other load reads exists for every length in 24..31, 40..47, 48..128 and
129..256 — i.e. every length **except 17..23 and 32..39** — and the rate is flat in the
length across the whole range, 2^-26.5..2^-27.05. Verifier `long-lanes:2` refines the
wording: at 17..23 and 33..39 a single-fold *byte* difference does exist (byte 0 is read
only by `load(0)`); **len 32 is the unique length in 17..48 where no byte difference
reaches only one folded multiply**, and the band statement is exact only for 8-byte
*word* complements.

The 32-byte row is a **bound, not a rate**, and verifier `long-lanes:2` flags explicitly
that it must not be published as foldhash's score: the score is a minimum over L, and the
neighbouring lengths it measured itself (len 24 → 28.61, len 40 → 29.38) already cap the
`fast` variant at ≤ ~28.6 bits, which the 8-byte row then lowers to ~26.6.

---

## 3. Key-free (every-seed) pairs

**No key-free pair was verified.** Every verifier that returned reported
`key_free = false` for the pair it measured, and all three `key-free`-lens verifiers
confirmed only the seed-dependent 8-byte pair. The README sentence at issue is:

> "This (plus other careful design throughout the hash function) ensures that it is not
> possible to create a list of inputs that collide for every instance of foldhash, and
> also prevents certain access patterns on hash tables going quadratric by ensuring that
> each hash table uses a different seed and thus a different access pattern."

**Within a single byte-slice key type the sentence survives, and not merely for lack of a
counterexample.** The `short-fold` lens gives a proof for the whole 1..16-byte path: for
equal lengths, `fold(u,v) = fold(u^dx, v^dy)` for all `u,v` forces `dy = 0` (take `v = 0`
and `u` with `u^dx = 1`: the left side is 0, the right side is `dy`) and then `dx = 0`
(take `u = 0`, `v = 1`), i.e. the messages are identical; for unequal lengths the same
argument forces `c = rot(c ^ d, r)` for all `c`, which fails at `c = 0` and `c = 1`. The
`long-lanes` lens found no key-free collision anywhere in 17..256. The `key-free` lens
likewise found none for a single byte-slice key type and recommends foldhash be scored
**26.6 bits in the byte-string table, not 0 bits like MUM**.

What the key-free lens actually searched (all UNVERIFIED, since its verifiers only
checked the seed-dependent headline pair):

1. **Unread bytes** — for every length 0..100000 the union of the 8-byte windows read is
   exactly `[0, len)`; 5,764,800 single-bit flips across lengths 1..1200 all changed the
   hash under at least one of 8 random seed instances; zero dead bits.
2. **Length-extension across `rotate_right(accumulator, len)`** — no two distinct lengths
   in 0..2,000,000 share even the coarse signature (`len mod 64`, number of folded
   multiplies, lane-combine flags), and no two distinct lengths in 0..20000 share the
   full symbolic (rotation, ordered op list) program. Direct probes at (7,8), (4,8),
   (1,8), (8,7) bytes: 0 / 2^31 each.
3. **Lane/round symmetry of `hash_bytes_long`** — 32-byte lane swap, 32-byte round-pair
   swap, 32-byte all-word complement, 24-byte word reversal, 17-byte complement: 0 / 2^32
   each.
4. **Broad corpus dedup** — 129,927,988 messages (2^26.95): all byte strings of length
   ≤ 3, all strings over {00,01,7f,80,fe,ff} of length 4..9, all {00,ff} strings of
   length 10..24, every single-byte edit of a fixed base at every length ≤ 512,
   window/periodic/zeroed variants, plus 2^25 random — hashed under one random hidden
   seed and deduplicated: **zero duplicates at all**.
5. **Difference scan at len 8** — 281 candidate 64-bit XOR differences (all-ones; all 64
   single bits; all 63 low runs; all 63 high runs; all 64 "all-ones minus one bit";
   0xAAAA…, 0x5555…, 0xF0F0…, 0x0F0F…, 0xFFFFFFFF00000000, 0x00000000FFFFFFFF,
   0x8000000000000001, 0x8000000080000001; 16 random) × 2^32 uniform seeds each. Only two
   produced any collision: `0xffffffffffffffff` (2^-26.51) and `0xfffffffffffffffd`
   (2^-28.83).
6. **16-byte asymmetric differences** — (ones,ones) 2^-26.91, (ones,0) 2^-27.54,
   (0,ones) 2^-27.19 at 2^31 each; nine other structured pairs 0 / 2^31.

**What the lenses did claim as key-free, all UNVERIFIED and all outside a single
byte-slice key type** (see §6): four *cross-key-type* `Hasher`-write-stream ambiguities
(255-byte `&str` vs the same 255 bytes as `&[u8]`; `1u8` = `1u16` = `1u32` = `1u64` =
`1u128`; `(u64,u64)` vs the `u128` with the same 16 LE bytes; `(7u32,9u32)` vs
`0x00000009_00000007u64`), whose root cause is that `fast::FoldHasher::finish` folds the
128-bit sponge but ignores `sponge_len`; plus two configuration-dependent families
(a fixed 16-byte pair that collides in 1000/1000 process runs with ASLR disabled, and
`SharedSeed::from_u64(u64::MAX)`, where all six words become `2^64-1`, an absorbing
element, so every key hashes to `0xffffffffffffffff`).

---

## 4. Seed entropy

`SharedSeed::from_u64` derives all six 64-bit "shared" words from a *single* u64 by
chaining three `folded_multiply(·, ARBITRARY5)` steps per word and then OR-ing
`FORCED_ONES = (1<<63)|(1<<31)|1` into each, so the 384-bit shared seed carries at most
64 bits of entropy and 61 effective bits per word; its mix has two fixed points, 0 and
2^64-1, and the code patches only the first (`from_u64(0)` → all six words
`0x8000000080000001`; `from_u64(u64::MAX)` → all six words `2^64-1`, which is absorbing
for `folded_multiply`, so every key hashes to `0xffffffffffffffff`). `generate_global_seed`
feeds that u64 from ASLR pointers plus wall-clock seconds and nanoseconds — measured over
20000 instrumented process runs on x86-64 Linux, `static_ptr - func_ptr` is a constant
(`0x4e840`) in all 20000 runs, `box_ptr - func_ptr` takes only 7446 distinct values,
`subsec_nanos` is ~30 bits and `secs` is attacker-known — while `gen_per_hasher_seed` is
`fm(fm(stack_ptr, ARBITRARY1 ^ thread_local_chain), ARBITRARY2)`, a stack address that
spans at most 2^30.0 reachable values with ASLR on, exactly 1 distinct value in 20000 runs
with ASLR off, and a thread-local chain whose orbit closes after λ = 6,519,491,933 =
2^32.60 hashers. **The measured rate does not change under the real seed model**: the
headline pair gives 668/2^36 = 2^-26.6163 (verifier `key-free:0`, model B), 688/2^36 =
2^-26.5737 (`key-free:1`), 72/2^33 = 2^-26.830 (`key-free:2`), 194/2^34 = 2^-26.4001 with
FORCED_ONES (`quality-and-seeds:0`), 224/2^34.39 = 2^-26.585 with real `from_u64`
(`quality-and-seeds:1`), and 337/2^35 = 2^-26.603 against foldhash's *own* generators in
8 real processes (lens, unverified) — all statistically indistinguishable from the
uniform-seed figure, so the generous model is not what makes the attack work. (Unverified
counter-current worth flagging: both the `short-fold` and `quality-and-seeds` lenses
measure the *one-sided* complement family getting 1.7–2.0× **better** under real seeding,
because FORCED_ONES pins bit 63 and bit 0 and forces the overflow branch the (M,0)
collision needs — 2^-26.09 vs 2^-26.84 at 16 bytes, 2^-26.12 vs 2^-26.89 at 64 bytes.)

---

## 5. Call-sequence conventions

Three conventions were modelled and measured throughout, and all three were validated
bit-for-bit against the real crate:

- **raw `Hasher::write(bytes)` + `finish()`** — the reference convention.
- **`HashMap<Vec<u8>>` / `HashSet<&[u8]>`** — `impl Hash for [T]` calls
  `write_length_prefix(len)`, which foldhash does *not* override, so it becomes
  `write_usize(len)` into the 128-bit sponge, then `write(bytes)`; `finish()` then returns
  `folded_multiply(len ^ accumulator, seeds[0])`. (This corrects the workflow brief, which
  assumed a plain `write(bytes); finish()`.)
- **`String` / `&str`** — on stable Rust the default `write_str` appends a `0xff`
  terminator, i.e. `write(bytes); write_u8(0xff); finish()`. foldhash overrides `write_str`
  only under its `nightly` feature, which drops the `0xff` and makes this identical to the
  byte-string case.

**For equal-length pairs the convention does not matter.** The collision happens in the
accumulator, and the length prefix, the `0xff` terminator, the final sponge fold and
quality's extra `folded_multiply(·, ARBITRARY0)` are all the *same* deterministic
post-map applied to both sides, so every accumulator collision survives. This was
measured, not assumed: `quality-and-seeds:0` got 179/179/179/179/179/179 across all six
fast/quality × raw/`Vec<u8>`/`str` framings at 2^34; `key-free:1` got exactly 2757 on five
counters at 2^38; `key-free:2` got 696 on all four of its counters at 2^36;
`quality-and-seeds:2` got 2677 in all six framings at 2^38; `long-lanes:2` got 0 in both
framings at 2^30 and 2^37. The formal relation is `hits_finish ≥ hits_accumulator` (the
final fold could merge two distinct accumulators, at ~2^-64 per seed), so "identical" is
right in practice and was observed exactly.

Two places where the convention *does* matter: (i) **unequal-length pairs do not survive
as `HashMap<Vec<u8>>` keys**, because the length prefix separates them — every pair
reported here is equal-length; (ii) `ff`*8 is not valid UTF-8, so the headline pair
applies to `Vec<u8>`/`&[u8]` keys and to anything going through `Hasher::write`, and a
`String`-key attack needs the UTF-8 pair `2141214121412141` / `debedebedebedebe` (or the
24-byte `c280c281c282c283…` / `3d7f3d7e3d7d3d7c…` pair), at the same rate.

---

## 6. Claims to discard as unverified

Every item below is a lens claim with **no returned verifier**. Keep it out of the
write-up, or mark it explicitly as unverified.

**Entire `short-fold` lens** (all three of its verifiers were killed before returning).
The 8-byte 00/ff pair itself survives, because it was independently verified through the
`key-free` and `quality-and-seeds` lenses — but these `short-fold` numbers and statements
do not:
- its own headline count 2689 / 2^38 = 2^-26.607 and every other row in its table;
- the 9-, 12-, 15- and 16-byte rows, and the 16-byte one-sided (M,0) row claiming
  FORCED_ONES improves the attack to 2^-26.086;
- the `HashMap<u64>` / `write_num` rows (2^-26.059 real seeds, 2^-26.82 uniform,
  2^-26.881 for keys 1000000 / !1000000) — note these are outside that lens's own scope
  and were never assigned to a lens that verified them;
- the runner-up delta row `fdffffffffffffff` at 2^-27.989;
- the claimed *proof* that no key-free pair exists in the 1..16-byte path (plausible and
  short, but unchecked);
- the unequal-length bound ε ≤ 2^-56 and the 0-hit unequal-length screens;
- the "LEAD FOR THE WRITE-UP, EXPLICITLY UNMEASURED" suggestion that wyhash's and
  rapidhash's 4..8-byte paths may also drop from L = 2 to L = 1 — the lens flags this as
  unmeasured itself; do not put it in a table.

**`long-lanes`** (`long-lanes:1` never returned):
- every row at 40, 48, 64, 128, 129, 200 and 256 bytes;
- the 24-byte valid-UTF-8 `String` pair (`c280c281c282c283…` / `3d7f3d7e3d7d3d7c…`);
- the structural map claim that a single-fold difference exists at every length except
  17..23 and 32..39 — `long-lanes:2` shows the wording is wrong as stated (it holds for
  8-byte *word* complements, and len 32 is the unique length in 17..48 with no
  single-fold *byte* difference);
- the negative controls at len 20 and the 32-byte (M,M)-on-both-folds run;
- the claim that FORCED_ONES makes the attack ~2× worse at len 64 (2^-26.04 vs 2^-26.91).

**`key-free`** (its three verifiers all checked the seed-dependent 8-byte pair only):
- **all four cross-key-type key-free families** — the 255-byte `&str` vs `&[u8]` pair, the
  `1u8` = `1u64` family, the `(u64,u64)` vs `u128` family and the `(7u32,9u32)` vs
  `0x00000009_00000007u64` family. These are the most quotable claims in the whole
  workflow and none of them was verified. Note also the lens's own caveat: they are
  ambiguities in the `Hasher` write stream *across key types*, not two `Vec<u8>` keys in
  one `HashMap<Vec<u8>,V>`, so they do not license scoring foldhash at 0 bits;
- the proposed fix (XOR `sponge_len` into the second operand of the final fold) and its
  measured effect;
- the 16-byte row (636 / 2^36) and the asymmetric-difference table;
- the exhaustive search inventory in §3 above (unread bytes, length-extension signatures,
  lane symmetry, the 129,927,988-message corpus, the 281-difference scan).

**`quality-and-seeds`** (`quality-and-seeds:2` verified the String pair;
`quality-and-seeds:0/1` the fast and quality headline; the rest is unverified):
- the two key-free claims — the ASLR-disabled fixed 16-byte pair
  `8835a2a68a5d69c50000000000000000` / `8835a2a68a5d69c51111111111111111` colliding in
  1000/1000 runs, and `SharedSeed::from_u64(u64::MAX)` collapsing every key to
  `0xffffffffffffffff`;
- the 16-byte diagonal and one-sided rows, and the `HashMap<u64>` one-sided row
  (2116 / 2^38);
- the real-generator run (337 / 2^35 against foldhash's own `gen_per_hasher_seed` and
  global `SharedSeed`);
- the FORCED_ONES isolation table (b|bit31 1.14×, b|bit63 1.29×, b|bit0 1.45×,
  b|FORCED_ONES 1.86×, real seeds[1] 1.98×; 471 vs 277 at 2^35);
- the weak-seed popcount table (weight 3 → 2^-6.6 … weight 16 → 2^-23) and the
  Pr[popcount ≤ 16] = 2^-18 figure;
- **all entropy measurements** — the 20000-run ASLR study, the ≤ 2^30.0 stack-pointer
  bound, the `static_ptr - func_ptr` constant, the zero-entropy `default-features=false` +
  `setarch -R` result, and the Brent cycle λ = 2^32.60. The lens itself flags the wasm32
  zero-entropy case as an unverified structural inference;
- the exhaustive reduced-width searches (w = 8/10 over all (α,β); the quality post-map
  characterisation at w = 16/20/24/28) and the claim that
  `quality::SeedableRandomState::with_seed` / `FixedState::with_seed` silently collapse
  distinct user seeds.

**Corrections to carry even for verified rows:**
- 24 bytes: use **ε = 2^-26.890, bits = 28.475** (verifier, 1105/2^37), not the lens's
  2^-27.006 / 28.59.
- 32 bytes: use the **exact two-sided Poisson bound ε ≤ 2^-35.117, bits ≥ 37.117**, not
  the lens's rule-of-three 2^-35.415 / 37.42 — and publish it as a structural remark, not
  as a score.
- 8-byte UTF-8 `String` pair: use **26.61**, not the lens's 26.48.
- The `short-fold` and `quality-and-seeds` lenses both write "(3/4)^63 = 2^-26.1, matching
  the measured 2^-26.59". Verifier `quality-and-seeds:0` shows the arithmetic is
  incomplete: (3/4)^63 is the probability of carry/borrow lock-step alone, and the
  collision additionally requires a side condition of marginal probability 2/3 (0.791
  conditioned on lock-step), giving (3/4)^63 × 0.791 = 2^-26.486. Verifier
  `quality-and-seeds:2` gets the same place with (3/4)^64 = 2^-26.5624 and an exact
  algebraic reduction. Do not print the bare (3/4)^63 = 2^-26.1 line.
- `long-lanes` writes the one-sided high half as "h-d+O(1)"; verifier `long-lanes:0`
  derives `d-h-c`, so it is the borrow chain of `d-h-c` that must be the bit-complement of
  the carry chain. The rate is unaffected.
- `key-free` writes that quality's post-map means "a few extra appear, since
  `fm(·, ARBITRARY0)` is not injective". No quality-only collision was ever observed
  (0 in 1.29e10 and 0 in ~1.48e11 trials); the extras are ~2^-64.

**Operational caveat for the missing verifiers.** Two agents reported killing sibling
processes on the shared Xeon with pattern-matching `pkill` (`key-free:2` ran
`pkill -f "foldhash_verify measure"` and killed a process in
`~/agents/foldhash-verify-key-free-0`; the `key-free` lens ran `pkill -x measure` and
killed another lane's `measure` job), and `key-free:0` reports its scratchpad source being
overwritten mid-run by an unrelated concurrent process. The four missing verifier results
should be assumed lost to that and to the workflow stop, not to a negative finding.
