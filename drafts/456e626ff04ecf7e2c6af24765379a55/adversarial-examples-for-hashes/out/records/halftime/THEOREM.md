# The fixed HalftimeHash24 theorem

This statement concerns `HalftimeHash-fork/halftime-hash.hpp` on the
`fixed-24byte-core` branch, based on `fix-neon-dispatch` (`8b03edf`).
`advanced::V1<3>`, `V2<3>`, `V3<3>`, and `V4<3>` return **three 64-bit
words**. The benchmark name `HalftimeHash24-fixed` selects `V4<3>`.
The Style64/128/256/512 wrappers continue to return **one 64-bit word**
and continue to use the distance-two core.

## Contract and domain

Let `b ∈ {1,2,4,8}` be the number of 64-bit lanes in a block,
`ε = 2^-32`, and `C = 8 + 8² + … + 8⁸ = 19,173,960`.
Keys are independent uniform 64-bit words; messages are fixed independently
of the key. The theorem is about this ideal-key family, not a family with a
64-bit seed expanded by the benchmark's PRNG. Arithmetic is modulo `2^64`,
with 32-bit additions performed modulo `2^32` before integer multiplication.
Bytes are eight bits; `uint64_t` and `size_t` are 64 bits; input words are
little endian, as on the two tested hosts. Input, key, and output objects
must meet the ordinary C++ size/lifetime requirements. The key is a real
contiguous `uint64_t` array of the reported extent; input and key storage
remain unchanged during the call, and output storage is disjoint from them.

The existing nine-entry forest stack is retained. Its finalizer requires a
zero sentinel, so the supported domain is:

* 24-byte core: `0 ≤ |m| < 168 b (C+1)` bytes.
* Style wrappers: `0 ≤ |m| < 144 b (C+1)` bytes.

The fixed code does not expand this domain or add a runtime length check.
At `C` complete groups the root counts are `[8,8,8,8,8,8,8,8,0]`.
For `n ≥ 1` groups the exact maximum live height is
`h(n) = floor(log_8(7n+1)) - 1 ≤ 7`; the number of live roots is at most 64.
The counts are the bijective-base-eight digits of `n`, obtained by repeatedly
emitting `1+(n-1) mod 8` and replacing `n` with `(n-1)/8`.
This is the promotion-before-insertion schedule of `DfsTreeHash`.
No statement is made for calls beyond that stack-safe domain.

## Exact 24-byte statement

For every fixed distinct pair `x,y` in this domain and every fixed vector
target `v ∈ (Z/2^64)^3`:

1. If `|x| ≠ |y|`, then
   `Pr[H(x)-H(y)=v] ≤ ε³ = 2^-96`.
2. If lengths agree and their padded raw tails differ, the same `ε³` bound holds.
3. If lengths agree and there are no complete groups, distinct inputs have
   differing tails, so the same `ε³` bound holds.
4. Otherwise, with `h` any common upper bound for the forest heights,
   `Pr[H(x)-H(y)=v] ≤ min(1, (h+2)²(h+5)·2^-96)`.

In particular these statements hold for collision target zero. They do not
assert independent output coordinates, uniform 192-bit outputs, or
cryptographic security after disclosure of the key.

At the abstract height `h=16`, the last expression is
`6804·2^-96`, whose negative logarithm is **83.2678325743 bits**.
The unexpanded C++ stack has `h≤7`, giving the stronger uniform bound
`972·2^-96`, or **86.0751874964 bits**. An `h=16` tree cannot actually be
processed by this header's retained stack. “Proved 83 bits” is a conservative
collision-bound label on the supported domain, not the length-normalized score.

## Length rule and its proof

For the three-word core, define these **word offsets relative to the core key**:

```
EHC keys                  [0,27)
reserved tree keys        [27,216)
forest-finalizer keys     [216,216+192b)
separate Toeplitz pool    starts at A = 216+192b
terminal length position q = 21b within that pool
```

If the remainder has `r < 168b` bytes, let `t = floor(r/(8b))+1`.
As before, the finalizer inserts all complete raw blocks and one extra block,
zero padded; the extra block is present even for an aligned or empty remainder.
There are `t≤21` such blocks. The pool is **right aligned**: their first key
position is `A+(21-t)b`. Output `j` shifts its keys by `jb` whole NH pairs.
After these blocks, append exactly **one scalar 64-bit word containing the
original total byte length**. Its key word in output `j` is
`K[A+q+jb]`, for `j=0,1,2`. There are still only three output words.
`HashWithLength` performs this final scalar NH operation after the horizontal
lane sum; modular addition makes that ordering immaterial.

The forest and tail key sets are disjoint for **both messages**, even when
their numbers of roots differ. No length word is broadcast to all lanes.
There is no variable-position length tag, extra output word, or probabilistic
length compression before this step.

For unequal lengths, write the difference in output `j` as

`R_j(K) + NH(length(x), K[A+q+jb]) - NH(length(y), K[A+q+jb])`.

Here `R_j` includes every forest and data-tail term. Every key in the data
tail of output `i` has index at most `A+q-1+ib`; its length key has index
`A+q+ib`. Thus `R_i` ignores the length key of output `j` whenever `i≤j`,
and the whole earlier output ignores it whenever `i<j`, regardless of the
sizes of either tail. Missing leading data words need not be modeled as NH
of zero; they simply contribute no term to `R_i`.

The two 64-bit lengths differ, so at least one of their 32-bit halves differs.
Fixing the key for that half, the integer-NH injectivity lemma permits at
most one value of the other 32-bit key half for any prescribed difference.
Expose these three other halves in output order. Earlier events ignore the
next exposed half, and each conditional event costs at most `ε`.
The joint probability is at most `ε³`. This argument permits overlapping
Toeplitz keys and arbitrary dependence between the residual outputs.

`certificates/lean/FixedLength.lean` proves this terminal-word statement as
`terminal_length_three_bound`, using the existing integer NH and
`triangular_adu` lemmas. Its residual hypothesis is exactly the key-index
inequality just established. Its axiom audit lists only `propext`,
`Classical.choice`, and `Quot.sound`; there are no placeholders or new axioms.
The proof also applies to every fixed nonzero difference target.

For equal lengths the appended length contributions cancel. The two tails
have the same support and key alignment. If they differ, the existing
last-differing-pair Toeplitz argument gives `ε³` after conditioning on all
prefix keys. If they agree, the common tail cancels and the repaired EHC /
forest / final-NH theorem applies unchanged.

## Encoder certificate and mapping to Lean

Each XOR encoder is a binary linear map on triples of blocks. The executable
`certificates/src/matrix.cpp` obtains its generator rows from the **actual
header**, by placing distinct basis bits in its input blocks. The independent
Python checker performs GF(2) Gaussian elimination for **every** subset of
surviving packets. A survivor packet contributes all three of its rows.

| Encoder | Data packets | Encoded packets | Rank required | Every subset checked | Critical survivor subsets | Distance |
|---|---:|---:|---:|---:|---:|---:|
| Encode2 | 6 | 7 | 18 | 128 | 7 | 2 |
| Encode3 | 7 | 9 | 21 | 512 | 36 | 3 |
| Encode4 | 7 | 10 | 21 | 1,024 | 120 | 4 |
| Encode5 | 5 | 9 | 15 | 512 | 126 | 5 |

Every set of at least `N-k+1` surviving packets has full input rank. If a
nonzero codeword had at most `k-1` nonzero packets, erasing them would leave
such a full-rank survivor set equal to zero, a contradiction. This proves
distance at least `k`. A basis input attains `k`, proving equality. Since the
source uses only XOR/copy operations, this bit-level proof applies separately
to every bit and physical lane, and hence to arbitrary input blocks.
The rank matrices, every subset/rank, and attaining flip distances are in
`certificates/rank-certificate.json`. Direct execution additionally checks
all 20,160 single-bit flips of an Encode3 group over the four widths.

The repaired capture references the changing row pointer; increments advance
one whole row; the first parity loop stops at the end of the **data** rows.
It never feeds parity rows back into the parity calculation.

| Checked mathematical hypothesis | Fixed code / discharge |
|---|---|
| Distance-three encoder, including on a differing physical lane | `Encode3`; exhaustive full-rank survivor certificate above. |
| Nine independently keyed integer NH symbols | `Hash` reads the flat key array at `3*i+j`; 27 distinct 64-bit words. |
| Literal T3 with nonzero 3-column minors and worst valuation 2 | Unchanged `Combine3`; the T3 entries are exactly those in Lean `Matrices.lean`; its 84 minors and projection polynomial are already kernel checked. |
| Key reuse across leaves and physical lanes is allowed | EHC `LoadOne` broadcasts symbol keys; the same 27 words are reused per group. No lane independence is assumed. |
| Fixed, key-independent forest schedule | `DfsTreeHash` depends only on the number of complete groups. Its bijective-base-eight root heights satisfy `lazyHeights_spec` in `StyleSchedule.lean`; `lazyShapes` embeds shorter roots with `TreeShape.skip`, preserving the key at each actual height. Leaf ordering is a fixed public bijection. |
| Independent tree keys by height and component | Tree word address `27+21*level+7*component+(child-1)`; child 0 is the unchanged accumulator, children 1..7 are NH-hashed. |
| Independent final NH by root position, component, and lane | `BlockGreedy::Insert(const Block (&)[3])` uses `216+b*(3*root+component)+lane`; the final horizontal sum is NH on the flattened root/lane list. |
| Injective word packing and modular lane sum | Little-endian loads, low/high 32-bit halves, `Plus32`, `Times`; scalar horizontal additions now explicitly convert operands to `uint64_t`. |
| Fresh tail pool, whole-pair shifts, injective equal-length padding | `ResetSeeds` reserves all 64 possible final roots, right-aligns the tail, and shifts by `b` pairs; the original bytes occur unchanged in the padding. |
| All-length distinction | The terminal scalar length word and `terminal_length_three_bound`, as proved above. |
| Uniform independent key coordinates with sufficient storage | The disjoint word ranges above and exact `GetEntropyBytesNeeded` extent; unused words integrate out. |

For the equal-tail case, select a lane where a leaf differs and use the
T3 subset bounds on that lane. Equality of full output blocks implies equality
on that lane. Conditional on the EHC keys, the three tree/final key sets are
independent, and each differing coordinate sequence costs at most `(h+1)ε`.
The checked projection polynomial gives
`((h+1)ε+ε)²((h+1)ε+4ε)`, exactly the displayed bound.
This is the lane version of Lean `scalar_halftime_three_bound` /
`scalar_halftime_6804`, using the same conditioning argument formalized for
broadcast lanes in the Style lemmas. It assumes no independence of EHC
outputs or physical lanes.

The existing Lean development formalizes the mathematical components and
abstract construction; the new Lean file formalizes the length extension.
The rank certificate and this source/layout argument discharge the remaining
implementation hypotheses. There is **no claim of a machine-checked C++
compiler, instruction semantics, or complete C++-to-Lean refinement**.

## Entropy sizing and C++ objects

`GetEntropyBytesNeeded<W,k>(n)` now interprets `n` in **bytes**. For a valid
call it returns eight times the exact exclusive maximum key-word index
read, including unused holes in the allocated prefix. It does not claim
that every intervening word is read. For an out-of-domain request it returns
the maximum over the valid domain, preserving its use for a maximum-size
allocation; that return value does not make an out-of-domain hash call valid.

Let `d=(6,7,7,5)` and `e=(7,9,10,9)` for `k=(2,3,4,5)` respectively,
`F=3e+63k`, `R` be the sum of the bijective-base-eight root digits,
and `t=floor((n mod (24db))/(8b))+1`. Then the required prefix in words is:

* For `k≠3`: `F + b*(k*R+t+k-1)`.
* For `k=3`: `216 + 215b + 1`, for every supported byte length.

The latter values are **432, 647, 1,077, 1,937 words** for the four widths.
A one-leaf witness actually reads only `[0,27)`, `[216,216+3b)`, and
`[216+212b,217+215b)`, totaling `28+6b` words. Every unused word can be
integrated out of the random-key experiment.

For the Style512 key allocator the public constant becomes 59,736 bytes;
its table reads remain inside the first 6,144 words. `MaxEntropyBytesNeeded`
now returns the maximum raw-core allocation (24,432 bytes at `b=8,k=5`).
The guard-page certificate tests all `b` and all `k` at short lengths,
group boundaries, and forest promotions: exact extents work, and one word
less faults in every tested cell.

The table extent is `8*(1+width)*256` words, 6,144 words for a Style wrapper.
Both tabulation helpers now index the caller's real flat `uint64_t` array;
there is no undersized multidimensional view or invented table subarray.
EHC key indexing likewise uses the caller's flat array, and the encoders take
actual three-block rows instead of reconstructing them through a cast.
The SIMD unsigned-sum repair removes the need for `-fwrapv`. In particular,
the AVX-512 helper explicitly reduces through vector intrinsics and unsigned
scalar additions: GCC 11's convenience reduction intrinsic contains a final
signed scalar addition. These are source
object/arithmetic repairs, not changes to the intended lookup addresses or
Style arithmetic.

## Scores in the requested metric

Let `L≥1` be an integer cap in **eight-byte words**, with both messages at
most `8L` bytes and individually in the supported domain. Use the output
collision floor `2^-192` for the 24-byte result; it is smaller than every
bound below. One valid cap envelope is

```
E_b(L) = 2^-96                                      if L < 21b,
         (h+2)^2(h+5) * 2^-96                       otherwise,
h = min(7, floor(log_8(floor(L/(21b))))).
```

For caps beyond the supported domain it suffices to keep the envelope at
its maximum supported value. The unequal-length bound is no larger than
this envelope. Below the first leaf the minimum of `log2(L/E_b(L))` is
96 at `L=1`. At the first leaf, `L/((h+2)^2(h+5)) = 21b/20 ≥ 1`.
At later height boundaries lengths grow by eight, whereas the coefficient
grows by at most `54/20 < 8`; within a plateau the score increases.
Thus **the certified normalized score is 96 bits, for every b**.

This is sharp for the ideal family: the one-byte messages `00` and `01`
have equal lengths, so their length terms cancel. Each output's only data
NH difference vanishes exactly when the high 32 bits of its particular
key word are zero. Those three word positions are distinct, so the collision
probability is exactly `2^-96`, attaining the score at `L=1`.

## Fixed Style wrappers

Their functional outputs are unchanged relative to the previous address-level,
modular-sum implementation. They still use Encode2 and do **not** append the
new raw-core length word. Their length is still handled by the original
length-byte tabulation tables. The table repair changes the C++ representation,
not any table index; the unsigned-sum repair preserves modular results.
Execution compares 40,000 Style results against the parent header.

Let `δ=2^-64`, `s=(1-ε)^(h+1)`, `a=1-s`. The existing checked sharp
core bound is `B₂(h)=(a+sε)(a+2sε) ≤ (h+2)(h+3)δ`, or `δ` with no group.
All core keys still lie below absolute word 2,048; the 16 output-byte tables
occupy `[2048,6144)` and are fresh after conditioning on lower words.
The overlap with the length tables is retained exactly.

For equal lengths the wrapper collision probability is
`δ+(1-δ)Pr[core(x)=core(y)]`. For unequal lengths it is at most
`2δ-δ²`, without assuming independence of core and length tables.
Lean `byteStyleHash_normalized` / `modeledStyleHash_normalized` consequently
give, for the exact supported byte domains and all positive word caps,

`Pr[Style(x)=Style(y)] ≤ L*(2^-63 - 2^-128)`.

The output floor is `2^-64`. The short pair `00` versus `01` has collision
probability exactly `2^-63 - 2^-128`, so each fixed Style wrapper has exact
normalized score **`64-log2(2-2^-64)`**, conservatively **63 bits**.
The table/object and unsigned-arithmetic fixes permit applying this argument
to the supported C++ implementation rather than relying on the old out-of-bounds
array view and signed-overflow execution contract.

The inherited Lean sources, pinned toolchain, and audits are included in
`certificates/lean/base/`; the new length proof imports that snapshot.
