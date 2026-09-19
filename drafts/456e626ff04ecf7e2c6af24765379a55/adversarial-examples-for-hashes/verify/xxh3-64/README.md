# XXH3-64 0.8.3: selected 32-byte block-1 complement pair

The selected pair has L = 4 and a sampled collision rate of 2,264,081/(3·2^30) = 2^-10.4745 over a uniform 64-bit API seed with the public default secret. Its estimated cap is 12.47 bits, with 95% interval [12.473, 12.476]. The central display is rounded to two decimals.

```text
M  = 0000000000000000000000000000000051151210404400000000008204000105
M' = 00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
key words = 2468b3bc26a44073
output = dd686b61e2f6dc69
```

Seed 0 does **not** collide. The first 16 bytes are zero; the second block consists of the positive/negative digit sums of the non-adjacent form of K2+K3+1. Complementing it cancels exactly when `fold(A,B) == fold(~A,~B)`. The unchanged first block and bijective avalanche preserve this equivalence.

```sh
cc -O3 -std=c11 xxh3_64_pair_check.c -lm -o check
taskset -c 24-31 nice -n 10 ./check 20
```

Observed: 725 / 2^20 at 32 bytes and the same 725 events at 128 bytes; 0 at 24 bytes. The 16-byte prefixes are identical and are deliberately not sampled. SMHasher3 verification 0x1AAEE62C, recorded witness checks, 32/128 event agreement and folded-product equivalence all pass. [Complete observed output](run_selected_xeon.txt), also saved as [expected output](xxh3_64_32B_check.expected.txt).

The standalone source embeds xxHash 0.8.3 with its license notices intact. Two sequential xoshiro256** streams are seeded by SplitMix64; default salt is `0x2026091901000477`. The sampler calls the full hash API. [CHECK_32B.md](CHECK_32B.md) describes the length controls.

The old `xxh3_64_verify.c`, `historical_base1143_check.c`, `supplied_rows.json` and [HISTORICAL.md](HISTORICAL.md) retain pair A / base-1143 for historical controls. They do not supply the selected score. [Selected row](selected_row.json) is current.

[Search](../../records/witness-searches/xxh3-64-pair/README.md); [independent upstream verifier](../../records/witness-searches/xxh3-64-pair-verify/README.md).

All page-pass executions were run on the Xeon on 2026-09-19 with `taskset -c 24-31 nice -n 10`. These small reruns validate the package and deterministic counts; the larger independently reproduced records linked below supply the published estimates. Messages are fixed before the key is sampled.
