# Validation and reproduction

From the deliverable directory:

```sh
cc -std=c99 -O3 -mpclmul -D_DEFAULT_SOURCE tests/test_chainhash128.c -o test128
./test128
./test128 vectors > actual.txt
diff tests/vectors-512.txt actual.txt
```

On AArch64 replace `-mpclmul` with `-march=armv8-a+crypto`, or
`-march=native+crypto` for Apple Clang. Define `CHAINHASH128_BLOCK_BYTES=256`
for the other configuration and compare `vectors-256.txt`.

`tests/run_x86.sh` runs hardware, VPCLMUL, schoolbook, 256-byte, portable and
Clang ASan/UBSan configurations. It writes logs to `../evidence`, as used in
the remote work layout. `python3 tests/oracle.py` independently regenerates
known-answer vectors and checks the Rabin irreducibility certificate;
`python3 tests/algebra.py` checks the two inverse coefficient maps and the
quintic circuit against Horner evaluation. Python scripts use only the
standard library.

Each C run compares the selected backend to bit-serial arithmetic on 12,000
randomly keyed inputs: every length 0..4096 once, then 7,903 pseudorandom
lengths in that range. Input bytes and alignment vary. There are 32 further
long inputs (28 up to 135168 bytes and four from 1 MiB through 2048569 bytes),
4,104 zero/ones-pattern cases, model-A schedules, NULL/empty input, and
1,026 tails ending immediately at a protected page boundary. Arithmetic tests
cover 20,000 random raw/reduced products and arbitrary 256-bit reductions,
and all 16,384 monomial products. The expected-vector byte strings and key
material are described in `vectors.json`. `chainhash128_selftest()` is a
small embedded smoke test, callable without an external test runner.

Backend results must match within a block-size configuration; **256-byte
and 512-byte variants are distinct functions**. The same deterministic
random stream and vector outputs are used on both hosts. The RNG here is a
reproducibility tool, not an implementation of ideal random-key sampling.

The saved evidence distinguishes the initial GCC sanitizer link failure
(the remote GCC runtime libraries were missing) from the successful Clang
ASan/UBSan run. It is not a test failure in the hash.
