# xxh3-64-pair: independent verification of the proposed witness (upstream source)

Verifies the proposed 32-byte pair for XXH3-64 (xxHash v0.8.3, `XXH3_64bits_withSeed`,
default secret, uniform 64-bit API seed) against the ORIGINAL upstream source, not the
searcher's port: `git clone --branch v0.8.3 https://github.com/Cyan4973/xxHash` (commit
e626a72bc2321cd320e953a0ccf1584cad60f363; `xxhash.h` sha256
17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b, `xxhash.c`
5c3591fe6e6c86a619eb26760e9520e37a6fd5152882ab5ad93f912e2a855966). `xxhash.c` is compiled
as its own translation unit and linked; the driver (`verify_pair.c`, sha256
511c4b217f1c747b9dce9a1c05bb8632beb6f56a6876a09370457d4f0791cc9d) includes only the public
header, so every hash is a real library call. The driver has its own splitmix64 ->
xoshiro256** sampler (one independent stream per worker, seeded from
splitmix64(salt ^ 0x6a09e667f3bcc908*(worker+1)), N/8 full-width 64-bit seeds each), and at
startup checks XXH_versionNumber()==803 and the 26 XXH3-64 `withSeed` vectors of upstream
`cli/xsum_sanity_check.c` (lengths 0..2367, seeds 0 and PRIME64). Salts are fresh
(0x0be5719ab202609x), unrelated to the searcher's 0x20260919xx000477 family.

Pair (identical to the proposal): M  = 0^16 || 51151210404400000000008204000105,
M' = 0^16 || aeeaedefbfbbffffffffff7dfbfffefa. Checked locally (Python, default secret
from the row's embedded header): K2 = db979083e96dd4de, K3 = 1f67b3b7a4a44072,
D = K2+K3+1 = faff443b8e121551; the NAF of D has positive-digit sum P = 0000444010121551
and negative-digit sum N = 0501000482000000 with P-N = D mod 2^64; bytes 16..23 / 24..31 of
M are little-endian P / N, M' complements exactly bytes 16..31, prefixes equal;
P-K2 = 2468b3bc26a44073 is the stated deterministic witness seed.

## Results (Xeon 8375C, `nice -n 10 taskset -c 24-31`, 8 threads, ~7 s wall per 2^30)

Every run: witness seed 2468b3bc26a44073 -> H(M)=H(M')=dd686b61e2f6dc69 PASS; seed 0 does
not collide (247112dc64beb5e3 vs 63c276ba3b34e28e); all 26 sanity vectors PASS.

| run | hits / 2^30 | log2 eps | 95% Clopper-Pearson | cap bits (L=4) |
|---|---|---|---|---|
| salt 0x0be5719ab2026091 | 755087 | -10.4737 | [-10.4770, -10.4705] | 12.4737 |
| salt 0x0be5719ab2026092 | 754660 | -10.4745 | [-10.4778, -10.4713] | 12.4745 |
| salt 0x0be5719ab2026093 | 754334 | -10.4752 | [-10.4784, -10.4719] | 12.4752 |
| pooled | 2264081 / 3*2^30 | -10.4745 | [-10.4763, -10.4726] | 12.4745 [12.4726, 12.4763] |
| smoke 2^20 salt 0x7e51 | 756 | -10.44 | | |

Proposal claimed pooled 2^-10.4746 [2^-10.4764, 2^-10.4727], cap 12.47: reproduced.

Controls through the same program and sampler (same protocol): row pair
8912a3da...||0^16 vs 76ed5c25...||0^16: 490 / 2^30 = 2^-21.06 [-21.19, -20.94], witness
c8eae1baae13330b -> 00ffe25bba202dfb PASS (row: 483-535). Memo pair (K0,~K1)||0^16:
29507 / 2^30 = 2^-15.15 [-15.17, -15.13], seed 0 collides at ab88e1e38f85bdca (searcher:
29132-29478). Both calibrate the harness against the row and the memo.

Verdict: confirmed. bits = log2(4 / 2^-10.4745) = 12.47 (L = 4 words); replaces 23.01 on the
row and beats the memo pair's 17.17. Sampled estimate, not a bound.

## Reproduce (Xeon copy in <xeon-work>/witness/xxh3-64-pair-verify/)

    git clone --depth 1 --branch v0.8.3 https://github.com/Cyan4973/xxHash.git
    gcc -O2 -std=c11 -Wall -c xxHash/xxhash.c -o xxhash.o
    gcc -O2 -std=c11 -Wall -IxxHash -o verify_pair verify_pair.c xxhash.o -lpthread -lm
    ./verify_pair 30 0x0be5719ab2026091 8 \
      0000000000000000000000000000000051151210404400000000008204000105 \
      00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa 2468b3bc26a44073 dd686b61e2f6dc69
    # -> 755087 (salts ...92 -> 754660, ...93 -> 754334); counts are deterministic given salt and 8 workers
    # controls: see run2.sh; CIs: python3 (scipy beta.ppf), ci_summary.txt

Files: verify_pair.c, run.sh, run2.sh, new_2p30_*.txt/.time, row_2p30.txt, memo_2p30.txt,
smoke_new_2p20.txt, sha256.txt, ci_summary.txt.
