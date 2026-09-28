# XXH32: 8-byte pair at 1 - 2^-17, 2^16-way families, long-input loop differential

Key model: `XXH32(input, len, seed)` with a uniformly random 32-bit seed (xxHash v0.8.3; SMHasher3 `XXH-32`).
Scores use L = number of 8-byte words of the longer message.

## What is checked

`xxh32_witness.c` (self-contained transcription of SMHasher3 `hashes/xxhash.cpp`; checks the SMHasher3 verification value `0x6FD78385` first):

- **[A] pair, 8 bytes (L = 1).** Short path: each 4-byte word does `h += w * P3; h = rotl(h, 17) * P4`. Adding 2^15 * P3^-1 to word 0 adds exactly 1 to rotl(V, 17), V = seed + P5 + 8 + w0 * P3, unless the top 17 bits of V are all ones; word 1 -= P4 * P3^-1 cancels the resulting +P4. V is uniform when the seed is, so

      M1 = 78786833322d7072   ("xxh32-pr")
      M2 = 78f8f29f570b9cb3

  collide for exactly 2^32 - 2^15 = 4,294,934,528 of the 2^32 seeds (checked exhaustively with `--exhaustive`): eps = 1 - 2^-17, score log2(1 / (1 - 2^-17)) = 1.1e-5 bits.
- **[B] 2^15-way family, 8 bytes:** p(S) = 1 - (2^15 - 1) / 2^17, about 3/4 (measured 6201/8192).
- **[C] 2^16-way family, 12 bytes (L = 2).** With d = 2^15 * P3^-1, c = P4 * P3^-1 and j1, j2 < 256: words (w0 + j1 d, w1 - j1 c + j2 d, w2 - j2 c). Analytic p(S) >= 1 - 2 * 255 / 2^17 = 0.99611; measured 8160/8192 = 0.99609 (95% [0.99449, 0.99723]); all 65536 messages distinct.
- **[D] 2^16-way family, 32 bytes (stripe loop):** lane word += j * (2^19 * P2^-1), next stripe -= j * (P1 * P2^-1), j < 16 per lane; p(S) >= 1 - 4 * 15 / 2^13; measured 8141/8192.

`loop_xxmur.c` (upstream `xxhash.h` v0.8.3; checks all four SMHasher3 values first): position-independent loop differential, lane l of stripes s, s+1: word (s, l) += `P2^-1 = 0xb6c92f47`, word (s+1, l) += `-2^13 * P1 * P2^-1 = 0x981d2000`. It fails exactly when the low 19 bits of V = acc + w * P2 are all ones, so eps = 1 - 2^-19 at any position. Exhaustive check at 256 B (offsets 180 and 196): exactly 8,192 of the 2^32 seeds fail (4,294,959,104 collide), every failure predicted by the model; seed 0 collides (`f2247a10`), back-solved seed `0x0b867048` fails. Random positions at 256 B .. 1 MB and disjoint-slot sets (2^16 in 64 KB, 64/64 keys; 2^10 in 1 MB, 32/32) agree with the model.

## Build and run

    cc -std=c11 -O2 -o xxh32_witness xxh32_witness.c -lm
    ./xxh32_witness 13 0x32 --exhaustive     # about 15 s

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    cc -std=c11 -O2 -pthread -Iupstream -o loop_xxmur loop_xxmur.c -lm
    THREADS=8 ./loop_xxmur exh32 256
    THREADS=8 ./loop_xxmur rand xxh32 65536 262144

Logs: `logs/xxh32_witness.log`, `logs/loop_xxmur_xxh32.log`.
