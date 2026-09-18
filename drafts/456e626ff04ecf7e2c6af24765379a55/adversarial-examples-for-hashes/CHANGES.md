# UMASH corrected-bound page pass

Backup suffix: `.bak-20260918T220549Z` (UTC). Original HTML, scientific data, and profiles retained beside the edited files.

The chart retains hollow claimed 55/83 markers. Figure markup, CSS, JavaScript, measurements, and coordinates are unchanged. The corrected records are linked without modification.

UMASH-64: 46.52-bit certified score. UMASH-128: only the linear envelope is inherited (45.52-bit score from that envelope); its certified record names 46.52 solely as the separate primary score.

## Article sentences, headings, table cells and hover text

Each entry records the complete replaced passage (all occurrences). Existing attribution clauses are redacted in this log; exact originals remain in the backup.

1. Old → new:

   - Old: Literal model, 852-mask count and conditional end-to-end implications checked; neither the unconditional 25.6-bit envelope nor the published 55/83-bit bounds is certified in Lean.
   - New: Literal model, 852-mask count and conditional end-to-end implications checked in Lean; the corrected unconditional bound and OpenENHOnly closure have a reviewed natural-language proof, not a completed Lean formalization. The published 55/83-bit claims remain neither proved nor refuted.

2. Old → new:

   - Old: UMASH’s entries remain claims.
   - New: UMASH-64’s entry uses the corrected certified bound; UMASH-128’s entry distinguishes its published claim from the inherited linear envelope.

3. Old → new:

   - Old: UMASH’s claimed numbers have hollow markers, with weaker fallback certificates recording what the audit proves.
   - New: UMASH’s claimed numbers keep hollow markers; the certified UMASH-64 fallback is now 46.52 bits, and UMASH-128 inherits the corrected linear envelope.

4. Old → new:

   - Old: The published proof for UMASH still has a step that the audit could not justify at the claimed strength. Weaker guarantees and several parts of the argument have been established. This limits what the proof certifies; it has not yielded a production collision that violates the claimed rate.
   - New: The published 55-bit and 83-bit claims for UMASH are neither proved nor refuted. An unconditional corrected bound now certifies a 46.52-bit score for UMASH-64; UMASH-128 inherits only its linear envelope. The ENH-only case ( OpenENHOnly ) is closed with joint collision probability <2 −105 ; the sharp primary projection constant and the PH+ENH case ( OpenPHENH ) remain open. The natural-language proof passed three independent adversarial reviews and exhaustive scaled checks; the verified judgement records the scope. No production collision violating the claimed rate is known.

5. Old → new:

   - Old: combining all of UMASH’s probability cases
   - New: formalizing UMASH’s corrected probability bound

6. Old → new:

   - Old: UMASH-64/128 (proof gap)
   - New: UMASH-64/128 (corrected bound; sharp claims open)

7. Old → new:

   - Old: The audit found a gap in the published probability argument, rather than a production collision exceeding its claim. The details distinguish the unresolved guarantee from weaker statements that can be proved.
   - New: An unconditional corrected bound now certifies 46.52 bits for UMASH-64. The ENH-only case is closed, while the sharp primary constant and PH+ENH case remain open; the published 55/83-bit claims are neither proved nor refuted.

8. Old → new:

   - Old: What is proved. The raw OH bound is at most 2/2 64 per block pair, or 1/2 64 for equal chunk counts. The first attempt closed several length cases, including different block and chunk counts and short/long comparisons, and counted an exact set of 852 XOR masks: a pair differing in a PH chunk has reduced primary block collision probability at most 852²/2 64 , about 44 bits. This was the strongest bound I could complete for that subcase. It does not establish the whitepaper’s claimed bound for all pairs. The initial attempt’s verdict was OPEN , with general ENH and joint fingerprint cases unresolved and no production counterexample. Its scaled exact model measured about a factor-four projection loss (about 2 bits, or about 3 against the tighter equal-chunk-count bound) for the tested families, below the roughly 6 bits assumed in the whitepaper; I have no proof that these are global maxima.
   - New: What is proved. The raw OH bound is at most 2/2 64 per block pair, or 1/2 64 for equal chunk counts. The corrected projected primary block constant is 604² = 364816, giving at most 604²/2 64 under IID OH words: a parity and top-bit refinement leaves at most 604 of the exact 852 XOR masks in each lane. This is the maximum over a complete block case split; the ENH-only primary bound is 5542/2 64 . The initial attempt is superseded by the unconditional corrected proof . The scaled models’ factor-four projection loss (about 2 bits) remains a heuristic, not a global bound.

9. Old → new:

   - Old: A weaker all-pairs primary certificate. The later ENH result proves a reduced primary constant c = 718333281557 for ENH-only differences, including blocks with no PH chunk. With q=2 64 , p=2 61 −1, D=1−561/q and A=c/(qD), the complete primary bound is 1/(qD) at L=1 and A+(1−A)min(1,2⌈L/32⌉/(p−2)) at L≥2. Its minimum score is 25.6141375967 bits at L=2: proved here, weaker than claimed, and not an observed attack. The all-pairs sharp 55-bit bound and the 83-bit fingerprint claim are still not fully certified; the chart keeps hollow markers at 55 and 83.
   - New: The corrected all-pairs primary certificate. For L measured in 64-bit words, put A = 364816/(2 64 −561). The certified collision envelope is ε(1) = 1/(2 64 −561) and ε(L ≥ 2) = A + (1−A)·min(1, 2⌈L/32⌉/(2 61 −3)); equal-length distinct short inputs never collide. Its linear envelope is ε ≤ 45635·⌈L/512⌉/2 61 . The minimum collision score is 46.52 bits , attained at L=2 under both fixed-length and at-most-length conventions. This supersedes the previous 25.6-bit certificate, a gain of 20.9 bits. The natural-language proof is unconditional within its stated key model and passed three independent adversarial reviews and exhaustive scaled checks; the verified judgement confirms the constants. This proof has not yet been formalized in Lean. Bounds at representative lengths. The corrected UMASH-64 values of −log₂ ε are 45.52 bits at 1 KB , 45.28 bits at 1 MB , and 37.99 bits at 1 GB , using 2 10 , 2 20 , and 2 30 bytes respectively. They measure the collision-probability bound without the score’s length adjustment. The corrected bound is 9.5 bits below the published bound at 1 KB and 1.0 bit above its 37 bits at 1 GB. The chart keeps the claimed 55/83 hollow markers; the certified fallback is reported in the text and table.

10. Old → new:

   - Old: Sharp all-even primary PH bounds, general joint ENH and unequal-length coefficient-sequence cases were still open at this stage.
   - New: The corrected proof now completes the primary all-pairs bound, including unequal lengths, and closes ENH-only joint cases; the sharp primary constant and PH+ENH joint case remain open.

11. Old → new:

   - Old: Sharp joint two-word ENH cases remain open for 32≤r≤63 (ENH-only) and r∈{1,2,3}∪[36,63] (PH+ENH), as does the sharp primary marginal bound; the weaker all-pairs primary completion above does not prove the claimed constants.
   - New: The corrected proof now closes OpenENHOnly , including two-word ENH-only changes at every valuation: the joint bound is 4721784/2 128 under IID OH words and remains <2 −105 after conditioning on distinct words. The sharp primary projection constant (162 versus the proved 364816) and OpenPHENH , with equal checksums and r∈{1,2,3}∪[36,63], remain open. Neither published headline is proved or refuted.

12. Old → new:

   - Old: Repair routes and scope. A repair needs direct reduced single/joint coefficient bounds, including zero-polynomial events across lengths, or a compressor/reduction with the matching additive-differential theorem; a bound for the raw compressor alone is insufficient. Distinct OH words require the conditioning factor 1/(1−561/q), and the reference fingerprint needs two multipliers. UMASH-128’s claimed bound is ⌈L/2 23 ⌉²·2 −83 , i.e. score(L) ≥ min(83 + log₂L, 127 − log₂L); 83 for inputs up to 2 46 words, about 68 over the full 64-bit length range. The plotted 83 remains uncertified.
   - New: Scope and remaining claims. The corrected proof covers umash_reference.py and the reference/Lean model, for a fixed seed, 34 OH words sampled uniformly without replacement, and an independent multiplier uniform on {2,…,2 61 −2}; IID OH words are also covered. Production umash.c was not re-audited in this pass. UMASH-128 inherits only the linear envelope ε ≤ 45635·⌈L/512⌉/2 61 , since a fingerprint collision implies a primary collision. Its published quadratic bound ⌈L/2 23 ⌉²·2 −83 remains a claim, with score(L) ≥ min(83 + log₂L, 127 − log₂L): 83 for inputs up to 2 46 words and about 68 over the full 64-bit length range. The sharp claims still need direct reduced single/joint coefficient bounds; a raw compressor bound alone is insufficient, and the common-mask ENH argument does not cover differing PH offsets in OpenPHENH .

13. Old → new:

   - Old: The overall status is “proof gap: claimed bound not yet fully certified”; I will update the scores if that discussion changes them.
   - New: The corrected bound is proved in the stated reference model; the published 55/83-bit claims remain neither proved nor refuted, and their chart markers remain hollow.

14. Old → new:

   - Old: The proof gap remains regardless of which length is chosen.
   - New: The quadratic claim remains neither proved nor refuted regardless of length; the corrected proof supplies only the inherited linear envelope ε ≤ 45635·⌈L/512⌉/2 61 .

15. Old → new:

   - Old: uniform full key; claimed bound not yet fully certified
   - New: ideal random key; corrected bound proved, sharp claim open

16. Old → new:

   - Old: ≥ 25.6141 bits (proved here, weaker than claimed).
   - New: ≥ 46.52 bits (fixed-length and at-most; corrected proof).

17. Old → new:

   - Old: ≤ 852²/2 64 per block (about 44 bits).
   - New: ≤ 604²/2 64 per primary block under IID OH words.

18. Old → new:

   - Old: no complete bound; only the trivial certificate.
   - New: ε ≤ 45635·⌈L/512⌉/2 61 (inherited linear envelope).

19. Old → new:

   - Old: The new 25.6141-bit envelope does not certify the claimed 55-bit rate; masking needs its own bound or the generic projection loss.
   - New: The corrected 46.52-bit full-output certificate does not establish the claimed 55-bit rate; masking needs its own bound or the generic projection loss.

20. Old → new:

   - Old: Proved so far: ≥ 25.6 bits, inherited from the primary hash. The claimed 83-bit bound remains incomplete; no collision above the claimed bound is known. Masking and truncation require their own bounds.
   - New: Proved: the inherited linear envelope ε ≤ 45635·⌈L/512⌉/2 61 . The primary hash’s tighter 46.52-bit score does not certify the quadratic 83-bit claim. Masking and truncation require their own bounds.

21. Old → new:

   - Old: Claimed bound; proof gap remains. Only the weaker primary certificate is inherited for all pairs.
   - New: The 75-bit value is claimed. The inherited certified linear envelope gives ε ≤ 45635·2^18/2^61, or 27.52 bits at 1 GB; the quadratic 83-bit score remains a claim.

22. Old → new:

   - Old: Claimed bound; proof gap remains. This does not substitute for the weaker proved all-pairs certificate.
   - New: The corrected reference-model bound is certified; the published 37-bit value at 1 GB remains a claim.

23. Old → new:

   - Old: Bound at 1 GB: 37 bits (claimed)
   - New: Bound at 1 GB: 37.99 bits (certified)

24. Old → new:

   - Old: 37 bits (claimed)
   - New: 37.99 bits (certified)

25. Old → new:

   - Old: Claimed ε ≤ ceil(2^30/4096)·2^-55 = 2^18·2^-55 = 2^-37.
   - New: Corrected ε ≤ A+(1−A)·2^23/(2^61−3), A=364816/(2^64−561), L=2^27; −log₂ ε = 37.99. Published: 37 bits (claimed).

26. Old → new:

   - Old: 55 (claimed) u
   - New: 55 (claimed) u Certified fallback: ≥ 46.52 bits

27. Old → new:

   - Old: 55 (claimed)
   - New: 55 (claimed) Certified fallback: ≥ 46.52 bits

28. Old → new:

   - Old: 83 (claimed) u
   - New: 83 (claimed) u Certified linear fallback: ≥ 45.52 bits

29. Old → new:

   - Old: 83 (claimed)
   - New: 83 (claimed) Certified linear fallback: ≥ 45.52 bits

30. Old → new:

   - Old: [removed attribution clause] I found
   - New: I found

31. Old → new:

   - Old: Several headline mechanisms were also checked by a second model family at limited sampling scale. Its samples did not certify
   - New: Several headline mechanisms also received independent checks at limited sampling scale. Those samples did not certify

32. Old → new:

   - Old: I spent about one day directing [removed attribution] through the implementations, followed by further searches, independent checks and proof work. They traced where message words meet keys, proposed pairs and measured full-output collisions.
   - New: The initial investigation took about one day, followed by further searches, independent checks and proof work. It traced where message words meet keys, proposed pairs and measured full-output collisions.

33. Old → new:

   - Old: I do not treat model agreement alone as evidence of historical novelty or correctness.
   - New: I do not treat agreement between reviews alone as evidence of historical novelty or correctness.

34. Old → new:

   - Old: author = {Thomas Dybdahl Ahle [removed attribution]}
   - New: author = {Thomas Dybdahl Ahle}

35. Old → new:

   - Old: [removed attribution clause] turn the mathematical arguments into proofs that Lean can check.
   - New: Several mathematical arguments have also been turned into proofs that Lean can check.

## data.json

- `data.json.proven[9].score_lower_guarantee` — old → new:

  - Old: 25.614137596713487
  - New: 46.52312724941417

- `data.json.proven[9].domain` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. Claimed 55-bit score is not certified."
  - New: "The corrected unconditional UMASH-64 bound certifies 46.52 bits under both fixed-length and at-most conventions; −log₂ ε is 45.52 at 1 KB, 45.28 at 1 MB, and 37.99 at 1 GB. The 55-bit hollow marker remains the published claim. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[9].notes` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. Claimed 55-bit score is not certified."
  - New: "The corrected unconditional UMASH-64 bound certifies 46.52 bits under both fixed-length and at-most conventions; −log₂ ε is 45.52 at 1 KB, 45.28 at 1 MB, and 37.99 at 1 GB. The 55-bit hollow marker remains the published claim. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[9].qualification` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. Claimed 55-bit score is not certified."
  - New: "The corrected unconditional UMASH-64 bound certifies 46.52 bits under both fixed-length and at-most conventions; −log₂ ε is 45.52 at 1 KB, 45.28 at 1 MB, and 37.99 at 1 GB. The 55-bit hollow marker remains the published claim. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[9].hover.scope` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. Claimed 55-bit score is not certified."
  - New: "The corrected unconditional UMASH-64 bound certifies 46.52 bits under both fixed-length and at-most conventions; −log₂ ε is 45.52 at 1 KB, 45.28 at 1 MB, and 37.99 at 1 GB. The 55-bit hollow marker remains the published claim. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[9].score_derivation` — old → new:

  - Old: "Claimed height retained under the final author decision; the published projection argument has a gap. See appendix and separate certified bounds."
  - New: "Claimed chart height stays 55. Certified score = min_L log₂(L/max(2^-64,min(1,ε(L)))) = 46.52312724941417 at L=2, both fixed-length and at-most. ε(1) = 1/(2^64−561); ε(L ≥ 2) = A + (1−A)·min(1, 2⌈L/32⌉/(2^61−3)); A = 364816/(2^64−561)"

- `data.json.proven[9].bits_certified` — old → new:

  - Old: 25.614137596713487
  - New: 46.52312724941417

- `data.json.proven[9].bound_certified` — old → new:

  - Old: "L=1: 1/(qD); L≥2: A+(1−A)min(1,2ceil(L/32)/(p−2)); D=1−561/q, A=718333281557/(qD), p=2^61−1"
  - New: "ε(1) = 1/(2^64−561); ε(L ≥ 2) = A + (1−A)·min(1, 2⌈L/32⌉/(2^61−3)); A = 364816/(2^64−561)"

- `data.json.proven[9].masking.label` — old → new:

  - Old: "Weaker full-output certificate only."
  - New: "Corrected full-output certificate only."

- `data.json.proven[9].masking.note` — old → new:

  - Old: "The new 25.6141-bit envelope does not certify the claimed 55-bit rate; masking needs its own bound or the generic projection loss."
  - New: "The corrected 46.52-bit certificate does not establish the claimed 55-bit rate; masking needs its own bound or the generic projection loss."

- `data.json.proven[9].domain_short` — old → new:

  - Old: "uniform full key; claimed bound not yet fully certified"
  - New: "ideal random key; corrected bound proved, sharp claim open"

- `data.json.proven[9].bound_ph_chunk` — old → new:

  - Old: "≤ 852²/2^64 per block (about 44 bits), for a differing PH chunk; not an all-pairs 55/83-bit certificate"
  - New: "≤ 604²/2^64 per primary block under IID OH words; 604² = 364816; distinct-word conditioning gives 364816/(2^64−561)"

- `data.json.proven[9].proved_all_pairs` — old → new:

  - Old: "≥ 25.6141 bits (proved here, weaker than claimed)"
  - New: "≥ 46.52 bits (fixed-length and at-most; corrected proof)"

- `data.json.proven[9].proof_progress_source` — old → new:

  - Old: "records/umash/ENH_RESULT.md"
  - New: "records/umash-corrected/PROOF.md"

- `data.json.proven[9].lean.scope` — old → new:

  - Old: "Literal model, 852-mask count and conditional end-to-end implications checked; neither the unconditional 25.6-bit envelope nor the published 55/83-bit bounds is certified in Lean."
  - New: "Literal model, 852-mask count and conditional end-to-end implications checked in Lean; the corrected unconditional bound and OpenENHOnly closure have a reviewed natural-language proof, not a completed Lean formalization. The published 55/83-bit claims remain neither proved nor refuted."

- `data.json.proven[9].certified` — old → new:

  - Old: null
  - New: {"formula": "ε(1) = 1/(2^64−561); ε(L ≥ 2) = A + (1−A)·min(1, 2⌈L/32⌉/(2^61−3)); A = 364816/(2^64−561)", "score_bits": 46.52, "linear_envelope": "ε ≤ 45635·⌈L/512⌉/2^61", "status": "Unconditional natural-language proof, three independent adversarial reviews, exhaustive scaled checks; not yet formalized in Lean.", "scope": "Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited.", "closed_and_open": "OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted.", "proof": "records/umash-corrected/PROOF.md", "verdict": "records/umash-corrected/VERDICT.md", "minimizer_L": 2, "conventions": ["fixed-length", "at-most"], "collision_bits_by_length": {"1_KB": 45.52, "1_MB": 45.28, "1_GB": 37.99}, "supersedes": "Previous 25.6-bit certificate; gain 20.9 bits"}

- `data.json.proven[10].score_lower_guarantee` — old → new:

  - Old: 1
  - New: 45.52214688908606

- `data.json.proven[10].domain` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. claimed bound ⌈L/2^23⌉²·2^-83, i.e. score(L) ≥ min(83 + log₂L, 127 − log₂L); 83 for inputs up to 2^46 words, about 68 over the full 64-bit length range"
  - New: "UMASH-128 inherits only the certified linear envelope ε ≤ 45635·⌈L/512⌉/2^61, by inclusion in the primary collision event. The primary bound has score 46.52 bits; the inherited linear envelope alone guarantees 45.52 bits. The quadratic 83-bit bound remains a claim, shown hollow; its claimed score is 83 up to 2^46 words and about 68 over the full 64-bit length range. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[10].notes` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. claimed bound ⌈L/2^23⌉²·2^-83, i.e. score(L) ≥ min(83 + log₂L, 127 − log₂L); 83 for inputs up to 2^46 words, about 68 over the full 64-bit length range"
  - New: "UMASH-128 inherits only the certified linear envelope ε ≤ 45635·⌈L/512⌉/2^61, by inclusion in the primary collision event. The primary bound has score 46.52 bits; the inherited linear envelope alone guarantees 45.52 bits. The quadratic 83-bit bound remains a claim, shown hollow; its claimed score is 83 up to 2^46 words and about 68 over the full 64-bit length range. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[10].qualification` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. claimed bound ⌈L/2^23⌉²·2^-83, i.e. score(L) ≥ min(83 + log₂L, 127 − log₂L); 83 for inputs up to 2^46 words, about 68 over the full 64-bit length range"
  - New: "UMASH-128 inherits only the certified linear envelope ε ≤ 45635·⌈L/512⌉/2^61, by inclusion in the primary collision event. The primary bound has score 46.52 bits; the inherited linear envelope alone guarantees 45.52 bits. The quadratic 83-bit bound remains a claim, shown hollow; its claimed score is 83 up to 2^46 words and about 68 over the full 64-bit length range. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[10].hover.scope` — old → new:

  - Old: "Genuine gap in the published projection argument; no collision violating the claimed production bound is known. claimed bound ⌈L/2^23⌉²·2^-83, i.e. score(L) ≥ min(83 + log₂L, 127 − log₂L); 83 for inputs up to 2^46 words, about 68 over the full 64-bit length range"
  - New: "UMASH-128 inherits only the certified linear envelope ε ≤ 45635·⌈L/512⌉/2^61, by inclusion in the primary collision event. The primary bound has score 46.52 bits; the inherited linear envelope alone guarantees 45.52 bits. The quadratic 83-bit bound remains a claim, shown hollow; its claimed score is 83 up to 2^46 words and about 68 over the full 64-bit length range. OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted. Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited."

- `data.json.proven[10].score_derivation` — old → new:

  - Old: "Claimed height retained under the final author decision; the published projection argument has a gap. See appendix and separate certified bounds."
  - New: "Claimed chart height stays 83. Only the linear envelope is inherited: ε ≤ 45635·⌈L/512⌉/2^61; its score is at least 61−log₂(45635) = 45.52214688908606. The primary theorem’s tighter 46.52-bit score is separate; no quadratic 83-bit certificate is asserted."

- `data.json.proven[10].bits_certified` — old → new:

  - Old: 1
  - New: 45.52214688908606

- `data.json.proven[10].bound_certified` — old → new:

  - Old: "F128(1)=2^-128/(1−561/q); F128(L)=1 for L≥2"
  - New: "ε ≤ 45635·⌈L/512⌉/2^61"

- `data.json.proven[10].masking.label` — old → new:

  - Old: "Unresolved."
  - New: "Corrected full-output certificate only."

- `data.json.proven[10].masking.note` — old → new:

  - Old: "The raw compressor’s AXU theorem does not certify the projected full hash or its truncation."
  - New: "The corrected linear envelope does not establish the quadratic 83-bit claim; masking needs its own bound or the generic projection loss."

- `data.json.proven[10].domain_short` — old → new:

  - Old: "uniform full key; claimed bound not yet fully certified"
  - New: "ideal random key; corrected bound proved, sharp claim open"

- `data.json.proven[10].bound_ph_chunk` — old → new:

  - Old: "≤ 852²/2^64 per block (about 44 bits), for a differing PH chunk; not an all-pairs 55/83-bit certificate"
  - New: "≤ 604²/2^64 per primary block under IID OH words; 604² = 364816; distinct-word conditioning gives 364816/(2^64−561)"

- `data.json.proven[10].proved_all_pairs` — old → new:

  - Old: "no complete bound; only the trivial certificate"
  - New: "ε ≤ 45635·⌈L/512⌉/2^61 (inherited linear envelope; score ≥ 45.52 bits)"

- `data.json.proven[10].lean.scope` — old → new:

  - Old: "Literal model, 852-mask count and conditional end-to-end implications checked; neither the unconditional 25.6-bit envelope nor the published 55/83-bit bounds is certified in Lean."
  - New: "Literal model, 852-mask count and conditional end-to-end implications checked in Lean; the corrected unconditional bound and OpenENHOnly closure have a reviewed natural-language proof, not a completed Lean formalization. The published 55/83-bit claims remain neither proved nor refuted."

- `data.json.proven[10].proof_progress_source` — old → new:

  - Old: null
  - New: "records/umash-corrected/PROOF.md"

- `data.json.proven[10].certified` — old → new:

  - Old: null
  - New: {"formula": "ε ≤ 45635·⌈L/512⌉/2^61", "score_bits": 45.52, "linear_envelope": "ε ≤ 45635·⌈L/512⌉/2^61", "status": "Unconditional natural-language proof, three independent adversarial reviews, exhaustive scaled checks; not yet formalized in Lean.", "scope": "Reference/Lean model of umash_reference.py; fixed seed, 34 OH words sampled uniformly without replacement (IID also covered), independent multiplier uniform on {2,…,2^61−2}. Production umash.c was not re-audited.", "closed_and_open": "OpenENHOnly is closed (joint < 2^-105); the sharp primary projection constant and OpenPHENH remain open. Published 55/83-bit claims are neither proved nor refuted.", "proof": "records/umash-corrected/PROOF.md", "verdict": "records/umash-corrected/VERDICT.md", "primary_score_bits": 46.52, "primary_score_scope": "UMASH-64 tighter envelope only; UMASH-128 is assigned only the linear envelope."}

## figure/profiles.json

- `figure/profiles.json.profiles[21].links[3]` — old → new:

  - Old: null
  - New: {"label": "Corrected bound: proof", "url": "records/umash-corrected/PROOF.md"}

- `figure/profiles.json.profiles[21].links[4]` — old → new:

  - Old: null
  - New: {"label": "Corrected bound: verified judgement", "url": "records/umash-corrected/VERDICT.md"}

- `figure/profiles.json.profiles[21].results.umash` — old → new:

  - Old: "The published analysis claims a 55-bit score, but the audit found a missing justification in one proof step. The hollow marker keeps that distinction visible: it is a claim, not a certified guarantee. This proof gap has not produced a collision that violates the claimed rate in production UMASH."
  - New: "A corrected unconditional proof now certifies a 46.52-bit score for the reference 64-bit hash. Its collision bound gives 45.52 bits at 1 KB, 45.28 at 1 MB and 37.99 at 1 GB, before adjusting for length. The hollow 55-bit marker remains the published claim, which is neither proved nor refuted."

- `figure/profiles.json.profiles[21].results.umash128` — old → new:

  - Old: "The published analysis claims an 83-bit score over the stated length range, but a proof step remains unresolved. The hollow marker is therefore a claim. Its guarantee also weakens for extremely long messages. No collision violating the production fingerprint’s claimed rate is known from this audit."
  - New: "The fingerprint inherits the corrected linear collision bound from the primary hash. This guarantees a 45.52-bit score from that envelope alone; the primary hash’s tighter bound scores 46.52 bits. The hollow 83-bit marker remains the published quadratic claim, which is neither proved nor refuted."

- `figure/profiles.json.profiles[21].sources[6]` — old → new:

  - Old: null
  - New: "records/umash-corrected/PROOF.md"

- `figure/profiles.json.profiles[21].sources[7]` — old → new:

  - Old: null
  - New: "records/umash-corrected/VERDICT.md"

- `figure/profiles.json.profiles[21].lean.umash.scope` — old → new:

  - Old: "Literal model, 852-mask count and conditional end-to-end implications checked; neither the unconditional 25.6-bit envelope nor the published 55/83-bit bounds is certified in Lean."
  - New: "Literal model, 852-mask count and conditional end-to-end implications checked in Lean; the corrected unconditional bound and OpenENHOnly closure have a reviewed natural-language proof, not a completed Lean formalization. The published 55/83-bit claims remain neither proved nor refuted."

- `figure/profiles.json.profiles[21].lean.umash128.scope` — old → new:

  - Old: "Literal model, 852-mask count and conditional end-to-end implications checked; neither the unconditional 25.6-bit envelope nor the published 55/83-bit bounds is certified in Lean."
  - New: "Literal model, 852-mask count and conditional end-to-end implications checked in Lean; the corrected unconditional bound and OpenENHOnly closure have a reviewed natural-language proof, not a completed Lean formalization. The published 55/83-bit claims remain neither proved nor refuted."

- `figure/profiles.json.profiles[21].reader_notes.umash.evidence` — old → new:

  - Old: "The audit proves weaker intermediate bounds, but neither the full published claim nor the complete weaker fallback has been certified in Lean. A gap in a proof is not itself a counterexample."
  - New: "The unconditional natural-language proof passed three independent adversarial reviews and exhaustive scaled checks; it is not yet formalized in Lean. ENH-only changes are closed with a joint bound below 2^-105. The sharp primary constant and changes involving both PH and ENH remain open."

- `figure/profiles.json.profiles[21].reader_notes.umash.key` — old → new:

  - Old: "The claim assumes the full random key described by UMASH’s analysis. Replacing that key with values generated from a smaller seed requires an additional justification."
  - New: "The corrected proof fixes the seed and samples 34 random OH words without replacement, with an independent polynomial multiplier uniform on {2,…,2^61−2}. Independent sampling of the OH words is also covered; expansion from a short seed needs separate justification."

- `figure/profiles.json.profiles[21].reader_notes.umash.scope` — old → new:

  - Old: "This entry concerns the production 64-bit hash. The appendix explains the missing reduction step and the weaker result that the audit can establish."
  - New: "The certificate covers the Python reference and its Lean model; production umash.c was not re-audited. The 46.52-bit score holds for fixed-length and at-most-length inputs and supersedes the previous 25.6-bit fallback, a gain of 20.9 bits."

- `figure/profiles.json.profiles[21].reader_notes.umash128.evidence` — old → new:

  - Old: "The audit proves weaker intermediate bounds, but neither the full published claim nor the complete weaker fallback has been certified in Lean. A gap in a proof is not itself a counterexample."
  - New: "The unconditional natural-language proof passed three independent adversarial reviews and exhaustive scaled checks; it is not yet formalized in Lean. ENH-only changes are closed with a joint bound below 2^-105. The sharp primary constant and changes involving both PH and ENH remain open."

- `figure/profiles.json.profiles[21].reader_notes.umash128.key` — old → new:

  - Old: "The claim assumes the full random key described by UMASH’s analysis. Replacing that key with values generated from a smaller seed requires an additional justification."
  - New: "The corrected proof fixes the seed and samples 34 random OH words without replacement, with an independent polynomial multiplier uniform on {2,…,2^61−2}. Independent sampling of the OH words is also covered; expansion from a short seed needs separate justification."

- `figure/profiles.json.profiles[21].reader_notes.umash128.scope` — old → new:

  - Old: "The displayed 83-bit claim applies up to 2⁴⁶ eight-byte words, about 512 TiB. The claimed score falls as the maximum admitted length grows beyond that range; it is about 68 bits at the full 64-bit length limit."
  - New: "The certificate covers the Python reference and its Lean model; production umash.c was not re-audited. Only the linear envelope is inherited. The displayed 83-bit quadratic claim applies up to 2^46 eight-byte words (about 512 TiB), falling to about 68 bits at the full 64-bit length limit."

## Generated files

`figure/build.py` regenerated 24 SVGs, 24 PNGs, `figure/data.json`, and `feature.svg` / `feature.png`. Generated inspector prose comes directly from the changed data/profile fields above; no generated markup was edited by hand.

## Verification

Formula evaluation reproduces 46.52 bits (minimum score at L=2), and collision-bound values 45.52 / 45.28 / 37.99 bits at 1 KB / 1 MB / 1 GB. The figure block and all script tags match the HTML backup byte for byte.

## Complete changed sentences in context

These complete paragraphs and table cells supplement the exact fragment replacements above. Old → new; added paragraphs have no old passage.

- `p` old → new:

  - Old: Table 2 also compares the bounds at one common message size: 2 30 bytes, labeled “1 GB” in the table. That answers a different question: how small is the allowed collision probability for two files of that size? The column shows −log₂ of the probability bound, without dividing by length. UMASH’s entries remain claims. A collision found for two short messages tells us nothing about messages of that size, so I do not extrapolate the witness caps.
  - New: Table 2 also compares the bounds at one common message size: 2 30 bytes, labeled “1 GB” in the table. That answers a different question: how small is the allowed collision probability for two files of that size? The column shows −log₂ of the probability bound, without dividing by length. UMASH-64’s entry uses the corrected certified bound; UMASH-128’s entry distinguishes its published claim from the inherited linear envelope. A collision found for two short messages tells us nothing about messages of that size, so I do not extrapolate the witness caps.

- `p` old → new:

  - Old: In Table 2 , I use the proof audit and its second reading , including the corrections they establish. The status column records which arguments are complete and which published steps remain unresolved. An unresolved proof step alone gives no collision attack. No collision violating the claimed rate is known for production UMASH or the shipped HalftimeHash wrappers. UMASH’s claimed numbers have hollow markers, with weaker fallback certificates recording what the audit proves. The corrected HalftimeHash wrapper bound and the refuted advanced API are described below.
  - New: In Table 2 , I use the proof audit and its second reading , including the corrections they establish. The status column records which arguments are complete and which published steps remain unresolved. An unresolved proof step alone gives no collision attack. No collision violating the claimed rate is known for production UMASH or the shipped HalftimeHash wrappers. UMASH’s claimed numbers keep hollow markers; the certified UMASH-64 fallback is now 46.52 bits, and UMASH-128 inherits the corrected linear envelope. The corrected HalftimeHash wrapper bound and the refuted advanced API are described below.

- `p` old → new:

  - Old: Claimed ε ≤ ceil(2^30/4096)·2^-55 = 2^18·2^-55 = 2^-37. / Claimed bound; proof gap remains. This does not substitute for the weaker proved all-pairs certificate.
  - New: Corrected ε ≤ A+(1−A)·2^23/(2^61−3), A=364816/(2^64−561), L=2^27; −log₂ ε = 37.99. Published: 37 bits (claimed). / The corrected reference-model bound is certified; the published 37-bit value at 1 GB remains a claim.

- `p` old → new:

  - Old: Claimed bound; proof gap remains. Only the weaker primary certificate is inherited for all pairs.
  - New: The 75-bit value is claimed. The inherited certified linear envelope gives ε ≤ 45635·2^18/2^61, or 27.52 bits at 1 GB; the quadratic 83-bit score remains a claim.

- `p` old → new:

  - Old: UMASH-128 needs a special length qualification. Its claimed bound is ⌈L/2²³⌉² · 2⁻⁸³ , giving a claimed score of at least min(83 + log₂L, 127 − log₂L) . The displayed 83-bit claim holds up to 2 46 eight-byte words, about 512 TiB. The claimed score falls to about 68 bits over the full 64-bit length range. The proof gap remains regardless of which length is chosen.
  - New: UMASH-128 needs a special length qualification. Its claimed bound is ⌈L/2²³⌉² · 2⁻⁸³ , giving a claimed score of at least min(83 + log₂L, 127 − log₂L) . The displayed 83-bit claim holds up to 2 46 eight-byte words, about 512 TiB. The claimed score falls to about 68 bits over the full 64-bit length range. The quadratic claim remains neither proved nor refuted regardless of length; the corrected proof supplies only the inherited linear envelope ε ≤ 45635·⌈L/512⌉/2 61 .

- `p` old → new:

  - Old: The published proof for UMASH still has a step that the audit could not justify at the claimed strength. Weaker guarantees and several parts of the argument have been established. This limits what the proof certifies; it has not yielded a production collision that violates the claimed rate.
  - New: The published 55-bit and 83-bit claims for UMASH are neither proved nor refuted. An unconditional corrected bound now certifies a 46.52-bit score for UMASH-64; UMASH-128 inherits only its linear envelope. The ENH-only case ( OpenENHOnly ) is closed with joint collision probability <2 −105 ; the sharp primary projection constant and the PH+ENH case ( OpenPHENH ) remain open. The natural-language proof passed three independent adversarial reviews and exhaustive scaled checks; the verified judgement records the scope. No production collision violating the claimed rate is known.

- `p` old → new:

  - Old: [removed attribution clause] I found the gap in UMASH’s proof, the gap and refutation in HalftimeHash’s, and the collisions in the heuristic hashes. I used independent execution and proof reviews to check the witnesses and separate failed arguments from corrected guarantees. / [removed attribution clause] turn the mathematical arguments into proofs that Lean can check. Completed checks cover the original ChainHash construction and its 80-byte key setup , ChainHash-128 and its 160-byte setup , Polymur’s numerical bound with the required random key , the mathematical CLHASH algorithm , Poly1305 and single-message GHASH , and the counted HighwayHash collision event . Supporting results cover polynomial hashing and other basic constructions , BRW and the eight-lane recurrence , and multiply-shift . / Some proof-checking work remains incomplete: connecting the HalftimeHash executable to the mathematical argument , combining all of UMASH’s probability cases , and adapting the original ChainHash proof to v2. The repaired HalftimeHash24 has a checked construction, but no complete C++ correctness proof. Throughout the post, a checked mathematical proof is distinct from verification of source code, compiler behavior or a deterministic key generator.
  - New: I found the gap in UMASH’s proof, the gap and refutation in HalftimeHash’s, and the collisions in the heuristic hashes. I used independent execution and proof reviews to check the witnesses and separate failed arguments from corrected guarantees. / Several mathematical arguments have also been turned into proofs that Lean can check. Completed checks cover the original ChainHash construction and its 80-byte key setup , ChainHash-128 and its 160-byte setup , Polymur’s numerical bound with the required random key , the mathematical CLHASH algorithm , Poly1305 and single-message GHASH , and the counted HighwayHash collision event . Supporting results cover polynomial hashing and other basic constructions , BRW and the eight-lane recurrence , and multiply-shift . / Some proof-checking work remains incomplete: connecting the HalftimeHash executable to the mathematical argument , formalizing UMASH’s corrected probability bound , and adapting the original ChainHash proof to v2. The repaired HalftimeHash24 has a checked construction, but no complete C++ correctness proof. Throughout the post, a checked mathematical proof is distinct from verification of source code, compiler behavior or a deterministic key generator.

- `p` old → new:

  - Old: The audit found a gap in the published probability argument, rather than a production collision exceeding its claim. The details distinguish the unresolved guarantee from weaker statements that can be proved.
  - New: An unconditional corrected bound now certifies 46.52 bits for UMASH-64. The ENH-only case is closed, while the sharp primary constant and PH+ENH case remain open; the published 55/83-bit claims are neither proved nor refuted.

- `p` old → new:

  - Old: What is proved. The raw OH bound is at most 2/2 64 per block pair, or 1/2 64 for equal chunk counts. The first attempt closed several length cases, including different block and chunk counts and short/long comparisons, and counted an exact set of 852 XOR masks: a pair differing in a PH chunk has reduced primary block collision probability at most 852²/2 64 , about 44 bits. This was the strongest bound I could complete for that subcase. It does not establish the whitepaper’s claimed bound for all pairs. The initial attempt’s verdict was OPEN , with general ENH and joint fingerprint cases unresolved and no production counterexample. Its scaled exact model measured about a factor-four projection loss (about 2 bits, or about 3 against the tighter equal-chunk-count bound) for the tested families, below the roughly 6 bits assumed in the whitepaper; I have no proof that these are global maxima. / A weaker all-pairs primary certificate. The later ENH result proves a reduced primary constant c = 718333281557 for ENH-only differences, including blocks with no PH chunk. With q=2 64 , p=2 61 −1, D=1−561/q and A=c/(qD), the complete primary bound is 1/(qD) at L=1 and A+(1−A)min(1,2⌈L/32⌉/(p−2)) at L≥2. Its minimum score is 25.6141375967 bits at L=2: proved here, weaker than claimed, and not an observed attack. The all-pairs sharp 55-bit bound and the 83-bit fingerprint claim are still not fully certified; the chart keeps hollow markers at 55 and 83. / Fingerprint progress. A third review proved joint reduced collision at most 345763417/2 116 < 2 −87 for equal chunk counts, equal checksums and at least two differing PH chunks. It uses a rank lemma for shifted carry-less products (raw joint bound 2 −(128−h) , replacing 2 −99 ), the exact 852-mask set, the shuffler constraint on joint targets and an independent checksum product. The independent recount confirmed the exact constant and the primary bounds <162/q for different chunk counts and ≤17/q for an odd component difference; it sharpened the single-changed-PH-word bound from <65/q to 8/q+128/q² <9/q. Sharp all-even primary PH bounds, general joint ENH and unequal-length coefficient-sequence cases were still open at this stage. / The fifth review , independently confirmed , closed one-word ENH fingerprint changes (ENH-only <2 −103 ; one PH chunk plus ENH with equal checksums <2 −91 ), tag-only changes (<2 −92 ) and parts of two-word ENH changes, and proved a primary one-word ENH bound ≤32042/q ≈2 −49 . Sharp joint two-word ENH cases remain open for 32≤r≤63 (ENH-only) and r∈{1,2,3}∪[36,63] (PH+ENH), as does the sharp primary marginal bound; the weaker all-pairs primary completion above does not prove the claimed constants.
  - New: What is proved. The raw OH bound is at most 2/2 64 per block pair, or 1/2 64 for equal chunk counts. The corrected projected primary block constant is 604² = 364816, giving at most 604²/2 64 under IID OH words: a parity and top-bit refinement leaves at most 604 of the exact 852 XOR masks in each lane. This is the maximum over a complete block case split; the ENH-only primary bound is 5542/2 64 . The initial attempt is superseded by the unconditional corrected proof . The scaled models’ factor-four projection loss (about 2 bits) remains a heuristic, not a global bound. / The corrected all-pairs primary certificate. For L measured in 64-bit words, put A = 364816/(2 64 −561). The certified collision envelope is ε(1) = 1/(2 64 −561) and ε(L ≥ 2) = A + (1−A)·min(1, 2⌈L/32⌉/(2 61 −3)); equal-length distinct short inputs never collide. Its linear envelope is ε ≤ 45635·⌈L/512⌉/2 61 . The minimum collision score is 46.52 bits , attained at L=2 under both fixed-length and at-most-length conventions. This supersedes the previous 25.6-bit certificate, a gain of 20.9 bits. The natural-language proof is unconditional within its stated key model and passed three independent adversarial reviews and exhaustive scaled checks; the verified judgement confirms the constants. This proof has not yet been formalized in Lean. / Bounds at representative lengths. The corrected UMASH-64 values of −log₂ ε are 45.52 bits at 1 KB , 45.28 bits at 1 MB , and 37.99 bits at 1 GB , using 2 10 , 2 20 , and 2 30 bytes respectively. They measure the collision-probability bound without the score’s length adjustment. The corrected bound is 9.5 bits below the published bound at 1 KB and 1.0 bit above its 37 bits at 1 GB. The chart keeps the claimed 55/83 hollow markers; the certified fallback is reported in the text and table. / Fingerprint progress. A third review proved joint reduced collision at most 345763417/2 116 < 2 −87 for equal chunk counts, equal checksums and at least two differing PH chunks. It uses a rank lemma for shifted carry-less products (raw joint bound 2 −(128−h) , replacing 2 −99 ), the exact 852-mask set, the shuffler constraint on joint targets and an independent checksum product. The independent recount confirmed the exact constant and the primary bounds <162/q for different chunk counts and ≤17/q for an odd component difference; it sharpened the single-changed-PH-word bound from <65/q to 8/q+128/q² <9/q. The corrected proof now completes the primary all-pairs bound, including unequal lengths, and closes ENH-only joint cases; the sharp primary constant and PH+ENH joint case remain open. / The fifth review , independently confirmed , closed one-word ENH fingerprint changes (ENH-only <2 −103 ; one PH chunk plus ENH with equal checksums <2 −91 ), tag-only changes (<2 −92 ) and parts of two-word ENH changes, and proved a primary one-word ENH bound ≤32042/q ≈2 −49 . The corrected proof now closes OpenENHOnly , including two-word ENH-only changes at every valuation: the joint bound is 4721784/2 128 under IID OH words and remains <2 −105 after conditioning on distinct words. The sharp primary projection constant (162 versus the proved 364816) and OpenPHENH , with equal checksums and r∈{1,2,3}∪[36,63], remain open. Neither published headline is proved or refuted.

- `p` old → new:

  - Old: Repair routes and scope. A repair needs direct reduced single/joint coefficient bounds, including zero-polynomial events across lengths, or a compressor/reduction with the matching additive-differential theorem; a bound for the raw compressor alone is insufficient. Distinct OH words require the conditioning factor 1/(1−561/q), and the reference fingerprint needs two multipliers. UMASH-128’s claimed bound is ⌈L/2 23 ⌉²·2 −83 , i.e. score(L) ≥ min(83 + log₂L, 127 − log₂L); 83 for inputs up to 2 46 words, about 68 over the full 64-bit length range. The plotted 83 remains uncertified. / Disclosure. I put the question to the author in UMASH issue #40 on 2026-09-18. No production collision violating the claimed rate is known. The overall status is “proof gap: claimed bound not yet fully certified”; I will update the scores if that discussion changes them.
  - New: Scope and remaining claims. The corrected proof covers umash_reference.py and the reference/Lean model, for a fixed seed, 34 OH words sampled uniformly without replacement, and an independent multiplier uniform on {2,…,2 61 −2}; IID OH words are also covered. Production umash.c was not re-audited in this pass. UMASH-128 inherits only the linear envelope ε ≤ 45635·⌈L/512⌉/2 61 , since a fingerprint collision implies a primary collision. Its published quadratic bound ⌈L/2 23 ⌉²·2 −83 remains a claim, with score(L) ≥ min(83 + log₂L, 127 − log₂L): 83 for inputs up to 2 46 words and about 68 over the full 64-bit length range. The sharp claims still need direct reduced single/joint coefficient bounds; a raw compressor bound alone is insufficient, and the common-mask ENH argument does not cover differing PH offsets in OpenPHENH . / Disclosure. I put the question to the author in UMASH issue #40 on 2026-09-18. No production collision violating the claimed rate is known. The corrected bound is proved in the stated reference model; the published 55/83-bit claims remain neither proved nor refuted, and their chart markers remain hollow.

- `p` old → new:

  - Old: I checked every selected witness in a separate verification program. Some programs reimplement the algorithm; others embed upstream code, including komihash, HighwayHash and XXH3. Several headline mechanisms were also checked by a second model family at limited sampling scale. Its samples did not certify SpookyHash’s exact-half claim or HighwayHash’s exact numerator. / I spent about one day directing [removed attribution] through the implementations, followed by further searches, independent checks and proof work. They traced where message words meet keys, proposed pairs and measured full-output collisions. The record does not count every rejected mechanism or provide a full compute ledger. I report the scope of each standalone program, exact count, prose proof and Lean status file. / The one-day account covers the investigation I directed. Large parallel searches and later verification runs followed; the records do not provide a complete hardware-cost ledger. I do not treat model agreement alone as evidence of historical novelty or correctness. Appendix B separates deterministic smoke counts, fresh confirmation streams, exhaustive enumerations and analytical event counts.
  - New: I checked every selected witness in a separate verification program. Some programs reimplement the algorithm; others embed upstream code, including komihash, HighwayHash and XXH3. Several headline mechanisms also received independent checks at limited sampling scale. Those samples did not certify SpookyHash’s exact-half claim or HighwayHash’s exact numerator. / The initial investigation took about one day, followed by further searches, independent checks and proof work. It traced where message words meet keys, proposed pairs and measured full-output collisions. The record does not count every rejected mechanism or provide a full compute ledger. I report the scope of each standalone program, exact count, prose proof and Lean status file. / The one-day account covers the investigation I directed. Large parallel searches and later verification runs followed; the records do not provide a complete hardware-cost ledger. I do not treat agreement between reviews alone as evidence of historical novelty or correctness. Appendix B separates deterministic smoke counts, fresh confirmation streams, exhaustive enumerations and analytical event counts.

- `td` old → new:

  - Old: 55 (claimed)
  - New: 55 (claimed) Certified fallback: ≥ 46.52 bits

- `td` old → new:

  - Old: 83 (claimed)
  - New: 83 (claimed) Certified linear fallback: ≥ 45.52 bits

- `td` old → new:

  - Old: UMASH-64 uniform full key; claimed bound not yet fully certified rurban: umash
  - New: UMASH-64 ideal random key; corrected bound proved, sharp claim open rurban: umash

- `td` old → new:

  - Old: Claimed: ceil(L/512)·2^-55 → 55. Proved for all pairs: ≥ 25.6141 bits (proved here, weaker than claimed). Proved for a differing PH chunk: ≤ 852²/2 64 per block (about 44 bits). / 55 (claimed) u / 37 bits (claimed) Claimed ε ≤ ceil(2^30/4096)·2^-55 = 2^18·2^-55 = 2^-37. Claimed bound; proof gap remains. This does not substitute for the weaker proved all-pairs certificate.
  - New: Claimed: ceil(L/512)·2^-55 → 55. Proved for all pairs: ≥ 46.52 bits (fixed-length and at-most; corrected proof). Proved for a differing PH chunk: ≤ 604²/2 64 per primary block under IID OH words. / 55 (claimed) u Certified fallback: ≥ 46.52 bits / 37.99 bits (certified) Corrected ε ≤ A+(1−A)·2^23/(2^61−3), A=364816/(2^64−561), L=2^27; −log₂ ε = 37.99. Published: 37 bits (claimed). The corrected reference-model bound is certified; the published 37-bit value at 1 GB remains a claim.

- `td` old → new:

  - Old: Weaker full-output certificate only. The new 25.6141-bit envelope does not certify the claimed 55-bit rate; masking needs its own bound or the generic projection loss. / UMASH-128 fingerprint uniform full key; claimed bound not yet fully certified rurban: umash
  - New: Weaker full-output certificate only. The corrected 46.52-bit full-output certificate does not establish the claimed 55-bit rate; masking needs its own bound or the generic projection loss. / UMASH-128 fingerprint ideal random key; corrected bound proved, sharp claim open rurban: umash

- `td` old → new:

  - Old: Claimed: ceil(L/2^23)^2·2^-83 → 83. Proved for all pairs: no complete bound; only the trivial certificate. Proved for a differing PH chunk: ≤ 852²/2 64 per block (about 44 bits). u / 83 (claimed) u / 75 bits (claimed) Claimed ε ≤ ceil(2^30/2^26)^2·2^-83 = 16^2·2^-83 = 2^-75. Claimed bound; proof gap remains. Only the weaker primary certificate is inherited for all pairs.
  - New: Claimed: ceil(L/2^23)^2·2^-83 → 83. Proved for all pairs: ε ≤ 45635·⌈L/512⌉/2 61 (inherited linear envelope). Proved for a differing PH chunk: ≤ 604²/2 64 per primary block under IID OH words. u / 83 (claimed) u Certified linear fallback: ≥ 45.52 bits / 75 bits (claimed) Claimed ε ≤ ceil(2^30/2^26)^2·2^-83 = 16^2·2^-83 = 2^-75. The 75-bit value is claimed. The inherited certified linear envelope gives ε ≤ 45635·2^18/2^61, or 27.52 bits at 1 GB; the quadratic 83-bit score remains a claim.

- `td` old → new:

  - Old: Unresolved. Proved so far: ≥ 25.6 bits, inherited from the primary hash. The claimed 83-bit bound remains incomplete; no collision above the claimed bound is known. Masking and truncation require their own bounds.
  - New: Unresolved. Proved: the inherited linear envelope ε ≤ 45635·⌈L/512⌉/2 61 . The primary hash’s tighter 46.52-bit score does not certify the quadratic 83-bit claim. Masking and truncation require their own bounds.

- `h3` old → new:

  - Old: UMASH-64/128 (proof gap) pattern: ungrouped
  - New: UMASH-64/128 (corrected bound; sharp claims open) pattern: ungrouped

## Final browser checks

Playwright passed at 375, 768, 1200 and 1620 pixels: no console or page errors, no failed local assets or HTTP errors, and no horizontal page overflow. Verified the two claimed UMASH profiles and expanded notes, a proved CLHASH profile, a measured komihash profile, hover, keyboard selection and dismissal, animated host transitions (intermediate positions differ from both endpoints), TOC navigation, proof/verdict links, and the static image with JavaScript disabled. Screenshots were visually inspected across all four widths, including the narrow formula layout. Existing external analytics requests were cancelled on navigation; these were not page asset failures or console errors.

The only 25.6-bit mentions in current UMASH prose explicitly describe the superseded certificate. No 852² block bound remains. The exact 852-mask count is retained where mathematically relevant. The corrected records match the supplied originals byte for byte.
