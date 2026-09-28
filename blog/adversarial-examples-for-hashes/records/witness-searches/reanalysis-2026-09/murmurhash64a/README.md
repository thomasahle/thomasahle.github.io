# MurmurHash64A: key-free pairs and 2^16-way sets

Key model: `MurmurHash64A(key, len, seed)` with a uniformly random 64-bit seed (SMHasher3 `MurmurHash2-64`). The same function is libstdc++'s 64-bit `std::_Hash_bytes`, which `std::hash<std::string>` calls with the fixed seed `0xc70f6907` on LP64 targets. Every result here is key-free: it holds for every seed, so it also holds for that fixed seed.
Scores use L = number of 8-byte words of the longer message.

## What is checked

`murmurhash2_witness.c` (transcription of SMHasher3 `hashes/murmurhash2.cpp`; checks `0x27864C1E` and `0x1F0D3804` first). Each 8-byte block is `k = mix(w)` (a public bijection), then `h ^= k; h *= m`. A XOR difference in bit 63 of `h` survives `* m`, so flipping bit 63 of `mix(w)` in one block and again in the next cancels.

- **Cross-length pair, 8 vs 7 bytes (L = 1).** `h0 = seed ^ (len * m)` differs between lengths 8 and 7 by the public constant `(8m) ^ (7m)`; choosing `mix(w) = t ^ (8m) ^ (7m)` for the 7-byte tail value t makes both inputs reach the same state (the tail does `h ^= t; h *= m`, like a block). Memory order:

      M1 (8 bytes) = dcc945912a274b56
      M2 (7 bytes) = 6d75726d757237   ("murmur7")

  Expected: `16777216 / 16777216` random seeds; under `0xc70f6907` both give `a7ae701751be100f`. Score log2(1 / 1) = 0 bits.
- **Same-length pair, 16 bytes (L = 2):** `4d75726d7572486173683634412d3136` vs `4d752f871058f0ef7368f34ddc12d9c4`, 16777216/16777216 seeds.
- **2^16-way set, 136 bytes (L = 17):** 17 blocks, the index bits choose which of blocks 0..15 open a bit-63 flip; block i XORs (open_i xor open_(i-1)) * 2^63 into `mix(w_i)`. All 65536 messages distinct; all equal for 4096/4096 random seeds (key-free by construction).

`libstdcxx_check.c` (checks `0x1F0D3804` first): a transcription of libstdc++ `hash_bytes.cc` (64-bit branch) agrees with MurmurHash64A on 2^20 random inputs (0 mismatches) and the 8-vs-7 pair collides under the default seed `0xc70f6907`.

`loop_xxmur.c`: the same two-block flip at any word position inside 256 B .. 1 MB of random data (every trial collided); 2^16-member sets in 64 KB and 2^10 in 1 MB (every key). 1 MB example (little-endian word values): offsets 647,944 and 647,952, `1ada7f923ecb5c4b -> 8c3299f7250e5c4b` and `e5ffc0ab54b26580 -> 74a7a6466e6f6580`.

## Build and run

    cc -std=c11 -O2 -o murmurhash2_witness murmurhash2_witness.c
    ./murmurhash2_witness 12 0x2 --exhaustive    # about 15 s; the exhaustive part is the 32-bit pair
    cc -std=c11 -O2 -o libstdcxx_check libstdcxx_check.c && ./libstdcxx_check

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    cc -std=c11 -O2 -pthread -Iupstream -o loop_xxmur loop_xxmur.c -lm
    THREADS=8 ./loop_xxmur fixed m64a 1048576 4096
    ./loop_xxmur cube m64a 65536 16 32

Logs: `logs/murmurhash2_witness.log`, `logs/libstdcxx_check.log`, `logs/loop_xxmur_m64a.log`.

## Prior work

Same-length key-free MurmurHash64A collisions and multicollisions: Aumasson, Bernstein and Boßlet, "Hash-flooding DoS reloaded", 29C3, 2012 (https://www.aumasson.jp/siphash/siphashdos_29c3_slides.pdf; the slides inject a bit-63 difference in one block of a 64-bit MurmurHash2-style function and cancel it in the next); Orson Peters, "Breaking CityHash64, MurmurHash2/3, wyhash, and more", 2024 (https://orlp.net/blog/breaking-hash-functions/), which builds 2^n colliding strings of 16n bytes and points out that libstdc++'s string hash is MurmurHash2 with seed `0xc70f6907`. The cross-length 8-vs-7-byte pair (absorbing the length term) is an extension of that trail.
