STATUS: PARTIAL

Task A is checked for full-output unsigned integer NH. Its normalized score is
**64 with one-word zero padding admitted**, and **65 with unpadded pairs only**.
Task B has new executable agreement checks and a checked NH primitive bridge,
but does not have a hypothesis-free theorem about the shipped binary or the
exact fixed header. The reasons include an actual upstream C++ bounds defect
and the nonuniform distribution of deterministically expanded keys.

## Remaining obligations

The exact, elaborated Lean propositions below are definitions (not axioms or
proved theorems) in `lean/ProvenHashes/Halftime/RefinementObligations.lean`.

1. `ProvenHashes.Halftime.StyleRefinement`. For each shipped lane count, there
   must exist a public `StyleScheduleFamily` for which the executable byte,
   lane, forest, key-address and tabulation computation equals `byteStyleHash`
   on every supported message and key. Random/boundary vectors agree, but no
   universal equation to the noncomputable probability model has been proved.
   A theorem about standard C++ source additionally needs the upstream invalid
   table view repaired or a specified compiler/machine semantics; execution
   alone cannot erase its undefined behavior.
2. `ProvenHashes.Halftime.Encode3Distance`. The concrete executable encoder
   must have distance three on all `Fin 21 → UInt64` inputs, with no encoder
   hypothesis. The exact-header/Lean generator rows agree and an external
   exhaustive GF(2) rank certificate succeeds. That certificate has **not**
   been imported as a kernel-checked distance theorem.
3. `ProvenHashes.Halftime.FixedHeaderBound`. In full:
   ```lean
   ∀ (b : ℕ), b ∈ [1,2,4,8] → ∀ (x y : FixedMessage b), x ≠ y →
     ∀ target : Fin 3 → Word64,
       uniformProb (fun k => fixedExec x k - fixedExec y k = target) ≤
         6804 / (2 : ℚ≥0) ^ 96
   ```
   `FixedMessage b` is the exact byte domain below `168*b*19173961`;
   `fixedExec` executes this lane's scalarized header with a uniform
   `Fin (217+215*b) → Word64` key. M4's scalar theorem still has an encoder
   distance premise; M5 proves the two-output Style model, not this all-length
   three-output fixed model. The inherited `terminal_length_three_bound`
   retains its residual-key noninterference premise. The concrete broadcast
   lane, flat-key and terminal-length discharge is still missing. Consequently
   the fixed row may say **vector agreement checked**, but cannot yet say
   **exact header theorem ✓ checked** under the requested no-hypotheses rule.
4. `ProvenHashes.Halftime.SeededStyleBound`. This is the requested normalized
   Style collision inequality with the **64-bit seed** of the supplied fixed
   SMHasher adapter uniform, using `Exec.seedExpand`, instead of a uniform
   flat key array. The exact proposition is:
   ```lean
   ∀ (b : ℕ), b ∈ [1,2,4,8] → ∀ (L : ℕ), 1 ≤ L →
     ∀ (x y : StyleMessage b), x ≠ y → x.1.val ≤ 8*L → y.1.val ≤ 8*L →
       uniformProb (fun seed : Word64 =>
         Exec.style b (Exec.seedExpand (UInt64.ofNat seed.val)) (messageArray x.2) =
         Exec.style b (Exec.seedExpand (UInt64.ofNat seed.val)) (messageArray y.2)) ≤
           (L : ℚ≥0) * (1 / 2^63 - 1 / 2^128)
   ```
   Functional expansion agrees in tests, but uniform-key transfer is
   mathematically impossible: `no_uniform_seed_expansion_two_words` proves
   that no 64-bit-seed function even produces two independent uniform words.
   This does **not** prove `SeededStyleBound` false; it shows why M5 does not
   prove it. The upstream header itself accepts a caller-provided key array
   and has no key expander; this obligation applies when using the adapter.

All these constructs are formalizable. None is excused as inherently
unformalizable. The distinction is between an open theorem, a finite execution
check, and a false distribution-preservation claim.

## Milestones and signatures

### A — checked integer NH, including the padding rule

`NH64.lean` defines the full 128-bit output using M1's integer, unsigned
representatives **after** 64-bit modular addition. It does not use field NH:

```lean
def nh64 {n : ℕ} (x k : Fin n × Bool → ZMod (2 ^ 64)) : ZMod (2 ^ 128)

theorem nh64_adu {n : ℕ} (x y : Fin n × Bool → ZMod (2 ^ 64))
    (hxy : x ≠ y) (t : ZMod (2 ^ 128)) :
    uniformProb (fun k => nh64 x k - nh64 y k = t) ≤ 1 / (2 : ℚ≥0) ^ 64

theorem nh64_padded_adu {n : ℕ} (x y : Fin n → ZMod (2 ^ 64))
    (hxy : x ≠ y) (t : ZMod (2 ^ 128)) :
    uniformProb (fun k => nh64 (pad64 x) k - nh64 (pad64 y) k = t) ≤
      1 / (2 : ℚ≥0) ^ 64
```

`pad64` uses consecutive `(2*i,2*i+1)` positions, inserts exactly one zero
word for odd original lengths, and is injective at each fixed original word
length. Distinct original lengths are not compared, even if padding produces
the same length. `nh64_padded_collision` specializes the difference target to
zero. The cap theorem `nh64_padded_normalized` divides the exact collision
probability by any positive word cap and bounds it by `2^-64`. Its conclusion
is stronger than requiring `n ≤ L`, so no missing cap hypothesis is involved.

`nh64_witness_probability` proves that unpadded `(0,0)` and `(1,0)` collide
with probability exactly `2^-64`. `nh64_padded_witness_probability` proves the
same for one-word messages `0` and `1` padded with zero. Thus the score-64
bound is attained at one word. `nh64_unpadded_normalized` and
`nh64_unpadded_score_sharp` prove the normalized unpadded value `2^-65`,
attained at two words. There is no logarithm approximation in these rational
certificates.

### B — executable model and primitive bridge

`Executable.lean` is a separate total executable scalarization with unsigned
`UInt64`/`UInt32` operations, little-endian byte loads, literal T2/T3 matrices,
Encode2/Encode3, broadcast EHC and tree keys, the promotion-before-insertion
forest stack, root-major finalization, overlapping Toeplitz tails, fixed
right alignment and terminal scalar length, flat byte tables, and the
supplied adapter's deterministic seed expansion.

`ExecutableBridge.lean` proves:

```lean
theorem machine_mix_agrees_nh32 (x k : UInt64) :
  ((Exec.mix x k).toNat : ZMod (2 ^ 64)) =
    nh32 (fun p : Fin 1 × Bool => machineHalves x p.2)
      (fun p => machineHalves k p.2)
```

It connects wrapped 32-bit addition followed by integer multiplication to the
actual M1 primitive. It is not a proof that the complete executable equals
M5. `RefinementObligations.lean` makes that missing equation explicit.

## Construct-by-construct boundary

| Construct | Parent proof / extension and new evidence | Remaining limit |
|---|---|---|
| Byte parsing and padding | M5 proves little-endian packing and equal-length injectivity. Full-output vectors test empty, aligned, partial and unaligned data. | Universal executable-to-M5 parsing equation remains open. |
| SIMD lane assembly and horizontal sum | M5 has broadcast keys and root/lane flattening; new `machine_mix_agrees_nh32` checks the NH primitive. Fixed scalar, native SIMD, and executable Lean outputs agree. | No compiler/instruction semantics theorem, and no complete scalarization equation to the noncomputable model. |
| Forest scheduler and leaf order | Parent proves bijective-base-eight counts, capacity 19,173,960, ≤64 roots and height ≤7. Executable model implements the source's promotion and root order and tests promotion boundaries. | Default `modeledStyleHash` uses a chosen bijection; the new evaluator must still be connected to an admissible public schedule, not simply identified with that arbitrary default. |
| Encode3 | Not called by any Style64/128/256/512 wrapper (they use Encode2). Old upstream Encode3 has a one-packet output-difference witness, so distance three fails. Fixed Encode3 matches Lean's generator rows, with exhaustive external survivor ranks. | Kernel-checked concrete distance-three proof is still open. |
| Key expansion | Header has no expansion. Supplied fixed adapter's 36,000 expanded words agree with Lean (four seeds, 9,000 words each). General and concrete seed-cardinality impossibility theorems compiled. | Functional agreement does not preserve the ideal uniform-key probability experiment. A seeded bound needs separate analysis. |
| Flat table representation | Fixed header uses flat real arrays; upstream/fixed observed Style outputs agree. | Upstream UBSan finds `index 8 out of bounds` in a `[3][256]` view at line 1025. This blocks a standard-C++ source refinement for the shipped header despite observed agreement. |

## Corrections and scope

- `2^-65` is the **length-normalized unpadded value**, not the raw collision
  probability. The latter is sharply `2^-64`. If the intended row meant a raw
  `2^-65` collision bound, the checked witness refutes that interpretation.
- The supplied `/Users/ahle/repos/fast-polynomials/sections/appendix_adversarial.tex`
  contains no NH or HalftimeHash row in this checkout. A snapshot is in
  `evidence/appendix_adversarial.tex`. We used the explicitly permitted full
  128-bit UMAC NH output. No unspecified 64-bit reduction is certified.
  This agrees with the equal-length `2^-w` bound in the original UMAC paper:
  [UMAC proceedings paper](https://www.cs.ucdavis.edu/~rogaway/umac/umac_proc.pdf) (Theorem 4.2).
- `/Users/ahle/repos/smhasher3/hashes/halftime_hash.cpp` is absent. The permitted
  upstream-header fallback was used: `base-neon.hpp`, checked byte-for-byte
  against `8b03edf:halftime-hash.hpp` from the supplied fork. Its scalarized
  address-level behavior is tested, not assumed to be defined standard C++.
- The fixed header's 83.2678-bit bound is the conservative `6804/2^96`
  *collision bound*, not a 64-bit API or its normalized score. Its output is
  24 bytes. The retained stack supports height ≤7; no height-16 execution is
  claimed. The fixed header uses the ideal array-key contract from THEOREM.md.
- Finite vector agreement is reported as finite evidence. It is not a
  universal refinement proof, and external rank checking is not presented
  as a Lean theorem.

## Final execution evidence

The final run passed **1,216 cases** over lane counts 1, 2, 4 and 8: **4,864
output words** agree between the fixed C++ header and the executable Lean
model. Fixed scalar, native SIMD and Clang UBSan outputs agree word-for-word;
1,216 upstream Style outputs also agree under the observed modular execution.
There are 800 deterministic pseudo-random length cases plus 416 short and
boundary cases, with distinct reproducible fixture seeds. These are functional
regression tests, not collision-rate measurements or uniform-key experiments.

Boundaries include group counts 8/9, 64/65, 72/73, 512/513, 584/585,
4096/4097 and 4680/4681, each at one byte below, exactly at, and one byte above
both relevant group sizes. The largest message is 6,291,265 bytes. Inputs use
all alignment offsets modulo eight. No test claim extends to every supported
length or every possible key/message.

The exact-header/Lean encoder generator comparison passed; all 128 Encode2
and 512 Encode3 survivor subsets were rank-checked externally. Four seed
vectors compare every one of 9,000 expanded words, totaling 36,000 words.
The expected upstream UBSan failure and one-packet Encode3 witness reproduce
in the same final script.

`lake build` passed (7,390 jobs, cached Mathlib), and all **251 Halftime proof
declarations** plus **57 inherited base proof declarations** passed their
explicit axiom audits. The 251 include nine new NH64 results, two seed-space
results, one primitive bridge, and the two imported fixed-length results.
There are no remaining proof holes in reported theorem declarations.

## Reproduction, audits and provenance

All Lean and C++ compilation, probability proof checking, vector execution,
and rank checks ran on the Xeon with `nice -n 10 taskset -c 88-95` and
`LEAN_NUM_THREADS=8`. The Mac only edited, read, and transferred files.
Workspace `~/agents/lean-nh-halftime` was created with `cp -a` from
`~/agents/lean-halftime`, including its 6.3 GB `.lake` cache. Mathlib was not
rebuilt. The complete Lean source mirror is `./lean/`.

Run on the Xeon:

```bash
cd ~/agents/lean-nh-halftime
bash reproduce.sh
```

The script builds the proof library and executable, audits every proof,
compares C++/Lean vectors, regenerates the encoder rank certificate, checks
key expansion word-for-word, checks fixed-header UBSan execution, and
reproduces the upstream table-view and Encode3 defects.

Toolchain: Lean 4.24.0; Mathlib v4.24.0,
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
Parent lane: `c456cedba8c2a8f639f7ee614f8b01dbd49cb4f4`.
Parent M5 proof checkpoint: `0f9aabf3c3fcb08470e30a7f42f16270f028e128`.
Fixed-header fork: `e1b52cf9efd4016c25a144bc6e2e9e805fe632ab`.
Fixed header SHA-256:
`77d30baf4c6dc12e270236400e5323ccd4ef7a17e0aac877b3cbe4bc325ecbad`.
Upstream header SHA-256:
`09cbe0c51d40dda53ac513ec4199795afbecca21457074224ace35f166cb9ec9`.

Proof/test commit: `6b18383da5f034667a9dc1f0c96e7069109878e5`.
Final build/audit/vector log: [evidence/reproduction.log](evidence/reproduction.log),
mirrored from `~/agents/lean-nh-halftime/evidence/reproduction.log`.
Tool and source provenance: [evidence/PROVENANCE.txt](evidence/PROVENANCE.txt).
Per-theorem elaborated signatures and `#print axioms` results are in
`lean/HalftimeAudit.txt`, with inherited base proofs in `lean/FullAudit.txt`.
Only `propext`, `Classical.choice`, and `Quot.sound` are allowed. The scripts
reject placeholders, untrusted computation proof declarations, custom axioms,
and unsafe declarations in the reported sources.
