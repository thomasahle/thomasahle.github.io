STATUS: COMPLETE

# ChainHash-128: ideal keys and model A

The requested collision bounds are proved for the **strided 512-byte fallback
design, W=32, S=1**, with no remaining field, encoding, stage-probability,
independence, or finalizer assumption. The final theorems cover distinct byte
strings of lengths below `2^128`, including empty messages and partial words.

## Remaining obligations

None for the selected design and requested theorem/evidence scope.

## Design selection and corrections

The inspected adjacent `REPORT.md` said measurements were in progress and
contained **no RECOMMENDED DEFAULT design**. Following the explicit selection
rule in the task, this lane therefore formalizes `chainhash-128/SPEC.md` and
its strided header. The adjacent SPEC's delivered default B does not override
that report-based selection rule. The inspected report is preserved in
[ADJACENT_REPORT.md](reference/design-selection/ADJACENT_REPORT.md), and the
selected specification and header are in [reference/](reference/).

No requested mathematical statement needed correction or weakening. The
final ideal theorem removes the inherited, unnecessary `len + 511` overflow
restriction: rounding uses natural numbers, and the only length-domain cap
is `length < 2^128`. The C header implements the representable subdomain
`length < 2^64` on ordinary 64-bit targets.

## Main signatures

The final exports are in [ChainHash128.lean](lean/ProvenHashes/ChainHash128.lean),
namespace `ProvenHashes.ChainHash128`, opening `ProvenHashes.ChainHash`.
`uniformProb` is the exact `ℚ≥0` ratio of event cardinality to key-space
cardinality. `Key41 = Fin 41 → Word 128`, with all 41 words independently
uniform, including zero; `hashBytes` returns the full `BitVec 128`.

```lean
theorem ideal_bound_bytes (n : ℕ) (m m' : List UInt8)
    (hm : m.length < 2^128) (hm' : m'.length < 2^128) (hne : m ≠ m')
    (hn : max 1 ((m.length+511)/512) ≤ n)
    (hn' : max 1 ((m'.length+511)/512) ≤ n) :
    uniformProb (fun k : Key41 => hashBytes k m = hashBytes k m') ≤
      min 1 (((n+2 : ℕ) : ℚ≥0) / 2^128)

theorem modelA_bound_bytes (L : ℕ) (hL : 0 < L) (hcap : 16*L < 2^128)
    (m m' : List UInt8) (hm : m.length ≤ 16*L) (hm' : m'.length ≤ 16*L)
    (hne : m ≠ m') :
    uniformProb (fun k : ModelA.Key =>
      hashBytes (ModelA.expandedKey k) m =
        hashBytes (ModelA.expandedKey k) m') ≤ ModelA.epsilonAtMost L
```

`ideal_bound` states the ideal result for the direct word-operation
`referenceHash`. `modelA_coarse_bound` proves the convenient all-length bound
`min 1 ((n+63)/2^128)` for arbitrary distinct messages and a common block-count
cap `n`. The fixed-length theorem in [ModelA.lean](lean/ProvenHashes/ModelA.lean)
is also proved:

```lean
theorem reference_collision_bound_fixed (L : ℕ) (hL : 0 < L)
    (m m' : Message) (hm : m.length = 16 * L) (hm' : m'.length = 16 * L)
    (hne : m ≠ m') :
    uniformProb (fun k : Key =>
      referenceHash (expandedKey k) m = referenceHash (expandedKey k) m') ≤
      epsilonFixed L
```

Model A samples ten independent uniform field words (`s,u,y,z,c0,…,c4,tau`,
160 bytes), and expands only the PH table as `k_i = s^(i+1)` for `i < 32`.
`expandedKey_power` proves that schedule in the actual 41-word key layout.
It is a separate theorem from the 656-byte ideal-key family.

## Exact envelopes and chart scores

Put `q = 2^128`. For positive `L` in **16-byte words**, let
`n = (L+31)/32`, `R = L - 32*(n-1)`, and `G = (R+3)/4`, using natural division.
The definitions and bounds match the selected SPEC exactly:

- Ideal: `min(1, (n+2)/q)`.
- Model A, at most `16L` bytes: `min(1, E(L)/q)`, where
  `E(L) = 8G` if `n=1`, and `n+62+[R≥29]` otherwise.
- Model A, exactly `16L` bytes: `min(1, (d(L)+n+1)/q)`, where
  `d(1)=1`, `d(2)=2`; otherwise, with `h=min(L,32)`,
  `d(L)=4*((h-1)/4) + (if h%4=1 then 3 else 4)`.

The score is the minimum of `log2(L/epsilon(L))` for the stated **upper-bound
certificate**, not an assertion that every bound is attained by a message pair.

| Model/certificate | Chart score, bits |
| --- | ---: |
| Ideal keys, at-most length | **126.41503749927884** (`128-log2(3)`) |
| Model A, at-most length | **125** (`128-log2(8)`) |
| Model A, fixed length | **126.41503749927884** |

All minima occur at `L=1`. The Lean score lemmas prove the numerator bounds
`n+2 ≤ 3L`, `E(L) ≤ 8L`, and `d(L)+n+1 ≤ 3L`, with the endpoint numerators
3 and 8. Clipping at one preserves these bounds. The same ideal and model-A
at-most scores hold in the common 8-byte-word chart units on the C domain:
use `ceil(L/64)` blocks for ideal keys and `E(ceil(L/2))` for model A.
The coarse `(n+63)/q` model-A envelope is not used to compute the chart score.

## Milestones and proof coverage

| Requested component | Checked declarations and scope |
| --- | --- |
| GCM field | `ChainHash128.modulus_irreducible`, `field_card`: `F = AdjoinRoot (X^128 + (X^7+X^2+X+1))`, `Fintype.card F = 2^128`. |
| Rabin certificate | `binary_rabin128`, all 128 `residue_step_*` identities, `residue_bezout`, `modulus_frobenius_certificates`. The generator checks all JSON residues and both supplied Bézout witnesses; Lean verifies the polynomial identities. |
| Raw PH | Width-128 `block_difference_bound` and generic `clnh_comparable_nonzero_bound`; same-count arbitrary-target bound `1/q`, nested-count nonzero-target bound `1/q`; `raw_block_degree ≤ 254`. Products are unreduced binary-polynomial products. |
| Byte stream | `block_encoding_injective`, `rawStream_collision_bound`, `stream_collision_bound`, `splitWords_injective`: 16-byte words, 64-byte groups, 512-byte blocks, strided pairs, padding including the key-only second pair, one empty block, and length XOR in both last-block halves. |
| Recurrence | `recurrence_injective` and inherited field-generic recurrence collision bounds: `n/q` for distinct equal-length streams; `(max(n,n')+1)/q` for unequal lengths. |
| Twist/finalizer | `wordAdd_matches`, `twist_bijective`, `finalizer_fivewise`, and `finalStage_collision_bound`. Integer addition is modulo `2^128`, including the low-to-high carry. Five-wise uniformity is at five distinct finalizer inputs. |
| Concrete correspondence | `referenceHash_matches` relates the direct word-operation specification to the field-valued model; `referenceHash_expanded` relates the seeded layout. `outputBytes_length`, `outputBytes_decode`, and `outputBytes_injective` establish lossless 16-byte little-endian serialization. |
| Model A | `seededStream_fixed_bound`, `reduced_lengthMask_injective`, `seededStream_common_count_bound`, `collision_bound_atMost`, and the exported byte theorem. Same-count PH degree is at most 32; unequal-group nonzero-target degree is at most 62, with the required short-message refinements. |

## Axioms, builds, and vector evidence

The full reproduction run passed. [ChainHash128Audit.lean](lean/ChainHash128Audit.lean)
and [ChainHash128Audit.txt](lean/ChainHash128Audit.txt) record `#check` and
`#print axioms` for **all 379 theorems** in the lane, including inherited
lemmas and certificate helpers. Every axiom list is a subset of
`[propext, Classical.choice, Quot.sound]`; the final byte theorems and modulus
irreducibility theorem use exactly these three. The checker rejects missing
audit output, extra axioms, proof placeholders, `native_decide`, custom axioms,
and unsafe declarations in the reported proof modules.

Changed lemmas received incremental `lake build` checks. Independent
single-identity certificate modules were checked by four builders within the
same eight-CPU allocation. The complete project build and final reproduction
also passed. Only unused-simp/variable linter warnings remain.

[Vector evidence](lean/vectors/REPORT.json): **252/252 comparisons passed**
between the executable Lean scalar transcription and both the supplied
portable and dispatched header paths. The corpus covers both key models,
empty messages, partial words, group/block boundaries, random messages below
10,000 bytes, and zero/one/all-ones edge keys. The report records SHA-256 hashes
of the header, Lean/C harnesses, corpus, and outputs.

These vector checks are executable evidence, not a universal proof that the
C header or its compiled code refines the proved word-operation specification.
The formal `referenceHash_matches` theorem concerns the word-operation and
field-valued Lean models; it does not claim a formal bridge from the separate
executable vector transcription or a compiler-correctness theorem.

## Commits, environment, logs, reproduction

- Verified proof-source commit: **`0545edec14192488861b3f17e4ff3f42aac058ca`**.
- Copied parent HEAD: `a3939c01b87d962ae170776b656a154300d5f3ca`.
- Parent model-A proof commit: `408d147c7f0aa7e123c6027b36a26821522e84a6`.
- Mathlib v4.24.0: `f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.
- Lean 4.24.0: `797c613eb9b6d4ec95db23e3e00af9ac6657f24b`.
- Workspace: `<xeon-host>:<xeon-work>/lean-chainhash128`, branch `chainhash128`.
- Final build/audit/vector log: [logs/reproduction128.log](logs/reproduction128.log).
- Incremental history: [logs/incremental128.log](logs/incremental128.log), including resolved development failures.
- Allocation/toolchain evidence: [logs/allocation128.txt](logs/allocation128.txt).

The workspace was copied with its `.lake` cache from the named parent.
Mathlib was not rebuilt. All builds used `nice -n 10 taskset -c 48-55` and
`LEAN_NUM_THREADS=8`; the CLHASH lane had reported COMPLETE before these CPUs
were used. The Mac performed editing, transfer, and lightweight inspection.
The sources, audits, vectors, and logs are mirrored under `./lean/` and `./logs/`.

Reproduce on the Xeon with the preserved cache:

```sh
cd <xeon-work>/lean-chainhash128
bash lean/build.sh
```

[build-remote.sh](build-remote.sh) transfers this mirror and invokes the same
reproduction script remotely. It does not run Lean or compile C on the Mac.
