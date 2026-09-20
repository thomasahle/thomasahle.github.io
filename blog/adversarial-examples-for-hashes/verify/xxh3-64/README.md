# XXH3-64 0.8.3: scored 24-byte pair under a random secret; 32-byte block-1 complement pair with the default secret

Two key models, one program (`xxh3_64_pair_check.c`, xxHash 0.8.3 embedded verbatim):

* **random-secret** (the model the article scores, added 2026-09-19): a fresh uniform 192-byte secret per trial through `XXH3_64bits_withSecret` (1536 hidden bits; the API minimum is 136 bytes). The scored pair W0 has L = 3 and a pooled rate of 527/2^36 = 2^-26.96 (independent verifier against upstream v0.8.3: 260/2^35 through `withSecret`, 267/2^35 with seed and secret both random), cap **28.5 bits**, 95% interval [28.4, 28.7]. Record: `../../records/witness-searches/random-secret/xxh3-64/`.
* **default secret** (the historical experiment, now the caveat for the seeded interfaces): a uniform 64-bit API seed through `XXH3_64bits_withSeed` with the public default secret. The 32-byte NAF pair has L = 4 and a sampled rate of 2,264,081/(3·2^30) = 2^-10.4745, cap 12.47 bits, 95% interval [12.473, 12.476]. The same rate holds, hit for hit, through `XXH3_64bits_withSecretandSeed` and the streaming `reset_withSecretandSeed` for inputs of at most 240 bytes, because at those lengths xxhash.h hashes with the default secret and ignores the custom one; the random-secret mode asserts that equality trial by trial.

```text
W0 (scored, random secret)
M  = 000000000000000000000000000000000000000000000000
M' = ffffffffffffffff00000000000000000000000000000000
192-byte witness secret = c9eb2c4bbc54b9cd...0a9dad3752ee254a   (full hex in the C file)
XXH3_64bits_withSecret output = 24453ecc9793506c for both messages

NAF (default-secret caveat)
M  = 0000000000000000000000000000000051151210404400000000008204000105
M' = 00000000000000000000000000000000aeeaedefbfbbffffffffff7dfbfffefa
key words = 2468b3bc26a44073
output = dd686b61e2f6dc69
```

W0 complements bytes 0..7; at 24 bytes those bytes feed only block 0, so under a uniform secret the pair collides exactly when `fold(x, y) == fold(~x, y)` for the two uniform keyed words of that block (the (M, 0) differential of lo ⊕ hi of a 64x64-bit product). It does not use the secret value. For the NAF pair, seed 0 does **not** collide; the first 16 bytes are zero and the second block consists of the positive/negative digit sums of the non-adjacent form of K2+K3+1 for the default-secret words K2, K3. Complementing it cancels exactly when `fold(A,B) == fold(~A,~B)`; the construction does not transfer to an independently random secret, where the pair is an ordinary block complement at 2^-26.6.

```sh
cc -O3 -std=c11 xxh3_64_pair_check.c -lm -o check
taskset -c 24-31 nice -n 10 ./check 20                 # default secret: NAF pair over seeds, lengths 16/24/32/128
taskset -c 24-31 nice -n 10 ./check random-secret 20   # random secret: W0 through withSecret, plus the controls
taskset -c 24-31 nice -n 10 ./check random-secret 30   # 2^30 keys, about 4 min
```

Default mode, observed: 725 / 2^20 at 32 bytes and the same 725 events at 128 bytes; 0 at 24 bytes. The 16-byte prefixes are identical and are deliberately not sampled. SMHasher3 verification 0x1AAEE62C, recorded witness checks, 32/128 event agreement and folded-product equivalence all pass. [Complete observed output](run_selected_xeon.txt), also saved as [expected output](xxh3_64_32B_check.expected.txt). `make check` reads the `collisions=` counts of this mode: `0 725 725`.

Random-secret mode, observed (`run_2p20_random_secret.txt`, Xeon 2026-09-19): startup asserts the recorded W0 witness through `XXH3_64bits_withSecret`; then, per trial, one seed and 24 secret words are drawn from one xoshiro256** stream (default salt `0x2026091901000477`) and four counts are taken: W0 through `withSecret` **0 / 2^20** (expected 2^-26.96 x 2^20 = 0.008); the NAF pair through `withSeed` **746 / 2^20** (default-secret control); the NAF pair through `withSecretandSeed` with the same seed and a fresh secret **746 / 2^20**, with **0** outputs differing from `withSeed` (the custom secret is ignored at 32 bytes; the program exits non-zero otherwise); W0 through `withSeed` **0 / 2^20** (default-secret control). At 2^30 keys (`run_2p30_random_secret.txt`) the same four counts were **8** (W0 `withSecret`, 2^-27.0, sampled cap 28.58), **754,374** (NAF `withSeed`, 2^-10.475), **754,374** with 0 differing outputs (NAF `withSecretandSeed`) and **7** (W0 `withSeed`); the 8/2^30 sample is consistent with the pooled 2^-26.96.

The standalone source embeds xxHash 0.8.3 with its license notices intact. In default mode two sequential xoshiro256** streams are seeded by SplitMix64; the sampler calls the full hash API. [CHECK_32B.md](CHECK_32B.md) describes the length controls of the default mode.

The old `xxh3_64_verify.c`, `historical_base1143_check.c`, `supplied_rows.json` and [HISTORICAL.md](HISTORICAL.md) retain pair A / base-1143 for historical controls. They do not supply the selected score. [Selected row](selected_row.json) is current.

Random-secret measurement and independent verification: `../../records/witness-searches/random-secret/xxh3-64/` (searcher) and `../../records/witness-searches/random-secret/xxh3-64/verify/` (verifier, with the W0 witness log `verify/logs/w0_secret_2p35.txt`).

All page-pass executions were run on the Xeon on 2026-09-19 with `taskset -c 24-31 nice -n 10`. These small reruns validate the package and deterministic counts; the larger independently reproduced records linked above supply the published estimates. Messages are fixed before the key is sampled.
