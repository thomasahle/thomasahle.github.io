# UMASH corrected-bound proof: consolidated verdict

Date: 2026-09-19. Proof under review: `scratchpad/codex/umash-goal/PROOF.md`
(889 lines, automated prover, maximum effort, not previously human-read).
Three independent adversarial lenses (logic chain; ENH fibre lemma + scaled/64-bit
enumeration; constants + Lean-proposition match) each returned `refuted = false`.
All computation ran on the Xeon under `~/agents/umash-verify/<lens>/`.

## Decision: HOLDS (no corrections to the bound or its constants)

Every lemma 3.1 through 9.2 was re-derived by hand against `umash_reference.py`
by at least one lens (Lemma 4.3, the new fibre lemma, by all three). Every
constant was reproduced by algorithms different from the prover's (carry-chain
automaton, signed-digit recursion, literal small-width enumeration, plus the
exact completeness identity sum_{d,t} 2^(64-popcount d) = 8q+72). The prover's
own `checks/run_xeon.sh` regenerates byte-identical certificates. Scaled
exhaustive checks of the actual bounds (w = 5..9 all pairs, w = 8..16 faithful
families with m0 = 8, ~7e7 instances, ~2e9 collisions) and 22,000 true-64-bit
fibres (r = 0..63, wraps, zero/all-ones masks, unequal tags) found zero
violations. No reviewer found a broken step or a counterexample.

Claims (1)-(4) stand exactly as stated:

| claim | statement | status |
|---|---|---|
| (1) | Pr[UMASH-64 collision] <= 45635 ceil(L/512) / 2^61 (seed fixed, distinct strings of length <= 8L, OH words without replacement or IID, multiplier uniform on {2..p-1}) | holds |
| (2) | eps(L) <= A + (1-A) min(1, 2 ceil(L/32)/(p-2)), A = 364816/(2^64-561); short/short 1/(q-561), equal-length short never | holds |
| (3) | projected ENH-only primary collision <= 5542/2^64 for any fixed common XOR mask, all valuations r | holds (loose by 25-100x empirically) |
| (4) | joint two-compressor ENH-only <= 4721784/2^128 < 2^-105 < 2^-87, closing `OpenENHOnly` for 32 <= r <= 63 | holds (IID and after /(1-561/q)) |

### Certified constants (all three lenses agree, independently recomputed)

- Mask set: |D| = 852; per-j (|j| <= 8) 1,184,123,361,62,355,121,178,1;
  pattern histogram {1:1, 2:435, 4:357, 8:59}; T_r = 2771,615,127,3,1;
  N_r = 852,248,64,2,1; bit-0 and bit-63 splits 248/604; D ∩ 16Z = {0};
  every nonzero mask has top bit >= 60.
- Block ledger (MAX over a disjoint exhaustive case split, not a sum):
  unequal chunk counts 82; odd PH 17; even PH 604^2 = 364816; ENH-only
  2*T_0 = 5542 (r >= 4) and R*T_r = 2771,1230,508,24 (r = 0..3); tag-only
  16*255*66 = 269280. C = 364816.
- Message ledger: short/long 9; unequal block counts 82; equal counts C;
  short/short different lengths exactly 1/q, same length never. A = C/(q-561),
  log2 A = -45.5232.
- (18): 2^61 (A + 32/(p-2)) = 1941055691508763227645934840993350667993088 /
  42535295865117306584003665538960066195 = 45634.00000000000004..., ceiling
  45635. (2) <= (1) checked exactly for L = 1..4096 and around 512h +- 1 up to
  h = 2^61; 45635/2^61 vs 1/2^55 is the factor 713.05 = 2^9.48.
- Joint: 5542 * 852 = 4721784 < 2^23; small-r joint 2360892, 610080, 130048,
  384. p = 2^61-1 prime by the certificate p-1 = 2 3^2 5^2 7 11 13 31 41 61
  151 331 1321, witness 37. C(34,2) = 561.

## Certified numbers for the post

Metric as in the post: score = min_L log2(L / max(2^-64, min(1, eps(L)))),
L in 64-bit words, eps floored at 2^-64 (q = 2^64 convention).

- eps formula (UMASH-64, primary, ideal keys):
  eps(1) = 1/(2^64-561) (short/short, different lengths; equal-length short
  inputs never collide);
  eps(L >= 2) = A + (1-A) min(1, 2 ceil(L/32)/(2^61-3)), A = 364816/(2^64-561).
  Linear envelope: 45635 ceil(L/512)/2^61.
- Score: 46.52 bits, attained at L = 2, under BOTH the at-most and the
  fixed-length convention (L = 1 gives 64.0).
- Per length, log2(1/eps): 1 KB (L = 128) 45.52; 1 MB (L = 2^17) 45.28;
  1 GB (L = 2^27) 37.99. (log2(L/eps): 52.52, 62.28, 64.99.)
- Published: 55 at L = 1 (1 KB / 1 MB / 1 GB: 55 / 47 / 37). Corrected is
  8.5 bits below the claimed score, 9.5 below at 1 KB, 1.7 below at 1 MB,
  and 1.0 ABOVE at 1 GB (the published per-word slope 2^-64 is twice the
  actual polynomial slope 2 ceil(L/32)/(p-2) ~ 2^-65 L).
- Previously certified (weakA = 718333281557): 25.61 bits. Gain: 20.9 bits.
- Optional, not in PROOF.md as written: a length-aware ledger (the even-PH case
  needs >= 2 chunks, i.e. L >= 3; at L = 2 the block constant is 269280) gives
  46.96 (at-most, L = 2) and 47.11 (fixed-length, L = 3). Use 46.52 unless the
  proof is amended.
- UMASH-128 / fingerprint: inherits (1) by inclusion of its collision event in
  the primary event, i.e. a LINEAR 45635 ceil(L/512)/2^61 envelope; the
  published quadratic 83-bit claim is neither proved nor refuted.

## What changes in the post's UMASH note

- The "certified fallback" for UMASH-64 rises from 25.6 to 46.5 bits (ideal
  keys: OH words without replacement, multiplier uniform on {2..p-1}); the
  displayed claim stays 55/83 per the author's 2026-09-18 decision, with the
  gap note and the link to issue #40.
- Say what is now closed: the ENH-only case at every valuation (the Lean
  `OpenENHOnly` proposition, 32 <= r <= 63, is now a theorem on paper with
  4721784/2^128 < 2^-105), and a complete unconditional all-pairs bound
  45635 ceil(L/512)/2^61, about 2^9.5 weaker than the published envelope at
  short lengths and slightly stronger than it at 1 GB.
- Say what is still open: the sharp constant (162 vs 364816 per block) and the
  PH+ENH equal-checksum joint case (`OpenPHENH`), which the new method does
  not reach. The factor-4 (2-bit) loss of the scaled models remains a
  heuristic, not a bound.
- Replace any "852^2/2^64 per block" wording by "604^2/2^64 per block"
  (the even-PH parity/top-bit refinement), if the note quotes the block
  constant.
- Scope caveat to keep: the model is the Python reference / Lean model (tag
  = seed ^ (size % 256) in the high ENH word); production `umash.c` was not
  audited in this pass beyond the ENH fold line.

## Draft comment for backtrace-labs/umash#40 (no AI attribution)

> Follow-up with an unconditional, if weaker, bound for the reference construction.
>
> Building on the 852-mask set S from the first comment, I now have a complete
> proof (natural language, computer-assisted, checked by three independent
> re-derivations, not yet formalised) that for every fixed seed, every two
> distinct inputs of at most 8L bytes, OH words sampled without replacement and
> the multiplier uniform on {2, ..., p-1},
>
>     Pr[UMASH-64 collision] <= 45635 * ceil(L/512) / 2^61,
>
> or more precisely eps(L) <= A + (1-A) min(1, 2 ceil(L/32)/(p-2)) with
> A = 364816/(2^64-561). This is 45635/64 ~ 2^9.5 weaker than the README's
> ceil(L/512)/2^55 at short lengths and slightly stronger from about 1 GB on
> (the polynomial term is 2 ceil(L/32)/(p-2), i.e. half the README's per-word
> slope).
>
> The per-block constant 364816 = 604^2 comes from the even-PH-difference case:
> after conditioning one key word, the XOR difference d*v xor c has bit 0 and
> bit 127 fixed, and only 604 of the 852 masks are available in each lane. The
> other cases are smaller: odd PH differences 17, tag-only < 269280, unequal
> chunk counts 82, and, new, the ENH-only case at most 5542/2^64 at every
> valuation of the change and with any common XOR mask (a lifting argument: for
> fixed A, wrap, mask m and pattern t there is at most one B with L = L' and
> the high lanes congruent mod p, because the low bits of AB depend only on the
> low bits of B). The same lifting gives 5542 * 852 / 2^128 < 2^-105 for the
> primary and secondary compressors jointly in the ENH-only case, which closes
> the "two final words change, high valuation" case that the earlier comments
> left open.
>
> What is still not proved: the constant 162 (or anything near the README's
> 2^-55 per block) and the case where exactly one PH chunk and the final ENH
> pair change with equal checksums. The 604^2 count treats the two lanes of a
> single difference as independent and is certainly far from tight; the
> scaled models still suggest the true loss is about 2 bits.
>
> Full write-up (proof, certificates, and the scripts that regenerate the mask
> tables and the rational inequalities) available on request; I will link it
> once it is cleaned up. Verified against umash_reference.py; I did not
> re-audit umash.c beyond the ENH fold.

## What remains open, and whether the method extends

- `SharpPrimaryProjection` = PrimaryIdentityBound(162/(q-561)). Needs the block
  constant down from 364816 to 162. Binding case: even PH (604^2). The two lane
  masks come from the same v, so the true count is much smaller than 604^2, but
  no argument bounds the pair count; the ENH constant 5542 would also have to
  fall (empirically it is 25-100x loose, per-A fibres <= 36 vs cap 5542).
  Plausibility: the mask machinery is the right tool, but reaching 162 needs a
  joint lane count for clmul differences, a new lemma, not a refinement.
- `OpenPHENH` (one PH chunk + two ENH words change, equal checksums, r in
  {1,2,3} or 36..63). Lemma 4.3 needs a COMMON XOR offset; here the PH offsets
  differ between the two messages, and PROOF.md Section 11 correctly says
  treating them as common would be invalid. The method does not extend as is;
  one would need a two-offset fibre lemma or to exploit the PH key freedom
  jointly with the ENH pair.
- Not covered by any lens: semantic correspondence of the Lean definitions
  (`jointEvent`, `Valid`, `blockTag`) to the reference beyond the reads done
  here; production `umash.c` vs the Python reference; the Codex `checks/`
  scripts themselves (reproduced but not relied upon).

## Presentation items to fix in PROOF.md before a Lean lane (none affect the bound)

1. State the case split explicitly as disjoint and exhaustive: (chunk counts
   differ) | (same counts, some PH chunk differs: odd coordinate exists / all
   even) | (same counts, PH agree, final data differ) | (chunks agree, tags
   differ). The block constant is a max over these, union bounds only inside.
2. Lemma 4.4: add the step "on the event, (A,B) -> (A, beta, m, t) is an
   injection into a set of size q * 2 * T_0" (beta = [B+eps >= q], m = U xor U',
   t = X_high & m in P(m) by Lemma 3.1).
3. Theorem 4.5: say that the uniform 5542 for r <= 3 rests on R*T_r <= 2*T_0
   (2771, 1230, 508, 24 <= 5542) and bounds a different event (low lane only).
4. Lemma 4.3: one sentence that z(B) = U & m = ((AB) & m) xor (M_high & m) xor t,
   so the count depends on B only through low product bits (U, U' vs X_high,
   Y_high differ by the common offset M_high xor L).
5. Note that enhChanges = 2 makes both increments nonzero; the proof also covers
   eps = 0, which Lean does not need; r = min XOR valuation = additive valuation
   after exchanging operands.
6. Optional tightenings, not needed: Lemma 6.1 constant 66 -> 48 (tag-only
   195840); length-aware C(2) = 269280.
7. Section 10 wording: exhaustive_lift.cpp at w = 3,4 uses p = 1 (no real
   projection); only w = 5,6 exercise it and none is faithful (m0 != 8). Say so,
   and point to the faithful w = 8..16 checks from this review instead.
8. Lean integration: the all-pairs theorem handles short/short by actual
   collision, so it does NOT give PrimaryIdentityBound(A) (which quantifies over
   constant-polynomial identity). `certified64_of_primary_and_short` cannot be
   reused by swapping 718333281557 -> 364816; a new assembly lemma is needed and
   `UMASHConstants.lean` still carries weakA = 718333281557.

## Ready for a Lean lane: yes, with the assembly rewritten. Order

1. Mask certificate: D (852), patterns P(d), T_r, N_r, parity/top-bit 604,
   D ∩ 16Z = {0}, min top bit 60 (decide/native_decide over the carry
   automaton, or import the table and prove soundness + the completeness
   identity 8q + 72).
2. Lemma 3.1 (x - y = 2(x & d) - d, |j| <= 8) and Lemma 3.2.
3. Lemma 4.1 (uniform on R Z/q) and Lemma 4.2 (R T_r / q).
4. Lemma 4.3 (fibre uniqueness, least-differing-bit) and Lemma 4.4 (injection
   into q * 2 * T_0), Theorem 4.5.
5. Lemma 5.1 (clmul injective in v), 5.2 (17 targets), 5.3 (604^2).
6. Lemma 6.1 (66 q point mass), 6.2 (tag-only 269280), 6.3 (82 q - 81),
   Theorem 6.4 (block max, with the explicit disjoint split).
7. Lemma 7.1 and Theorem 7.2 -> `OpenENHOnly` (this can be discharged first
   and independently: it needs only 1-4, 5.1 and 7.1).
8. Lemmas 8.1-8.4 (encoding injective, short branch, finalizer bijective,
   identity bound with >= 1 long message).
9. Lemma 9.1 (distinct-key rejection 561), Lemma 9.2 (roots, p prime,
   multiplier set p-2), then the new assembly: identity bound C/(q-561) for
   pairs with a long message + direct short/short bound -> `CertifiedAllPairs64`
   with certifiedEnvelope L = min(1, 45635 ceil(L/512)/2^61) (or the sharper (2)).
10. `CertifiedAllPairs128` by the existing inheritance theorem.

## Reviewer artifacts

- Lens 1 (logic): `scratchpad/design/umash-verify/logic/verify_constants.py`,
  `constants_out.json` (PASS), `enh_bound_check.c` with enh_w{5,6,8}.json
  (0 failures); Xeon `~/agents/umash-verify/logic/run.log`.
- Lens 2 (ENH fibre): masks64.py, enh_scaled.c, brute_check.py, enh_fibre64.c
  under `~/agents/umash-verify/` on the Xeon; exhaustive_lift rerun sha256
  4ec229f0... identical.
- Lens 3 (constants): `scratchpad/design/umash-verify/constants/` (my_certify.py,
  enh_scaled.c, compare.py, short_check.py, refine.py, README.md, xeon/ with
  my_certify.json 56/56 PASS and regenerated proof certificates under xeon/theirs/).
