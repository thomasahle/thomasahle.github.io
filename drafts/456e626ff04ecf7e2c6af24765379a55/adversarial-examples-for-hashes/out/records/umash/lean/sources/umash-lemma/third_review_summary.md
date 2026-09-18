# Third UMASH review (author-run, 2026-09-18) — the reduction bound for the hard fingerprinting subcase, condensed

Setting: q = 2^64, p = 2^61−1, R(z) = (z_lo mod p, z_hi mod p).  Independent uniform OH key words.

THEOREM (subcase (b)): two blocks with the same chunk count n ≤ 16, equal XOR checksums, at least two differing
PH-mixed chunks (ENH chunks and tags may differ):  Pr[R(OH(x)) = R(OH(y)) and R(OH'(x)) = R(OH'(y))] ≤ 345763417/2^116
< 2^-87 (≈ 2^-87.635); also holds under the reference generator's rejection of repeated words (Pr[A] ≥ 1 − 561/q > 2/3
and every table ratio < 2/3).

Ingredients:
1. Shufflers s_i = T + T^{n−i} (i < n−1), s_{n−1} = T, with T the lane-wise 1-bit left shift; s_i + s_j = T^{n−j}(I + T^{j−i})
   or T^{n−i}; kernel = ker T^h for some 1 ≤ h ≤ 15.
2. Rank lemma: for nonzero w-bit polynomial d, z ↦ d·z (carry-less) restricted to the low w−h bits of each lane has
   rank ≥ w − h (proof by valuation v of d and triangularity of multiplication by the odd part g).  Hence
   max_z Pr[T^h Δ = z] ≤ 2^-(64−h) for a PH difference Δ = d_a⊙K_b ⊕ d_b⊙K_a ⊕ c, and for two differing PH chunks i<j
   (conditioning on the other chunk keys):  Pr[A = A0, B = B0] ≤ 2^-(128−h)  (raw joint bound; worst 2^-113, vs the
   paper's 2^-99).
3. D = {u ⊕ v : u ≡ v mod p} has exactly 852 elements (carry recurrence over v = u + jp, 1 ≤ j ≤ 8, plus 0); a
   reduced collision needs both lanes of the compressor's XOR difference in D.
4. Shuffler constraint: B ⊕ T(A) has prescribed low h bits per lane, so the joint targets number at most N_h² with
   N_h = max_c #{(u,v) ∈ D² : (v ⊕ (u≪1)) mod 2^h = c}.
5. "Long" differences (bits 3..h−1 all one, h ≥ 5) force bits 3..h−2 of the underlying word to be all 0 or all 1
   (from 2(u & d) = d + kp, |k| ≤ 8, p ≡ −1 mod 2^h); the equal checksums make the secondary's checksum-PH term
   Z = X ⊙ Y with X, Y independent uniform (twisting keys translated by the common checksum) → conditional bounds
   max_z Pr[Z_lo mod 2^k = z] ≤ (k+2)2^-(k+1), max_z Pr[Z_hi mod 2^k = z] ≤ 2^-k + k/q → ρ_h = min{1, (h+1)2^(4−h)}
   conditional on all original chunk keys.
6. Combination: Pr ≤ 2^-(128−h) · min{N_h², S_h² + ρ_h N_h²} with S_h counting non-long secondary differences.
   Table (h: N_h, S_h, bound/2^-87): 1: 514608, 514608, 0.2409; 2: 278664, 278664, 0.1413; 3: 153492, 153492, 0.0857;
   4: 104060, 104060, 0.0788; 5: 99768, 2966, 0.1448; 6: 96425, 2916, 0.2706; 7: 93140, 2866, 0.5050;
   8: 89913, 2816, 0.5303; 9: 86744, 2766, 0.5493; 10: 83633, 2716, 0.5632; 11: 80580, 2666, 0.5735;
   12: 77585, 2616, 0.5821; 13: 74648, 2566, 0.5921; 14: 71769, 2516, 0.6093; 15: 68948, 2466, 0.6440.
   Worst h = 15: (2466² + 68948²/128)·2^-113 = 345763417/2^116.

ADDITIONAL CASES PROVED:
- Different chunk counts: primary reduced collision ≤ 81·(2q−1)/q² < 162/q (fibre count is valid here because after
  conditioning the shorter output is fixed and the longer's final ENH pair is fresh); joint < 117596449/q² < 2^-101.
- Different checksums with ≥ 1 differing PH chunk: primary ≤ 852²/q; joint ≤ 852⁴/q² < 2^-89.
- A differing PH chunk with an ODD XOR difference in at least one component: primary ≤ 17/q (bit-by-bit borrow
  argument: each k ∈ [−8, 8] determines the free key word uniquely).  Other chunks and tags may differ.
- Exactly one word of one PH chunk changes, everything else identical: primary < 65/q (including even differences).

REMAINING (per the review): sharp primary bound for equal-chunk-count pairs whose PH differences are all even or whose
changes are confined to ENH; joint matching-checksum case with exactly one differing PH chunk plus a differing ENH
chunk; not all ENH-only and tag-only cases; the end-to-end unequal-length coefficient-sequence and short/long
comparisons.  Verification program and certificate exist in the reviewer's sandbox (not yet in our records).
