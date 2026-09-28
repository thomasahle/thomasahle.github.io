# MurmurHash2 (32-bit) and MurmurHash2A: key-free pairs and 2^16-way sets

Key model: `MurmurHash2(key, len, seed)` and `MurmurHash2A(key, len, seed)` with a uniformly random 32-bit seed (SMHasher3 `MurmurHash2-32`, `MurmurHash2a`). Apache Kafka's default producer partitioner uses MurmurHash2 with the fixed seed `0x9747b28c`; the results are key-free, so they hold there too.
Scores use L = number of 8-byte words of the longer message.

## What is checked

`murmurhash2_witness.c` (SMHasher3 `hashes/murmurhash2.cpp` transcription; checks `0x27864C1E` and `0x1F0D3804` first). Each 4-byte block is `h *= m; h ^= mix(w)`; a bit-31 XOR difference survives `* m`, so two consecutive bit-31 flips of `mix(w)` cancel.

- **Pair, 8 bytes (L = 1):**

      M1 = 6d75726d75723221   ("murmur2!")
      M2 = ed53ffbaf550bf6e

  collide for all 2^32 seeds (exhaustive: `4294967296 of 2^32`); under Kafka's seed both give `b626e14b`. Score 0 bits.
- **2^16-way set, 68 bytes (L = 9):** 17 blocks, cube generator as for MurmurHash64A with bit 31; all 65536 distinct; all equal for 4096/4096 random seeds.

`murmurhash2a_witness.c` (checks `0x7FBD4396` first): MurmurHash2A uses the same `mmix` block step, so the same construction works: pair `6d6d6832612d3862` vs `ed4bf57fe10bc5af` collides for all 2^32 seeds (exhaustive), and a 68-byte 2^16-way set collides for 4096/4096 random seeds.

`kafka_check.c` (checks `0x27864C1E` first): a C transcription of Kafka's `Utils.murmur2` equals MurmurHash2 with seed `0x9747b28c` on 2^20 random inputs, and the pair lands in the same partition. `KafkaMurmur2Check.java` runs the Java method copied verbatim from Kafka and prints `b626e14b b626e14b`.

`loop_xxmur.c`: the two-block flip at any word position inside 256 B .. 1 MB of random data (every trial collided); 2^16-member sets in 64 KB, 2^10 in 1 MB. 1 MB example (little-endian word values): offsets 925,680 and 925,684, `4c181de1 -> fe8b3f61` and `c30b81d8 -> 757ea358`.

## Build and run

    cc -std=c11 -O2 -o murmurhash2_witness murmurhash2_witness.c && ./murmurhash2_witness 12 0x2 --exhaustive
    cc -std=c11 -O2 -o murmurhash2a_witness murmurhash2a_witness.c && ./murmurhash2a_witness 12 0x2a --exhaustive
    cc -std=c11 -O2 -o kafka_check kafka_check.c && ./kafka_check
    java KafkaMurmur2Check.java      # JDK 11+

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    cc -std=c11 -O2 -pthread -Iupstream -o loop_xxmur loop_xxmur.c -lm
    THREADS=8 ./loop_xxmur fixed m2 1048576 4096

Logs: `logs/murmurhash2_witness.log`, `logs/murmurhash2a_witness.log`, `logs/kafka_check.log`, `logs/KafkaMurmur2Check.log`, `logs/loop_xxmur_m2.log`.

## Prior work

The MurmurHash2 pair and multicollisions reproduce Aumasson, Bernstein and Boßlet, "Hash-flooding DoS reloaded: attacks and defenses", 29C3, 2012 (slides: https://www.aumasson.jp/siphash/siphashdos_29c3_slides.pdf). The slides show the MurmurHash2 block step `k *= m; k ^= k >> 24; k *= m; h *= m; h ^= k` (in the 64-bit-word form used by CRuby), inject a top-bit difference in one block, cancel it in the next, and build multicollisions by concatenation; their summary names MurmurHash v2 and v3. Orson Peters, "Breaking CityHash64, MurmurHash2/3, wyhash, and more" (2024), treats the 64-bit MurmurHash64A. MurmurHash2A is not named in either; the transfer is immediate because its block step is the same.
