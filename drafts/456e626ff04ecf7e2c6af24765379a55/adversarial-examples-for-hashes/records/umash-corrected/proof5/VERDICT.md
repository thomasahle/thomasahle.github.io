# UMASH fingerprint proof, round 5 (PROOF5.md): consolidated verdict

Date: 2026-09-19. Proof under review: `scratchpad/codex/umash-goal5/PROOF5.md`
(751 lines, automated prover, maximum effort, not previously human-read), with
`PROGRESS.md`, `checks/` (three certificates, 1,776 C/reference comparisons,
`run_xeon.sh`) and `materials/` (PROOF.md, PROOF2.md, PROOF3.md with their
verdicts = verified; PROOF4.md = the UMASH-64 headline proof, under separate
review in `umash-verify4/`, no verdict yet; subcase-b and ENH-verify rounds;
UMASHObligations.lean; umash.pdf; umash_reference.py; the C library fetched at
backtrace-labs/umash HEAD 9709e1123c753e7ef34bc560cbdefac6fd33fe47).
Four independent adversarial lenses (model fidelity to umash.c; logic chain and
independence structure; counts/64-bit fibres; constants and scores) each
returned `refuted = false` with no corrected constants. All computation ran on
the Xeon under `<xeon-work>/umash-verify5/<lens>/` (nice -n 10, taskset -c 24-31,
ulimit -v 32 GB); local copies under `scratchpad/design/umash-verify5/{model,
logic,counts,constants}/`.

Claim under review: for the two-compressor fingerprint as implemented in the C
library (34 shared OH words without replacement, two independent multipliers
f_0, f_1 uniform on {2..p-1}; IID words and {1..p-1} also covered),

    Pr[fp(x) = fp(y)] < (81/128) ceil(L/2^23)^2 2^-83 < ceil(L/2^23)^2 2^-83

for every fixed seed and distinct x, y of at most 8L bytes, i.e. the published
fingerprint headline (paper p. 12, eps_fp < ceil(s/2^26)^2 2^-83 with s = 8L).

## Decision: HOLDS (model fidelity first; two recorded dependencies; wording corrections only)

**Model fidelity (weighted first): confirmed, and the coverage gap closed.**
The supplied umash.c / umash.h / umash_long.inc / umash_reference.py are
SHA-256-identical to upstream HEAD 9709e11 (three lenses, independent clones).
Two lenses wrote their own models of the fingerprint from umash.c, not from
umash_reference.py, and matched the library bit-for-bit: lens 1 on 3,300 cases
(every size 0..1100 twice, block boundaries 256k+{-17..17} to k = 40, random
sizes to 20,000, 65536/65537/131071, multipliers (1,1), (p-1,p-1), (p-2,2),
(2,p-1), (1,p-1), seeds 0 / 2^64-1 / 2^63, 360 keys from umash_params_derive),
each case through umash_fprint, umash_full(which = 0/1) and the incremental
sink with 0-5 random cut points, in FOUR builds: UMASH_LONG_INPUTS = 0, = 1
generic, the DEFAULT dynamic-dispatch build (resolved to the VPCLMULQDQ
multi-block routine, h_long_impl() == 2, on the 8375C) and a default build of
upstream HEAD; lens 3 on 6,072 fingerprints (sizes 0..8193) plus 12,036 checks
of finalize^-1(out_i) mod p = P_i(f_i). Lens 1 additionally matched 28,469 raw
(primary, secondary) lane pairs from oh_varblock_fprint against the model's
compress() for every last-block chunk count 1..16 and byte count 1..256, and
verified PROOF5's identity (6) (finalize^-1 of the C output < 8p and equal mod p
to the degree-2n polynomial in the model's lo/hi lanes, no constant term) on
6,374 (case, compressor) pairs. This closes the only coverage hole in Codex's
own 1,776 comparisons (built with UMASH_DYNAMIC_DISPATCH = 0, so the
VPCLMULQDQ path and the sink API were never exercised); PROOF5 Section 11
correctly disclaims a formal equivalence for every machine-code path.

Structural facts read from the source and confirmed by all four lenses:
- both compressors use oh[0..31] for the chunks; the secondary adds the twisting
  words oh[32], oh[33] into the keyed checksum (lrc) before its clmul; the
  checksum is over all n chunks with their XOR keys BEFORE the additive ENH
  treatment of the final chunk;
- secondary shuffler: PH at 0-based position i < n-1 receives T + T^(n-1-i),
  the last PH receives T (v128_shift = per-lane doubling, _mm_add_epi64 /
  vshlq_n_u64, no cross-lane carry); the ENH (Y, ((H + tag) mod q) xor Y) and
  the checksum PH are XORed unshifted; the VPCLMULQDQ routine computes the same
  shifts (chunk j gets 15-j plus 1) and the same lrc;
- poly[i] = (f_i^2 mod p, f_i), row i used for output i in umash_fp_medium and
  umash_fp_long; horner_double_update / add_mod_fast / mul_mod_fast keep the
  accumulator in [0, 8p) and congruent to (f^2)(acc + lo) + f hi mod p (2,000
  random operand sets plus all extreme combinations), so "equal fingerprint
  words => both accumulators equal => P_i(f_i) = 0 in F_p"; the 8p
  representation enters only Prop 11.1's lower bound, never the upper bound
  (unlike PROOF4); finalize is a bijection (GF(2) inverse computed);
- short path (n <= 8): oh[n] and oh[n + OH_SHORT_HASH_SHIFT = 4], same
  SplitMix-style bijection; medium (9..16): first 8 + last 8 bytes as one ENH
  chunk with oh[0], oh[1], tag seed ^ n, twisted lrc; long: full blocks tag =
  seed, final block tag = seed ^ (uint8_t) n_bytes, final chunk = last 16 bytes;
  BLOCK_SIZE = 256 bytes, multi-block threshold 1,024 bytes;
- umash_params_prepare masks raw multipliers to 61 bits and keeps f in
  {1..p-1} (f = 1 and f = p-1 kept; 0, p, 2^64-1 resampled), resamples repeated
  OH words from the two spare poly[i][0] fields; PROOF5's p-2 root denominator
  is conservative for that range.

**Mathematics: every new step re-derived, every constant reproduced.** Lemma
7.1 (composition: conditional on OH the two comparison polynomials are fixed;
f_0, f_1 independent; [r + (1-r) z_0][r + (1-r) z_1] with nonnegative
coefficients; only Pr(Z_0) + Pr(Z_1) <= a and Pr(Z_0 and Z_1) <= b are
averaged; NO independence of the compressors is used), the new secondary
marginal (Lemma 4.1: changed keyed checksum, offset form of PROOF Lemma
5.2/5.3 over the twisting pair, 604^2/q; Lemma 4.2: equal checksums with a PH
change, twist products cancel, d(.)V xor c with c independent of V, ker(T +
T^k) = ker T = span{2^63, 2^127} since T + T^k = (I + T^(k-1)) T with an
invertible first factor, bit 127 of the image fixed so at most two V per
target, S kills lane bit 0 so at most 604 masks per lane, 2 x 604^2/q; Lemma
4.3: different counts q^-2 + C/q, ENH-only C/q, tag-only 4080 x 48/q), the
ten-row block ledger (disjoint and exhaustive: different counts | same count,
some PH differs: checksums differ / equal with >= 2 PH / equal with exactly 1
PH forcing an ENH change: one word, two words r = 0, two words r >= 1 | PH
agree, final data differ | data agree, tags differ), the message-level rows
(different block counts: primary < 82/q, secondary < 46/q from 9 x 5 = 45
targets with bit 127 fixed, joint 82 x 46/q^2; short/long 9/q, 9/q, 81/q^2 at
noise indices len and len + 4; short/short 1/q^2 direct), and the distinct-key
transfer a = (C_0 + C_1)/(q - 561), b = J/(q(q - 561)) were re-derived by hand
by lenses 2, 3 and 4 and attacked with exhaustive scaled two-compressor models
(W = 5..10, p = 3, 7, 31, 127) and exact true-64-bit fibre counts (GF(2) linear
algebra over the free key / twisting pair): zero violations. Lemma 4.2's
linear algebra was checked at full width by three lenses (all 15 shufflers and
all 120 (count, position) shufflers; fibres of V -> S(d(.)V) <= 2 for 474
values of d including X^j, all-ones, p, jp, q-8; exhaustive at widths 2..10).

**The quadratic accounting (c):** ceil(L/2^23)^2 comes solely from the roots
term rho(L)^2 with rho = 2 ceil(L/32)/(p-2) <= 2^19 H/(p-2), H = ceil(L/2^23),
because d = 2 ceil(L/32) is the degree (blocks = ceil(L/32), two coefficients
per block, no constant term; verified against umash_reference for every byte
length 9..16384 and against umash.c on every tested size). There is NO union
bound over block pairs: equal block counts pick one differing (chunks, tag)
tuple before the keys are sampled (PROOF.md Lemma 8.1, encoding injective via
block count plus (uint8_t) tag). E(L) <= b + a rho + rho^2 <= H^2 (b + a r_0 +
r_0^2) = H^2 K 2^-83 uses H >= 1 to absorb the cross term (0.0874 of K) and the
joint term (0.0403 of K) into the quadratic; the roots term is 0.5000 of K.

**Codex's certificates reproduce.** All four lenses re-ran
certify_fingerprint.py, verify_certificate.py and check_c_model.py (only the
CPU-affinity assertion patched 40-47 -> 24-31): PASS; regenerated JSON
identical to the shipped copies except the environment block; check_c_model
outputs sha256 e04950ff...4115f2 identical; 120 shufflers, 3,549 scaled
slices, 16 block-swap examples reproduced.

### Two recorded dependencies (satisfied on paper, outside the three-lens verdicts)

1. **J = 345763417 x 2^12 (equal checksums, >= 2 PH chunks), the 2^37 row
   (one PH + one ENH word), the 852^2 row (one PH + two ENH words, r = 0) and
   the tag-only joint 11946240 x 2^-116** are imported from the subcase-b and
   umash-enh-verify rounds. Those rounds returned CONFIRMED reviews, but they
   are not part of the PROOF/PROOF2/PROOF3 three-lens verdicts, and their Lean
   propositions (`SubcaseBBound`, `PHOneWordENHBound`, `TagOnlyBound`) are
   defined, not proved. Lens 2 re-derived their arguments at the logic level
   (rank/point bound 2^-(128-h), (u, v) low-bit constraint, twist cancellation,
   kappa/W conditional structure) and lenses 2, 3 and 4 recounted the whole
   N_h/S_h table exactly (h = 15: N = 68948, S = 2466; J reproduced). Lens 3
   re-derived the 852^2 row independently (I + T + T^k unipotent). Robustness
   (exact): the PUBLISHED headline (K < 1) survives J up to 10.25x larger
   (joint block bound up to about 2^-84.3) or the marginal sum C_0 + C_1 up to
   5.26x larger (to 3,856,644); the 81/128 refinement has only 0.8% slack
   (K = 0.62760 vs 0.63281), so an error of about 13% in J would break 81/128
   while leaving the headline intact.
2. **C_0 = 3125 (primary marginal) is PROOF3 Theorem 6.1**, itself importing
   PROOF2 Theorem 9.2 (tag-only primary < 1/(8q)); both are three-lens
   verified. Fallback if PROOF2 9.2 were ever withdrawn: C_0 -> 269280
   (195840 with the 66 -> 48 refinement), a -> (269280 + 729632)/(q - 561) =
   1.36x larger; K rises by about 0.03 and the headline still holds.

PROOF5 does NOT depend on PROOF4 or `SharpPrimaryProjection` (confirmed by all
lenses; PROOF4 is used only in Section 10's optional pointwise comparison).
Conversely PROOF4's fate changes nothing in (1)-(2).

### Scope of the theorem (stated by PROOF5, restated so nobody over-reads it)

- Ideal full-key distribution: 34 OH words without replacement (or IID),
  multipliers independent and uniform on {2..p-1} (or {1..p-1}). Not a theorem
  about umash_params_derive's Salsa20 expansion of a 32-byte secret (whose
  rejection loop can also fail and return false).
- Fixed seed shared by both strings (tags seed ^ (n mod 256) then differ only
  in the low byte, which Lemma 4.3 and the tag-only rows use). Comparisons
  under different per-call seeds are outside the claim, as in the paper.
- The C architecture with two multipliers. The Python reference's `UmashKey`
  has one `poly` field, so `umash(..., secondary=True)` reuses the multiplier:
  Prop 11.1 (A||B vs B||A, A = 256 zero bytes, B = 256 bytes 0x01, collides
  for every OH key and seed at f = p-1 because (p-1)^2 = 1 mod p makes the
  mod-8p update commute over full blocks) gives that variant a guaranteed
  collision fibre of probability 1/(p-2) = 2^22 x 2^-83, refuting the 83-bit
  claim for the shared-multiplier reference (all four lenses reproduced the
  collision on the reference and, with f_0 = f_1 = p-1, on all three C builds;
  with f_0 random and f_1 = p-1, hash[1] still collides while hash[0] differs,
  confirming the C library's independent use of poly[0] and poly[1]).

## Certified constants and scores (all four lenses, independently recomputed)

- q = 2^64, p = 2^61-1, m = p-2 = 2^61-3; |D| = 852 (carry-state automaton
  and signed-digit recursion), per-j counts 1,184,123,361,62,355,121,178,1,
  bit-0 and bit-63 splits 248/604, T_0 = 2771, completeness identity
  17q - 72p = 8q + 72.
- Ledger column maxima: C_0 = 3125 (primary), C_1 = 2 x 604^2 = 729632
  (secondary; tag-only 195840 and different-counts 604^2 + 1/q are below),
  J = 1416246956032 (joint; J/q^2 = 2^-87.6348; 82 x 46, 81, 852^2, 2^37,
  170906186782, 4721784, 11946240 x 2^12 all below).
- a = 732757/(2^64 - 561) = 2^-44.52; b = J/(2^64 (2^64 - 561)) = 2^-87.63;
  rho(L) = min(1, 2 ceil(L/32)/(2^61-3)); E(1) = 1/(q(q - 561));
  E(L) = b(1 - rho)^2 + a rho (1 - rho) + rho^2 for L >= 2; 0 < E(L) <= 1.
- K = 2^83 (b + a r_0 + r_0^2), r_0 = 2^19/(2^61-3):
  K = 61555182062916914709644342474504392009252905535823413248 /
  98079714615416883696934812005564729285413570653510954055
  = 0.62760360084939750... = 0.5000 (roots) + 0.0874 (cross) + 0.0403 (joint);
  5/8 < K < 81/128 = 0.6328125 by integer cross-multiplication (128 num < 81
  den), so 81/128 cannot be replaced by 5/8 by the same envelope. E(L) <
  (81/128) H^2 2^-83 < H^2 2^-83 checked exactly at 3,868 / 20,665 / 70,802
  grid lengths including every 32 / 512 / 2^23 plateau boundary +-33 up to
  2^66-2^79, and analytically via rho <= r_0 H.
- Scores of E under the post's convention (min_L log2(L / max(2^-128, min(1,
  E(L)))), L in 64-bit words; the 2^-128 floor never binds since E >= b):
  | domain | minimiser | score | certified interval |
  |---|---:|---:|---|
  | 1 <= L <= 2^23 (64 MiB) | L = 2 | 88.634778062847499 | (88.63, 88.64), brute force over all 2^18 plateau starts |
  | 1 <= L <= 2^46 (the post's UMASH-128 row domain, NOT in PROOF5) | L = 2^46-31 | 83.99999997 | lens 4 only, exact rationals |
  | 1 <= L <= 2^61-1 (full byte domain) | L = 2^61-31 | 68.999999999999140 | (68.99, 69.00), convexity of F(t)/(32t-31) with positive remainder |
  | all positive L | L = 2^65-95 | 64.99999999999999999754 | (64.99, 65.00); beats L = 2^65-63 by 4.7e-52, an exact-arithmetic distinction only |
- Published formula ceil(L/2^23)^2 2^-83 scores exactly 83 on L <= 2^23,
  82.99999983 on L <= 2^46 (at L = 2^46-2^23+1; the post's "83" is a
  rounding), 67.99999999999475 = 7 + log2(2^61-2^23+1) on L <= 2^61-1; the
  (81/128) form adds exactly log2(128/81) = 0.660150 (83.66 / 68.66).
- Pinned lengths, -log2 E (proved) vs published: 1 KB (L = 128) 87.635 vs 83;
  1 MB (L = 2^17) 87.582 vs 83; 64 MiB (L = 2^23) 83.672 vs 83; 1 GB (L = 2^27)
  75.984 vs 75; 5 GB (L = 625e6) 71.558 vs 70.542; 16 TB 48.00 vs 47. E <= 2^-83
  holds up to L = 10,912,384 words = 83.25 MiB (published threshold 64 MiB).
- Inherited linear envelopes: PROOF4 precise I_4 scores 56.18301637674 at L = 2,
  coarse 58 ceil(L/512)/2^61 55.14202; PROOF3 precise 53.38299, coarse 423 form
  52.27549. E <= I_4 <= 58 ceil(L/512)/2^61 and E <= I_3 at every sampled L
  (strict before L = 2^65-63, equal to 1 after); identity (16) U - E =
  (1-r){r(1-a) + A_4 - b(1-r)} verified symbolically, A_4 > b, a < 1. The
  coarse quadratic (81/128) H^2 2^-83 is below the coarse linear
  58 ceil(L/512)/2^61 for every L until the linear form clips at 1 near
  L = 2^64.14 (the quadratic reaches 1 near 2^65.33): the quadratic dominates
  at every L of the size_t domain.
- Prop 11.1 fibre: 1/(p-2) = 2^-61.0000 = 2^22 x 2^-83 (reference range
  {2..p-1}); with independent C-range multipliers {1..p-1} the fibre needs
  f_0, f_1 in {1, p-1}: (2/(p-1))^2 ~ 2^-120, negligible and inside the r^2 term.
- Slack, for the record (not corrections): Lemma 4.2's largest achievable
  necessary-event fibre at 64 bits is 1208 = 2 x 604 (image of V -> S(d(.)V)
  in one lane when the multiplying difference is a power of two) against the
  proof's 729632, and observed true conditional counts <= 12 per 2^64; Lemma
  4.1 true counts <= 137 where enumerable; the ENH-only joint entry keeps the
  older 4721784 = 5542 x 852 rather than 3125 x 852 (valid, loose). The proof
  has no lower bounds except Prop 11.1.

## Wording corrections to PROOF5.md (none affect the bound)

1. Section 11, last paragraph: "requires both to equal p-1 and has probability
   1/(p-2)^2" is the {2..p-1} statement; umash_params_prepare accepts f = 1,
   and f = 1 also makes A||B vs B||A collide (mulsq = multiplier = 1, symmetric
   update acc <- acc + lo + hi; verified 6/6 on the reference and on the C
   builds). With the C range the fibre is (2/(p-1))^2 ~ 2^-120. Say so.
2. Section 3 indexes PH positions 1-based ("T + T^(n-i) at position i < n-1")
   while umash_reference.shuffle and umash.c are 0-based (T + T^(n-1-i), T at
   the last PH); Lemma 4.2 uses only ker S = ker T and that S kills lane bit 0,
   so nothing depends on it, but the two conventions should be reconciled.
3. Section 2 / 9: state that the post's UMASH-128 row domain is 2^46 words
   (data.json `length_cap_words`) and give its score (E: 83.99999997 at
   L = 2^46-31; published formula 82.99999983; strengthened form 83.660). The
   88.63 figure is for L <= 2^23 only and must not be transplanted to the row
   without the domain.
4. Section 9: say that L_u = 2^65-95 beating L_s = 2^65-63 (by 4.7e-52) is an
   exact-arithmetic distinction with no practical content.
5. Section 12: cores 40-47 -> whatever the re-run used; the affinity assertion
   in all three scripts should read the CPU set from the environment.
6. Section 5, ENH-only joint row: 4721784 could be 3125 x 852 = 2662500 (not
   binding either way).
7. Section 11: record that the dynamic-dispatch VPCLMULQDQ path and the
   umash_sink API were NOT covered by checks/c_bridge.c (UMASH_DYNAMIC_DISPATCH
   = 0); they are covered by the model lens's 3,300-case run
   (`umash-verify5/model/xeon_logs/`), which the text may cite.
8. Section 6: the "nine representatives / at most five with a fixed top bit /
   45 targets" count needs p >= 8 (at p = 3, 7 there are 11 and 10
   representatives); harmless at p = 2^61-1, but a scaled reproduction must use
   p >= 31.

## What the post may say for UMASH-128

Conditional structure: the UMASH-128 numbers below rest on PROOF5 plus the
three-lens-verified PROOF/PROOF2/PROOF3 and the two recorded dependencies
above; they do NOT depend on PROOF4. PROOF4's separate verdict decides only
UMASH-64's 55-bit headline (and, if PROOF4 falls, the inherited 128-bit
fallback marker would be 53.38 instead of 56.18, which is moot because E
dominates both).

- Marker: the published "83 (claimed)" becomes certified on the post's own
  domain (L <= 2^46 words): published formula 82.99999983 ~ 83, strengthened
  coarse form 83.66, fine envelope 83.99999997 (display "83.99" or "84.0",
  author's rounding choice; do not display 88.63 without "through 64 MiB").
- Note text (replace "The inherited 53.38-bit certificate does not establish
  the quadratic 83-bit claim ..."): "Proved for the C architecture (two
  independent polynomial multipliers) with ideal keys, fixed seed:
  eps < (81/128) ceil(L/2^23)^2 2^-83, i.e. the published bound with 0.66 bits
  to spare. The finer proved envelope scores 88.63 bits through 64 MiB, 84.0
  through 2^46 words, and 69.0 over the full 64-bit length range (the
  published formula itself scores 68 there). The Python reference's
  single-multiplier fingerprint is NOT covered: with the multiplier p-1 (or 1)
  swapping two full blocks collides for every key, probability 2^-61 for that
  pair. Masking to fewer bits still needs its own bound or the generic
  projection loss. Natural-language proof with exact certificates, four
  independent adversarial reviews, not yet in Lean."
- Pinned card "Bound at 1 GB: 75 bits (claimed)" -> "75.98 bits (certified)";
  "2^-70 at 5 GB" -> 71.56; "2^-83 through 64 MB" extends to 83.25 MiB.
- The Lean status line ("The published 55/83-bit claims remain neither proved
  nor refuted") must change to: 83 proved on paper (four reviews), Lean
  formalization outstanding; 55 per PROOF4's verdict.
- If PROOF4 also HOLDS: "both published UMASH headlines are true for ideal keys
  and the C architecture; the proof gap in the whitepaper's entropy-loss
  sentence was a gap in the argument, not in the bound." If PROOF4 is REFUTED
  or only partially holds: keep UMASH-64 at its certified fallback (53.38, or
  whatever PROOF4's verdict certifies) and state the two rows separately.

## Draft addendum for backtrace-labs/umash#40 (no AI attribution)

> Second update, this time on the fingerprint. I can now derive the README's
> fingerprint bound for the C library's architecture:
>
>     Pr[umash_fprint(x) = umash_fprint(y)] < (81/128) ceil(L/2^23)^2 2^-83
>                                            < ceil(L/2^23)^2 2^-83
>
> for every fixed seed, distinct inputs of at most 8L bytes, the 34 OH words
> sampled without replacement (or IID) and the two polynomial multipliers
> independent and uniform on {1, ..., p-1} (the range umash_params_prepare
> accepts). That is the whitepaper's ceil(s/2^26)^2 2^-83 with s = 8L, so the
> published fingerprint headline holds, with a factor 128/81 (0.66 bits) to
> spare. The finer form of the bound is
>
>     E(L) = b (1-rho)^2 + a rho (1-rho) + rho^2,
>     rho = 2 ceil(L/32)/(p-2), a = 732757/(2^64-561), b = 1416246956032/(2^64 (2^64-561)),
>
> which in the min_L log2(L/eps) metric gives 88.63 bits through 64 MiB and
> just under 69 bits over the whole 64-bit length range (the README formula
> itself gives 68 there, because ceil(L/2^23)^2 grows faster than L).
>
> The argument does not go through the "entropy lost to F" sentence. It bounds
> three events over the shared OH key: each compressor's reduced output
> colliding on its own (primary 3125/2^64 from the previous comment; secondary
> 2 x 604^2/2^64, new), and both colliding at once (largest case 345763417 x
> 2^-116, from the equal-checksum case with at least two changed PH chunks,
> which I mentioned last time as being written up), then uses only the
> independence of the two multipliers: conditional on the OH key the two
> comparison polynomials are fixed, so the collision probability is at most
> [r + (1-r) z_0][r + (1-r) z_1] with r = 2 ceil(L/32)/(p-2) the root bound and
> z_i the per-compressor events, averaged with the marginal and joint bounds.
> Nothing assumes the two compressors are independent; they share the OH key
> throughout. The two new secondary lemmas: when the keyed checksums differ,
> the twisting pair (oh[32], oh[33]) makes the checksum PH difference an
> affine function of a fresh key with the same 604^2 count as before; when the
> checksums agree and some PH chunk changed, the twist terms cancel, and the
> shuffled PH difference is d * V xor c with c independent of the free key V,
> where the shufflers T + T^k have kernel {0, 2^63, 2^127} on each 128-bit
> lane (T + T^k = (I + T^(k-1)) T with the first factor invertible), giving
> 2 x 604^2.
>
> One thing you may want to know about umash_reference.py: `UmashKey` carries
> a single `poly`, so `umash(..., secondary=True)` reuses the primary
> multiplier, while umash.c uses `poly[0]` for the first output and `poly[1]`
> for the second. For the single-multiplier variant the quadratic bound is
> false: with f = p-1 (or f = 1) the modular update commutes across full
> blocks, so x = A||B and y = B||A (A = 256 zero bytes, B = 256 bytes 0x01)
> give equal fingerprints for every OH key and seed, i.e. a collision
> probability of at least 1/(p-2) ~ 2^-61 for that pair, 2^22 times the
> README's 2^-83. The C library is not affected: with independent
> multipliers that fibre has probability about 2^-120. I mention it only
> because the literate reference is what the whitepaper's proof describes.
>
> Scope, as before: ideal keys (not the Salsa20 expansion), fixed seed, the
> full 128-bit output. The bound was checked against umash.c at upstream HEAD
> (9709e11) in all build configurations, including the default VPCLMULQDQ
> dispatch and the incremental sink API, on several thousand inputs per
> configuration; the proof is natural language with integer certificates,
> re-derived independently four times, not yet formalised. Write-up and
> scripts available on request.
>
> [If PROOF4 holds, add: "The same method also closes the 64-bit constant;
> details in a separate comment." If not, omit.]

## What remains open

- Lean: `Published128` (which matches (1)'s second inequality exactly:
  ((L + 2^23 - 1)/2^23)^2 / 2^83, and `hash128` carries two independent
  multipliers per LEAN_OBLIGATIONS.md item 2) is unproved; `SubcaseBBound`,
  `PHOneWordENHBound`, `TagOnlyBound`, `DifferentChecksumsBound` and the
  PROOF3 chain are defined, not proved. See the ordering below.
- The subcase-b and ENH-verify rounds are CONFIRMED reviews but not three-lens
  verdicts; the headline tolerates a 10x error in J, the 81/128 form 13%.
- Key derivation: nothing is proved about umash_params_derive's Salsa20
  expansion or the rejection loop's failure mode.
- Per-call seeds, truncated/masked outputs, and the streaming API's
  equivalence beyond the empirical 3,300-case check are outside the theorem.
- PROOF4 (UMASH-64, 55-bit headline) is under separate review in
  `umash-verify4/`; PROOF5 neither needs nor supports it.
- Tightness: Lemma 4.2 is loose by about 600x (1208 achievable vs 729632
  proved), Lemma 4.1 by more; the true fingerprint constant is well below
  81/128, but the argument does not show it and nothing in the post needs it.

## Lean ordering for PROOF5, after PROOF.md steps 1-9 and PROOF3 steps 10-21

22. Imported joint rows as named propositions with the Section 3 hypotheses
    (same 34 IID OH words for both compressors, arbitrary fixed tags, twisting
    keys used only by the secondary): `SubcaseBBound` (>= 2 PH, equal
    checksums, J/q^2, N_h/S_h table by decide over the certified mask set),
    `PHOneWordENHBound` (2^37/q^2), the r = 0 two-word row (852^2/q^2, I + T +
    T^k unipotent), `TagOnlyBound` joint (11946240 x 2^-116), PROOF2 Theorem 1
    (170906186782/q^2), PROOF.md Theorem 7.2 (4721784/q^2). Prove them
    separately in that order (subcase-b first, it is binding).
23. Shuffler algebra over BitVec 128 per lane: T = per-lane doubling,
    S_k = T + T^k = (I + T^(k-1)) T, I + T^(k-1) invertible (nilpotent T),
    ker S_k = span{2^63, 2^127}, ker on {bit 127 = 0} = {0, 2^63}, lane bit 0 of
    S_k(x) = 0; decide over k = 1..15 (and the 120 (count, position) pairs).
24. Lemma 4.1 (changed keyed checksum): the secondary difference is affine in
    the twisting pair with the quadratic term cancelling; apply PROOF.md Lemma
    5.2/5.3 in offset form -> 604^2/q (odd difference 17/q).
25. Lemma 4.2 (equal checksums, PH change): twist cancellation; d(.)V xor c
    with c independent of V; fibre <= 2 via step 23 and bit 127 = 0 of clmul;
    at most 604 masks per lane with fixed bit 0 -> 2 x 604^2/q.
26. Lemma 4.3 rows: different chunk counts q^-2 + C/q (unique fresh-pair
    solution of the checksum equality), ENH-only C/q, tag-only 4080 x 48/q.
27. Theorem 5.1: the ten-row disjoint exhaustive block ledger; column maxima
    C_0 = 3125 (from PROOF3 step 17/19), C_1 = 729632, J; distinct-key transfer
    a = (C_0 + C_1)/(q - 561), b = J/(q(q - 561)) (reuse PROOF.md 6.3).
28. Section 6 message level: reuse `EncodingInjectiveLong`, `EncodedBlocksValid`
    and the finalizer bijection; `DifferentBlockCountsBound` for both
    compressors (82/q, 46/q, joint 82 x 46/q^2, the 45-target count needs
    p >= 8); short/long (9/q, 9/q, 81/q^2 at OH indices len, len + 4);
    short/short 1/q^2 by actual collisions (equal-length short strings never
    collide, `vec_to_u64` injective per length); the two comparison
    polynomials P_0, P_1 with independent multiplier coordinates, degree
    <= 2 ceil(L/32), no constant term, identity (6).
29. Lemma 7.1: conditional on the OH key, Pr[P_0(f_0) = 0 and P_1(f_1) = 0]
    <= [r + (1-r) z_0][r + (1-r) z_1] (root bound in F_p per multiplier,
    independence of f_0, f_1 only); average -> E(L) = b(1-rho)^2 +
    a rho(1-rho) + rho^2.
30. Rational certificate (8): K < 81/128 by norm_num on the integer
    cross-product; ceil(L/32) <= 2^18 ceil(L/2^23); E(L) <= H^2 K 2^-83 ->
    `Published128` (and the 81/128-strengthened statement). Keep the IID and
    {1..p-1} variants as corollaries (p-2 < p-1; IID envelope <= distinct).
31. Optional: the score intervals (88.63, 88.64), (68.99, 69.00) by integer
    100th powers, and the dominance E <= I_4 (needs PROOF4's envelope; skip if
    PROOF4 is refuted).
32. Separately, NOT under `Published128`: `referenceFingerprint` (shared
    multiplier) with Prop 11.1 as a proved lower bound 1/(p-2), so the Lean
    file records why the reference variant is excluded.

## Reviewer artifacts

- Lens 1 (model): `scratchpad/design/umash-verify5/model/` (harness.c
  including umash.c with modes for arithmetic / block lanes / identity (6) /
  short path / Prop 11.1; model.py written from the C source; extra_checks.py
  for the Lemma 4.2 kernel facts; run_xeon.sh, run_extra.sh; xeon_logs/ with
  source_diff.txt, source_hashes.txt, upstream_head.txt and the four-build
  logs, output digest 2069ff48...6334 in both runs).
- Lens 2 (logic): `scratchpad/design/umash-verify5/logic/` (logic_recompute.py
  + xeon/logic_recompute.json, kernel_check.py + xeon/kernel_check.json,
  run_xeon.sh, README.md, xeon/run.log, xeon/theirs/checks/* regenerated on
  cores 24-31).
- Lens 3 (counts): `scratchpad/design/umash-verify5/counts/` (fp_model.py,
  fibre64.c, fibre64_v2.c, scaled_fp.c, exact_check.py, short_check.py,
  logs/ incl. fp_model.out, fibre64*.out, scaled*.out, exact.out, short.out;
  goal5_rerun/ with the byte-identical regenerated certificates).
- Lens 4 (constants): `scratchpad/design/umash-verify5/constants/`
  (constants_check.py + constants_result.json, reference_checks.py +
  reference_result.json, run_xeon_constants.sh, rerun_*.json/log,
  upstream_hashes.log, constants.log, constants2.log, reference.log).
