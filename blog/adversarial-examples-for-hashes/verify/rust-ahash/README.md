# aHash 0.8.12 AES: selected pair E

E has 424 bytes and L = 53. The upstream crate counted 967,503 / 2^32 collisions, about 2^-12.116, for a sampled cap of 17.84 bits (95% interval 17.84–17.85). The earlier selected pair A_2 (56 bytes, L = 7, three S-boxes, 12,627 / 2^33 = 2^-19.376, 22.18 bits) is kept in the program and below as the historical selected pair; A_1 is statistically tied with it and the 64-byte B_0 mirror gives about 22.38 bits.

The model draws four independent uniform `RandomState` words. The byte-slice sequence is `write_usize(len)` followed by `write(bytes)`, equivalent to `hash_one(&[u8])`. A direct `write` call alone is a different input model. The AES backend is required for these results.

E is all zero bytes except ten:

| byte | 24 | 25 | 26 | 27 | 280 | 344 | 345 | 346 | 347 | 384 |
|---|---|---|---|---|---|---|---|---|---|---|
| M  | 7c | 06 | 7e | 7b | 08 | 80 | 08 | 82 | 80 | 03 |
| M' | 81 | 08 | 81 | 80 | 03 | 7d | 06 | 7d | 7b | 08 |

```text
M  = 0000000000000000000000000000000000000000000000007c067e7b000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000080000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000008008828000000000000000000000000000000000000000000000000000000000000000000000000003000000000000000000000000000000000000000000000000000000000000000000000000000000
M' = 00000000000000000000000000000000000000000000000081088180000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000030000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000000007d067d7b00000000000000000000000000000000000000000000000000000000000000000000000008000000000000000000000000000000000000000000000000000000000000000000000000000000
key words = 0e17425e994c2579 4e057e193aec29dc 665cc79160d3c675 3d80ed4ec94cfe49
output = 93ecf09ded4decc5
```

The words above are **internal** keys. Public `RandomState::with_seeds` arguments are `4b3f63b8a19c360e f05118d60e0525b0 a6f0ee26a9af96a8 020438fb7c0bf75e`: arguments XOR PI2 give the internal words. The explicit output is asserted by both programs before sampling.

For messages longer than 64 bytes aHash runs four AES lanes and two additive lanes: the last 64 bytes seed them, then each 64-byte block is folded in with one `aesdec` per lane and one shuffle-and-add per additive lane. E is a two-S-box "lane echo" in lane 1: the tail (byte 384) puts a one-byte difference into the lane when the lanes are seeded, the first loop block (bytes 24–27) cancels it through a single inverse S-box (difference-table entry 4/256; the four bytes are the InvMixColumns column of the S-box output), the fifth block (byte 280) injects a second one-byte difference and the sixth (bytes 344–347) cancels it the same way, and by then the five byte differences the first pair left in the additive lane have been shuffled onto the five bytes of the second, so they cancel additively. Two S-boxes at 4/256 plus small carry terms give 2^-12.12, 0.12 bits under the two-S-box floor. A permissive truncated-trail enumeration with a GF(2^8) feasibility filter finds no one-S-box trail and no two-S-box trail before six iterations; 424 bytes is the shortest message for lane 1 that keeps the tail and the loop blocks from reading a byte in two roles (lanes 0, 2, 3 need 440, 428, 444 and score 17.89–17.92 bits). The pair does **not** collide under SMHasher3's correlated convention `with_seeds(s,s,s,s)` (0 in 2^24): both S-box transitions need independent lane keys, which the real `RandomState` supplies. The three-S-box A_2 trail is described in [HISTORICAL.md](HISTORICAL.md); [search and derivation](../../records/witness-searches/ahash-trail/README.md); [upstream results](../../records/witness-searches/ahash-trail-verify/RESULTS.md).

```sh
# x86-64; use -march=armv8-a+crypto on ARM, or -DAHASH_SOFT_AES for portable AES
cc -O2 -std=c11 -maes ahash_pairs.c -lm -o check
taskset -c 24-31 nice -n 10 ./check 24
# optional final argument selects E, A_2, A_1, B_0, historical A, or fallback B
cd native
RUSTFLAGS='-C target-feature=+aes,+ssse3' cargo build --release --locked
taskset -c 24-31 nice -n 10 ./target/release/ahash_trail_verify pairs_echo.txt 32 2026 8   # E and the historical pairs; pairs_confirm.txt for A_1/A_2/B_0
```

The C program checks SMHasher3 AES 0x3BF4383B and fallback 0x53C9F167 and each explicit collision. Its selected E run observed **3783 / 2^24 = 2^-12.11** under independent words and 0 / 2^24 in the correlated scalar-seed model (see above). Historical A_2 at 2^24: 27 / 25; A_1: 24/28; B_0: 28/22; historical A: 26/11; fallback B: 0/0 ([earlier C run](run_selected_xeon.txt)). The `20` smoke sequence is `217 0 0 2 0 2 1 0 1 2 0 0`.

The native crate run for E is [run_echo_xeon.txt](native/run_echo_xeon.txt): 2^32 keys, 8 threads, 76 s, **967,503 collisions for the 424-byte pair** (cap 17.844, 95% interval 17.841–17.847), with A_2 in the same run at 6,250 / 2^32 = 2^-19.39 as control and the 440/428/444/448-byte lane variants at 17.87–17.92 bits. The earlier native run for A_2, A_1, B_0 and the historical pair is [run_selected_xeon.txt](native/run_selected_xeon.txt). The native program asserts the selected example output before sampling.

`confirm_pairs.txt` line 2 has been relabelled **B_0_STALE56B**: its 56-byte bytes are retained as a negative control. The real 64-byte B_0 is in `native/pairs_confirm.txt`, along with A_1 and A_2. Older native programs and [HISTORICAL.md](HISTORICAL.md) remain historical references. The C implementation is original code; upstream aHash is MIT/Apache-2.0. The crate’s ARM AES path also requires its `nightly-arm-aes` feature.

The E measurements were run on the Xeon on 2026-09-19 (native, 8 threads) and 2026-09-20 (C, M2 Pro); the earlier page-pass executions on the Xeon on 2026-09-19 with `taskset -c 24-31 nice -n 10`. These small reruns validate the package and deterministic counts; the larger independently reproduced records linked below supply the published estimates. Messages are fixed before the key is sampled.
