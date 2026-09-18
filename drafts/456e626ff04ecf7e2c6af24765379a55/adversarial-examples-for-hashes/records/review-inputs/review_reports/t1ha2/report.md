The collision mechanism and claimed rate are **confirmed at sampling precision**. The standalone implementation matches the reference verification value, the supplied example collides, and direct sampling of the independently derived weak-seed class gives a conditional rate of `2^-4`, implying a uniform-seed contribution of `2^-30`.

Implementation: [verify.cpp](verify.cpp). Integer derivation and statistics: [analyze.py](analyze.py). Raw outputs: [validation.txt](validation.txt), [uniform.txt](uniform.txt), [class.txt](class.txt), and [analysis.txt](analysis.txt).

**Reference and validation.** I read `ref/t1ha.cpp`, including `tail64`, `mux64`, `init_ab`, `init_cd`, `mixup64`, `final64`, the block/tail routines, the `t1ha2` wrapper, and the `REGISTER_HASH(t1ha2_64, ...)` block. I also read the multiplication semantics in `ref/Mathmult.h` and `_ComputedVerifyImpl` in `ref/Hashinfo.cpp`. The implementation uses explicit little-endian byte loads, unsigned 64-bit wraparound, and full unsigned 128-bit products. It includes the long-input path required by verification, and neither includes nor compiles the SMHasher3 framework.

For each length `i=0..255`, I hashed bytes `0..i-1` with seed `256-i`, serialized all 256 full 64-bit outputs little-endian into 2,048 bytes, and hashed that buffer with seed zero. The resulting full hash is `0x11bc72558f16c948`; its first four output bytes are `48 c9 16 8f`, giving **`0x8F16C948`**, exactly the `verification_LE` in `ref/t1ha.cpp:1679`. The reference's empty-input vectors for seeds zero and all ones also pass.

**Messages and example.** Hex strings are decoded as bytes, with the lengths shown; there is no padding of the input message.

| Input | Length | Message hex | Hash at seed `0x3c805cc67a687f30` |
|---|---:|---|---|
| m1 | 16 bytes | `406b68c281a55e00158ed10a0d96f8ff` | `0xf2a1196d24fddaab` |
| m2 | 11 bytes | `d1d86241d24d9f990323e0` | `0xf2a1196d24fddaab` |

Both full 64-bit outputs match. Their serialized little-endian bytes are `ab da fd 24 6d 19 a1 f2`. Before finalization, both have `a=0x9de37c592b9f25af` and `b=0x96ff6662d4bec5d7`. For m1, the example's `L=0x96b1df91fb0202ed` satisfies the claimed class condition.

**Sampling.** Every trial calls the complete validated hash implementation on **both** messages and compares the entire 64-bit outputs; no trail test filters which messages get hashed. The RNG is a locally implemented SplitMix64, with increment `0x9e3779b97f4a7c15` and mixing multipliers `0xbf58476d1ce4e5b9` and `0x94d049bb133111eb`. The initial states below were fixed before either sampling run. Class trials sample with replacement by retaining all 38 free bits from a fresh RNG output and inverting the class mapping. Every inversion is checked. Runs are sequential and use one thread.

| Experiment | SplitMix64 initial state | N | Full-output collisions | Measured rate | log2(rate) |
|---|---|---:|---:|---:|---:|
| Uniform 64-bit seeds | `0x243f6a8885a308d3` | 67,108,864 = `2^26` | 0 | 0 observed | `-infinity` |
| Uniform seeds within the stated class | `0x13198a2e03707344` | 4,194,304 = `2^22` | 262,144 | 0.0625 | -4 |
| Class contribution over uniform seeds | — | inferred | — | `9.31322574615479e-10` | -30 |

I followed the final explicit request for `2^26` uniform trials **plus** `2^22` class trials: 71,303,168 sampled trials in total, with neither experiment exceeding `2^26`. This interprets the earlier seed cap per experiment; the two requested sample sizes cannot both fit under an aggregate `2^26` cap.

The uniform run visited the class once and found no collisions. At the claimed `2^-30` rate, `2^26` trials have **0.0625 expected collisions, not approximately 64**; observing zero has probability about 0.9394. Thus that run alone is inconclusive about the rate. Its one-sided 95% upper bound is approximately `4.464e-8`. The class sample gives a Wilson 95% interval of `[0.0622687443, 0.0627320571]`, or an implied uniform contribution with log2 in `[-30.0053480, -29.9946533]`. These intervals use the usual independent-uniform-trial approximation for the PRNG samples. The team's reported 12 collisions in `2^34` trials would also be compatible with a rate near `2^-30` (16 expected); I did not reproduce that larger run.

**Why the collision happens.** Both lengths take exactly two short-input mixing steps in `T1HA2_TAIL`: the first multiplies `(length + first_word)` by `P2`, XORs its low half into the seed, and adds its high half to the length; the second multiplies the seed-dependent state plus the zero-extended tail by `P1`, XORs the low product into the other state word, and adds the high product into the first. The differing lengths therefore change public constants as well as the message words. For this pair, the first-step low products differ in only four XOR bit positions. One particular choice of the resulting four addition/subtraction signs makes the high-half change of the second product cancel the first-state difference, provided there is no input wrap. A 26-bit condition on the second product's low half simultaneously cancels the second-state difference. When these conditions hold, the entire `(a,b)` state is equal before `final64`, so the complete hash outputs must coincide. All 262,144 sampled collisions had this exact state equality.

The class was derived as follows, using `lo` and `hi` for the halves of a full 128-bit product and arithmetic modulo `2^64` unless stated otherwise. For a message of length `n`, first word `w`, and zero-extended tail `t`, the source gives

```text
ell = lo(((n + w) mod 2^64) * P2)
B   = n + hi(((n + w) mod 2^64) * P2)
u   = seed XOR ell
v   = u + t
(H,L) = full_product(v, P1)
(a,b) = (u + H, B XOR L)
P1 = 0x82434fe90edcef39
P2 = 0xd4f06db99d67be4b
```

The independently computed public constants are:

| Constant | m1 | m2 |
|---|---|---|
| `ell` | `0x5426a9f0b16cd070` | `0x542429e0b16cd074` |
| `B` | `0x004eb9f32fbcc73a` | `0x7fc833a349f9df18` |
| `t` | `0xfff8960d0ad18e15` | `0x0000000000e02303` |

`ell XOR ell2 = 0x0002801000000004`, with set bits 2, 36, 47, and 49. I enumerated all 16 possible signed additive differences `da=u2-u` and both possibilities for each input wrap and low-product carry. Only the following combination can make the first state word equal:

```text
da = +2^2 - 2^36 - 2^47 - 2^49
   = -703756161253372
D  = (da + t2 - t) mod 2^64 = 0x0004e9e2f60e94f2
D * P1 = 0x0002800ffffffffc_5285762fa24517e2
hi(D * P1) = -da
required low-product carry = 0
required input wrap = 0
```

Let `d=lo(D*P1)` and `M=B XOR B2=0x7f868a5066451822`. Equality of the second state word requires `L+d = L XOR M`. Since `(L XOR M)-L = M-2*(L & M)`, this becomes exactly

```text
(L & 0x7f868a5066451822) == 0x16808a1062000020
```

The mask has 26 set bits. Because `P1` is odd, multiplication by it modulo `2^64` is bijective; XOR with `ell` and addition of `t` are also bijections. Thus the class has exactly `2^38` seeds and density **`2^-26`**. Its direct sampler is

```text
R = next_splitmix64()
L = (R & ~M) | 0x16808a1062000020
P1_inverse = 0xb605a92816b14f09
seed = ((L * P1_inverse - t) mod 2^64) XOR ell
```

Membership in this class alone is not sufficient to collide. For the unique state-merging trail, the four sign bits must also agree: `(u & 0x0002801000000004) == 0x0002801000000000`. Additionally, `v < 0xfffb161d09f16b0e` prevents the input wrap. The class itself guarantees no low-product carry: even its largest `L`, plus `d`, is only `0xe97f75ef9dffffdf`. In the class sample, 262,174 seeds had the required four signs and equal second state words; 30 failed to merge the first state word because of the input wrap, leaving 262,144 actual collisions. The measured conditional rate is exactly `1/16` in this sample; I do not assume the four signs are mathematically independent of the class, or claim an exact population probability from this observation.

The derivation characterizes all **pre-finalization state-equality** collisions for this pair. It does not prove that unrelated collisions of the 128-to-64-bit finalizer never occur outside the class. Therefore the density-weighted measured rate is the confirmed contribution of this mechanism, and an estimate of the claimed overall rate, rather than an exhaustive count across all `2^64` seeds.

**Exact reproduction commands**, from this directory, with Apple clang 17.0.0 on arm64 Darwin:

```sh
clang++ --version
shasum -a 256 ref/*
clang++ -O3 -std=c++17 -Wall -Wextra -Wpedantic verify.cpp -o verify
./verify validate > validation.txt
./verify uniform 67108864 0x243f6a8885a308d3 > uniform.txt
./verify class 4194304 0x13198a2e03707344 > class.txt
python3 analyze.py > analysis.txt
shasum -a 256 -c ref.sha256
```

No network access was used. All created files are in this directory, and the reference files were left unchanged. Their SHA-256 digests, checked before and after the work, are:

```text
fb0ad09280aae231050527b46d2f807381b62c9d49b36e0b046c117cefb33c4d  ref/Hashinfo.cpp
521c1429eb1a13fd2b24c1217a2bde6ba60e9fc5e301d1e2719a6be044bd5395  ref/Mathmult.h
90038a6bba53e94f9dc73c48e40e8c25482fffb719a226be66c4093b69a932b5  ref/t1ha.cpp
```

The validated hash, matching example, exact class-density derivation, and measured conditional rate support the claimed weak-seed collision at approximately `2^-30` over uniform seeds. The uniform run's zero count is expected at this sample size. Exact population-rate and outside-class exclusivity claims remain beyond what the sampling establishes.

VERDICT: CONFIRMED
