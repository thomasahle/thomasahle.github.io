# Fifth UMASH review (author-run, 2026-09-18): ENH-dominated and tag-only cases — condensed
Setting: q = 2^64, p = 2^61−1, R lane-wise reduction; ENH as implemented: P = AB + qτ (mod q²), E = F(P) with
F(L,H) = (L, H⊕L); tags τ = seed ⊕ (block_size mod 256), so |τ' − τ| ≤ 255 (uses the implementation's tags).
Facts about D (852 masks): #{d ∈ D : 2^r | d} = 852, 248, 64, 2, 1 for r = 0,1,2,3,≥4 — so congruent words with XOR
difference divisible by 16 are equal; every nonzero d ∈ D has top bit ≥ 60.
Lemma 1 (one-word NH change A' = A + δ, r = v2(δ)): (AB mod q) ⊕ (A'B mod q) is UNIFORM on the multiples of 2^r
(probability 2^r/q each).  Not extended to two-word changes.
Lemma 2 (high XOR given low equality): Pr[E_lo = E'_lo, E_hi ⊕ E'_hi = e] ≤ κ(e)/q with
κ(e) = min{2^{h(e)}, 5 + 2^{71−h(e)}, 5 + ceil(4q/(g(e) − 255)) if g(e) > 255}, h = popcount, g(e) = 2^{bitlength(e)} − e.
Common checksum mask (equal checksums): Z = U ⊙ V; pattern weight W(v) = min{1, 16 f_H(v)}; Σ_{v∈D} W(v) < 51.
RESULTS (joint reduced-compressor collision, both compressors):
- ENH-only, one input word changes: r ≤ 3: ≤ (N_r 2^r/q)² < 2^-108 (N_r 2^r = 852, 496, 256, 16); r ≥ 4: primary needs raw
  low equality + high XOR ∈ D → Σ_{e∈D} κ(e) = 32042 → joint ≤ 852·32042/q² = 27299784/2^128 < 2^-103.
- One PH chunk + ENH, equal checksums, one-word change: r ≤ 3 < 2^-108; r ≥ 4: Φ_S = Σ_{u,v∈D, x=(I+S)^-1(u⊕v)<2^63}
  κ(u⊕x) W(v); max over the 15 shufflers ⌈Φ_S⌉ = 111924178297 < 2^37 → < 2^-91.
- Tag-only (all expanded chunks agree): primary needs H within 255 of a boundary mod 2^60 (≤ 16·255 values) and
  Pr[⌊AB/q⌋ = h] < 48/q → ≤ 195840/q; secondary conditional ≤ 61·2^-52 → joint ≤ 11946240·2^-116 < 2^-92.
- Two-word ENH changes (r = min(v2 δ, v2 ε)): raw low-word equality has probability exactly 2^r/q; ENH-only r ≥ 4:
  ≤ 852·2^{r−128}; PH+ENH equal checksums r ≥ 4: < 51·2^{r−128}; r = 0 closed (odd lifting); ENH-only r = 1,2,3:
  ≤ 113280 N_r 2^{2r−128} < 2^-101.  Bounded below 2^-87: ENH-only 0 ≤ r ≤ 31; PH+ENH r = 0 or 4 ≤ r ≤ 35.
  NOT closed: ENH-only 32 ≤ r ≤ 63; PH+ENH r ∈ {1,2,3} or 36 ≤ r ≤ 63.  These are not counterexamples.
- Primary-hash consequence (from the same lemmas): ENH-only one-word change: primary reduced collision ≤ 852/q (r ≤ 3)
  and ≤ 32042/q ≈ 2^-49 (r ≥ 4).  The sharp primary marginal bound for all pairs remains open.
- Distinct-word key generator: factor 1/(1 − 561/q); all rounded bounds have slack.  Certificate in the reviewer's
  sandbox (integer arithmetic for D, Σκ, ΣW, the 15 Φ_S sums); computer-assisted, not proof-assistant formalised.
