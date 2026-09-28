# XXH3 v0.8.3: weak-key multicollision on the long path (> 240 bytes)

One condition on one 64-bit secret word makes 2^(4(B+1)) fixed messages of 1024 B + 576 bytes collide
together, in both XXH3-64 and XXH3-128 (full output). The condition holds for exactly 2^32 - 1 of the
2^64 values of that word, a density of 2^-32.0. For the default secret it never holds.

## Mechanism

In the bulk loop, stripe n (64 bytes) of a 1024-byte block processes lane i (0..7) as

    x = D[n][i] ^ K[n+i]            K[m] = little-endian 64-bit word at secret byte 8m
    acc[i ^ 1] += D[n][i]           raw input word, unkeyed
    acc[i]     += lo32(x) * hi32(x)

and each block ends with a bijective scramble. The same secret offsets repeat in every block.

A toggle for lanes a = 2t and a + 1 uses two positions that read the same secret word K = K[j]:
(lane a, stripe j - a) and (lane a + 1, stripe j - a - 1). Both positions hold the word
D = `0x000000007fffffff`, and the toggle complements both. Each of acc[a] and acc[a+1] then changes
by dg + dD, where dD = ~D - D = 2^64 - 1 - 2D and dg = M32 (M32 - l - h), with M32 = 2^32 - 1 and
l, h the halves of D ^ K. For this D the change is zero iff

    (lo32(K[j]) ^ 0x7fffffff) + hi32(K[j]) = 0xfffffffe

With j = 7 there are four such toggles per block (t = 0..3). Their byte offsets inside block b are
1024b + {448, 392}, {336, 280}, {224, 168}, {112, 56} (first and second word of toggles t = 0..3).
Every toggle in every block, including the final partial block, is decided by the same word K[7].
So a secret in the class makes every one of the 2^(4(B+1)) combinations collide, and a secret outside
it separates them. Measured over uniform secrets, one toggle pair collides at the class density
(pooled 40 / 2^37 = 2^-31.68 [2^-32.16, 2^-31.23] at 576 bytes, every hit in the class, 64- and
128-bit results never disagreed). Four toggles together also collide at about 2^-32, not 2^-128.

Other API entry points. `withSeed` derives K[7] = kSecret[7] - seed, so the seed class has the same
density. `withSecretandSeed` uses only the custom secret above 240 bytes. The default secret admits
no toggle word: for each of K[1..21] of kSecret, no 64-bit D satisfies the toggle equation
(exhaustive over the 2^33 candidates per word, `default_secret_toggle.c`). This covers the one-word,
two-lane toggle family only.

As a pair this is weaker than the short-input pairs (2^-32 at L >= 72 words, about 37.8 bits). Its
point is the size of the colliding set: 16 messages at 576 bytes, 256 at 1600 bytes, 2^16 at 3648
bytes, for a single condition on the secret.

## Files

* `xxh3_longmc.c`: search program. Mode 0 measures the one-toggle rate over uniform secrets; mode 1
  checks the whole 2^(4(B+1)) set for class secrets; mode 2 records the largest class of the fixed
  set for uniform secrets.
* `xxh3_toggle.c`: position check. It uses fresh random content and a random valid block and lane
  pair per trial, for lengths 256 B to 1 MB.
* `xxh3_longpath_verify.c`: an independently written verifier (`full16`, `single`, `combo`, `krate`).
* `default_secret_toggle.c`: the exhaustive default-secret check.
* `../xxh3-crosslen/witness_check.c`: deterministic check of the 2^16-member witness below.
* `logs/`, `SOURCES.md` (pinned upstream header).

## Build and run

    curl -sSL -o xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    cc -O2 -pthread -o xxh3_longmc xxh3_longmc.c -lm
    cc -O2 -pthread -o xxh3_toggle xxh3_toggle.c -lm
    cc -O2 -pthread -o xxh3_longpath_verify xxh3_longpath_verify.c -lm
    cc -O2 -o default_secret_toggle default_secret_toggle.c
    ./xxh3_longmc 1 3 12 0xc1a55 8          # 2^12 class secrets, whole 2^16 set, 3648 B
    ./xxh3_longmc 2 3 1024 0x7a11 8         # 1024 uniform secrets, largest class of the set
    ./xxh3_longmc 0 0 36 0xa11ce 8          # one toggle, 2^36 uniform secrets, 576 B (also 0xb0b5)
    for L in 256 1024 65536 1048576; do j=7; [ $L = 256 ] && j=1
      ./xxh3_toggle 0 $L $j 12 0x1$L 8; ./xxh3_toggle 1 $L $j 16 0x2$L 8; done
    ./xxh3_longpath_verify full16 64 class 5120 8     # and 128; and unif
    ./xxh3_longpath_verify single 64 class 1048576 7 8
    for j in $(seq 1 21); do ./default_secret_toggle $j | tail -1; done

## Expected output

| run | result |
|---|---|
| longmc mode 1, 3648 B, 2^12 class secrets | whole 2^16 set collided (64 and 128) for 4096, failed for 0 |
| longmc mode 2, 1024 uniform secrets | largest full 64-bit and 128-bit class 1 (min, 1%, median, max); low 16 bits min 6, median 7, max 10 to 11 (as for a random function) |
| longmc mode 0, 576 B, 2^36 uniform secrets, two seeds | 25 and 15 hits, all in the class, 0 disagreements between 64 and 128 bits; pooled 40 / 2^37 = 2^-31.68 |
| toggle, class secrets, 256 B (j = 1) / 1 KB / 64 KB / 1 MB | 4096 / 4096 each, 64 and 128 bits |
| toggle, uniform secrets, same lengths | 0 / 2^16 each |
| verifier full16, 5120 B, 8 class secrets | all 65536 subsets collide with the base, 64 and 128 bits, 8/8 secrets |
| verifier full16, uniform secrets | only the empty subset matches |
| verifier single, class, 256 B to 1 MB | rate 1 (64 and 128 bits); uniform: 0 |
| default_secret_toggle j = 1..21 | solutions = 0 for every j; control with a class word: 8 solutions including D = 000000007fffffff |

## Witness (checked by `witness_check.c`)

Length 3648 B (3 full blocks + 576-byte tail). Member c (16 bits): for block b = 0..3 and t = 0..3,
bit 4b + t of c selects D = `000000007fffffff` or ~D, written little-endian at byte offsets
1024b + 64(7 - 2t) + 8(2t) and 1024b + 64(6 - 2t) + 8(2t + 1); all other bytes are zero. Under the
192-byte secret below (K[7] = `0b8185638b818564`) all 65536 members give XXH3-64 `b3c0a2ccc5a6d117`
and XXH3-128 `3131016698da6968b3c0a2ccc5a6d117` (high64 then low64). Through `withSeed` with seed
`kSecret[7] - K[7]` = `0x40a4b51e5b0eb07c` all members give XXH3-64 `7758cac86271a5bd` and XXH3-128
`2329b3c05ce848bb7758cac86271a5bd`.

    4a74bed0463010c855c2dc0b688f05eace58aa0bbf246c536c17b7c2f664260aca2b4bdef2940033410a169f771719fa
    0617401460a987596485818b6385810b68eed8f1ee5f42ddb2404faae88bccc51b03fc683dda077754cbf10ea69a34c2
    b893f217d9771c37877726d513f5384e70a2b77dab6df4bc4a6156045a4b8f6cacd398b771bdd1af0c582a785a742c1e
    41cd07c57f0f68b24189308bb18afef120b704ff7c43b24a6f7d21a1f352502816b46f0d32089119b516b1b95e497144

## Logs

* `search_*_x86.txt`: the search programs on an Intel Xeon 8375C (AVX2 build). The longmc logs come
  from a build made before the source was last edited.
* `search_*_arm64.txt`: the class, largest-class and position runs re-run from this directory's source
  on Apple M-series (NEON build). Class and position results are identical to the Xeon logs; the
  largest-class statistics differ only in sampling (low 16 bits max 10 here, 11 on the Xeon). The
  2^36-secret rate runs (`longmc` mode 0) were not re-run; their logs are the Xeon ones.
* `verify_longpath_summary.txt`: the verifier's summary of its Xeon runs, including the `krate`
  runs varying only K[7] over uniform values (1 toggle: 1 / 2^34; 4 toggles: 4 / 2^34).
* `verify_longpath_arm64.txt`: the verifier's `full16` and `single` runs re-run on Apple M-series (NEON).
* `default_secret_toggle_arm64.txt`: the exhaustive default-secret check and its control.
