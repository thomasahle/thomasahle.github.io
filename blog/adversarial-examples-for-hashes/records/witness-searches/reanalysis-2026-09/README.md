# Re-analysis of 28 September 2026: programs and logs

Each folder reproduces one new or changed result in the post's [changelog](../../../index.html#changelog). Every program checks the implementation it tests (an SMHasher3 verification value, upstream test vectors or a linked upstream build) before it counts collisions. Upstream sources are pinned by URL and sha256 in each folder's SOURCES.md (and in [SOURCES.md](SOURCES.md) for the key-free families). `<xeon-host>` and `<scratch>` in logs stand for the compute host and working directory.

| folder | result |
|---|---|
| [a5hash-128](a5hash-128/README.md) | a5hash-128: an 18-byte every-seed family, and a 2^384 long-input family |
| [abseil](abseil/README.md) | absl::Hash (abseil-cpp 73d2688 = LTS 20260817.0): every-seed families by build |
| [ahash](ahash/README.md) | aHash 0.8.12 (AES path): lane-tied echoes, few-way multicollisions |
| [city-farm](city-farm/README.md) | CityHash64 v1.1.1 and FarmHash64 NA: a 16-byte every-seed family, and a long-input differential |
| [gxhash](gxhash/README.md) | gxhash 3.5.0: the shortest every-seed flooding family (18 bytes), and any-chunk replacement |
| [halftime24](halftime24/README.md) | HalftimeHash24 (advanced 24-byte API): which entry point, and one key condition for every leaf |
| [komihash](komihash/README.md) | komihash 5.34: an every-seed pair at 73 bytes, and a 2^17-member family |
| [murmurhash2](murmurhash2/README.md) | MurmurHash2 (32-bit) and MurmurHash2A: key-free pairs and 2^16-way sets |
| [murmurhash3](murmurhash3/README.md) | MurmurHash3_x64_128: a 144-byte every-seed family (four choices per 16-byte block) |
| [murmurhash64a](murmurhash64a/README.md) | MurmurHash64A: key-free pairs and 2^16-way sets |
| [museair-v2](museair-v2/README.md) | MuseAir v2 (crate museair 0.6.0): an 80-byte pair that collides for every seed |
| [museair](museair/README.md) | MuseAir v0.3: an 18-byte every-seed family |
| [mx3-mum-mir](mx3-mum-mir/README.md) | mx3 v3, MUM and mir: 10-byte every-seed families and key-free pairs at every length |
| [nmhash32](nmhash32/README.md) | nmhash32 v2: lane-tied trail T2, a fixed set that floods one bucket for every seed |
| [poly1305](poly1305/README.md) | Poly1305: the L = 1 score is exactly 104 bits; the whole score lies in [103.78, 104] |
| [polymur](polymur/README.md) | PolymurHash 2.0: an attained 2^-53.90 pair under the shipped seeding |
| [t1ha2](t1ha2/README.md) | t1ha2_atonce (v2.1.4): a top-bit trail through the 32-byte loop |
| [umash128](umash128/README.md) | UMASH-128 (umash_fprint): an explicit 128-bit collision, and why 83.99 bits is the right bound |
| [xxh3-crosslen](xxh3-crosslen/README.md) | XXH3 v0.8.3: cross-length pairs on the short paths (random-secret model) |
| [xxh3-toggle](xxh3-toggle/README.md) | XXH3 v0.8.3: weak-key multicollision on the long path (> 240 bytes) |
| [xxh32](xxh32/README.md) | XXH32: 8-byte pair at 1 - 2^-17, 2^16-way families, long-input loop differential |
| [xxh64](xxh64/README.md) | XXH64: key-free pair, 2^16-way family, long-input loop differential |
