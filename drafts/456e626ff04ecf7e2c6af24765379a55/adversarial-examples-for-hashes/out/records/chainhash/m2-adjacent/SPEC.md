# ChainHash-x86 v1

ChainHash-x86 returns 64 bits. The selected configuration uses **1024-byte
blocks, adjacent pairs of 64-bit words, S=1**, the existing three-key
GF(2^64) recurrence, and the existing twisted quintic finalizer. Its ideal-key
collision bound is `(max(1,ceil(L/128))+2)/2^64` for distinct byte strings of
at most `8L` bytes. Its per-word certificate score is **62.4150374993 bits**.

This is a separate hash family from the strided-pair M2 implementation.
Neither its outputs nor its seed verification constants are interchangeable
with `chainhash-256` or the older S=2 `chainhash-1k`.

## Arithmetic and representation

Let `F = GF(2)[X]/(X^64 + X^4 + X^3 + X + 1)`. A numerical `uint64_t` word
`w` represents `sum_i bit_i(w) X^i` in F. Field addition is XOR; field
multiplication is carry-less multiplication followed by reduction modulo
that polynomial. The same 64 bits also represent an ordinary unsigned
integer for input decoding and the finalizer's integer twist.

`clmul64(a,b)` is the **unreduced** product in GF(2)[X], represented by
`(lo64,hi64)`. Its degree is at most 126. No integer multiplication is used
in the hash proper. SplitMix64 in the convenience key constructor does use
integer multiplication.

All input and serialized key words are little endian. The returned C integer
is the full 64-bit result; serialize it little endian for a canonical digest.
There is no input alignment requirement.

## Ideal key and API

The key consists of **137 independently uniform 64-bit words**, occupying
**1096 bytes**. Every bit string is a valid key, including all zeroes.

| Word positions | Name | Bytes | Purpose |
|---|---|---:|---|
| 0–127 | `k[0..127]` | 1024 | PH key, reused at the same positions in every block |
| 128, 129, 130 | `u,y,z` | 24 | Three independent recurrence keys |
| 131–135 | `c0,c1,c2,c3,c4` | 40 | Independent finalizer circuit parameters |
| 136 | `tau` | 8 | Integer input twist |

The C structure contains exactly these words, with ordinary `uint64_t`
alignment and no cache or pointer members. Key setup performs no field
arithmetic.

```c
chainhash_x86_key chainhash_x86_key_from_bytes(const uint8_t bytes[1096]);
chainhash_x86_key chainhash_x86_key_from_words(const uint64_t words[137]);
uint64_t chainhash_x86(const chainhash_x86_key *, const void *, size_t);
uint64_t chainhash_x86_portable(const chainhash_x86_key *, const void *, size_t);
int chainhash_x86_backend(void); /* 0 portable; 1 AVX2; 2 AVX-512 */
```

`chainhash_x86_key_from_seed(uint64_t)` expands successive SplitMix64 outputs
in the above order. This convenience constructor is used by SMHasher3; its
2^64-member subfamily **does not inherit the ideal-key theorem**. The ideal
API consumes all 1096 random bytes. No entropy compression claim is made.

The header can be compiled with another block size B that is a positive
multiple of 256. That selects a different family with `B/8+9` words and
`8(B/8+9)` key bytes; the same formulas below apply after replacing 1024 by B.
The delivered default and SMHasher registration use B=1024.

## Level 1: adjacent carry-less PH

For a byte string m of length `ell < 2^64`, set

`n = max(1, ceil(ell/1024))`.

Partition it into n consecutive blocks. The empty message has one empty
block. In a block containing r bytes, use `g=ceil(r/16)` adjacent pairs.
Zero-pad only the final partial pair to 16 bytes, decode its words as
`w[0],...,w[2g-1]`, and compute in the unreduced polynomial ring

`C = XOR_{i=0}^{g-1} clmul64(w[2i] XOR k[2i], w[2i+1] XOR k[2i+1])`.

For r=0 this is the empty sum 0. In particular, 1–8 bytes activate exactly
one pair; its second multiplicand is the key word `k[1]`. There is no second
key-only pair as in the previous strided 32-byte grouping.

Split every block result losslessly as `C_j = a_j + X^64 b_j`. XOR the
**total byte length** into both halves of the **last block only**:

`a_n ^= ell; b_n ^= ell`.

No per-block universal reduction is performed. The full 128-bit PH result
is the pair of field elements consumed by level 2. Retaining it avoids both
extra multiplications and an additional collision term.

## Level 2, twist, and finalizer

All operations in the following recurrence are in F:

`P_0 = z`,

`P_j = a_j + (b_j + y)(P_{j-1} + u)`, for `j=1,...,n`.

Interpret `P_n` as its unsigned word and set

`v = (P_n + tau) mod 2^64` using **integer addition with carries**.

Reinterpret v in F and evaluate the same three-multiplication circuit as
ChainHash-64:

`q = v*v`,

`r = (q+c0)(v+q+c1)`,

`H = (v+c2)(r+c3)+c4`.

The five `c` words are circuit parameters, not literal polynomial
coefficients. Their bijection with the five lower coefficients of a monic
quintic is the existing proved coefficient-map theorem.

## Bounds and length domain

For any fixed distinct m,m', independent of the ideal key, let
`N=max(n(m),n(m'))`. Then

`Pr[H_K(m)=H_K(m')] <= min(1,(N+2)/2^64)`.

Equivalently, for integers `1 <= L <= 2^61-1` and messages of at most `8L`
bytes,

`epsilon(L) = (max(1,ceil(L/128))+2)/2^64`

is a valid (possibly loose) certificate. On this domain it is below one.
The per-word score uses the same 8-byte unit as ChainHash-64:

`min_L log2(L/epsilon(L)) = 64-log2(3) = 62.4150374993`.

It is attained by the certificate at L=1: `max(1,ceil(L/128))+2 <= 3L`
for every positive L. This is a score of the upper bound, not a claim that
some message pair attains that bound.

The mathematical byte domain is all lengths below 2^64. The C API also
requires a valid object of `len` bytes, a non-null key, and `len` representable
by `size_t`. `data` may be null exactly when `len=0`. The implementation uses
remaining-byte subtraction; it never forms `len+B-1`, so no additional
block-count overflow exclusion is required.

## SIMD evaluation and dispatch

The portable reference uses fixed-width C99 unsigned arithmetic and
bit-serial polynomial multiplication. It requires no `__int128` extension.

The AVX-512 path loads 64 message bytes and 64 key bytes, XORs them, and uses
`VPCLMULQDQ imm=0x10` on that one register. Each of its four 128-bit lanes
contains exactly one adjacent message pair. Thus one instruction computes
four independent 64x64 products for 64 input bytes, with **no pair-rearranging
shuffle**. Four independent accumulation vectors cover each 256-byte group.
After a block, XOR folding produces the original 128-bit PH sum.

The AVX2 fallback uses 256-bit key/input loads and extracts their 128-bit
lanes for ordinary PCLMULQDQ. It needs AVX2, PCLMUL and SSSE3; VPCLMUL is not
required. Both hardware paths use the existing exact two-fold GF(2^64)
reducer, whose second carry-less multiplication is a 16-entry PSHUFB table.
Only the low lane of the reduced result is live.

Runtime dispatch checks CPUID PCLMUL, SSSE3, AVX, OSXSAVE, AVX2, and XGETBV's
XMM/YMM save-state bits. AVX-512 additionally requires AVX512F, VPCLMULQDQ,
and the opmask/ZMM save-state bits. It requires no AVX512BW/DQ/VL. The cached
decision uses relaxed atomics for race-free C99 and C++11 calls. ISA-specific
functions have GCC/Clang target attributes, so compiling a translation unit
without ISA flags is supported. An executable globally built with
`-march=native` retains that executable's native baseline requirement.

`CHAINHASH_X86_FORCE_PORTABLE` omits hardware code and dispatch. Explicit
AVX2/AVX-512 entry points are available in supported x86 builds for testing;
call them only after the corresponding backend capability check.
