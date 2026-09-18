Independent verification of komihash v5.34, using the unmodified vendored `ref/komihash.h` as the reference. The claimed collision rate and example are confirmed experimentally. The collision persists after the tested 40-byte suffix and disappears at the resolution of the 128-byte experiment; zero observations do not prove an exactly zero probability.

I read the vendored header, `ref/komihash.cpp`, and the verification procedure in `ref/Hashinfo.cpp`. [independent.cpp](independent.cpp) implements the hash separately, with explicit little-endian loads, unsigned 128-bit products, array-based lanes, and a bytewise tail with its terminating one bit. It uses no reference functions or macros. [oracle.cpp](oracle.cpp) alone includes the reference header, in a separate translation unit without link-time optimization. The SMHasher3 framework was not compiled. No network access was used, and all created files are in this directory.

Validation succeeded:

- The exact SMHasher3 procedure hashes keys of lengths 0–255 with seeds `256-i`, writes all 256 results little-endian into 2,048 bytes, and hashes those bytes with seed zero. Both implementations produced `0x694e7ee28157ff6d`, whose first four little-endian bytes give **`0x8157FF6D`**. Thus this v5.34 header also reproduces the value registered for v5.27.
- Both implementations produced **`0x05ad960802903a9d`** for the 32 bytes of `"This is a 32-byte testing string"` with seed zero. The terminating NUL is excluded.
- There were zero mismatches in 24,582 general oracle comparisons covering every length 0–4,096, offsets 0–7, five ordinary seeds per length, and an independently sampled 128-bit preseed per length.
- A further 4,096-seed audit tested both messages against the oracle at every length 64–128: 532,480 comparisons, zero mismatches. Every length 64–127 had the same 3,729 colliding seeds; length 128 had zero.
- During each large experiment, both outputs at all three lengths were checked against the oracle for the first 65,536 sampled states: 393,216 comparisons per seed model, zero mismatches. The carry predicate below was checked against actual folded states for those states and against full-output collision outcomes for every sample at lengths 64 and 104, with zero disagreements.
- The validation suite also passed under UndefinedBehaviorSanitizer, with no diagnostics. An additional AddressSanitizer+UndefinedBehaviorSanitizer run produced no output and was interrupted; no AddressSanitizer success is claimed.

The messages are exactly these hexadecimal byte strings, each 64 bytes:

```text
A = 0000000000000000447370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444
B = 0100000000000000457370032e8a191311111111111111112222222222222222f0f9dda4c1c0a15e9cf534900ea6f5e033333333333333334444444444444444
```

For length 104, I appended the common 40-byte suffix `00 01 02 ... 27` (hex). For length 128, I appended the common 64-byte suffix `00 01 02 ... 3f` to the original messages. The boundary audit used prefixes of that same suffix. All words are interpreted little-endian.

Sampling used one thread and my own xoshiro256** implementation, initialized by four consecutive SplitMix64 outputs from the printed constants:

| Purpose | RNG initialization constant |
|---|---|
| Uniform 64-bit `UseSeed` | `0x4b4f4d495f563534` |
| Uniform 128-bit preseed `(S1,S5)` | `0x5052455345454431` |
| Validation and boundary audit | `0x56414c4944415445` |

The ordinary experiment used each full 64-bit RNG output directly as `UseSeed`. The preseed experiment used two successive outputs directly as `(S1,S5)` at entry to the inner hash, without applying the 64-bit preseed mapping again. This is a separate seed model, not the distribution induced by uniformly sampling `UseSeed`. Neither model filters seeds or conditions on a weak class: the sampled class has density **1** in its respective seed space, so its measured rate is already its total rate. Each experiment used **N = 16,777,216 = 2²⁴** sampled seeds/states, reusing each state across all three lengths. The two experiments total 2²⁵ sampled states; including validation, the work remains below the 2²⁶ limit. Counts below compare both complete 64-bit hash outputs, not internal predicates or truncated hashes.

| Seed model | Bytes | N | Full-output collisions | Measured rate | log₂(rate) |
|---|---:|---:|---:|---:|---:|
| Uniform 64-bit `UseSeed` | 64 | 16,777,216 | 15,277,062 | 0.910583853721619 | −0.135136216863736 |
| Uniform 64-bit `UseSeed` | 104 | 16,777,216 | 15,277,062 | 0.910583853721619 | −0.135136216863736 |
| Uniform 64-bit `UseSeed` | 128 | 16,777,216 | 0 | 0 | −∞ (empirical) |
| Uniform 128-bit preseed | 64 | 16,777,216 | 14,681,414 | 0.875080466270447 | −0.192512411711726 |
| Uniform 128-bit preseed | 104 | 16,777,216 | 14,681,414 | 0.875080466270447 | −0.192512411711726 |
| Uniform 128-bit preseed | 128 | 16,777,216 | 0 | 0 | −∞ (empirical) |

The 95% Wilson intervals are `[0.910447220891436, 0.910720298530070]` for the ordinary-seed rate and `[0.874922172793486, 0.875238587984007]` for the preseed rate. These contain the claimed approximately 0.910588 and 0.875, respectively. For each 128-byte experiment, the one-sided 95% binomial upper bound from zero observations is `1 - 0.05^(1/N) = 1.78559542065602e-7`, or log₂ probability below approximately **−22.4170914318676**. These are sampling intervals under the usual pseudorandom-sampling model; the reused seeds across lengths are not independent replications. I did not rerun the claimant's 2³²-seed experiment or claim its exact collision count.

For the explicit seed **`0x27d1f77dc2a01269`**, the independent implementation and reference oracle agree on every value:

| Bytes | Hash(A) | Hash(B) |
|---|---|---|
| 64 | `0x113c6b88bc913857` | `0x113c6b88bc913857` |
| 104 | `0x9b8ea36483af6041` | `0x9b8ea36483af6041` |
| 128 | `0x3ce5cbb5234da7a2` | `0x466ff6a205c54a12` |

Why the collision happens: the source initializes `S2=S1 xor IV2` and `S6=S5 xor IV6` (header lines 930–935), then multiplies message-masked lanes and adds the product's high half to each high accumulator (lines 648–675 and 727–752). Write `x=S1`, `a=S5`, `c=0x5ea1c0c1a4ddf9f0`, `d=IV6=0xbe5466cf34e90c6c`, and `y=a xor c`, where `S1,S5` are the post-preseeding state. The chosen words make the first two lane products exactly the same: `x*y` for A and `(x xor 1)*y` for B. Their identical low halves cancel in the final XOR fold (lines 939–940); both remaining folded words depend on their high half `q` only through `F(q)=(a+q) xor ((a xor d)+q)`, with all additions modulo 2⁶⁴. Flipping bit zero of `x` changes `q` by zero or one in magnitude. Since `d` has two trailing zero bits and bit 2 set, `F` changes on an increment only when `(a+q)&3 == 3`, and on a decrement only when `(a+q)&3 == 0`. Otherwise the two folded states coincide exactly. The sampled high halves were already equal for 10,776,774 ordinary seeds and 8,390,330 uniform preseeds; the carry cancellation added another 4,500,288 and 6,291,084 collisions, respectively. This also explains why the biased ordinary preseed distribution gives a higher rate than a uniform 128-bit state. At every length 64–127 the fold occurs immediately after this first block, so equal folded states followed by any identical suffix necessarily retain equality. At 128 bytes the second 64-byte block instead processes the still-distinct individual lanes before folding, removing that guarantee and the observed high-probability trail.

Exact successful build/run commands, executed from this directory with Apple clang 17.0.0 on arm64 Darwin:

```bash
clang++ -std=c++17 -O3 -Wall -Wextra -Wconversion -Wshadow independent.cpp oracle.cpp verify.cpp -o verify
./verify --validate-only > validation.log
./verify --samples 16777216 > results.log
clang++ -std=c++17 -O1 -g -fsanitize=undefined -fno-omit-frame-pointer -Wall -Wextra -Wconversion -Wshadow independent.cpp oracle.cpp verify.cpp -o verify_ubsan
./verify_ubsan --validate-only > ubsan.log
shasum -a 256 ref/* independent.cpp independent.hpp oracle.cpp oracle.hpp verify.cpp
```

The additional sanitizer attempt was the following command; its execution was interrupted after producing no output:

```bash
clang++ -std=c++17 -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer -Wall -Wextra -Wconversion -Wshadow independent.cpp oracle.cpp verify.cpp -o verify_sanitize && ./verify_sanitize --validate-only > sanitizer.log
```

Full sampling output is in [results.log](results.log); validation output is in [validation.log](validation.log) and [ubsan.log](ubsan.log). SHA-256 digests of all reference files matched before and after the work:

```text
fb0ad09280aae231050527b46d2f807381b62c9d49b36e0b046c117cefb33c4d  ref/Hashinfo.cpp
f39219ae314dae977936ffa666b679a03a281d2ff6f0478dded3dfb9a5be829e  ref/komihash.cpp
ca1a1b40a24ee97d48cca1ac92617d105838483bf71198a6aa3a431ff165e768  ref/komihash.h
```

The verdict confirms the claimed approximate rates, explicit example, and observed suffix-length behavior. It does not assert an exact collision probability of zero at 128 bytes: only the bound above and the loss of the first-block cancellation guarantee were established.

VERDICT: CONFIRMED
