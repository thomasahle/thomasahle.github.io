STATUS: PARTIAL

Both requested collision-bound rows, BRW injectivity and degree, and the
corrected score statements are fully proved. The sole discrepancy is the
requested *exact* finite-range BRW score of 63, which is false. No collision
proof or algebraic lemma remains open.

## Remaining obligations

The literal requested equality is the following precise Lean proposition:

```lean
ProvenHashes.ChartScores.brwScore ProvenHashes.ChartScores.maxLength = (63 : ℝ)
```

It cannot be proved: `brw_minimum_bracket.1` proves its strict opposite,
`63 < brwScore maxLength`. The minimum is attained at `maxLength = 2^61-1`
(`brw_minimum`), and its floor is 63 (`brw_whole_bits`). This report remains
PARTIAL under the literal exact-score requirement; interpreting the chart's
63 as a conservative whole-bit score resolves this discrepancy. All other
requested statements are proved without additional assumptions.

## Corrections

For the published bound ε(L) = (2L-1)/2^64, the exact minimum is

```
log₂(2^64 (2^61-1) / (2^62-3))
  = 63 + log₂((2^62-2)/(2^62-3)) > 63.
```

Lean proves that it is strictly between 63 and 64, is attained at L=2^61-1,
and has integer floor 63. Thus the displayed integer BRW row is certified,
but the real-valued minimum is not exactly 63. The exact eight-lane minimum
is 61 at L=1. `score_formulas` ties both scores to L divided by their
published bounds; `probability_floor` checks the paper's 2^-64 probability
floor leaves those bounds unchanged for positive L.

Source locations: `sections/experiments.tex`, Injective-Lanes and BRW;
`sections/injective.tex`, score definition and chart. The cited
`appendix_adversarial.tex` discusses the experiments. Bernstein's relevant
recursion and injectivity proof are §5.2–5.5 of
https://cr.yp.to/antiforgery/pema-20071022.pdf, rather than §2/§3.

## Milestones

1. Copied `~/agents/lean-hash` to `~/agents/lean-brw` using `cp -a`, including
   its full `.lake` cache. No Mathlib rebuild. All compilation ran on the
   Xeon with `LEAN_NUM_THREADS=8`, `nice -n 10`, CPU set 40-47.
2. Proved Bernstein's four base cases and general power-of-two recursion.
   Proved degree ≤2L-1 and injectivity for all fixed lengths. The split
   decoder extracts the left block from high coefficients, uses the fixed
   right top coefficient to recover the middle word, then recovers the
   right block. These are proofs from the recursion, not injectivity
   hypotheses.
3. Applied the finite-field root count to get BRW collision ≤(2L-1)/2^64.
   The theorem holds for every fixed L, hence throughout 1≤L≤2^61-1.
4. Proved the eight-lane bound from the existing one-chain theorem and
   independent-key composition. The key space is `(Fin 3 → GF64) × GF64`.
   Every lane shares the same `(u,y,z)`; the fourth key w is independent.
   The output is `∑ j : Fin 8, Recurrence.hash (lane j) (u,y,z) * w^j`.
   To bound collision of the complete lane vector, the proof selects one
   differing lane. It does not assume the lane outputs are independent.
5. Proved injectivity of fixed-length padding and round-robin dealing.
   L field words become N=(L+1)/2 pairs, padding only a final odd word with
   zero. Pair i goes to lane i mod 8. Lane j has `(N+7-j)/8` pairs, so the
   last round can be incomplete. There are no extra zero pairs. The
   maximum lane length is `rounds L = ((L+1)/2+7)/8`, exactly
   ceil(ceil(L/2)/8). The collision bound is `(rounds L+7)/2^64`.
6. Proved the score minimizers, the exact eight-lane minimum, the strict
   BRW endpoint bracket, and its whole-bit score. Final `lake build` and
   the complete axiom audit passed.

## Signatures

Complete machine-generated signatures for every new theorem and lemma are
in `lean/BRWAudit.txt`. The main statements are:

```lean
ProvenHashes.BRW.polynomial_recursion
  (k n : ℕ) (hk : 2 ≤ k) (hlo : 2^k ≤ n) (hhi : n < 2^(k+1))
  (m : ℕ → F) :
  polynomial n m = polynomial (2^k-1) m * (X^(2^k) + C (m (2^k-1))) +
    polynomial (n-2^k) (fun i => m (2^k+i))

ProvenHashes.BRW.encode_injective (n : ℕ) :
  Function.Injective (encode (F := F) (n := n))

ProvenHashes.BRW.encode_degree {n : ℕ} (m : Fin n → F) :
  (encode m).natDegree ≤ 2*n-1

ProvenHashes.BRW.collision_bound_gf64 {L : ℕ}
  (m m' : Fin L → GaloisField 2 64) (hne : m ≠ m') :
  uniformProb (fun x : GaloisField 2 64 => hash m x = hash m' x) ≤
    ((2*L-1 : ℕ) : ℚ≥0) / 2^64

ProvenHashes.EightLanes.word_collision_bound_gf64 {L : ℕ}
  (m m' : Fin L → GaloisField 2 64) (hne : m ≠ m') :
  uniformProb (fun k => wordHash m k = wordHash m' k) ≤
    ((rounds L+7 : ℕ) : ℚ≥0) / 2^64

ProvenHashes.ChartScores.brw_minimum (L : ℕ)
  (hL : 1 ≤ L) (hmax : L ≤ maxLength) :
  brwScore maxLength ≤ brwScore L

ProvenHashes.ChartScores.brw_minimum_bracket :
  63 < brwScore maxLength ∧ brwScore maxLength < 64

ProvenHashes.ChartScores.brw_whole_bits :
  ⌊brwScore maxLength⌋ = (63 : ℤ)

ProvenHashes.ChartScores.lane_minimum :
  laneScore 1 = 61 ∧ ∀ L : ℕ, 1 ≤ L → 61 ≤ laneScore L
```

In the generic polynomial statements, `F` is an arbitrary field. Names
inside each signature resolve within the namespace on the first line;
`uniformProb` is `ProvenHashes.uniformProb`. The GF64 results carry no
unproved field, injectivity, degree, or independence hypotheses.

## Axioms and verification

- 25 new theorem/lemma declarations audited in `lean/BRWAudit.txt`.
- 82 total theorem/lemma declarations audited in `lean/FullAudit.txt`,
  including the parent results used by this lane.
- Every axiom list is a subset of `propext`, `Classical.choice`, `Quot.sound`.
- The reproduction script checks audit inventory coverage and rejects
  `sorry`, `admit`, `native_decide`, custom axioms, and unsafe declarations
  in the Lean source modules.
- Exact probabilities are nonnegative rational cardinality ratios over
  the full finite uniform key space.
- This formalizes field arithmetic and full field outputs, not machine
  instructions or a seed-expansion implementation.

## Commits and provenance

- Proofs, audit output, and reproduction scripts:
  `f5085e4959b4e23e65c8f29e09d09afe9d6d5ced` in `~/agents/lean-brw`.
- Parent lane: `e70213feed3dac6280fe8d7ecdc35dcfd0cf1cc8`.
- Mathlib v4.24.0: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Lean 4.24.0: `797c613eb9b6d4ec95db23e3e00af9ac6657f24b`.
- Paper repository HEAD inspected: `04c73a79a47f6107ed20ae1d4792e3e24ca6bc6f`.
- Bernstein PDF SHA256:
  `89b5f61c1cf3f654b2ce1c0a0cf89d6918451b399ea3b1797e82f37044d7fdef`.

## Reproduction and build logs

On the Xeon:

```sh
cd ~/agents/lean-brw/lean
./reproduce.sh
```

Final build/audit log:
`/home/thomas-ahle/agents/lean-brw/final-verification.log`.
Full build and audit log:
`/home/thomas-ahle/agents/lean-brw/lean/Reproduction.log`.
Local mirrors: `./final-verification.log`, `./lean/Reproduction.log`,
`./lean/BRWAudit.txt`, `./lean/FullAudit.txt`, and all Lean sources in
`./lean/`. Local reproduction helper: `./remote-build.sh`.
