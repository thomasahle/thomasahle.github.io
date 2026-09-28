# XXH64: key-free pair, 2^16-way family, long-input loop differential

Key model: `XXH64(input, len, seed)` with a uniformly random 64-bit seed (xxHash v0.8.3; SMHasher3 `XXH-64`).
Scores use L = number of 8-byte words of the longer message.

## What is checked

`xxh64_witness.c` (self-contained transcription of SMHasher3 `hashes/xxhash.cpp`; checks the SMHasher3 verification value `0x8F8224C4` first and exits 1 on mismatch):

- **[A] key-free pair, 16 bytes (L = 2).** Word 1 is chosen so that `round(0, w)` changes by XOR 2^36; after `h ^= k; h = rotl(h, 27)` the difference sits in bit 63, survives `* P1 + P4`, and word 2 (round output changed by XOR 2^63) cancels it. This holds for every seed. Memory order (little-endian words):

      M1 = 616476657273617269616c2d78786836   ("adversarial-xxh6")
      M2 = 815775cd1627cf6569616c2dc92cdb1f

  Expected: `collisions 16777216 / 16777216 random seeds; control pair 0`; seed 0 gives `fee615c7742f80cd` for both.
  Score: log2(L / eps) = log2(2 / 1) = 1 bit.
- **[B] key-free 4-way set, 24 bytes (L = 3).** Two chained bit-63 flips. Expected `all 4 equal for 1048576 / 1048576 random seeds`.
- **[C] 2^16-way family, 64 bytes (L = 8).** For lanes l = 0..3 and j_l in [0, 16): word l = x_l + j_l * (2^33 * P2^-1), word 4+l = y_l - j_l * (P1 * P2^-1) (mod 2^64). The set fails only if, in some lane, the top 31 bits of V = acc + x_l * P2 are at least 2^31 - 15: probability at most 4 * 15 / 2^31 = 2.8e-8 over the seed. Expected: `distinct messages: 65536 of 65536`, `all 65536 equal for 65536 / 65536 random seeds` (with arguments `24 16 0x55`), and a back-solved failing seed whose class has 4096 of 65536 members.

`loop_xxmur.c` (uses the unmodified upstream `xxhash.h` v0.8.3; see SOURCES.md; checks all four SMHasher3 values first): position-independent loop differential. In lane l of stripes s and s+1 (both inside the 32-byte stripe loop), add `P2^-1 = 0x0ba79078168d4baf` to word (s, l) and `-2^31 * P1 * P2^-1 = 0x9c90005b80000000` to word (s+1, l). The pair collides whenever the low 33 bits of V = acc + w * P2 are not all ones; acc entering stripe s is a bijective function of the seed for any fixed prefix, so the collision probability is at least 1 - 2^-33 at any position, whatever the surrounding bytes. Logs: random messages, positions and seeds at 256 B, 1 KB, 64 KB and 1 MB (every trial collided, model/outcome disagreements 0); back-solved seeds (2048 of 4096 planted failures fail and 2048 near-failures collide, exactly as predicted); disjoint slots give 2^16-member sets in 64 KB (64/64 keys) and 2^10 in 1 MB (32/32 keys).

1 MB example (little-endian word values): offsets 1,029,400 and 1,029,432, `bcf02d7c47a38fa5 -> c897bdf45e30db54` and `44c55e28ef97740c -> e1555e846f97740c`; 4096/4096 seeds collide.

## Build and run

    cc -std=c11 -O2 -o xxh64_witness xxh64_witness.c
    ./xxh64_witness 24 16 0x55          # about 40 s; default family trials are 2^12

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    cc -std=c11 -O2 -pthread -Iupstream -o loop_xxmur loop_xxmur.c -lm
    THREADS=8 ./loop_xxmur rand xxh64 1048576 32768
    THREADS=8 ./loop_xxmur fixed xxh64 1048576 4096
    THREADS=8 ./loop_xxmur plant xxh64 65536 4096
    ./loop_xxmur cube xxh64 65536 16 64

Logs: `logs/xxh64_witness.log`, `logs/loop_xxmur_xxh64.log` (trimmed to the XXH64 runs).
