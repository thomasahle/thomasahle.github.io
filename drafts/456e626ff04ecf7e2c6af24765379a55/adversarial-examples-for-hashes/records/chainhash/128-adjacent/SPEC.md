# ChainHash-128 adjacent, version 1

Both designs use 512-byte blocks, one GF(2^128) recurrence per block, and a
128-bit result. They are different keyed families. `CHAINHASH128_DESIGN=1`
selects A; `=2` selects B (the delivered default). Every backend of a given
design has identical bytes. This document specifies the default block size;
the optional 256-byte build substitutes that size and corresponding key counts.

## Delta from SPEC_128.md

| Item | Original strided | A: PH128 adjacent | B: independent PH64 × 2 |
|---|---|---|---|
| Data pair | words (0,2), (1,3) per 64 B | 128-bit words (0,1), (2,3) | 64-bit words (0,1), (2,3), in **both** lanes |
| Active granularity | 64 B | 32 B | 16 B |
| Scalar CLMULs per 32 B (full blocks) | 3 Karatsuba / 4 schoolbook | 3 Karatsuba / 4 schoolbook | 4 |
| Raw block output | 256 bits | 256 bits | two 128-bit outputs, all bits preserved |
| Independent PH key | 512 B | 512 B | 1024 B |
| Full ideal key | 656 B | 656 B | 1168 B |
| Final length field | 128-bit ell | 128-bit ell | (ell mod 2^64, ell mod 2^64), in 64-bit limbs |
| Recurrence / finalizer | GF(2^128) | same function | same function |

Padding activates only the final pair, not an additional key-only pair.
The B length field is deliberately replicated into **both 64-bit limbs** of
both recurrence inputs (using ell mod 2^64). On the C domain, keeping the old `(ell,0)` mask would leave one PH
lane without the nonzero target needed by the unequal-pair-count lemma.

There is no correlated-PH-seed theorem in this delivery. In particular the
old 160-byte `key_from_bytes` constructor and its seeded bounds are **not**
carried over: the new constructor consumes the full ideal key. Design A here
is not the old document's “key model A”. SplitMix64 is only a convenience and
benchmark fixture; its seed-expanded subfamily has no claimed bound below.

## Arithmetic, keys, domain

Let R = GF(2)[X], Pi = X^128 + X^7 + X^2 + X + 1, F = R/(Pi), q=2^128,
and Q=2^64. Bit i represents the coefficient of X^i. Words, keys and output
are little endian. The reduction constant is 0x87, with integer-bit encoding,
not GHASH's external bit-string encoding. The original arithmetic convention
is retained. Pi is irreducible; `tests/algebra.py` independently checks Rabin's
criterion. Products in R are carry-less and unreduced; products in F reduce
modulo Pi. Addition is XOR except for the explicit final integer twist.

Mathematical messages have length 0 <= ell < q, as in the original specification.
The C interface on ordinary 32/64-bit targets implements the representable
subdomain ell < 2^64. Let n=max(1,ceil(ell/512)). The empty message has one
empty block. No input padding or alignment is required. NULL is permitted
only for zero length. The loop subtracts remaining bytes and does not form
`ell+511`. On 32-bit platforms the representable subdomain is smaller. For B, the low
64-bit length residue suffices even on the larger mathematical domain: two
distinct lengths with the same block count differ by at most 512 < 2^64,
so their residues differ. Different block counts are distinguished by the
formal recurrence degree. No 128-bit-sized C object or length API is claimed.

A's key is k[0..31] in F, followed by u,y,z,c0,c1,c2,c3,c4,tau in F.
B's key is k0[0..63], then k1[0..63], all 64-bit words, followed by the same
nine F words. **All words in the selected layout are independently uniform,
including zero.** PH positions are reused across blocks. The C representation
packs consecutive 64-bit key words into `ch128_word {lo,hi}`: B's second lane
starts at `words[32]`; its recurrence suffix starts at `words[64]`. A's suffix
starts at `words[32]`. There are no derived or cached fields in the key.

## Level 1, A

For a block with r bytes, zero-pad to 32 ceil(r/32) bytes and decode 128-bit
words w. In R compute

    C = XOR_{j < ceil(r/32)} (w[2j] XOR k[2j]) * (w[2j+1] XOR k[2j+1]).

The empty sum is zero. Split C = a + X^128 b losslessly; degree(C) <= 254.
On the last block only, XOR the 128-bit F encoding of ell into both a and b
(the high 64-bit limb is zero on the C interface).

Writing each multiplicand as a0 + X^64 a1 and b0 + X^64 b1, schoolbook
accumulates L=a0*b0, H=a1*b1, M=a0*b1+a1*b0 (four CLMULs). Karatsuba uses
L,H,T=(a0+a1)*(b0+b1) (three), then M=T+L+H. These components are XOR-reduced
across pairs before the single reconstruction `(L + X^64 M) + X^128 H`.
Both algorithms define exactly the same raw C.

## Level 1, B

For a block with r bytes, zero-pad to 16 ceil(r/16) bytes and decode 64-bit
words w. Both independently keyed lanes process **the entire same block**:

    C_s = XOR_{j < ceil(r/16)} (w[2j] XOR k_s[2j]) *
                                      (w[2j+1] XOR k_s[2j+1]),  s in {0,1}.
    C_s = l_s + X^64 h_s.
    a = l_0 + X^64 l_1;   b = h_0 + X^64 h_1.

The degree of each C_s is at most 126. On the last block only, XOR
`d = ell_64 + X^64 ell_64`, where ell_64 = ell mod 2^64, into **both** a and b.
Equivalently, each lane receives `ell_64*(1+X^64)` in its raw 128-bit output before the halves are packed.

This is a bijective packing of all 256 raw bits, not truncation, reduction,
or assignment of different data to different lanes. A raw block-stream
collision means that **both** independent lane equations hold. Merely
retaining the low 64 bits of each C_s would be invalid: a difference X^63 in
one word gives a truncated collision with probability 1/2 in that lane.

## Levels 2 and 3 (unchanged)

For the ordered stream `(a_t,b_t)`, t=1..n:

    P_0 = z
    P_t = a_t + (b_t+y)*(P_(t-1)+u)
    v = P_n boxplus tau
    G_1 = v*v
    G_2 = (G_1+c0)*(v+G_1+c1)
    H = (v+c2)*(G_2+c3)+c4

All additions except boxplus are XOR; products are in F. Boxplus is integer
addition modulo 2^128, **including the carry from the low limb**. Serialize
H as exactly 16 little-endian bytes. The implementation's shifted state
Q=P+u removes one XOR at recurrence boundaries without changing this rule.

## Ideal-key theorem and exact PH bound

For fixed distinct byte strings m,m', independent of the key, let
N=max(1,ceil(max(|m|,|m'|)/512)). For **either A or B**,

    Pr[H(m)=H(m')] <= min(1, (N+2)/2^128).

This is a collision bound, not a cryptographic MAC claim and not a claim
about every individual key. “128-bit PH bound” below means an upper bound
with exactly that denominator; probabilities for particular message pairs
can be smaller, including zero.

1. **Raw PH lemma at width b.** For distinct padded inputs with the same
   number of pairs, the XOR difference contains a nonzero word difference
   delta multiplying its partner's uniform b-bit key. Condition on every
   other key. In the integral domain R, `delta*k=C` has at most one solution
   among 2^b choices. Thus every target has probability at most 2^-b.
   For nested different pair counts, the same bound holds for every
   **nonzero** target. Induct on fresh pairs A*B: A=0 has probability 2^-b,
   and otherwise at most one B solves the equation. If the previous
   nonzero-target atom is at most 2^-b, the next is at most
   `(1-2^-b)*2^-b + 2^-b*2^-b = 2^-b`.
   The base is a same-count equation or the impossible equation 0=C.
   The nonzero condition matters: a fresh product is zero with probability
   `2^(1-b)-2^(-2b)`, not 2^-b.

2. **A stream.** A necessary nontrivial block equation has probability at
   most 1/q. For unequal byte lengths with common block count, the final
   block equation's target is `(ell XOR ell')*(1+X^128)`, nonzero. For equal
   byte lengths, select any block where the data differ. Nested padding is
   exactly the active-pair convention required in step 1.

3. **B stream — exact statement.** In the same selected block, each lane
   equation has probability at most 1/Q. For unequal lengths its target is
   `(ell_64 XOR ell'_64)*(1+X^64)`, nonzero in **both** lanes: common
   block counts imply |ell-ell'| <= 512 < Q. For equal lengths
   both lanes see the selected differing block. The key sets are disjoint,
   hence the two events are independent, and

       Pr[both raw lane equations hold] <= (1/Q)*(1/Q) = 2^-128.

   The constant 1 is sharp: with identical length and a difference in just
   one multiplicand, a zero target forces its partner key to one value in
   each lane, giving exactly 1/Q^2. This is not a square of a whole-hash
   bound. Only level 1 is doubled; there is one GF(2^128) recurrence and one
   finalizer. Reusing PH keys between blocks costs no factor N: whole-stream
   equality implies one selected block equation. Reusing keys **between
   lanes** would invalidate the independence step.

4. **Composition.** For equal stream lengths n, the formal recurrence
   polynomial is injective in the ordered stream, and its universal leading
   homogeneous part is `(Z+U)Y^n`. The difference for distinct streams has
   total degree at most n; Schwartz–Zippel with independent u,y,z gives
   n/q. For unequal stream lengths, formal degrees differ and give at most
   (N+1)/q, with no PH-collision term. The monic quintic finalizer has
   independent uniform coefficients: the same explicit coefficient
   bijection as in SPEC_128.md applies over F. At any two distinct inputs
   its collision probability is exactly 1/q. Integer translation by tau
   is a bijection. Therefore equal counts cost at most
   `1/q + N/q + 1/q`, and unequal counts at most `(N+1)/q + 1/q`.

For completeness, the finalizer coefficient bijection is explicit. Write
`b=c0+c1`, `d=c0*c1` and `H(v)=v^5+e4*v^4+...+e0`. Then

    e4=1+c2; e3=b+c2; e2=c0+c2*b
    e1=d+c3+c0*c2; e0=c4+c2*(d+c3).

Its inverse is

    c2=e4+1; b=e3+c2; c0=e2+c2*b; c1=b+c0; d=c0*c1
    c3=e1+d+c0*c2; c4=e0+c2*(d+c3).

Thus independent uniform c0..c4 give independent uniform e0..e4. At two
distinct v values, one nonzero linear coefficient in their output difference
is uniform, proving the exact 1/q finalizer collision probability used above.

The proof uses the original field-generic PH, ordered-stream, finalizer and
twist arguments. B adds independent product measure and the lossless packing
and duplicated length mask. No new Lean proof or compiler correctness proof
is claimed. These are mathematical bounds plus the delivered executable
arithmetic, backend and byte-stream checks.

## Score (both designs)

Use the certificate score `min_L log2(L / epsilon^<= (L))`, with L positive
**8-byte words**, and both message lengths at most 8L within the byte-length
C-interface domain. The same minimum holds on the larger mathematical
length domain. Then

    epsilon^<= (L) = min(1, (ceil(L/64)+2)/2^128)
    score = 128-log2(3) = 126.41503749927884 bits.

The numerator is at most 3L, with equality at L=1, which proves the minimum.
The same minimum holds in 16-byte-word units. This exceeds 124 bits. It is
not a uniform 124-bit collision probability at arbitrarily large lengths:
the explicit bound grows with N. At at most 512 B, 1 KiB and 1 MiB its
numerators are respectively 3, 4 and 2050, for either design.

## Backends and API

`chainhash128_key_from_ideal_bytes` and `chainhash128_key_from_bytes` both
read `CHAINHASH128_KEY_BYTES` independent bytes. `key_from_words` copies the
same layout; `key_from_splitmix64` is outside the theorem. `chainhash128`
returns a `ch128_word`; `chainhash128_store` serializes it. Explicit portable,
AVX2, AVX-512 and PMULL entry points are available where compiled. Backend
IDs are 0 portable, 1 AVX2+PCLMUL, 2 AVX512F+VPCLMUL, 3 ARM PMULL.

x86 dispatch checks CPUID **and OS XCR0 save support**, with a race-free atomic
cache. AVX2 uses XMM PCLMUL (AVX2 alone does not provide VPCLMUL); memory loads
address the adjacent operands directly. ZMM B uses one keyed load and one
VPCLMUL per lane per 64 bytes, with four accumulators per lane. A schoolbook
uses masked de-interleaving loads; `CHAINHASH128_A_GATHER` compares gathers
using all four 128-bit lanes. Horizontal folds, reconstruction and recurrence
arithmetic are outside the PH product loop.

ARM PMULL requires the crypto compiler target; otherwise the portable path
is selected. B uses LD2 .2d for both data and keys, then PMULL/PMULL2 on
matching lanes. For two adjacent pairs `(A,B),(C,D)`, A uses two LD2 .2d
loads and one lane LD2 to form outer limbs `[A,D]` and inner limbs `[B,C]`.
Matching low/high PMULL lanes then compute `A*B` and `D*C`. The same three
loads arrange the key operands. This reads 80 bytes per 64-byte group from
each of the data and key arrays, entirely within the group; the overlapping
loads avoid register lane permutations. Karatsuba uses six PMULLs per group,
schoolbook eight. Short tails are copied to a zeroed local pair.
`CHAINHASH128_SCHOOLBOOK` selects four-product arithmetic on ARM; x86 PH is
always schoolbook. Neither path shuffles lanes inside its full-block PH loop.

The instruction semantics are specified in the [Intel instruction reference](https://cdrdv2-public.intel.com/835757/325383-sdm-vol-2abcd.pdf)
and [Arm ACLE intrinsic reference](https://arm-software.github.io/acle/neon_intrinsics/advsimd.html).
In particular, `vld2_u64` maps to LD1, whereas `vld2q_u64` maps to LD2; the
full-block ARM loops use the latter where de-interleaving is required.
