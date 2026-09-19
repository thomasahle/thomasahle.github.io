# UMASH corrected-bound proof, round 3 (PROOF3.md): consolidated verdict

Date: 2026-09-19. Proof under review: `scratchpad/codex/umash-goal3/PROOF3.md`
(680 lines, automated prover, maximum effort, not previously human-read), with
`PROGRESS.md`, `checks/` and `materials/` (PROOF.md = verified round-1 bound with
its VERDICT.md; PROOF2.md = PH+ENH joint case, verification in progress).
Three independent adversarial lenses (logic chain; counts/64-bit fibres;
constants + Lean match) each returned `refuted = false`, no corrected constants.
All computation ran on the Xeon under `<xeon-work>/umash-verify3/<lens>/`
(nice -n 10, taskset -c 56-63, 8 threads, 32 GB); local copies of scripts and
logs under `scratchpad/design/umash-verify3/{logic,counts,constants}/`.

## Decision: HOLDS, conditional on one recorded dependency

PROOF3's constants, lemmas and assembly hold exactly as stated. Every new lemma
(3.1 (a)-(e), 4.1, 5.1, 5.2, 5.3, 5.4, 6.1, 6.2, the envelope and score of
Section 7) was re-derived by hand by all three lenses; every 64-bit constant was
reproduced by independent code (carry automaton, completeness identity
8q+72, three-way class count, exact rationals); the prover's own
`checks/run_xeon.sh` regenerates every certificate byte-identically on cores
56-63. No lens found a broken step, a dropped case, or a counterexample.

The one thing to record: **C = 3125 depends on PROOF2.md Theorem 9.2**
(tag-only primary < 1/(8q)), which PROOF3 Section 2 item 3 imports beside the
human-verified PROOF.md lemmas without flagging that PROOF2 is still under
separate verification. All three lenses re-derived Theorem 9.2 by hand
(16|d| <= 4080 boundary crossings of a 2^60 multiple; g = 2^(k+1)-m <= 255
forces popcount(m) >= 54; <= 8*2^10 low words per H; N = 0 excluded;
d(N) < 2^36 via Lemma 9.1's six finite maxima 81/2, 256/27, 81/25, 16/7,
16/11, 16/13 with product 127401984/25025 < 16^4; total 255*2^53 < q/8) and
found no gap, so the dependency is satisfied on paper. Until the PROOF2 review
closes it is a hand-checked premise, not an independently reviewed one.
Fallback if Theorem 9.2 were withdrawn: the tag-only row reverts to PROOF.md's
verified 269280 (195840 with the optional 66 -> 48 refinement), the block
constant becomes 269280, the linear coefficient 33692/2^61 and the score
46.96 bits (still above the old 46.52 because the PH and ENH cases improved).

## What tightened the old constant, and that nothing was lost

Old ledger (PROOF.md, verified): max(82, 17, 604^2 = 364816, 5542, 269280) =
364816. New ledger (PROOF3 Theorem 6.1): max(82, 1123, 3125, <1/8) = 3125.

- **604^2 -> 1123 (Theorem 4.1, new Lemma 3.1).** PROOF.md Lemma 5.3 fixed bit
  0 of the low lane and bit 127 of the difference and counted the two lane masks
  independently (604 x 604). PROOF3 splits on s = minimum 2-adic valuation of
  the nonzero PH coordinate XOR differences and r = minimum ENH increment
  valuation (r = inf if the final pair is unchanged):
  s = 0 -> 17 (imported odd-PH lemma, arbitrary other changes);
  s in {1,2,3} -> exact identity (7) q Pr[Z xor E in D] = M sum_d Pr[E mod M =
  d mod M] plus a finite convolution over all 2^s x 2^s increment residues
  (maxima 1704, 3408, 6816 = 852 M) -> 852;
  s >= 4 with both ENH increments in 16Z (incl. unchanged) -> D cap 16Z = {0}
  forces the low lane to 0, PH bit 127 = 0 fixes bit 63 of the high mask, one
  free key per target by clmul injectivity -> 604 (one lane, not two);
  s >= 4, r in {0,1,2,3} -> identity (7) at width s with Lemma 3.1 and the
  valuation census 184/62/1 -> 852, 1060+s, 512+s, 24; max 1123 at s = 63, r = 1.
  Coverage: {s >= 1} x {all increment pairs incl. (0,0)} is exactly the old
  "all differing coordinates even" case; s ranges over 0..63, r over 0..3 and
  ">= 4 incl. absent"; multiple differing chunks are handled by the global
  minimum s (every other chunk difference and the affine constant lie in 2^s Z,
  PROOF.md Lemma 7.1); tags enter only the high lane, which is used only where
  the ENH keys are conditioned. The identity (7) needs Z uniform on 2^s Z and
  independent of E under IID keys: both supplied. Nothing dropped.
- **5542 -> 3125 (Lemmas 5.1-5.3, Theorem 5.4).** PROOF.md Lemma 4.4 gave one B
  per (A, beta, m, t) over all T_0 = 2771 mask/pattern pairs, 2 x 2771 = 5542.
  Lemma 5.1: the lifting congruence (12) depends on (m,t) only through
  f_{m,t}(l) = m - 2((l & m) xor t) mod 2^r, whose label is
  (m mod 2^(r-1), (m-2t) mod 2^r) (the sign at bit r-2 is invisible, which is
  the whole gain); F_4 = 36, F_r = 26r-75 for 5 <= r <= 63, max 1563; the zero
  class is exactly {(0,0)}. Lemma 5.2: one B per (A, beta, class). Lemma 5.3:
  the zero class costs q not 2q (fixed additive target for A'B'-AB mod q^2,
  checked across both wrap intervals incl. A = 0, A' = 0, eps = 0). Total
  (2F_r-1) q, min with the low-equality count 2^r q; r <= 3 keeps 2771, 1230,
  508, 24. Max 3125 at r = 63. Hypotheses: r >= 4 forces L = L' under the common
  low offset; PH keys conditioned to a common offset; delta of minimum valuation
  by exchanging operands; tags arbitrary and fixed. Same necessary condition
  (m in D, t in P(m)) as before, pairs merely merged by identical equation.
- **269280 -> < 1/8** is the PROOF2 import above. 82 and 17 are retained.
- The case split (chunk counts differ | same count, some PH chunk differs |
  same count, PH agree, final data differ | data equal, valid tags differ) is
  disjoint and exhaustive; the block constant is a max, union bounds only inside.
  The primary ledger correctly does not split on checksum equality (checksums
  enter only the secondary compressor).

## Certified constants (all three lenses, independently recomputed)

- Masks: |D| = 852; per-j 1,184,123,361,62,355,121,178,1; sum |P(m)| = 2771;
  pattern histogram {1:1, 2:435, 4:357, 8:59}; N_r = 852,248,64,2,1;
  T_r = 2771,615,127,3,1; v2 histogram 604/184/62/1; nonzero masks divisible by
  8 = {q-8}; D cap 16Z = {0}; top-bit split 248/604; every nonzero mask has top
  bit >= 60; prefix maxima 604, 362, 240; completeness 8q+72.
- Lemma 3.1 verified exhaustively at widths 4..10 (all delta != 0, all eps with
  v2(eps) >= v2(delta), all operand pairs; 7.3e11 visits at n = 10; 0 failures).
- Classes: F_4 = 36, F_r = 26r-75 (5 <= r <= 63), F_63 = 1563, verified three
  ways (labels, affine signatures, literal value tables for r <= 12 under three
  XOR offsets), zero class {(0,0)} for every r. Trap: labelling by (m mod
  2^(r-1), j) gives 37 at r = 4 because j = +-8 coincide mod 16.
- ENH numerators: 2771, 1230, 508, 24 (r <= 3); min(2^r, 2F_r-1) = 16, 32, 64,
  128, 256, 317, ..., 3073, 3125 (r = 4..63); 2^r binds for r <= 8.
- PH numerators: 17; 852 (s = 1,2,3); 604 (s >= 4, increments in 16Z);
  852 / 1060+s / 512+s / 24 (s >= 4, r = 0..3); max 1123.
- True-64-bit ENH fibres (lens 2, enh64.cpp): r in {4..24, 26..63}, 6
  structured instances each, all 2^r low-equal B enumerated for r <= 24 (4.1e9
  visits), every real collision has m in D, t in P(m), a unique (beta, class)
  per A, zero class <= 1 per A, and the real-collision set equals the
  constructive solve of (12); max per-A count 10 at r = 63 vs bound 3125.
- Scaled end-to-end (q = 2^w, p = 2^(w-3)-1, m0 = 8): w = 8 all 72 (s, r)
  combinations and w = 9 binding cases, exact collision counts over all keys
  within the proof's case formulas (worst ratio 0.095), identity (7) exact in
  every conditioning; grouped fibres w = 5..10, 10,908 cases, 0 failures;
  Lemma 5.3 exhaustive at widths 4..8. Widths 5, 6 of the prover's own grouped
  check have floor((q-1)/p) = 10, 9 (not faithful); 7..9 are.
- Envelope: A = 3125/(2^64-561) = 625/3689348814741910211 = 2^-52.390;
  2^61 (A + 32/(p-2)) = 422.6250000000000119..., so 423 is the LEAST integer
  coefficient (422 fails at L = 512); eps(1) = 1/(q-561), eps(L >= 2) =
  A + (1-A) min(1, 2 ceil(L/32)/(p-2)) <= 423 ceil(L/512)/2^61 checked exactly
  at ~5,000 lengths incl. 512k +- 1 up to 2^79; eps(2) =
  1448530578388042537297 / 8507059173023461316800733107792013239.
- Score: R* = 2/eps(2); R*^50 > 2^2669, R*^100 < 2^5339, R*^1000 in
  (2^53382, 2^53384): 53.382 < s < 53.384, decimal 53.38299177234803536...;
  minimiser L = 2 under both the at-most and the fixed-length convention
  (L = 1 gives 64.0 in both). IID envelope (3125/q) <= distinct envelope.
- Slack (not corrections): Lemma 3.1(c) observed 2^r/M vs proved 2^(r+1)/M,
  (d) 3 vs 5, (e) <= 8 vs n+12; true r = 1 convolution 557 vs 1060+s; grouped
  ENH fibres <= 4.7/q at widths 8-10 vs bounds 71-317. The proof has no lower
  bounds; all constants are union bounds.

## Certified numbers for the post (UMASH-64, primary, ideal keys)

Metric as in the post: score = min_L log2(L / max(2^-64, min(1, eps(L)))),
L in 64-bit words.

- eps(1) = 1/(2^64-561) (short/short, different lengths; equal-length short
  inputs never collide); eps(L >= 2) = A + (1-A) min(1, 2 ceil(L/32)/(2^61-3)),
  A = 3125/(2^64-561). Linear envelope 423 ceil(L/512)/2^61.
- Score 53.38 bits at L = 2 (both conventions). Was 46.52 (PROOF.md), 25.61
  before that. Published: 55.
- Per length, log2(1/eps): 1 KB (L = 128) 52.36; 1 MB (L = 2^17) 47.93;
  1 GB (L = 2^27) 37.9999. (Published ceil(L/512)/2^55: 55 / 47 / 37; previous
  45.52 / 45.28 / 37.99.) log2(L/eps): 59.36 / 64.93 / 65.00.
- Corrected is 1.62 bits below the published score, 2.64 below at 1 KB, 0.93
  ABOVE at 1 MB and 1.0 above at 1 GB; the corrected envelope drops below the
  published one from L = 5633 words (44 KB) on, because the polynomial slope
  2 ceil(L/32)/(p-2) is half the published per-word slope.
- UMASH-128 / fingerprint: inherits the LINEAR 423 ceil(L/512)/2^61 envelope
  (53.38 bits) by inclusion (`certifiedAllPairs128_of_64`); the quadratic
  83-bit claim is neither proved nor refuted. PROOF3 Section 1's "no new
  fingerprint claim" should say this in one sentence.

## Update for the post's UMASH note

- Certified fallback for UMASH-64 rises from 46.5 to 53.4 bits (ideal keys:
  OH words without replacement or IID, multiplier uniform on {2..p-1}); the
  displayed claim stays 55/83 per the author's 2026-09-18 decision, with the
  gap note and the link to issue #40.
- Say what is now closed: a complete unconditional all-pairs bound
  423 ceil(L/512)/2^61, i.e. 423/64 = 6.6x (2.7 bits) weaker than the README's
  envelope at short lengths and stronger than it from 44 KB on; per-block
  constant 3125/2^64 (was 604^2); the ENH-only case at every valuation with
  numerator min(2^r, 2F_r-1) <= 3125 (`OpenENHOnly` remains closed via PROOF.md
  Section 7).
- Say what is still open: the sharp per-block constant (published envelope
  needs any C <= 255; proved 3125, gap 12.25x = 3.6 bits; the Lean's 162 is a
  stricter sufficient target) and the PH+ENH equal-checksum joint case
  (`OpenPHENH`, PROOF2, under verification). The scaled-model factor ~4
  (2 bits) remains a heuristic.
- Dependency wording: the 53.4-bit figure uses the tag-only bound < 1/(8q)
  from the PROOF2 round (hand-checked by three reviewers, formal review
  pending); the fully reviewed figure is 46.96 bits (block constant 269280).
  If the note quotes one number, quote 53.4 with this footnote, or wait for
  the PROOF2 verdict.
- Scope caveat to keep: the model is the Python reference / Lean model (tag =
  seed ^ (size % 256) in the high ENH word); production `umash.c` not re-audited
  beyond the ENH fold.

## Draft comment for backtrace-labs/umash#40 (no AI attribution)

> Update on the unconditional bound from the previous comment: the per-block
> constant drops from 604^2 to 3125, and the envelope to
>
>     Pr[UMASH-64 collision] <= 423 * ceil(L/512) / 2^61,
>
> more precisely eps(L) <= A + (1-A) min(1, 2 ceil(L/32)/(p-2)) with
> A = 3125/(2^64-561), for every fixed seed, distinct inputs of at most 8L
> bytes, OH words sampled without replacement (or IID) and the multiplier
> uniform on {2, ..., p-1}. That is 423/64, about 2.7 bits, weaker than the
> README's ceil(L/512)/2^55 at short lengths, and stronger than it from about
> 44 KB on (the polynomial term is 2 ceil(L/32)/(p-2), half the README's
> per-word slope). In the min_L log2(L/eps) metric the bound is 53.38 bits at
> L = 2; at 1 KB / 1 MB / 1 GB it is 52.4 / 47.9 / 38.0 bits.
>
> Two counting lemmas do the work. (1) PH differences: instead of counting the
> two lane masks of a single clmul difference independently (604^2), split on
> the least 2-adic valuation s of the PH coordinate differences and the least
> valuation r of the ENH increments. The low lane of the PH difference is
> uniform on the multiples of 2^s and independent of the ENH lane, so the mask
> event has the exact form q Pr[Z xor E in S] = 2^s sum_{m in S} Pr[E = m mod
> 2^s], a finite convolution for s <= 3 and, for s >= 4, a width-s statement
> about the XOR of two truncated NH products (uniform for odd r, 2^(r+1)/2^s
> per target of valuation r, 5/2^s and (s+12)/2^s for the two special mask
> shapes). With the mask census (184 masks of valuation 1, 62 of valuation 2,
> one divisible by 8, none by 16) the worst case is 1123/2^64. (2) ENH-only
> changes at valuation r >= 4: the lifting congruence from the previous comment
> depends on the mask/pattern pair (m, t) only through l -> m - 2((l & m) xor t)
> mod 2^r, and these functions fall into F_r classes with F_4 = 36 and
> F_r = 26r - 75 for 5 <= r <= 63 (the sign of the coefficient at bit r-2 is
> invisible mod 2^r). One B per (A, wrap, class), and the zero class costs
> 2^64 rather than 2*2^64, gives min(2^r, 2F_r - 1)/2^64 <= 3125/2^64 at r = 63.
> The tag-only case (same data, block sizes differing mod 256) is below
> 1/(8*2^64) by a divisor-count argument.
>
> Still not proved: the README's constant. The linear envelope ceil(L/512)/2^55
> would follow from any per-block constant <= 255; the proved one is 3125, a
> factor 12.25 (3.6 bits). Every step is a union bound, and the exhaustive
> scaled models sit far below the bounds (the width-s lemma is loose by 2x per
> clause, the ENH classes by 10x or more), so I expect the true constant is
> well under 255, but the argument as it stands does not show it. The
> equal-checksum PH+ENH case for the fingerprint is written up separately and
> is being checked.
>
> As before: natural-language proof with integer certificates, three
> independent re-derivations, exhaustive checks of the new lemmas at widths
> 4..10 and of the ENH classes at production width; not yet formalised.
> Verified against umash_reference.py; umash.c not re-audited beyond the ENH
> fold. Write-up and scripts available on request.

## What remains between the proved coefficient and the published one

- Proved: 423 ceil(L/512)/2^61 from C = 3125. Published: 64 ceil(L/512)/2^61
  = ceil(L/512)/2^55, which follows from the same assembly with any C <= 255
  (C = 256 fails: coefficient exactly 64.0000..02). Gap 3125/255 = 12.25x =
  3.6 bits in the block constant, 1.62 bits in the score.
- Binding case: ENH-only, r = 63, numerator 2F_63 - 1 = 3125. The ENH
  numerator min(2^r, 2F_r-1) alone exceeds 255 from r = 8 on (F_8 = 133, 265).
  Second: PH, s = 63, r = 1, numerator 1123 (true convolution value about 557).
- Does the method plausibly close it? Partly. The ENH count allows two
  candidates per nonzero class without using the high-word restriction beyond
  the class label; the production-width fibre check found at most 10 real
  collisions per A at r = 63 against a cap of 3125, and the grouped scaled
  fibres sit 15-70x under their bounds. Using the full high equation (not just
  its class) or averaging over A rather than bounding per A is the natural next
  lemma and could plausibly bring the ENH row under 255. The PH row needs the
  observed 2x slack in Lemma 3.1(c) and the discarded pattern condition in (7);
  1123 -> ~557 is provable by tightening (c) to 2^r/M, but 557 > 255 still, so
  the high projection must be re-used in the small-r cases. Reaching the Lean's
  162 requires both. The mask/valuation machinery is the right tool; the
  remaining factors are counting slack, not a missing idea. The joint
  (fingerprint) case is a separate matter (PROOF2).
- Not covered by any lens: semantic match of the Lean definitions to the
  reference beyond the reads done here; production umash.c; the Codex checks/
  scripts themselves (reproduced, not relied upon).

## Presentation items to fix in PROOF3.md before a Lean lane (none affect the bound)

1. Section 2 item 3: mark PROOF2 Theorem 9.2 as an import under separate
   review; state the fallback constant 269280 explicitly.
2. Section 1: the sharp target is "any C <= 255", not "numerator 162"; add
   the one-sentence UMASH-128 linear inheritance.
3. Theorem 4.1: say "bit 63 of the conditioned ENH high XOR difference" for
   "the prescribed top bit", and note that the Z-uniformity step generalises
   PROOF.md Lemma 7.1 from one chunk to all differing chunks.
4. Certificate: derive the s >= 4 rows from D with an explicit Lemma 3.1 clause
   per mask instead of hard-coding [852, 1060+s, 512+s, 24]; extend
   validate_primary.cpp's convolution check to width 9 or 10.
5. Section 8: cores 40-47 -> 56-63; say which grouped-fibre widths are faithful
   (7..9) and that 5, 6 have m0 = 10, 9.
6. PROGRESS.md Round 3 still lists the superseded PH constants 1208/1448/1920;
   Round 4 supersedes them.
7. Envelope (2) is stated with the distinct-key A; IID readers get a slightly
   loose bound (3125/q would do). Harmless.

## Lean ordering for PROOF3's lemmas, after PROOF.md's steps 1-9

PROOF.md's order (mask certificate; Lemma 3.1/3.2; 4.1/4.2; 4.3/4.4/4.5;
5.1-5.3; 6.1-6.3/6.4; 7.1/7.2 -> OpenENHOnly; 8.1-8.4; 9.1/9.2 + assembly)
stays. PROOF3 inserts:

10. Mask census extras (decide over the certified table): v2 histogram
    184/62/1, {m != 0 : 8 | m} = {q-8}, D cap 16Z = {0}, top-bit split 248/604,
    prefix counts n_s(c) for s <= 3 (the 84 convolution tables) and the
    valuation-restricted counts for s >= 4.
11. Lemma 3.1 (width-uniform, n >= 4): (a) triangular bijection for odd delta;
    (b) 2^r/M; (c) Hensel lifting with odd linear coefficient, <= 2 roots;
    (d) the r = 1, v2(e) = 2 split (b even / b odd, even-even recursion to (a)
    at width n-2, odd-odd four square roots); (e) the M-8 target via
    Y + Y' = M-8 + 2(Y mod 8) and the (v+1)/M product mass. State each clause
    as a separate theorem over BitVec n with n a variable; (a) and (b) are
    reusable from PROOF.md 4.1.
12. Identity (7): Z uniform on 2^s Z and independent of E (from PROOF.md 5.1,
    7.1 generalised to all differing chunks) => q Pr[Z xor E in D] =
    M sum_d Pr[E mod M = d mod M]. Then the s = 1..3 convolution (8) by
    decide, and the s >= 4 rows as explicit sums of Lemma 3.1 clauses over the
    census of step 10.
13. Theorem 4.1 (1123/q): the case split on (s, r) with the 604 branch
    (D cap 16Z = {0} + bit 127) and the 17 branch imported. This discharges a
    strengthened `PrimaryPHBound` (numerator 1123 for phDiffCount >= 1, any
    enhChanges, any tags) and subsumes `OddPHBound`.
14. Lemma 5.1 (affine expansion of f_{m,t}, label (10), XOR-translation
    invariance) and the class count (11) by decide over the 2771 pairs for
    each r = 4..63 (60 finite checks; label by (m mod 2^(r-1), (m-2t) mod 2^r),
    NOT by j). Zero class = {(0,0)}.
15. Lemma 5.2 (one B per (A, beta, class)): reuse PROOF.md 4.3's
    least-differing-bit argument with the class replacing (m,t).
16. Lemma 5.3 (additive target mod q^2, <= 1/q across both wrap intervals).
17. Theorem 5.4: (2F_r-1) q count, min with 2^r q, r <= 3 rows from PROOF.md
    4.2; numerator 3125. Replaces `WeakENHBlockBound`'s role.
18. Tag-only: either PROOF2 Lemma 9.1 (divisor bound, finite maxima) +
    Theorem 9.2 (`TagOnlyBound` with 1/(8q)), or, until PROOF2 is reviewed,
    PROOF.md 6.2 with 269280 and the fallback constant.
19. Theorem 6.1 (block max over the explicit disjoint split) and Corollary
    6.2 (identity bound C/(q-561) for pairs with a long message, reusing
    PROOF.md 8.1-8.4, 9.1-9.2).
20. Section 7 assembly: eps(L) envelope, rational certificate (13)
    422 < 2^61 (A + 32/(p-2)) < 423, R*^50 > 2^2669 and R*^100 < 2^5339 by
    norm_num on integers -> `CertifiedAllPairs64` with certifiedEnvelope L =
    min(1, 423 ceil(L/512)/2^61) (or the sharper (2)); the short/short branch
    stays the direct actual-collision bound (not `PrimaryIdentityBound`), so
    the round-1 caveat about `certified64_of_primary_and_short` still applies
    and `UMASHConstants.lean` must carry A = 3125/(q-561), not weakA.
21. `CertifiedAllPairs128` by the existing inheritance theorem.

## Reviewer artifacts

- Lens 1 (logic): `scratchpad/design/umash-verify3/logic/` (RUNLOG.txt,
  recompute_constants.{py,log,json} ALL ASSERTIONS PASSED, reconstruction.log,
  lemma31_check.c + lemma31_n4-10.json, scaled_block.cpp + scaled_w8.jsonl,
  scaled_w9_binding.jsonl, scaled_w9_enh.jsonl, run_xeon*.sh, run.log).
- Lens 2 (counts): `scratchpad/design/umash-verify3/counts/` (my_masks.py,
  lowxor.c, nhinj.c, enh64.cpp, scaled_full.cpp, proof2_arith.py, README.md,
  RESULT.md, xeon_logs/ incl. the byte-identical regenerated
  theirs/checks/manifest.json; the 42 spurious FAIL lines in scaled_ph_w8.log
  were a bug in the reviewer's first harness, corrected in scaled_ph2_*.log).
- Lens 3 (constants): `scratchpad/design/umash-verify3/constants/`
  (my_constants3.py 53/53 PASS, compare3.py 16/16 MATCH, lowxor_scaled.c,
  grouped_fibre.cpp, run_xeon.sh, xeon/*.log, xeon/*.json, xeon/regen_sha256.txt
  = xeon/supplied_sha256.txt).
