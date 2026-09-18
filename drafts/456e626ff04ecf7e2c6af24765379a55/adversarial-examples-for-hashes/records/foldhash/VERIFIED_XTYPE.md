# Cross-key-type every-instance collisions in foldhash 0.2.0 — verified 2026-09-18 with the real crate

Program: Xeon ~/agents/foldhash-xtype (Rust, foldhash = "=0.2.0", foldhash::fast::RandomState::default() per trial,
std Hash impls, 1000 independent RandomState instances).  Results (equal hashes / trials):
- 1u8 == 1u16 == 1u32 == 1u64 == 1u128: 1000/1000
- (0x1122334455667788u64, 0x99aabbccddeeff00u64) == the u128 with the same little-endian bytes: 1000/1000
- (7u32, 9u32) == 0x0000000900000007u64: 1000/1000
- a 255-byte &str == the same 255 bytes as &[u8]: 1000/1000  (str: write(bytes)+write_u8(0xff); [u8]: write_usize(255)+write(bytes);
  the sponge holds 0xff in both cases and finish() ignores sponge_len)
- SharedSeed::from_u64(u64::MAX) "absorbing" claim: 0/1000 — NOT reproduced; discard.
Cause: fast::FoldHasher::finish() folds the 128-bit sponge value but not its bit-length (sponge_len), so integer writes
of different widths with the same value, and a str terminator vs a length prefix with the same numeric value, produce
identical write-stream states.  These are every-instance collisions ACROSS key types (values of different Rust types
fed to the same Hasher); within a single HashMap<K> with a fixed key type the byte-slice path has no every-instance
pair (lens proof, unverified).  Fix candidate: XOR sponge_len into the second operand of the final fold.
