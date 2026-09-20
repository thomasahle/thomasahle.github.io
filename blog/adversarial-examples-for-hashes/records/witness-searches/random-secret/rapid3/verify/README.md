# rapidhash v3, random-secret model: independent verification

Independent re-measurement (2026-09-19) of the two rates claimed by the `rapid3` random-secret
study one directory up: the row's current 32-byte pair A and the proposed 24-byte word-0-complement
pair, both with the 64-bit seed and all eight 64-bit secret words uniform (576 bits of key).

What is independent here:

* **Hash body = the unmodified upstream header.** `upstream/rapidhash_v3.h` is
  `rapidhash.h` at tag `rapidhash_v3` of github.com/Nicoshev/rapidhash
  (sha256 `807874eeb23339eaa4b435fbadd33d46718c88e32f3eb67bab3eb965eecc626e`), fetched fresh and
  included verbatim; the harness calls its header-visible
  `rapidhash_internal(key, len, seed, secret)` in the default configuration
  (`RAPIDHASH_COMPACT`, `RAPIDHASH_FAST`, the one the public `rapidhash()` / `rapidhash_withSeed()`
  wrappers use).  The study being verified used an SMHasher3-derived port instead.
* **Fresh sampler.** `verify_rapid3.c` was written from scratch: OpenMP static partition of the key
  index space, counter-mode splitmix64 (word `j` of key `k` is `sm64(base + (9k + j)·golden)`, so
  every key is reproducible from `(rngseed, k)`), each key draws the seed and then eight secret
  words.  The study used xoshiro256** streams.  A second build (`-DRNG_CHACHA`) draws the key
  material from ChaCha20 blocks (counter = key index) as an RNG cross-check at 2^30.
* **Self-checks.** `selftest` re-asserts the page's recorded witness (seed `3187ae8a8617e034`,
  shipped secret: both 32-byte messages hash to `a7ee6375a78a86f0`, via `rapidhash_internal` and
  via the public `rapidhash_withSeed`).  `crosscheck` compares upstream `rapidhash_internal` with
  the study's port (`rapid3_port.h`, copied here unchanged) on 2 000 000 random
  (message, length 0..300, seed, secret) tuples, half with the shipped secret.

## Files

| file | purpose |
|---|---|
| `verify_rapid3.c` | sampler (`selftest`, `pair LOG2N RNGSEED HEXM HEXM2 [random\|default]`) |
| `crosscheck.c`, `upstream_driver.c`, `port_driver.c` | upstream-vs-port equality check |
| `rapid3_port.h` | the study's port, only used by `crosscheck` |
| `ci.py` | exact Garwood 95 % Poisson interval, `ci.py COUNT LOG2TRIALS L` |
| `run_all.sh` | matched-size runs (2^30, fresh 2^34, fresh 2^35 per pair; controls) |
| `run_stage2.sh` | extra precision: fresh 2^37 per pair, pair D at 2^34 |
| `logs/` | one log per run (`NN_<what>.txt`), with the example colliding key |
| `RESULTS.md` | the numbers |

## Reproduce

```
curl -sSL -o upstream/rapidhash_v3.h https://raw.githubusercontent.com/Nicoshev/rapidhash/rapidhash_v3/rapidhash.h
sha256sum upstream/rapidhash_v3.h      # 807874ee...626e
./run_all.sh && ./run_stage2.sh         # 8 threads; about 30 min on a Xeon 8375C with shared cores
python3 ci.py COUNT LOG2N L             # e.g. ci.py 532 35.585 4
```

Single runs: `./verify_rapid3 pair 35 3001 $A $A2 random` etc., with the hex messages as in
`run_all.sh`.  Compute: Xeon 8375C, `nice -n 10 taskset -c 24-31`, 8 OpenMP threads on cores shared
with other jobs.
