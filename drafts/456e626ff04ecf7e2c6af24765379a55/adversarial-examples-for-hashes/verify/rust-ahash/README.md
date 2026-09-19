# aHash 0.8.12 AES: selected pair A_2

A_2 has 56 bytes and L = 7. The upstream crate counted 12,627 / 2^33 collisions, about 2^-19.376, for a sampled cap of 22.18 bits (95% interval 22.16–22.21). A_1 is statistically tied; the 64-byte B_0 mirror has a similar rate but L = 8 gives about 22.38 bits.

The model draws four independent uniform `RandomState` words. The byte-slice sequence is `write_usize(len)` followed by `write(bytes)`, equivalent to `hash_one(&[u8])`. A direct `write` call alone is a different input model. The AES backend is required for these results.

```text
M  = 22313233343536373839263b3c3d3e3f407b42404445464748494a4b4c4d4e4f0df97e025c5d5e5f606162636465666768696a6b6c6d6e6f
M' = 1f313233343536373839343b3c3d3e3f406d422f4445464748494a4b4c4d4e4fff098d055c5d5e5f606162636465666768696a6b6c6d6e6f
key words = ec6baf58f9a18793 b5e2ee792117d0ff 893c786e1e8196a8 a4edbe691f677fb2
output = 21a01e6ddf40c84a
```

The words above are **internal** keys. Public `RandomState::with_seeds` arguments are `a9438ebec17194e4 0bb688b615fedc93 499051d9d7fdc675 9b696bdcaa2076a5`: arguments XOR PI2 give the internal words. The explicit output comes from the [independent upstream log](../../records/witness-searches/ahash-trail-verify/records/confirm_33_seed78.log) and is asserted by the native program.

The three-S-box trail {0,10} → {1,3} → bytes 32–35 must cancel alongside the shuffled addition accumulator. A_2 reduces additive carry loss from about 0.69 to 0.38 bits. [Search and derivation](../../records/witness-searches/ahash-trail/README.md); [upstream results](../../records/witness-searches/ahash-trail-verify/RESULTS.md).

```sh
# x86-64; use -march=armv8-a+crypto on ARM, or -DAHASH_SOFT_AES for portable AES
cc -O2 -std=c11 -maes ahash_pairs.c -lm -o check
taskset -c 24-31 nice -n 10 ./check 24
# optional final argument selects A_2, A_1, B_0, historical A, or fallback B
cd native
RUSTFLAGS='-C target-feature=+aes,+ssse3' cargo build --release --locked
taskset -c 24-31 nice -n 10 ./target/release/ahash_trail_verify pairs_confirm.txt 26 79 8
```

The C program checks SMHasher3 AES 0x3BF4383B and fallback 0x53C9F167 and each explicit collision. Its selected A_2 run observed **27 / 2^24** under independent words, and 25 / 2^24 in the separate correlated scalar-seed model. A_1: 24/28; B_0: 28/22; historical A: 26/11; fallback B: 0/0. [Complete C run](run_selected_xeon.txt). The `20` smoke sequence is `0 2 0 2 1 0 1 2 0 0` ([log](run_check_xeon.txt)).

The native crate run observed **89 / 2^26 for A_2**, 122 for A_1, 96 for real B_0, 91 for the historical pair, and 0 for the stale 56-byte control. [Native log](native/run_selected_xeon.txt). The native program asserts the selected example output before sampling. These smaller samples do not replace the 2^33 estimate.

`confirm_pairs.txt` line 2 has been relabelled **B_0_STALE56B**: its 56-byte bytes are retained as a negative control. The real 64-byte B_0 is in `native/pairs_confirm.txt`, along with A_1 and A_2. Older native programs and [HISTORICAL.md](HISTORICAL.md) remain historical references. The C implementation is original code; upstream aHash is MIT/Apache-2.0. The crate’s ARM AES path also requires its `nightly-arm-aes` feature.

All page-pass executions were run on the Xeon on 2026-09-19 with `taskset -c 24-31 nice -n 10`. These small reruns validate the package and deterministic counts; the larger independently reproduced records linked below supply the published estimates. Messages are fixed before the key is sampled.
