# Sources

| file | pinned source | sha256 |
|---|---|---|
| `upstream/xxhash.h` (needed by `loop_xxmur.c`, not copied) | https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h (tag v0.8.3, commit e626a72) | `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b` |
| MurmurHash2_32, MurmurHash2_64 (= MurmurHash64A), MurmurHash2A bodies in `murmurhash2_witness.c`, `murmurhash2a_witness.c`, `loop_xxmur.c` | https://github.com/fwojcik/smhasher3/blob/7ad8939d/hashes/murmurhash2.cpp | `7a70fffa9709105ed5dd03e69d829575b97d8727c5d28e12c7faf55f9a29c278` |
| original MurmurHash2 / MurmurHash64A / MurmurHash2A | https://github.com/aappleby/smhasher/blob/07bb4de10a63e8cc2e1724865454eba635742383/src/MurmurHash2.cpp | `8cdec1fed73de41e4ee91ab78fe5ba76110582263064dcdcc0b770a25c0573f3` |
| Kafka `Utils.murmur2` (seed `0x9747b28c`; copied into `KafkaMurmur2Check.java`, transcribed in `kafka_check.c`) | https://github.com/apache/kafka/blob/38d8adec14d543d1238393b363e0d96caa3948af/clients/src/main/java/org/apache/kafka/common/utils/Utils.java (lines 497-541) | `42a7755e2864417670be9caa1021a4adf657f479e725b99d7f0c7042715bc65e` |
| Kafka default partitioner (`partitionForKey` = `toPositive(murmur2(key)) % numPartitions`) | https://github.com/apache/kafka/blob/38d8adec14d543d1238393b363e0d96caa3948af/clients/src/main/java/org/apache/kafka/clients/producer/internals/BuiltInPartitioner.java (lines 410-415) | `735db63e965fc8639ffce99374d0bd30189cac13343b7cdef2f385a6a8b8a05c` |

Fetch the header before building `loop_xxmur.c`:

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    shasum -a 256 upstream/xxhash.h   # 17973c0d...5e4b

SMHasher3 verification values checked at startup (registry in the files above): XXH-64 `0x8F8224C4`, XXH-32 `0x6FD78385`, MurmurHash2-64 `0x1F0D3804`, MurmurHash2-32 `0x27864C1E`, MurmurHash2a `0x7FBD4396` (little-endian verification values; the xxHash values use the canonical big-endian output bytes, as SMHasher3 does).
