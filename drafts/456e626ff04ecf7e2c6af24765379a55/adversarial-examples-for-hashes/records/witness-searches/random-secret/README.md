# Random-secret key model: measurements for wyhash, rapidhash v1, rapidhash v3, XXH3-64 and XXH3-128

Date: 2026-09-19. Each folder holds a search lane (the harness, its candidate lists, run scripts and
logs) and, under `verify/` or `independent/`, an independently written verifier with its own RNG and
logs. All runs were made on an Intel Xeon Platinum 8375C (`nice -n 10 taskset -c 24-31`, 8 threads on
cores shared with other jobs); `<xeon-host>` and `<scratch>` in the files stand for the compute host and
the working directory.

Key model: every trial draws a fresh uniform 64-bit seed and every secret word the API accepts,
uniformly and independently (wyhash: four words, 320 key bits; rapidhash v1: three words, 256;
rapidhash v3: eight words, 576; XXH3-64 and XXH3-128: a 192-byte secret through `withSecret`, 1536).
The pair is fixed before any key is drawn; a hit is full-output equality. Intervals are exact central
95% Poisson (Garwood) intervals on the pooled counts.

| row | pair | pooled sample | rate | cap (bits, 95%) | folder |
|---|---|---|---|---|---|
| wyhash final v4.3 | A, 32 B, L = 4 | 1088 / 3 x 2^35 | 2^-26.50 | 28.5 [28.4, 28.6] | `wyhash/` (searcher `wyrs.cpp`; verifier `verify/wyverify.cpp`) |
| rapidhash v1 | A, 32 B, L = 4 | 160 / 2^34 | 2^-26.68 | 28.7 [28.5, 28.9] | `rapid1/` (searcher `rs_rapid1.cpp`; verifier `independent/rs_verify.cpp`) |
| rapidhash v3 | A, 32 B, L = 4 | 2386 / 2^37.83 | 2^-26.61 | 28.6 [28.5, 28.7] | `rapid3/` (searcher `rs_rapid3.c`; verifier `verify/verify_rapid3.c`) |
| XXH3-64 0.8.3 | W0, 24 B, L = 3 | 527 / 2^36 | 2^-26.96 | 28.5 [28.4, 28.7] | `xxh3-64/` (searcher `xxh3_rs.c`; verifier `verify/xxh3_verify.c`) |
| XXH3-128 0.8.3 | F, 32 B, L = 4 | 479 / 2^35.46 | 2^-26.56 | 28.6 [28.4, 28.7] | `xxh3-128/` (searcher `rs128.c`; verifier `verify/verify_rs128.c`) |

The mechanism in every row is the XOR differential of `fold(a, b) = lo64(ab) ^ hi64(ab)` over two
uniform operands (all-ones on both, about 2^-26.6; all-ones on one, about 2^-26.9), which never uses
a secret value, so the default public secrets give the same class. The default-secret controls in each
folder record that, together with the default-secret-only results the random model excludes: the
every-seed annihilation pairs of wyhash and rapidhash (a message word equal to a shipped secret word)
and the 2^-10.47 NAF pair of XXH3-64 (whose bytes encode the default-secret words). Upstream headers
that are fetched by the reproduction commands are not duplicated here; see the `SOURCES.md` files.
