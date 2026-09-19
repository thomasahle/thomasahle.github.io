# MuseAir algorithm v2 (crate 0.6.0)

This is a separate package and score from MuseAir v0.3. The selected 32-byte pair has L = 4 and an upstream-confirmed rate 36,060/(1.5·2^32) = 2^-17.447, yielding a sampled cap 19.45 bits (95% interval 19.43–19.46).

```text
M  = 0000000000000000000000000000000000000000000000000000000000000000
M' = 00000000000000404a048402a910048a00000000000000a80000000000000000
key words = 040963434fe5e368
output = ef46cda19c0bbc67
```

Seed 0 does not collide. The scored key model is one uniform 64-bit seed for `museair::hash`. `bfast::hash` collides on the same seeds. The two 128-bit functions use **two** independent uniform seeds and have the same measured rate on a different event set.

For 17–32 bytes the tail enters as `wmul(C4 ^ seed ^ u, C5)`. Flipping tail bits 59/61/63 changes the product by ±21·C5; the product's XOR image matches a fixed head difference when the multiplication carries agree. The selected head XORs are 0x4000000000000000 and 0x8a0410a90284044a. This bounded-search witness is not a proof that no better pair exists.

```sh
cc -O3 -std=c11 museair_v2_verify.c -lm -pthread -o check
taskset -c 24-31 nice -n 10 ./check 24
# The literal-message sampler and at-seed helper are also supplied:
cc -O3 -std=c11 measure.c -lm -pthread -o measure
cc -O3 -std=c11 at_seed.c -o at_seed
cd native
cargo build --release --locked
taskset -c 24-31 nice -n 10 ./target/release/museair_v2_verify 0000000000000000000000000000000000000000000000000000000000000000 00000000000000404a048402a910048a00000000000000a80000000000000000 26 79 8 040963434fe5e368 0
```

The standalone C program uses the independently cross-checked port in `museair2.h` and asserts the selected collision and seed-0 noncollision before sampling. Default `24 1` uses one SplitMix64 stream. The Xeon observed **84, 84, 81, 81 / 2^24** for hash, bfast::hash, hash128, bfast::hash128 respectively ([log](run_selected_xeon.txt)); `20 1` gives `3 3 0 0` ([log](run_check_xeon.txt)).

The native Rust program links directly to pinned crates.io `museair = 0.6.0`, with Cargo.lock supplied. Its independent xoshiro256** streams at `26 79 8` gave **388, 388, 387, 387 / 2^26**, and reproduced both the example and seed-0 noncollision ([native log](native/run_selected_xeon.txt)). The example output is ef46cda19c0bbc67 (bfast: 8d303f6a6db80174). The smaller package checks do not replace the large confirmation samples.

[Search and C-port validation](../../records/witness-searches/muse-v2/README.md); [independent upstream verifier](../../records/witness-searches/muse-v2-verify/README.md). The supplied C port is MIT; upstream crate 0.6.0 is MIT OR Apache-2.0 (per its Cargo.toml); preserve upstream license notices when redistributing the crate. The historical v0.3 every-seed witness does not transfer to v2 (0/2^24). No v0.3 timing is reused for v2.

All page-pass executions were run on the Xeon on 2026-09-19 with `taskset -c 24-31 nice -n 10`. These small reruns validate the package and deterministic counts; the larger independently reproduced records linked below supply the published estimates. Messages are fixed before the key is sampled.
