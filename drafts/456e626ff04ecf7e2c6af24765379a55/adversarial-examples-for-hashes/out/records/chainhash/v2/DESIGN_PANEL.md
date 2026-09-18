# ChainHash-x86 level-1 design panel — ranking and recommendation

Judged against the author's goal: at least as fast as HalftimeHash-512 (19.29 B/TSC) and
XXH3-64 (19.69) on the Xeon 8375C under the SMHasher3 bulk protocol, with a proved
ideal-key bound of the paper's shape and per-word score >= 62 bits.
All figures are bytes per TSC cycle (TSC = 2.9 GHz; measured core clock 3.36-3.43 GHz on the
light AVX-512 licence, so 1 core cycle = 0.863 TSC cycles).

## Verdict in one line

**None of the three candidates wins on its own merits; the winner is the by-product of
candidate C — a shuffle-free *adjacent-pair* carry-less PH.** Candidates A (VPMULUDQ NH) and
B (IFMA-52 NH) are rejected: both are slower than the already-shipped ChainHash-1k (16.36)
and both add the integer-NH proof burden. Candidate C's dual engine is also rejected, but its
port measurements show the shipped ZMM PH stage was giving away 37% of the CLMUL ceiling to
`VSHUFI64X2` lane fixups that exist only to preserve the strided pairing.

## Ranking

### 1. Adjacent-pair carry-less PH, ZMM VPCLMULQDQ, no shuffles (candidate C's by-product) — ADOPT

| item | value |
|---|---|
| level-1 stage, 256 KiB | **31.98-33.35** (pure CLMUL instruction stream 37.15) |
| shipped strided layout, same harness | 19.98 |
| level 1 + recurrence proxy, one recurrence / 1024 B | 26.06-26.18 |
| full hash, Codex lane, B=1024 S=1 | **24.84 / 24.78** (two passes) |
| controls, same box | XXH3-64 19.69-19.82, HalftimeHash-512 19.24-19.39 |
| new lemmas | **zero** |
| key | 137 words = 1096 B at B=1024 (1 key word per 8 message bytes, exactly the paper's rate) |
| short-input path | unchanged; small-key average 104.3 cycles |

Why it is fast, from the measured port model: `VPCLMULQDQ zmm` is 1.7227 TSC for 4 products
(2.00 core cycles) and engages **both** 512-bit pipes — the "clmul is p5-only" hypothesis is
false on this part (xmm delivers 1 product/core cycle, zmm delivers 4 per 2 cycles; a single
port cannot do that). Exactly one extra 512-bit ALU uop per `VPCLMULQDQ zmm` is free
(`+8 vpxorq` and `+8 vpaddd` cost nothing: 1.706 both ways), while `+8 vshufi64x2` costs
2.575 = the exact sum of the two run alone. Cross-lane shuffles are p5-only and serialise
fully behind clmul. Removing them is therefore not a micro-optimisation, it is the whole gap:
19.98 -> 31.98.

Proof cost is zero because the appendix's CLNH lemma is stated for an arbitrary fixed pairing
`pi` applied to key and data alike. Only the layout definition changes, not a single
probability step: `clnh_difference_bound` (full width), `clnh_natDegree_le` (degree <= 126, so
the low/high split into F x F is injective), `clnh_nested_nonzero_bound` (unequal active pair
sets, nonzero target from the length XOR), then `lem:ph:injective`, `lem:ph:sz`,
`lem:ph:level2`, `lem:ph:chain`, `lem:ph:finalizer`, `lem:ph:twist`, `thm:ph:collision`,
and the 5-wise independence theorem — all verbatim. The Lean work is re-indexing
`ByteEncoding.lean` and `KeyLayout.lean` to the adjacent pairing and to 137 words; no new
probability, integer-NH or reduction lemma.

Cost to be explicit about: `pi` is part of the function definition, so this is a **new hash
family**, not an x86 tweak. Digests and seed verification constants differ from
chainhash-256/chainhash-1k on every platform. The one piece of good news is that the adjacent
pairing is also the natural NEON layout (PMULL/PMULL2 pair lane j with lane j), so the ARM
path should need no shuffles either — but the M2 must be re-measured before the M2 lane is
switched, and nothing on the Mac was touched by any panelist.

### 2. Candidate A — two-lane integer NH over 32-bit words, VPMULUDQ on ZMM — REJECT

Full verified spec 16.23 (B=1024), 17.44 at 4 KiB blocks; product stage alone 18.41-18.49.
The ceiling is exact and is a port count: per 64 message bytes, per lane, one `VPADDD`,
one `VPSRLQ`, one `VPMULUDQ`, one `VPADDQ` = 4 p0/p5 uops, doubled for the second lane that
2^-64 demands = 8 uops = 4 core cycles = 16 B/core-cycle = 18.5 B/TSC. That is **below
HalftimeHash-512 before a single byte of fold or recurrence is paid**. The one-lane reference
(eps = 2^-32 only) runs 28.51, which is exactly why XXH3 and HalftimeHash can reach 19.3-19.7:
they pay one multiply stream, not two. Proof cost is also the highest of the three: integer NH
almost-Delta-universality over Z_{2^64} with mod-2^32 key addition is standard (BHKKR,
CRYPTO'99) but entirely new for this Lean project — none of `Carryless.lean` transfers,
because the difficulty is the mod-2^w wraparound, and the both-words-differ case (naive
rectangle count gives 2^{-w+1}, truth is 2^-w) is the bulk of the work. Key is 2120 B
(1104 with Toeplitz) against the paper's rate, and full zero padding of the last block is
load-bearing (integer NH's nested-pair bound is 2^-31, not 2^-32, since
Pr[(m+k)(m'+k') = 0 mod 2^64] = (2^33-1)/2^64), which forces a separate short path below 1024 B.
Verified byte-identical on 4297 lengths; the work is sound, the design is simply capped.

### 3. Candidate B — two-lane integer NH over 52-bit limbs, AVX-512 IFMA — REJECT

Level-1 stage alone 16.26 (1 KiB blocks drop to 11.08 on a gcc codegen cliff; 14.39 forced-
unrolled), best end-to-end full hash 12.81 — below the shipped ChainHash-1k. IFMA fuses the
multiply and accumulate (3 uops per group instead of 4), but the parse is the wall: a
no-unpack parse gets only lambda = 4 message bytes per 52-bit limb, and a fully packed radix-52
parse needs a byte-granular cross-lane gather (`VPERMI2B`, 2 core cycles), landing at ~17.3
B/core-cycle ~ 16.7 B/TSC. Beating 19.3 would need lambda >= 6.03 at u = 1 and no 1-uop
instruction moves bits across qword lanes. On top of that, IFMA density costs clock:
3.37 GHz at 0% IFMA, 2.80 at 50%, 2.29 at 87.5%, measured with a dependent add chain — the
carry-less stage keeps the light licence, which is worth ~17% on its own. Key 525 words
(4200 B), i.e. 4 key bytes per message byte streamed from L1. Proof cost is lower than A
(NH cited at w = 52, one new elementary IFMA-exactness lemma, length carried as data so plain
AU suffices) but the speed is not there.

### Candidate C's dual engine itself — REJECT

The two engines do not overlap. `8 clmulZ + 8 mulx` costs 2.580 = the exact sum of the two
alone; the vector partner fares no better at any mix ratio. Paired A/B, best of 21 interleaved
trials: dual/PH = 0.970 (2048+64), 0.952 (2048+128), 0.980 (4096+64), 0.957 (1024+64),
0.930 (512+64); 50/50 gives 23.98 against PH's 33.35. Marginal NH bytes cost 2.03x what PH
bytes cost. The one niche where it should have ridden free — the recurrence-latency-bound
256-byte configuration — failed too (256 PH + 64 NH with recurrence 13.63 vs 256 PH alone
16.00), because folding the NH accumulator to one word needs two p5-only cross-lane extracts
sitting on the feedback path. Its epsilon analysis is nevertheless worth keeping on file: the
disjoint-case argument (PH differs / PH equal and NH differs / both equal, lengths differ)
gives 2^-64 with no reduction term, where the lazy union bound would have lost a bit.

## Scoreboard

| design | measured full hash | level-1 ceiling | new lemmas | key bytes | short path |
|---|---:|---:|---|---:|---|
| adjacent-pair PH (recommended) | **24.84** | 33.35 (37.15 raw) | 0 | 1096 | unchanged |
| shipped ChainHash-1k | 16.36 | 19.98 | 0 | 328 | unchanged |
| shipped ChainHash-256 | 15.40 | 19.3 (xmm) | 0 | 328 | unchanged |
| A: NH-32 x2 | 16.23 | 18.49 | 3 (one hard) | 2120 / 1104 | new, mandatory |
| B: NH-52 IFMA x2 | 12.81 | 16.26 | 5 (one new, easy) | 4200 | new, mandatory |
| C: dual engine | 23.98-32.34 (level 1) | 33.35 | 6 (two new statements) | 2176 | unchanged |

## Recommendation to the implementation lane

The lane has already converged on this design, and its `SPEC.md` matches the panel's
conclusion. Confirmed, with the following as the panel's concrete sign-off.

1. **Layout.** Adjacent pairs of 64-bit words, `C = XOR_i clmul64(w[2i] ^ k[2i], w[2i+1] ^ k[2i+1])`,
   unreduced, split as `C = a + X^64 b`. One `VPCLMULQDQ zmm imm=0x10` per keyed 64-byte load:
   each 128-bit lane already holds one adjacent pair, so **no `VSHUFI64X2`, no `VPERM*`, no
   `VPSHUFB` anywhere in the product stage**. Any shuffle reintroduced into that loop costs its
   full 0.853 TSC serialised behind the clmuls — the measured 19.98 vs 31.98 gap.
2. **Register plan.** Four independent ZMM accumulators over each 256-byte group (four is
   enough to cover the 7.0-TSC ZMM clmul latency at 1.72 reciprocal throughput; more only
   adds fold cost). Per 64 bytes: one `VMOVDQU64` message load, one keyed `VPXORQ` (free —
   one extra 512-bit ALU uop per clmul is measured free), one `VPCLMULQDQ`, one `VPTERNLOGQ`
   or `VPXORQ` accumulate. Keys resident, constant displacements — the gcc cliff candidate B
   hit at 1 KiB (indexed operands / key spills, 16.3 -> 11.1) is real; check the objdump of the
   final loop for an indexed memory operand and unroll to constant displacements if it appears.
   Fold the four accumulators to the 128-bit PH pair once per block, outside the product loop.
3. **Which recurrence.** Keep the existing three-key GF(2^64) recurrence
   `P_0 = z`, `P_j = a_j + (b_j + y)(P_{j-1} + u)` — the level-1 output is 128 bits, so the
   GF(2^128) variant in `chainhash128_SPEC.md` is not needed and would cost multiplications.
   **S = 1 with B = 1024**: one recurrence per 1024 bytes. The recurrence-frequency proxy
   measured 14.0 (per 256 B), 21.4 (512 B), 26.1 (1024 B), 28.8 (2048 B); the 256-byte
   configuration is feedback-latency-bound and cannot reach 19.7 by level-1 work alone, while
   2048 B buys ~2 B/cycle for double the key and a worse tail. Keep the 2-CLMUL-plus-PSHUFB
   exact reduction (the high half of `b*27` is a nibble, so the second fold is a 16-entry
   register table) and keep the next sub-block's PH between the recurrence product and its
   reduction.
4. **Length and tail.** Length XORed into both halves of the last block only, as now; the
   nested-pair case is covered by `clnh_nested_nonzero_bound` with the nonzero target
   `(ell ^ ell')(1 + X^64)`. Do not adopt full zero padding — it is only needed by the integer-NH
   designs and it would break the short path.
5. **Do not revisit** (measured, not speculated): scalar MULX as a second engine (fully
   serialises behind clmul at every vector width); any integer-NH stage as the level-1 primitive
   (2^-64 needs two multiply streams, which caps it at 18.5 B/TSC); IFMA-52 (parse-bound at
   ~16.7 and clock-poor); an XOR lane-fold over integer NH accumulators (NH is Delta-universal
   for subtraction mod 2^64, not XOR); a keyed 128->64 universal reduction of the PH output
   (adds 2^-64 to the bound and 3 clmuls per block for nothing).
6. **Carry these two caveats into the post.** (a) The adjacent pairing changes the function on
   every platform, so the M2 must be re-measured and re-verified before the ARM lane follows;
   the panel expects it to be shuffle-free there too, but that is a prediction, not a
   measurement. (b) Key grows from 328 B to 1096 B at B = 1024 — still one key word per eight
   message bytes, the paper's rate, and no Toeplitz trick is needed.
