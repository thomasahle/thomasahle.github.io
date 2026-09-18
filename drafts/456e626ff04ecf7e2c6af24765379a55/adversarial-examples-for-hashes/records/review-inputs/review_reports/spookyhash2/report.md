The collision mechanism and a rate consistent with 1/2 are independently verified. The example seed is correct. The stronger wording **“exactly half of all seeds”** is not established by this experiment or by the derived cancellation condition, so the verdict on that literal claim is inconclusive.

Implementation and validation: `verify.cpp` is a standalone C++17 implementation written after reading `ref/spookyhash.cpp` and `_ComputedVerifyImpl` in `ref/Hashinfo.cpp`. It implements both the short and long V2 paths, explicit little-endian decoding/encoding, and unsigned 64-bit arithmetic. It uses cyclic indexed schedules for the mixing operations. No SMHasher framework, reference translation unit, or external hash library was compiled or linked. The two message strings are read directly from `PROMPT.md`; only their hexadecimal bytes are used. Both decode to **286 bytes**.

For each output width, I hashed keys of lengths 0 through 255, with bytes `0,1,...,length-1` and `h1=h2=256-length`, concatenated the corresponding little-endian outputs, hashed the concatenation with `h1=h2=0`, and interpreted the first four output bytes little-endian.

| Verification | Computed | Reference | Result |
|---|---|---|---|
| V2 64-bit, required | `0x972C4BDC` | `0x972C4BDC` | PASS |
| V2 32-bit, additional check | `0xA48BE265` | `0xA48BE265` | PASS |
| V2 128-bit, additional check | `0x893CFCBE` | `0x893CFCBE` | PASS |

The reference wrapper sets both seed words to the same 64-bit seed (`ref/spookyhash.cpp:345`) and returns the first output word for the 64-bit variant. I also tested the underlying function with independently drawn seed words.

The exact build, smoke-test, and main experiment commands were:

```sh
clang++ --version
clang++ -std=c++17 -O3 -Wall -Wextra -Wpedantic verify.cpp -o verify
./verify 10 PROMPT.md > smoke.txt
./verify 24 PROMPT.md > results.txt
cat results.txt
shasum -a 256 ref/spookyhash.cpp ref/Hashinfo.cpp verify.cpp PROMPT.md
```

Compiler: Apple clang 17.0.0, target `arm64-apple-darwin27.0.0`. The program uses **one thread**, no network, and only files in the current directory. The reference files were not modified; their SHA-256 values agreed before and after the experiment:

```text
cfb0f2d68e338c7ce252c24b2c0c5141928950463595a9889cb9719d381915f3  ref/spookyhash.cpp
fb0ad09280aae231050527b46d2f807381b62c9d49b36e0b046c117cefb33c4d  ref/Hashinfo.cpp
d51b9ca816428ef9119f13bf2f52b26ef0433abc4168defa0faae84e84d1f9c5  verify.cpp
fff920070060da6b1c3e6e5f34268d501307ff70f64dbb7fb90a6a1dcd3f4f39  PROMPT.md
```

Sampling uses my implementation of **xoshiro256\*\***, initialized by four successive SplitMix64 outputs. The printed SplitMix64 initial-state constants are `0x243f6a8885a308d3` for equal seeds and `0x13198a2e03707344` for independent seeds. Equal-seed trials use one 64-bit draw as both `h1` and `h2`. Independent-seed trials use two successive draws, one for each seed word. No bits are masked, no seeds are rejected, and neither run is conditioned on a weak-seed class: the sampled domains have density 1. As requested, these are reproducible pseudorandom samples of the uniform seed domains.

Every trial computes **both complete hashes**, including all three final rounds, then compares the entire 64-bit output and separately both 128-bit output words. Internal-state conditions are recorded as diagnostics, never substituted for hashing or used to skip trials.

| Seed mode | N | Full 64-bit collisions | Full 128-bit collisions | Measured probability | log2(probability) |
|---|---:|---:|---:|---:|---:|
| `h1=h2=seed` | 16,777,216 = 2^24 | 8,389,753 | 8,389,753 | 0.500068247318268 | -0.999803093302810 |
| Independent `h1,h2` | 16,777,216 = 2^24 | 8,391,198 | 8,391,198 | 0.500154376029968 | -0.999554633684612 |

These main runs total 2^25 seed assignments and 50,331,648 random 64-bit seed words, below 2^26 even when counting the words separately. The preliminary smoke test used 1,024 assignments per mode from the same stream prefixes, so it adds no distinct sampled assignments and is not pooled into the results. Under the usual independent-sample interpretation of the PRNG draws, approximate 95% Wilson intervals are `[0.499828994, 0.500307501]` and `[0.499915123, 0.500393629]`. The counts differ from their half-rate expectations by 0.559 and 1.265 standard deviations, respectively.

At the requested example seed `h1=h2=0x0000000000000004`, output words are listed in function order `(hash1, hash2)`:

```text
m1 64-bit:  0xe3f63e3f415763ac
m2 64-bit:  0xe3f63e3f415763ac
m1 128-bit: (0xe3f63e3f415763ac, 0x324f320615c4e120)
m2 128-bit: (0xe3f63e3f415763ac, 0x324f320615c4e120)
m1 s11 immediately before the second Mix's final rotate: 0x664818c926047db5
```

As a noncolliding check, seed 0 gives `m1=(0x87672d64efa29429, 0xf42ca1c50731e7f4)` and `m2=(0x18ceaf21f313d59e, 0xdf99196eb42fac66)`. Its relevant pre-rotate value for m1 is `0x80b79d11c98b2e7a`.

Why the collision happens: independently comparing the bytes gives exactly three changes, at zero-based offsets 191 (`0x26 -> 0xa6`), 279 (`0xea -> 0x6a`), and 285 (`0x34 -> 0x14`). The source processes two 96-byte Mix blocks, then adds a padded 94-byte tail whose last two padding bytes are `0x00,0x5e`. At the final step of the second Mix (`ref/spookyhash.cpp:102`), the first change toggles bit 63 of the m1 pre-rotate value `x=s11`. It also toggles bit 63 of `s10` through XOR; the subsequent addition of the unchanged `s0` preserves the modular difference 2^63. Rotation of `s11` by 46 moves the toggled bit to bit 45, giving the m2-minus-m1 arithmetic difference +2^45 when `bit63(x)=0`, or -2^45 when it is 1. V2 End adds the tail directly before mixing (`ref/spookyhash.cpp:142`): the change in tail word 10 contributes -2^63, which cancels the s10 difference modulo 2^64; tail word 11 contributes -2^45. Thus all 12 words entering EndPartial are identical when `bit63(x)=0`, guaranteeing identical final outputs. Otherwise only word 11 differs, by -2^46. This derivation does not assume that the two seed words are equal.

The diagnostic trace checked this full 12-word difference in **every main trial**, with zero errors. In each mode, the number of zero pre-rotate high bits, equal states entering EndPartial, 64-bit collisions, and 128-bit collisions was identical. There were zero sampled output collisions outside the cancellation class and zero misses inside it. The sufficient class therefore has measured density 0.500068247318268 for equal seeds and 0.500154376029968 for independent seeds; conditional collision probability inside it is exactly 1 by the state-equality argument. Its implied unconditional contribution equals its density.

The remaining limitation matters to the literal claim: the internal bit is a nonlinear function of the seeds after mixing a fixed message. Neither its being a single bit nor the invertibility of the full 768-bit mixing state proves balance over the restricted 64-bit equal-seed domain or 128-bit independent-seed domain. No exact balance proof was obtained. Also, unequal internal states do not by themselves exclude accidental equality of the projected 64-bit or 128-bit outputs, so the claimed “iff” for outputs is established empirically here, while the “iff” for states entering EndPartial follows algebraically. The implementation, example, deterministic cancellation trail, and approximately half-rate vulnerability are confirmed; exact probability 1/2 over all seeds remains unresolved.

VERDICT: INCONCLUSIVE
