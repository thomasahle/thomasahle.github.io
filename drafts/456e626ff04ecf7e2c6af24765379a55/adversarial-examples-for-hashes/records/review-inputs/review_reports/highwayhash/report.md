# Independent verification of HighwayHash-64/128/256

The supplied pair **does collide in the complete 1,024-bit internal state**, and the explicit example is correct. Independent sampling found **3 joint collisions in 50,331,648 class keys** and **0 in 16,777,216 uniform keys**. These observations are consistent with the claimed conditional rate. They do not establish the exact fraction or the literal total-probability lower bound, so the verdict on the full claim is **INCONCLUSIVE**, with the collision construction itself confirmed.

All work was local, without network access. The reference files were read and left unchanged; their SHA-256 checks passed afterward. To resolve the conflicting instructions to create `ref/c/` and not modify `ref/`, the unmodified C sources and header were copied into `oracle/` and `oracle/c/`. No SMHasher3 framework was compiled. Sampling used one thread, with a fixed, preselected total of **67,108,864 = 2^26 keys**. The requested 2^28 class sample would violate the overarching limit and was not run.

**Implementation and validation.** `verify.cpp` independently implements reset, packet updates, little-endian loading, remainder handling, permutation, and all three finalizers. Its zipper operation uses explicit byte permutations rather than copying the reference mask expressions. It compares all 16 internal state words before finalization and computes both messages' full 64-, 128-, and 256-bit outputs for every sampled key; it does not infer output equality merely from a state match. The common finalization trajectory is evaluated through rounds 4, 6, and 10. Every observed collision was additionally checked against the compiled portable-C oracle, including its full state.

Validation passed:

- All **66 published HighwayHash64 vectors** in `ref/highwayhash_test.c`: lengths 0–64 with the published key, plus the additional 33-byte case with key `{1,2,3,4}`. The separately compiled original C test also printed `Test success`.
- **4,096 additional deterministic cases** matched the C oracle for the full state and all three output widths. These cover every length 0–1024 and additional lengths through 8192, with random keys and contents. The validation SplitMix64 initial state was `0x6a09e667f3bcc909`.
- Both example messages matched the oracle in every mode and in the pre-finalization state.

The published-vector alternative in the instructions was used. `ref/Hashinfo.cpp::_ComputedVerifyImpl` was read, but no SMHasher3 verification constant is claimed: the supplied files contain neither its HighwayHash scalar-seed-to-four-word-key mapping nor an expected verification constant. The supplied C test explicitly has no published 128/256 vectors; those modes were validated against the portable-C oracle.

**Inputs and seed distribution.** Both literal hex strings in `PROMPT.md` decode to exactly 96 bytes. As little-endian 64-bit packet lanes, the first lane of each 32-byte packet is:

| Message | Packet 1 | Packet 2 | Packet 3 |
|---|---|---|---|
| m1 | `24192a2a01b331d1` | `24192a2ab4b332d1` | `0000000000000000` |
| m2 | `24192a2a01b332d1` | `24192a2ab3b330d1` | `0000000000000100` |

Every other packet lane is zero. No message adjustment or key search preceded sampling. SplitMix64 uses increment `0x9e3779b97f4a7c15`, multipliers `0xbf58476d1ce4e5b9` and `0x94d049bb133111eb`, and shifts 30, 27, 31. Each key consumes four consecutive outputs. The class run overwrites only the high half of word zero with `0xdbe6d5d5`, leaving the other 224 bits pseudorandom. The uniform run keeps all four generated words. This is reproducible PRNG sampling of the requested distributions, not an enumeration or a physical-randomness experiment.

The class follows directly from reset: `hi32(v0[0]) = 0xdbe6d5d5 XOR hi32(key[0])`. Setting this value to zero fixes exactly 32 key bits, so its density among uniformly random 256-bit keys is **2^-32**.

| Distribution | SplitMix64 initial state | N | Full-state collisions | Full 64 | Full 128 | Full 256 | All three jointly |
|---|---|---:|---:|---:|---:|---:|---:|
| Stated class | `243f6a8885a308d3` | 50,331,648 | 3 | 3 | 3 | 3 | 3 |
| Uniform 256-bit keys | `13198a2e03707344` | 16,777,216 | 0 | 0 | 0 | 0 | 0 |

The measured conditional rate is `3 / 50331648 = 5.9604644775390625e-8`, with **log2(rate) = -24**. Multiplication by the class density gives an estimated class contribution to the total rate of `1.3877787807814457e-17`, with **log2 = -56**. This contribution estimate is not a proved lower bound on the true total probability. The uniform sample's empirical rate is zero, **log2(rate) = -infinity**; no sampled uniform key belonged to the class.

The three class hits occurred at zero-based indices below. These are additional examples, distinct from the supplied key:

```text
15275631: dbe6d5d560d6ad90 08180258c36408fd 25a090a37e3800df fb6d1bf2d15911e5
38843416: dbe6d5d5d433964f 0bee8b222787b70f 5e1b71b6f7055580 437e5818d680d42a
39321207: dbe6d5d5699b3db1 46dab1e5809c2c61 ed604fa7624e47e6 9b2b4f26498b6f15
```

**Explicit example.** Key words are `dbe6d5d58afad71e a0142b42de197939 5bd2b2861106bd66 b6c304527caad524`. The independent implementation and C oracle both produced the following. Multiword outputs are printed as numeric 64-bit words in returned array order; each word is serialized little-endian when bytes are wanted.

| Mode | m1 output | m2 output |
|---|---|---|
| 64 | `f5eba26391be727f` | `f5eba26391be727f` |
| 128 | `46b567c5d05f08f3 7b1fa476689ef04e` | `46b567c5d05f08f3 7b1fa476689ef04e` |
| 256 | `38ac5302dbcd9e23 2fafb0b99164d0f3 ce0f093fd9d7df5f f198aa4797f74288` | `38ac5302dbcd9e23 2fafb0b99164d0f3 ce0f093fd9d7df5f f198aa4797f74288` |

**Why the collision happens, from the source.** Reset initializes `mul0[0]` to `0xdbe6d5d5fe4cce2f` and XORs the key into `v0` (`ref/highwayhash.c`, lines 16–32). The class makes `hi32(v0[0])` zero, so the first update's product `lo32(v1[0]) * hi32(v0[0])` cannot record the message difference in `mul0[0]` (lines 47–58). Also, m1's first lane is exactly minus `mul0[0]` modulo 2^64. With the fixed low half of the initial `v1[0]`, the first-packet difference of `+0x100` passes through the zipper additions as only `delta v0[0] = 2^40` and `delta v1[0] = 0x01000100`. The second packet's lane difference is `-0x01000200`, making its pre-multiplication `v1[0]` difference `-0x100`. Writing `H = hi32(v0[0])` before that update and `X = lo32(v1[0])` after adding its packet and `mul0`, the relevant products become `X*H` and `(X-256)*(H+256)`. When `X = H+256` and the byte carries permit this differential path, these products agree; the unchanged second multiplication and zipper additions cancel the `v0` difference, leaving only `delta v1[0] = -256`. The third packet adds precisely `+256` to m2's first lane, making all 16 words identical before finalization. The deterministic 4-, 6-, and 10-round finalizers therefore collide simultaneously. This explains a concrete collision path; it does not count all keys satisfying its carry conditions or rule out every other path.

The example trace confirms that path, with all differences defined as m2 minus m1 modulo 2^64:

```text
After packet 1: v0[0] = +0000010000000000
                v1[0] = +0000000001000100
Before packet 2 multiplication:
    H = 8295becf, X = 8295bfcf, X-H = 100
    X*H = (X-256)*(H+256) = 429c6de47cecba61
After packet 2: v1[0] = ffffffffffffff00  (-256)
After packet 3: no differences in any state word
```

**Statistical limits and the numerical claim.** The claimed conditional fraction is `235*239/2^40 = 5.1081769925076514e-8 = 2^-24.222616245454`, predicting **2.57103 hits** in the class sample; observing three is consistent. The two-sided 95% Clopper–Pearson interval from three hits is `[1.22919108745e-8, 1.74190055677e-7]`, or log2 interval `[-26.277715547624, -22.452834399832]`. Thus this experiment cannot independently establish the exact numerator `235*239`. Under the usual independent-trial model, zero uniform hits gives only a one-sided 95% upper bound of `1.78559542066e-7`, far too weak to measure a probability near 2^-56 or prove exclusion of other seed classes. The claimed class contribution predicts only `1.99538e-10` hits in that uniform sample.

There is also a literal rounding issue: multiplying the claimed exact conditional fraction by 2^-32 yields `235*239/2^72 = 1.1893401370634445e-17 = 2^-56.222616245454`, which is **less than**, not at least, `2^-56.2 = 1.2081315993396936e-17`. Writing the total contribution as approximately 2^-56.2 is reasonable; the strict lower bound does not follow from that fraction. Additional collision paths might raise the total, but this run does not establish that.

**Exact build and experiment commands**, run from this directory with Apple clang 17.0.0 on arm64 Darwin:

```sh
mkdir -p oracle/c build
cp ref/highwayhash.h oracle/c/highwayhash.h
cp ref/highwayhash.c oracle/highwayhash.c
cp ref/highwayhash_test.c oracle/highwayhash_test.c
shasum -a 256 ref/* > build/ref-before.sha256
python3 prepare.py
cc -O3 -std=c11 -Ioracle -c oracle_wrapper.c -o build/oracle.o
cc -O3 -std=c11 -Ioracle oracle/highwayhash.c oracle/highwayhash_test.c -o build/oracle_test
c++ -O3 -std=c++17 -Wall -Wextra verify.cpp build/oracle.o -o build/verify
./build/oracle_test
./build/verify validate > build/validation.log
./build/verify sample class 50331648 > build/class.log
./build/verify sample uniform 16777216 > build/uniform.log
python3 statistics.py > build/statistics.log
shasum -a 256 -c build/ref-before.sha256
```

The final validation log was regenerated after adding the explanatory multiplication trace; the sampling algorithm was unchanged and the samples were not rerun. Sampling took approximately 24.0 seconds for the class and 7.5 seconds for uniform keys. Raw evidence is in `build/validation.log`, `build/class.log`, `build/uniform.log`, and `build/statistics.log`; the independent implementation is `verify.cpp` and the C adapter is `oracle_wrapper.c`.

The example and full-state collision construction are confirmed, and the observed conditional rate supports the claimed order of magnitude. The exact probability and strict total lower bound remain unverified, so the full claim cannot be marked confirmed or refuted from this capped experiment.

VERDICT: INCONCLUSIVE
