# Sources

| file | origin | pin |
|---|---|---|
| `ahash_mc.c` | transcription of SMHasher3 `hashes/rust-ahash.cpp` (aHash AES path: from_random_state, hash_in, add_data, finish; constants PI2 and SHUFFLE_MASK), which follows aHash `src/aes_hash.rs`; AES rounds implemented byte-wise and cross-checked against AES-NI / ARMv8-AES | SMHasher3 verification value rust_ahash 0x3BF4383B (checked at start-up) |
| `native-crate-check/` | uses the published crate `ahash = "=0.8.12"` from crates.io through its public API (`RandomState::with_seeds`, `hash_one(&[u8])`) | `Cargo.lock` |
