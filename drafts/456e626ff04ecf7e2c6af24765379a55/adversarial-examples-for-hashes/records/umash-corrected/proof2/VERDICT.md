# VERDICT on PROOF2.md (OpenPHENH joint case + tag-only primary bound)

Date: 2026-09-19. Consolidates three independent adversarial review lenses
(logic / quadratic / constants), each run on the Xeon under
`<xeon-work>/umash-verify2/<lens>/` with `nice -n 10 taskset -c 56-63`.
Subject: `scratchpad/codex/umash-goal2/PROOF2.md` (702 lines) plus
`checks/` and `materials/`.

## Verdict: HOLDS (no corrections)

All three lenses return `refuted = false` with high confidence. No constant
needed correction. The three headline claims are verified as stated:

1. **Theorem 1 / OpenPHENH.** For two valid blocks with the same chunk
   count, equal data XOR checksums, exactly one differing nonfinal PH chunk,
   both final ENH words differing and minimum valuation r > 0,
   Pr[both lane-wise projected compressors agree] <= 170906186782 / 2^128
   = 2^-90.686 < 2^-90 < 2^-87. This is exactly the Lean proposition
   `OpenPHENH` (r in {1,2,3} u [36,63]); the bound survives conditioning on
   distinct OH words (divide by 1 - 561/q, still < 2^38 / q^2); no
   independence between the completed compressors is used; the common-offset
   lemma of PROOF.md is not applied across two PH offsets.
2. **Section 9 (tag-only primary).** Pr[primary projections agree] < 1/(8q)
   for tag-only pairs with any common PH offset. Verified. This is a
   PRIMARY, single-compressor bound; it is not the joint Lean `TagOnlyBound`
   (< 2^-92) and PROOF2 does not claim that.
3. **SharpPrimaryProjection remains open.** Nothing in PROOF2 assumes it,
   proves it or refutes it; Section 10 is labelled partial. No published
   headline (Published64 / Published128) is proved or refuted.

## What was checked (union of the three lenses)

Every lemma re-derived by hand by at least two lenses: 3.1, 3.2, 4.1, 4.2,
4.3, 5.1, 5.2, 6.1, 9.1, 9.2, 10.1, 10.2, and the Section 2 model
(shufflers S_1 = T, S_k = T + T^k for k = 2..15 matching
`umash_reference.shuffle(x, i+1, n)`; differences (6); I + S_k unipotent so
invertible; reduction (8) for r >= 4 via D cap 16Z = {0}; dP_hi < 2^63 from
clmul degree <= 126). The Section 8 hypothesis-to-Lean map (Valid, sameCount,
dataChecksum equality, phDiffCount = 1, enhChanges = 2, enhValuation = r,
uniformProb IID over 34 words, jointEvent = four lane congruences mod p) was
checked by all three against `UMASHObligations.lean`; the reference-model
match (shufflers, ENH fold `(L, (H+tau) xor L)`, twist cancellation) was
checked by code (0 mismatches in 20300 random checks; 60,000 random w = 64
OpenPHENH instances across all 15 shufflers, 0 failures).

Constants reconstructed independently (three separate implementations, one
run BEFORE the prover's scripts, 87/87 checks):
- |D| = 852; pattern histogram |P(d)| = 1:1, 2:435, 4:357, 8:59; T_0 = 2771;
  |D cap 2^r Z| = 852, 248, 64, 2, 1; bit-0 and bit-63 splits 248/604;
  min top bit 60; completeness mass sum |P(d)| 2^(64-h) = 8q + 72 =
  147573952589676413000.
- Per-r maxima of the 60 sums (all at shuffler k = 2): r = 1: 27196168693
  (61504 targets); r = 2: 13437880698 (4096); r = 3: 268435521 (4);
  every r >= 4: 170906186782 (362952 targets; exact
  3152662688168640344103384884752 / 2^64 = 170906186781.32...).
- Final constant = max over the four cases = 170906186782 < 2^38 =
  274877906944; every row individually < 2^38 (and < 2^41 as the certificate
  asserts); log2(C / q^2) = -90.686; still < 2^38 after / (1 - 561/q).
- Section 9: divisor product 127401984/25025 = 5090.99 < 16^4, so
  d(n) < 2^36 for n < 2^128; |d| <= 255; <= 16|d| = 4080 values of H;
  popcount(m) >= 54 so <= 8 * 2^10 low words; 4080 * 8192 * 2^36 = 255 * 2^53
  < 2^61 = q/8. (On the 15 actually reachable masks the count is even
  smaller: 4080 * 16 * 2^36 ~ 2^-76.6 q^2.)
- Section 10: 604^2 = 364816 < 162 * 2^12 = 663552 (89.07/q at rank >= 76);
  Lemma 10.2 ceilings (14, 144, 56, 9)/q with a PH chunk, (17, 18, 20, 24)/q
  without.

Exhaustive scaled checks of the NEW lemmas (each term separately, beyond
the shipped validate_quadratics.cpp scope): K_H terms and K_{L,r} terms for
ALL nonzero increment pairs (odd included) at w = 4..10, ALL tag pairs at
w <= 6, 0 failures across ~3.6e8 bound checks / ~2.7e11 operand visits;
Lemma 5.1 for all (b,c,d) and masks at n = 2..8 (~3.1e8 cases, 0 failures);
Lemma 3.2 weights for all masks at w = 3..11 (0 failures, ratio 1.0 attained,
i.e. sharp); Lemma 4.1 at q = 2^64 with exact isqrt root isolation (58
instances, max ratio 0.5) and 8.1e6 tiny instances. An exact scaled JOINT
model of Lemma 6.1 (six key words, reference PH/ENH/shufflers/twist,
arbitrary offsets and tags) at w = 8 (328 configs) and w = 9 (68 configs),
w = 10 partial: 0 violations, worst exact/bound ratio <= 0.0008.

Prover pipeline (`checks/run_xeon.sh`) reproduced by all three lenses on
cores 56-63: `open_ph_enh_certificate.json` identical to the shipped copy in
every field except `environment.affinity`; `validation.json` and
`independent_verification.json` byte-identical (1,057,030,144 NH visits,
1,775,714 low and 16,516,096 high bound checks, 48,103,424 line identities,
1,248,480 interval cases, 0 failures).

## Issues (none affect the bound)

Presentation / formalization hints:
- P1. The final constant is a MAXIMUM over 60 disjoint cases
  (r in {1,2,3,>=4} x k = 1..15), not a union or sum; the union bound is
  only over (u,v) target pairs inside one case. Section 6 should say so.
- P2. Lemma 5.2's case C - R a b = 0 (mod q) is handled only by the phrase
  "zero coefficient convention"; one sentence (s = v2(b), linear coefficient
  vanishes mod 2^(64-s), quadratic coefficient odd, Lemma 5.1 applies)
  closes the reading gap.
- P3. Theorem 1 is stated for r > 0, but only (8) needs r >= 4 and only
  Lemma 5.2 needs r >= 1; Lemmas 4.2/4.3 need only both increments nonzero
  and arbitrary tags. Say which lemma consumes which hypothesis.
- P4. Lemma 4.3's and Lemma 5.2's middle terms do not need L = L'; Lemma
  4.1 may count the vertex integer twice (harmless); Lemma 4.2 uses the real
  exponent 2^(h/2) while the certificate uses 2^ceil(h/2) (safe direction).
- P5. Theorem 1's r in [4,35] range is already covered by
  `ClosedPHTwoWordENHBound`; it is a superset, not a problem, but should not
  be described as new.

Coverage / process:
- C1. Shipped `validate_quadratics.cpp` tests the high lemma only for even
  increments and four tag profiles at w = 8, and Lemma 5.1 exhaustively in d
  only for n <= 5; no shipped certificate exercises Lemma 6.1's assembly or
  Lemma 3.2's weights (they are inputs to certify_open_ph_enh.py). Section 7
  describes this scope accurately. The reviewers' own tests close the gaps.
- C2. `checks/README.md`, `run_xeon.sh`, `certify_open_ph_enh.py` and
  `verify_certificate.py` pin and hard-assert cores 40-47; the certificate
  embeds an `environment` block, so the manifest sha256 (10360dff...) is
  tied to that run and the 56-63 regeneration hashes to 4a909f77.... The
  manifest should hash the certificate without the environment block, or
  say so.
- C3. Not covered: no Lean proof exists (paper proof only); the Lean
  definitions of `oh`, `ohSecondary`, `Valid`, `blockTag`, `encode` beyond
  what was read (reference-model match verified instead); production
  `umash.c`.
- C4. Scaled end-to-end probability tests cannot detect union-structure
  errors (C_w / q_w^2 > 1 for w <= 6); the Section 2 algebra was therefore
  checked at w = 64 directly, and the assembly by the exact scaled joint
  model above.

Observation: the bound is far from attained (extreme family
delta = eps = 2^(w-1) gives ~18/q^2 exact at w = 8..10 vs 1.7e11/q^2 bound).

## Consequences

**Post's UMASH note.** Both joint ENH propositions are now closed on paper:
`OpenENHOnly` (PROOF.md) and `OpenPHENH` (PROOF2.md). The "still open" note
should list only `SharpPrimaryProjection` (the sharp primary constant 162
and the all-message `PrimaryIdentityBound` assembly, including the
short/short constant-polynomial identity, which no lemma in PROOF.md or
PROOF2.md bounds). Nothing numeric changes: the certified envelope
45635 * ceil(L/512) / 2^61 and the 46.52-bit score rest on the even-PH block
constant C = 604^2 = 364816, untouched by PROOF2. The 83-bit UMASH-128 claim
still inherits only the linear envelope and remains neither proved nor
refuted. The primary ledger still has rows above 162: even-PH single-chunk
t <= 11 and multi-chunk even-PH (364816), ENH-only two-word r >= 8 (5542),
one-word ENH-only r = 0,1,2 (2771, 1230, 508). If the optional length-aware
ledger is ever used, Section 9 moves it from 46.96 (L = 2, tag-only 269280)
to 47.11 at L = 3 (at L = 2 the block constant becomes 5542, score 52.56).

**Issue #40 addendum.** Add: the second joint case (`OpenPHENH`: one PH
chunk changed, equal checksums, both final ENH words changed, any valuation
1..63) now has a paper proof with bound 170906186782 / 2^128 < 2^-90, so the
conditional `RequestedConditional55` premises are both discharged on paper;
the remaining gap is the sharp primary constant 162 (`SharpPrimaryProjection`),
where the best proved block constant is 364816 (A = 364816 / (2^64 - 561)),
i.e. the factor-~4 (2-bit) loss stands. Also note the tag-only primary case
is now < 1/(8q) (replacing 269280/q). Label all of this as reproduction of
the proof structure by three independent reviewers, not as a Lean theorem.

**Lean lane (lean-goal/umash-corrected).** Yes, take PROOF2's lemmas next,
in dependency order:
1. Section 2 algebra: shuffler set, (6)-(7), I + S_k invertible, reduction
   (8) for r >= 4 (needs D cap 16Z = {0} from the mask table already in the
   corrected-bound lane).
2. Lemma 5.1 (bits of a modular quadratic, 4 * 2^-floor(h/2)) — pure
   2-adic, self-contained, exhaustively checked at n <= 8.
3. Lemma 4.1 (quadratic interval count) — elementary real/integer counting.
4. Lemma 3.1 and Lemma 3.2 (PH point mass, triangular weights) — shared
   with PROOF.md; reuse.
5. Lemma 4.2 + 4.3 (K_H) and Lemma 5.2 (K_{L,r}) — the two new counts;
   formalize with the hypotheses as in P3/P4 (drop L = L' from the middle
   terms).
6. Lemma 6.1 + the 60-case maximum (P1) and the certificate as a `decide` /
   `norm_num` check on exact rationals; then Section 8's hypothesis map to
   `OpenPHENH` and the distinct-key transfer.
Section 9 (tag-only) is independent and can be formalized in parallel as a
primary lemma (Lemma 9.1 divisor bound + the counting in 9.2).

**Tag-only result toward SharpPrimaryProjection.** It removes one row from
the primary ledger (tag-only pairs: 269280/q -> < 1/(8q), in fact ~2^-76.6
on reachable masks), so tag-only pairs are no longer an obstruction. It does
NOT move the block maximum (364816, even-PH rows) nor the r = 0,1,2
one-word ENH rows, and does not address the all-message assembly or the
short/short identity. SharpPrimaryProjection stays open; the next targets
are the even-PH single-chunk t <= 11 rows and the multi-chunk even-PH
combination.

## Artifacts
- Logic lens: `scratchpad/design/umash-verify2/logic/` (logic_recompute.py,
  lemma_check.c, structure_check.py, xeon/recompute64.json).
- Quadratic lens: `scratchpad/design/umash-verify2/quadratic/`
  (affinity_patch.diff, scaled joint model, term-wise checks).
- Constants lens: `scratchpad/design/umash-verify2/constants/`
  (verify2_constants.py, scaled_new_lemmas.c).
- Xeon copies under `<xeon-work>/umash-verify2/{logic,quadratic,constants}/`.
