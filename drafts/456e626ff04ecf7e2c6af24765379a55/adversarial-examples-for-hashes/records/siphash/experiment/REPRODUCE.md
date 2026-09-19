# SipHash fixed-pair collision experiment (lens: EXPERIMENT)

Host: Intel Xeon Platinum 8375C (<xeon-host>), clang 21.1.8, `nice -n 10 taskset -c 24-31`,
directory `<xeon-work>/siphash-claim/experiment/` (mirror of this directory). Wall time: phase A 9.5 min, phase B ~5 min,
calibration ~1 min; all inside the 30-minute budget on 8 cores.

## Files
- `sipdiff.c`       harness: reference SipHash (any c,d, 64-bit output, mirrors veorq/SipHash) + fast fixed-width
                    paths for SipHash-1-0/1-1/1-2/1-3/2-4; `sipdiff selftest` checks the 64 official SipHash-2-4
                    vectors (key 00..0f, messages 00..(i-1)) and 10 000 fast-vs-reference cross-checks.
- `gendiffs.py`     difference lists: `single` (every 1-bit), `double` (every 2-bit), `struct` (complement, byte/nibble
                    masks, byte-swap masks, low/high halves, carry-chain runs 0x3,0x3f,..., top runs, mid runs; W=2 adds
                    the same in word 1 and a few two-word patterns).
- `run_phaseA.sh`   screen: all differences x {1-1,1-2,1-3,2-4} x {8-byte at 2^24 keys, 16-byte at 2^22 keys}, random
                    key and random base message per key.
- `select_candidates.py`  per (variant,width): top-12 |z| (popcount-mean bias), every difference with a 64-bit
                    collision, top-3 low-16 / low-32 partial collisions, plus bit0/bit63/complement0 references.
- `run_phaseB.sh`   candidates at 2^28 keys in three base-message modes: R (random base), F0 (FIXED pair 0 vs D),
                    FF (FIXED pair ff..ff vs ff..ff^D).
- `run_calib10.sh`  SipHash-1-0 (no finalization) at 2^24 keys: calibrates the full-collision channel.
- `analyze.py`      summaries with Clopper-Pearson 95% CIs; `summary.json` per phase.
- `phaseA/`, `phaseB/`, `calib10/`  raw outputs (one line per difference: d0 d1 name coll low32 low16 meanpop z hist0..8).

## Statistics
Per difference D and N keys: `coll` = #{k : H_k(m)=H_k(m^D)}; `low32`, `low16` = same on the low 32/16 output bits;
`meanpop` = mean popcount of H_k(m)^H_k(m^D) (32 for an ideal PRF), `z = (meanpop-32)/(4/sqrt(N))`.
Detectable-rate floor with 0 hits at N keys: 95% upper bound -ln(0.05)/N ~ 3/N (2^-22.4 at 2^24, 2^-26.4 at 2^28).

## Commands
    clang -O3 -march=native -o sipdiff sipdiff.c && ./sipdiff selftest
    for W in 1 2; do for c in single double struct; do python3 gendiffs.py $W $c > d${W}_$c.txt; done; done
    ./run_phaseA.sh                       # -> phaseA/*.out
    python3 analyze.py phaseA 6 > phaseA_summary.txt
    python3 select_candidates.py phaseA candidates 12
    ./run_phaseB.sh                       # -> phaseB/*.out   (LG=28 default)
    python3 analyze.py phaseB 6 > phaseB_summary.txt
    ./run_calib10.sh && python3 analyze.py calib10 6 > calib10_summary.txt
Seeds are fixed (0x<variant><W>7 for phase A, 0x<variant><W>b for phase B, 0x1017 for the calibration); the PRNG is
xoshiro256** seeded by splitmix64, so every number in the summaries reproduces exactly.
