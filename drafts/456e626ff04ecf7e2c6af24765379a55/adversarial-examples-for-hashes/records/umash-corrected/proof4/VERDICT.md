# VERDICT on PROOF4.md (UMASH-64, published headline bound via the mod-8p accumulator)

Date: 2026-09-19. Object under review: `scratchpad/codex/umash-goal4/PROOF4.md`
(785 lines, sha256 0c5de815d8968c67b7e1b5639da1e4eba007324e61acc990702c9639160a4c4b),
with the certificates in `checks/` (all ten JSON certificates regenerated
byte-identically on the Xeon by two lenses). Four adversarial lenses:
model fidelity (`model/`), logic (`logic/`), counts (`counts/`), constants
(`constants/`). All Xeon work under `<xeon-work>/umash-verify4/<lens>/`.

## 1. Verdict: HOLDS WITH CORRECTIONS (presentational only)

No lens refuted any step. Every constant was reproduced by independent code in
at least two lenses, every new counting lemma was confirmed exhaustively in
faithful scaled models, the logical chain was re-derived by hand, and the
load-bearing model claim (a) was demonstrated on the shipped code. The
corrections are wording: none changes a number in the theorem.

The result is a statement about the SHIPPED primary hash, not a variant
(Section 2). The published headline bound of UMASH-64,
Pr[collision] < ceil(s/4096) 2^-55 = 64 ceil(L/512)/2^61 (L in 64-bit words),
is TRUE in the reference key model, proved with coefficient 58 in place of 64.

Status of the logic lens's pending runs at the forced cutoff: all completed
afterwards and PASS (checked by the consolidator on the Xeon, logs in
`<xeon-work>/umash-verify4/logic/`):
- `reconstruct.log`: independent mask enumeration + tail recipe reproduce all
  56 B_r exactly (37 + 7.5(r-8) for r = 8..60, 434.5, 420.1875, 423.6875),
  r <= 3 counts 17/18/20/24, (18) = 204.7108..., 57.625 and 56.375, 58 least
  coefficient, envelope <= 58 ceil(L/512)/2^61 on 5017 sampled L, score in
  (56.18, 56.19). FAILURES: [].
- `scaled_w8.log` / `scaled_w10.log`: faithful scaled tail model (accumulator
  mod 8p_w, exact root counts over every multiplier): 32.6M + 6.4M identity
  pairs, 0 Lemma 7.1 violations, 0 bound violations (worst N/cap 0.23 / 0.11).
- `case_split.log`: Section 8 decision list simulated against the reference
  chunking on 56,261 message pairs, exactly one case per pair, 0 failures.
- `mod8p.log`: reference poly_reduce == closed form mod 8p, canonical output;
  5440 constructed pairs collide iff 8 | f j; the mod-p-only variant collides
  on every identity pair; finalizer rank 64.
- `c_vs_python.log` (run by the consolidator via `run_c.sh`, umash.c sha256
  aad7d50d... = HEAD 9709e11): selftest PASSED; 1080 cases, lengths 0..40000,
  0 mismatches between umash.c and umash_reference.py.

## 2. Model fidelity (the decisive question, special weight)

(a) Does the shipped primary hash keep the accumulator modulo 8p = 2^64 - 8
where Lemma 7.1 needs it, for all lengths?  YES, confirmed by all four lenses
on both umash_reference.py (byte-identical to HEAD) and umash.c at HEAD
9709e1123c753e7ef34bc560cbdefac6fd33fe47:

- Reference: `poly_reduce` computes acc = (f^2 mod p)(acc + lo) + f hi mod
  (2^64 - 8), canonical in [0, 8p); `umash_long` returns finalize(acc) with no
  reduction modulo p anywhere; finalize(x) = x ^ rotl(x,8) ^ rotl(x,33) is an
  F_2-linear bijection.
- C: horner_double_update = add_mod_slow(mul_mod_fast(m0, add_mod_fast(acc,x)),
  mul_mod_fast(m1, y)), m0 = f^2 mod p, m1 = f, output fully reduced in
  [0, 2^64-8); used by umash_medium (9..16 bytes, one ENH chunk from acc = 0),
  umash_long (per block, last-block tag seed ^ (uint8_t)n), and the vectorised
  >= 1024-byte path (split_accumulator_eval = add_mod_slow). umash_fp's
  hash[0] is the same computation. Arithmetic identity checked on ~3.06M
  tuples per build in four builds (gcc native with dispatch + inline asm, gcc
  generic long path, gcc without the long routine, clang native): 0
  mismatches, 0 unreduced outputs; split-accumulator chains ~5.6M steps, 0
  mismatches.
- All lengths: umash_full (which = 0, 1) and the incremental sink API agree
  with the Python reference on 4,836 vectors (6 keys incl. extreme OH words
  and multipliers 2, 3, 8, p-1, p-2; every length 0..1100, 1101..5000 step 41,
  block boundaries 1023..8193), plus 417 (counts lens) and 1080 (logic lens,
  lengths to 40000) further vectors: 0 mismatches.
- Short branch (<= 8 bytes) has no polynomial in either implementation; PROOF4
  never invokes Lemma 7.1 there (short/short and short/long bounded separately).
- Lemma 7.1's mechanism demonstrated on the real code: constructed projected
  identity pairs (identical prefix, one-chunk final blocks, raw high
  difference j p) collide iff 8 | f j on 456 C cases (four builds), 60 + 5440
  + 216,360 Python-reference trials, and the recovered accumulators differ by
  exactly f j p mod 8p. The variant finalize(acc mod p) collides for EVERY f
  on those pairs, so PROOF4's remark that a field reduction alone would
  remove Lemma 7.1 is correct and the 8p structure is material (counts lens:
  the unweighted grouping would give ~3200/q at r ~ 60, coefficient ~402).

Two model caveats, outside PROOF4's declared model, affecting only how the
public statement is scoped:
1. Multiplier set. umash.c `umash_params_prepare` accepts f in {1..p-1}
   (f != 0 && f < p, includes 1); the reference `is_acceptable_multiplier`,
   PROOF4 and the Lean PolyKey use {2..p-1}. Every f-dependent term is
   monotone in the right direction (weights floor((p-1)/d_j)/(p-1) <=
   n_j/(p-2), root bound d/(p-1) < d/(p-2)), so 58 and the envelope hold
   verbatim for the C sampler. PROOF4 must say so (PROOF.md s9 already did).
2. OH key sampling. umash.c does not rejection-sample; it repairs repeated OH
   words from two spare random words and regenerates on exhaustion, giving an
   injective 34-tuple distribution that is not exactly
   uniform-without-replacement (repeat-event mass <= 561/q ~ 2^-54.9). Salsa20
   key expansion is out of scope. PROOF4 disclaims both; the public statement
   must say "reference key model".

## 3. Logic (b): both key models proved

Every block-level and tail bound is proved for IID OH words (Sections 3-7);
PROOF.md Lemma 9.1 transfers any IID event bound b to b/(1 - 561/q) under 34
distinct words, valid for the joint (OH, f) event of Lemma 7.2 because f is
independent of the OH words; the root terms 2/(p-2) and rho(L) are
conditional on every OH key and need no transfer. Hence without replacement:
A = 205/(q-561), S = 435/(q-561) + 2/(p-2), eps(1) = 1/(q-561); IID: 205/q,
435/q + 2/(p-2), 1/q. Both statements of (1)-(2) follow.

Re-derived by hand (logic lens, confirmed structurally by counts/constants):
Lemma 7.1 (r >= 4 forces Y = Y' since D cap 16Z = {0}; difference f j p mod
8p, zero iff 8 | f j since p odd); Lemma 7.2's grouping (affine function on
bits >= 4 determined by label (21); one B per label and wrap by PROOF3 5.2;
realised (m*, t*) in its own group so weight max n_j; A mod 16 uniform since
both A-wrap intervals have length divisible by R >= 256; small-Q prefix
equation (23) for r = 61..63); Corollary 7.3 (identity-and-collision ledger
17/18/20/24, 2^r/q <= 128/q, B_r; non-identity root bound 2/(p-2) from a
nonzero degree-<= 2 polynomial in f); Theorem 4.5 case split (disjoint,
exhaustive, max 151 = 604/4); Theorem 6.2 (min(2|P(m)|, K(m) W(m)) with the
new 6(2^ceil(h/2)+1) rectangle term); Section 8 decision list (exactly one
case per pair, witnessing tuple fixed before sampling, identity used only as
a necessary event; the case-3 prefix-cancellation argument). Dependencies on
PROOF.md, PROOF2.md, PROOF3.md and umash-enh-verify are all previously
verified HOLDS; no open Lean proposition is used as a premise. PROOF2 Theorem
9.2 (tag-only) is stated "with any fixed common PH offset" and so covers
one-chunk final blocks.

Scaled exhaustive confirmations (counts lens, widths 8-14): Lemmas 3.1, 3.2
(0 failures over all (delta, eta) at w = 9, 10; 3/M and 4/M attained), 3.3
(two-offset form, all 43,520 increment pairs x all 4,096 offset pairs at
w = 8), 5.1 (all 65,025 increment pairs x 11 tag pairs x 255 masks, worst
ratio 0.992), 6.1 (all (V1, V2) for every mask at w = 8, 9, 10), Theorem 6.2
(exact event at w = 8 over 218,450 instances, max 4.61/q vs scaled 185/q),
Lemma 7.2 (lift_scaled w = 12, 13: 4.6M realised fibres, 0 violations;
tail_scaled w = 12, 13, 14; fibre64_v2 at 64 bits: 0 violations across
r = 55..63; necessity64: 6,681 constructed fibres). Lemma 4.2 pivot rules
soundness-tested (400,000 GF(2)-rank trials each row type, 0 violations) but
not proved independently; they cannot affect 58 unless wrong by > 41%.

## 4. Constants (c): 205, 435, 58 reproduced from the case constants

Independently recomputed (three lenses, code sharing nothing with the prover):
- Masks: |D| = 852, valuation counts (852, 248, 64, 2, 1), sum |P(m)| = 2771,
  pattern histogram {1:1, 2:435, 4:357, 8:59}, top-bit split 248/604 (the 248
  includes the zero mask), completeness sum 8q + 72.
- (18) = 944062127676554878353 / 2^62 = 204.7108... < 205 (and 204.71094 even
  without PROOF2's dense-276 term); (19) = 133.78125 < 134.
- PH: Lemma 4.3 = 4681/32 = 146.28 (t = 10 bound; t <= 9 tables < 145 with
  margin 7e-7), one-coordinate 78.75 / 81.125 / 84.53125; Lemma 4.4 =
  max(604/4, 26.25, 11.5) = 151; Theorem 4.5 = 151.
- A = max(82, 151, 204.71, 1/8, 82, 9, 1) -> 205/(q-561).
- Tail: B_r = 37 + 7.5(r-8) for r = 8..60 (427 at r = 60), B_61 = 434.5
  (exactly 1001888787503350020012/2305843009213693949, witness D = 1, E = 3,
  gap 0), B_62 = 420.1875, B_63 = 423.6875; special numerator = max(17, 18,
  20, 24, 128, B_8..B_63) = 434.5 -> 435.
- n_j = floor((p-1)/d_j) - floor(1/d_j) = (2^58-1, 2^59-1, 2^60-1, p-2) for
  v2(j) = 0, 1, 2, >= 3, equal to the literal count over {2..p-1}.
- 2^61 (205/(q-561) + 32/(p-2)) = 57.625... in (57, 58): 58 is the LEAST
  integer coefficient (57 fails at L = 512); 2^61 S = 56.375... in (56, 57).
  Published 64 holds with slack (needs tail < 496).
- Exact: A = 41/3689348814741910211,
  S = 207987039431075193985/8507059173023461316800733107792013239 = 2^-55.183.

## 5. Envelope and score (d)

eps(1) = 1/(q-561) (only short messages; exactly 1/(q-1) would be tighter,
harmless), eps(L >= 2) = max{A + (1-A) rho(L), S} with rho(L) =
min(1, 2 ceil(L/32)/(p-2)). Justified: the case (ordinary pair vs one-chunk
final ENH change with identical prefix) is a property of the fixed message
pair, ordinary pairs satisfy a + (1-a) rho <= A + (1-A) rho since a <= A and
rho <= 1, and S is L-independent because the comparison polynomial has degree
<= 2 after the common prefix cancels.

Score under the post's convention (min_L log2(L / max(2^-64, min(1, eps(L)))),
L in 64-bit words): ceil(L/32) <= L/2 for L >= 2 gives ordinary/L <= A/2 +
(1-A)/(p-2) < S/2, special/L <= S/2 with equality at L = 2, L = 1 gives
~64; so score = log2(2/S) = 56.18301637674461900825 at L = 2 (IID:
56.18301637674461905), inside (56.18, 56.19) as (26) asserts, under BOTH the
at-most and the fixed-length conventions (two distinct 9..16-byte inputs of
equal length are one one-chunk block each; the r = 61 witness has tag gap 0).
(2/S)^50 > 2^2809 and (2/S)^100 < 2^5619 checked in exact rationals.

Per-length (log2(1/eps), UMASH-64, reference keys): L = 2: 56.18; 1 KB
(L = 128): 55.18; 1 MB (L = 2^17): 47.995; 1 GB (L = 2^27): 38.000.
PROOF3 gave 53.38 / 52.36 / 47.93 / 37.9999; published 55 / 47 / 37. The
proven envelope is at or below the published one at every length.

## 6. Corrections to apply to PROOF4.md (none affects a number in the theorem)

1. Section 1 / statement: add that umash.c samples f uniformly on {1..p-1}
   and that every bound holds a fortiori there (weights n_j/(p-1) <=
   n_j/(p-2), root bound d/(p-1)); state that the model is the Python
   reference's multiplier set and that the C library's OH-repair sampler and
   Salsa20 expansion are out of scope.
2. Section 2, line 96: "248 masks with high bit 0" includes the zero mask
   (247 nonzero); harmless since every use is an upper count.
3. Lemma 7.2 table (lines 642-648): print the per-r values, not only strict
   ceilings: B_r = 37 + 7.5(r-8) for r = 8..60 (max 427), 434.5, 420.1875,
   423.6875; the "61 | 435" row should show 434.5 with 435 as the ceiling.
   Record the margins: 0.5 on 435, 0.375 on 58, 7e-7 on the t <= 9 table.
4. Line 105: cite "the argument for PROOF3.md (7)" (the per-mask form of the
   summed identity), not "(7)" itself.
5. Corollary 7.3: note that the r <= 3 numbers 17, 18, 20, 24 are the plain
   low-projection counts and differ from Lemma 3.3's 17, 20, 15, 16 because
   no PH conditioning is needed; both < 435.
6. Lemma 7.1: note that only the necessary-condition direction (collision =>
   congruence mod 8p) is used, so lazy representatives would also suffice;
   the shipped code in fact fully reduces.
7. Optional: eps(1) = 1/(q-1) exactly under sampling without replacement.

## 7. What the post may say

UMASH-64 marker: PROVEN >= 56.18 bits (min_L log2(L/eps), attained at L = 2)
in the reference key model (fixed seed, 34 OH words sampled without
replacement or IID, multiplier uniform on {2..p-1}; also for the C library's
{1..p-1}). The published linear headline eps < ceil(s/4096) 2^-55 =
64 ceil(L/512)/2^61 is TRUE in that model, proved with coefficient 58 by a
route that uses the implemented modulo-(2^64 - 8) accumulator rather than
the paper's field-projection step. Per-length: 55.18 / 47.995 / 38.000 bits
at 1 KB / 1 MB / 1 GB versus published 55 / 47 / 37.

Certified numbers: A = 205/(2^64 - 561), S = 435/(2^64 - 561) + 2/(2^61 - 3),
rho(L) = min(1, 2 ceil(L/32)/(p-2)), eps(1) = 1/(q-561), eps(L >= 2) =
max{A + (1-A) rho(L), S} <= 58 ceil(L/512)/2^61 < 64 ceil(L/512)/2^61;
2^61 S = 56.375; score log2(2/S) = 56.183.

Caveats to keep: computer-assisted natural-language proof with exact finite
certificates (ten JSON certificates, regenerated byte-identically by two
independent reruns), not Lean-formalised; production Salsa20 key expansion
and the C OH-repair sampler not covered; umash.c audited for the polynomial
accumulator / finalizer path and end-to-end equality with the reference on
~6,300 vectors across four builds.

UMASH-128: inherits only the linear 58 ceil(L/512)/2^61 (proven >= 56.18
bits, since UMASH-128 collision implies UMASH-64 collision on hash[0]); the
quadratic ceil(s/2^26)^2 2^-83 (83-bit) claim is neither proved nor refuted.

Wording for the UMASH note: "UMASH-64: the published bound
ceil(s/4096) 2^-55 holds in the reference key model; we prove
58 ceil(L/512)/2^61 with a minimum of 56.18 bits at two-word inputs. Our
proof does not go through the paper's intermediate step (projection identity
<= 162/q, which remains unvalidated) but through the accumulator modulo
2^64 - 8 that the implementation actually keeps. UMASH-128: the 83-bit claim
remains a claim; it inherits the 64-bit bound."

Wording for issue #40: "Update: we now have a complete proof that the
published UMASH-64 bound ceil(s/4096) 2^-55 is true in the reference key
model (34 distinct OH words, multiplier uniform on {2..p-1}; also for
umash.c's {1..p-1}), with coefficient 58/2^61 per 512-word block instead of
64/2^61. The route differs from the paper's: the intermediate claim that the
projected polynomial identity has probability <= ceil(2^64/|F|)^2 2^-63 =
162/q remains unvalidated (our best identity numerator is 205/q, and the
one-chunk final-block identity numerator exceeds 255/q), but the collision
bound is recovered by using the fact that the implementation keeps its
accumulator modulo 2^64 - 8 = 8 (2^61 - 1) rather than modulo 2^61 - 1: for
projected identities on a one-chunk final block, the extra three bits make
the two accumulators differ by f j p mod 8p, which is zero only when 8 | f j.
A final reduction modulo 2^61 - 1 alone would not give the published bound
by this route. The 83-bit UMASH-128 claim is still open."

## 8. What remains open

- UMASH-128 quadratic claim ceil(L/2^23)^2 2^-83 (Lean Published128): not
  addressed by PROOF4.
- SharpPrimaryProjection = PrimaryIdentityBound(162/(q-561)) as a standalone
  premise: not established; PROOF4 proves Published64 directly (its
  identity numerator is 205 with the unweighted one-chunk numerator ~1993
  > 255), so Conditional55WithPrimaryPremise is bypassed rather than
  discharged. The paper's intermediate step stays unvalidated.
- Lean: no formal proof; UMASHModel.lean's polyStep (mod q - 8), PolyKey
  {2..p-1}, DistinctOHKey and Published64 (ceil(L/512)/2^55 with lengths
  <= 8L) match PROOF4's model and units textually, so Published64 is the
  right direct target. Key64/DistinctOHKey distribution definitions were not
  in the supplied UMASHObligations.lean and should be checked when
  formalising.
- Lemma 4.2's pivot rules are soundness-tested, not independently proved
  (cannot affect 58 unless wrong by > 41%, 64 unless > 76%).

## 9. Lean ordering (suggested)

1. DistinctKeyAcceptance (PROOF.md 9.1; 1 - 561/q transfer) and
   EncodingInjectiveLong / EncodedBlocksValid (PROOF.md 8.1, mod8p.log 4-4c).
2. Lemma 7.1 as a standalone statement on polyStep: identical prefix, raw low
   equality, high difference j p => collision iff 8 | f j (pure ring
   arithmetic mod q - 8; the smallest new fact and the one the public
   statement rests on).
3. The finite certificates as decidable computations: mask set D (852),
   pattern sets, (18)/(19) sums, the 56 tail constants (22)/(23), the PH
   tables; each is a `decide`/`native_decide`-style obligation with the
   grouping lemma (Lemma 7.2) proved once abstractly.
4. Corollary 7.3 (S) and Theorem 6.2 / Theorem 4.5 (A) block bounds, then
   Section 8's decision list into PrimaryIdentityBound(205/(q-561)) for
   ordinary pairs plus the special-case S.
5. Envelope arithmetic (24)-(26) in ℚ, then Published64 (with 58 as an
   intermediate CertifiedAllPairs64-style statement), then
   certifiedAllPairs128_of_64 for the linear UMASH-128 inheritance.
SharpPrimaryProjection and Published128 stay as open propositions.

## 10. Lens confidence summary

Model fidelity: high. Logic: medium at cutoff, raised to high after the
pending runs completed and passed (Section 1). Counts: high for Sections 2,
3, 5, 6, 7, 8 and the constants; medium for Section 4's rank-table sub-cases.
Constants: high. Consolidated: HOLDS WITH CORRECTIONS (wording only).
