# Upstream sources (not copied)

MuseAir, crate `museair` 0.6.0 on crates.io (MIT OR Apache-2.0), repository
https://github.com/eternal-io/museair (tag `crate-0.6.0`). `src/lib.rs` sha256
123772c6360a31ef29f5d525d87d8f7738c8e8d883b284a6b20dd29fb1d42b1c.

The Rust programs depend on `museair = "=0.6.0"` and (for `museair2_check`) `hashverify = "0.1.0"`
(crates.io checksum 0d3e441d5a766e64ca462ea764bc22a6a04a12983a4bcf11f32f59911e56a49d). The logged runs
used a path dependency on the unpacked 0.6.0 crate with the same `src/lib.rs`. Both programs check the
crate's own `stability_v2` verification values at startup and exit on a mismatch.

`museair2_core.h` is an independent C11 port of `src/lib.rs`; it reproduces all eight
`stability_v2` values (hash, hash_folded, hash128, hash128_folded and the bfast variants).
