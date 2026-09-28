# XXH3 v0.8.3: cross-length pairs on the short paths (random-secret model)

Two fixed pairs of messages with different lengths. Each collides for a large fraction of uniformly
random 192-byte secrets passed to `XXH3_*_withSecret`. Both pairs were fixed before any secret was
drawn. A hit is equality of the full output (64 bits, or both 64-bit halves for XXH3-128).

| pair | messages (hex, memory order) | L | pooled count | rate | cap |
|---|---|---|---|---|---|
| XXH3-64, 4..8-byte path | `0000000000` (5 B) vs `0fe3fba2f2528c1d` (8 B) | 1 | 6868 / (5 x 2^32) | 2^-21.58 | 21.58 bits [21.54, 21.61] |
| XXH3-128, 9..16-byte path | `000000000000000000` (9 B) vs `00292432025470850029243202543025` (16 B) | 2 | 20069 / (5 x 2^32) | 2^-20.03 | 21.03 bits [21.01, 21.05] |

Pooled count = independent verifier (2^34 secrets) + search program re-run from this directory (2^32
secrets); intervals are exact central 95% Poisson (Garwood) intervals; cap = log2(L / rate).
The XXH3-128 m' is the little-endian words `0x8570540232242900, 0x2530540232242900`.

## Mechanism

XXH3-64, 4..8 bytes. The path computes `keyed = input64 ^ bitflip` with `bitflip = S[8..16) ^ S[16..24)`
(uniform under a random secret), then `rrmxmx`: `h = L(keyed)` with `L(x) = x ^ rotl(x,49) ^ rotl(x,24)`
(GF(2)-linear and invertible), `A = h * PRIME_MX2`, `B = A ^ ((A >> 35) + len)`, and a bijection. Two
messages of lengths l and l' collide iff `A' = A ^ (t + l) ^ (t + l')` with `t = A >> 35`. Because
`keyed` is uniform, this is an XOR-to-add differential of the multiplication by `PRIME_MX2` with a
fixed input XOR difference `Y = h ^ h'`. The pair uses m = five zero bytes (input64 = 0) and the
8-byte m' with input64 = `L^-1(0x08032aaa29309209) = 0xa2fbe30f1d8c52f2` (the path assembles
`input2 + (input1 << 32)`, so bytes 0..3 carry the high half).

XXH3-128, 9..16 bytes. The path computes `Z = w_lo ^ w_hi ^ K_lo`, `m128 = Z * P1`, adds
`(len - 1) << 54` to the low half, adds `g(w_hi ^ K_hi)` to the high half, and applies a bijection to
the pair. A length difference d = 7 is cancelled when `Z' = Z + d * 2^54 * P1^-1` (only the top ten
bits of the difference are non-zero); the resulting high-half difference is then matched by an
additive difference inside `g`. Both conditions are XOR-to-add differentials of uniform words.

Earlier records of this post stated that 1..16-byte inputs are injective per length and that
cross-length pairs are about 2^-64. Per length the statement holds; across lengths these pairs refute it.

## Files

* `xxh3_crosslen_search.c`: the search-side measurement program (pairs 0 and 1 above; pair 2 takes
  any hex pair on the command line). Checks the SMHasher3 verification values first.
* `xxh3_crosslen_verify.c`: an independently written verifier. It rebuilds both pairs from the
  path description, inverts `L` by Gaussian elimination and measures the rates with its own RNG.
* `witness_check.c`: deterministic check of the logged witnesses (explicit secrets and seeds below)
  and of the long-input multicollision set in `../xxh3-toggle/`.
* `logs/`: see below. `SOURCES.md`: the pinned upstream header.

## Build and run

    curl -sSL -o xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    cc -O2 -pthread -o xxh3_crosslen_search xxh3_crosslen_search.c -lm
    cc -O2 -pthread -o xxh3_crosslen_verify xxh3_crosslen_verify.c -lm
    cc -O2 -o witness_check witness_check.c
    ./witness_check                                        # about 1 s; ALL CHECKS PASSED
    ./xxh3_crosslen_search 0 32 0x5eed64 8 0               # XXH3-64,  withSecret, 2^32 secrets
    ./xxh3_crosslen_search 1 32 0x5eed128 8 0              # XXH3-128, withSecret, 2^32 secrets
    ./xxh3_crosslen_search 0 30 0x5eed64b 8 1              # XXH3-64,  withSeed,   2^30 seeds
    ./xxh3_crosslen_search 1 30 0x5eed128b 8 1             # XXH3-128, withSeed,   2^30 seeds
    ./xxh3_crosslen_verify 64secret 34 8                   # verifier, 2^34 secrets (and 64seed,
    ./xxh3_crosslen_verify 128secret 34 8                  #   128seed; *full modes redraw all 192 bytes)

Search arguments: `<pair> <log2 keys> <rng seed> <threads> <api 0=withSecret 1=withSeed>`.

## Expected output

| run | hits | rate | cap |
|---|---|---|---|
| search, XXH3-64 withSecret, 2^32 | 1441 | 2^-21.507 | 21.51 [21.43, 21.58] |
| search, XXH3-128 withSecret, 2^32 | 4074 | 2^-20.008 | 21.01 [20.96, 21.05] |
| search, XXH3-64 withSeed, 2^30 | 339 | 2^-21.595 | 21.60 [21.44, 21.75] |
| search, XXH3-128 withSeed, 2^30 | 1471 | 2^-19.477 | 20.48 [20.40, 20.55] |
| verifier, XXH3-64 withSecret, 2^34 | 5427 | 2^-21.594 | 21.59 [21.56, 21.63] |
| verifier, XXH3-64 withSeed, 2^34 | 5343 | 2^-21.617 | 21.62 [21.58, 21.66] |
| verifier, XXH3-128 withSecret, 2^34 | 15995 | 2^-20.035 | 21.04 [21.01, 21.06] |
| verifier, XXH3-128 withSeed, 2^34 | 24249 | 2^-19.434 | 20.43 [20.42, 20.45] |

The search counts are deterministic for the given RNG seed and thread count. The verifier's cross-check
with all 192 secret bytes redrawn gave the same rates at 2^28 (2^-21.62 vs 2^-21.56; 2^-20.09 vs 2^-20.08).

With `withSeed` the relevant secret words are default-secret words plus or minus one seed, so they are
not independent; the XXH3-128 pair is then somewhat more likely (2^-19.43, cap 20.43 bits).

## Witnesses (checked by `witness_check.c`)

XXH3-64, 5 B vs 8 B, `XXH3_64bits_withSecret`, both outputs `79a5d45b5d6567cd`, secret (192 B):

    083975da0b858e44ded126719bdbb4b04138fac5657195e1d6ba9d970bc3973eb4be66f9c271c6378d7c1c7989a419ee
    ff8156e6935d52d7b4066e81a207b719c2bdb60695a029e2b1f9ee58663d4d926dc891a744136596453d4f4c8cba3823
    6735556a74b5b230ef6b11b6dc07dba024b6eaac9405622a692093e940f7b7f2990b3e5fb8b8a4bbe95bde730d82e724
    f8e186ec37ba6409d57a73acf00e29d1ae8cb1294fd3fe51fe138225bad81d20af5bc92ff1b71fe145c8c9badf888361

XXH3-128, 9 B vs 16 B, `XXH3_128bits_withSecret`, both outputs `edc8fa6d9f82abe85cff7dddfa69543d`
(high64 then low64), secret (192 B):

    dae7287b9891ff498ff032063746ae83d16a86a48aa99b1ede3d5c81f35805607220ed05603051b57c696dd19498b392
    a8316a1e611fe4545b73796e900610e8d3a39b9d1083ec70531adc206c01265a3af822d87a6d3cf4199d738a393fa11c
    6ebd219b9c8675434b8a50cbb3e51006b731057c36e2b81798eac463b3268953b1e395f206d3873e2f74fc75ea6cec59
    5924ac103016991b3a0b769d232a764ebf174a335900069349756fbfb421b6496cdefb8c314795f7bd5927a027b6090c

withSeed: XXH3-64 seed `0xc656a656224499e4` gives `042637874fec2910` for both messages; XXH3-128 seed
`0x223bbf0662aa12b5` gives `ef2a458a93a83ca6f65ab3c2989a22f9` for both.

## Logs

* `search_*_arm64.txt`: the commands above, run from this directory's source (Apple M-series, NEON build).
* `search_*_x86_first-build.txt`: the first measurement on an Intel Xeon 8375C with an earlier build of
  the same program (1393 / 2^32 for XXH3-64, 4046 / 2^32 for XXH3-128). That build derived its thread
  streams differently, so its counts differ from the re-run; its first thread's stream is the same, so
  the logged witness secrets are identical. Its printed 95% intervals are wrong for counts above about
  700 (a bracketing bug fixed in the current source); the counts and witnesses are correct. It is not
  pooled with the re-run, because the two share a stream.
* `verify_*_2p34_x86.txt`: the independent verifier on the Xeon.
* `witness_check_arm64.txt`: output of `witness_check`.
