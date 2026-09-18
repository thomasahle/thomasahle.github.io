# Evidence and reproduction

The final header SHA-256 is
`77d30baf4c6dc12e270236400e5323ccd4ef7a17e0aac877b3cbe4bc325ecbad`.
`verification-summary.json` indexes the accepted results. `rank-certificate.json`
contains every survivor subset and its GF(2) rank, not just a pass/fail flag.
`scores-layout.json` records allocation sizes, length limits, and score plateaus.

`src/` contains the executable checks and parsers. `logs/` contains their raw
results, random seeds, completion checkpoints, and compiler diagnostics.
`bench/` contains the adapter, scratch-tree patcher, unchanged shipped control
registration, runners, raw timings, per-run load gates, and provenance.
`provenance/` identifies the hosts, compilers, timer implementations, and builds.
No benchmark or random-key experiment is used as a proof of a 96-bit exponent.
The upstream header retains the repository's Apache 2.0 license. The borrowed
SMHasher seed adapter's MIT notice is preserved in
`bench/LICENSE-SMHasher3-Halftime.txt`.

## Reproduce on the Xeon

Run from a directory with `halftime-hash.hpp` and `certificates/`. Use a fresh
output directory if preserving the supplied logs. GCC needs AES-NI and OpenMP;
the supplied native configuration selects scalar/SSE2/AVX2/AVX-512 at widths
1/2/4/8. All expensive commands below use nice priority and at most 24 workers.

```bash
mkdir -p certificates/logs
bash certificates/src/verify-xeon.sh
nice -n 10 g++ -std=c++17 -O2 -Wno-ignored-attributes certificates/src/matrix.cpp -o matrix
./matrix > certificates/generator-matrices.jsonl
python3 certificates/src/rank.py certificates/generator-matrices.jsonl
nice -n 10 g++ -std=c++17 -O2 -march=native -Wno-ignored-attributes certificates/src/flips.cpp -o flips
./flips > certificates/logs/flips.jsonl
nice -n 10 g++ -std=c++17 -O3 -march=native -Wno-ignored-attributes -c certificates/src/api.cpp -o api.o
nice -n 10 g++ -std=c++17 -O3 -march=native -fopenmp certificates/src/witness.cpp api.o -o witness
for b in 1 2 4 8; do
  nice -n 10 ./witness "$b" 268435456 24 1 > "certificates/logs/condition-b${b}.jsonl"
  nice -n 10 ./witness "$b" 68719476736 24 0 > "certificates/logs/witness-b${b}.jsonl"
done
nice -n 10 g++ -std=c++17 -O2 -march=native -Wno-ignored-attributes certificates/src/extra.cpp api.o -o extra
nice -n 10 ./extra > certificates/logs/extra.json
python3 certificates/src/score.py
```

The witness pair is two `168*b`-byte messages: all zero, and all zero except
byte `48*b` is one (64-bit word `6*b` equals one). Condition `1` clears only
the high half of core key word 6. `api.cpp` is compiled separately, without
LTO, and each trial calls it twice and compares all 192 output bits. The
one-leaf live ranges are `[0,27)`, `[216,216+3b)`, and
`[216+212b,217+215b)`. Every live word receives new AES-CTR output. Rerandomizing
all holes independently is separately checked in `extra.cpp`.

For parent Style output comparisons, first extract
`git show fix-neon-dispatch:halftime-hash.hpp > base-neon.hpp` and compile
`verify.cpp` with `-DHH_BASE -fwrapv`. The parent requires that arithmetic
qualification; the fixed code is tested without it. The binary vector layout
is 448 bytes of raw-core results followed by 32 bytes of Style results per
input. Compare the final 32 bytes of each of the 10,000 records.

The Xeon's GCC runtime package lacks `libubsan`. The final GCC check compiled
`verify.cpp` with GCC's `-fsanitize=undefined -fno-sanitize-recover=all` and linked
that object using `clang++ -fsanitize=undefined`. Clang 21's installed runtime
handled all instrumentation. The independent Clang UBSan run also passed.
No sanitizer suppression, `-fwrapv`, or `-fno-strict-aliasing` is used for the fix.

## Accepted interrupted-run accounting

Width 1/2/4 runs were paused for timing and then rebalanced. The final totals
add a printed completed checkpoint from part 1 to the complete part 2 result;
unprinted in-flight work is discarded. `logs/witness-parts.json` gives those
counts and independent seeds are in each log. The last AVX-512 unsigned-sum
edit changed only the width-8 function and dispatcher displacements.
`logs/final-object-equivalence.json` certifies identical width-1/2/4 function
bytes, relocations, and constants. Width 8 was rerun from zero with the final
binary, including the conditioning run. Its earlier chunks are excluded.

The two small ELF API objects are retained in `objects/`. From the repository
root, `python3 certificates/src/check_objects.py certificates/objects/api.o
certificates/objects/api-unsigned512.o` independently rechecks that identity
without executing the objects. `src/compare_style.py` compares the Style slices
of the parent and fixed vector files.

`src/rebalance.py`, `src/final-xeon.sh`, and `bench/xeon_speed.py` document this
particular execution history; the sequential commands above are the simpler
way to repeat it. `src/collect_verification.py` verifies the supplied historical
log layout and counts only actual completion events/checkpoints.

## Lean

`lean/FixedLength.lean` is the new terminal-word proof; `lean/FixedLength.log`
is its axiom audit. `lean/base/` is the supplied inherited Lean development,
including source, pinned toolchain/Mathlib manifest, and previous audits.
`lean/base-source-sha256.json` identifies that snapshot. No `.lake` cache or
compiled Lean object is bundled.

With Lean 4.24.0 and the pinned Mathlib cache available:

```bash
cd certificates/lean/base
lake exe cache get
LEAN_NUM_THREADS=1 nice -n 10 lake build
LEAN_NUM_THREADS=1 nice -n 10 lake env lean ../FixedLength.lean
```

The actual added proof was checked against the already-built inherited
development on the Xeon. It introduces no new axioms or proof placeholders.
The rank checker and the source-to-model mapping in `THEOREM.md` are separate
from the Lean kernel; this is not a compiler-verification claim.

## SMHasher3

Patch a scratch copy with
`python3 certificates/bench/patch_tree.py SCRATCH_TREE halftime-hash.hpp`.
The patch is intended for an unpatched copy. It adds the two registrations
and a 192-bit Speed/Sanity dispatch; it leaves SpeedTest, buffers, iteration
counts, timers, and all control implementations unchanged. Build normally
with the same compiler flags as the parent tree, then run:

```bash
python3 certificates/bench/run_speed.py Xeon8375C /path/to/SMHasher3 certificates/bench/Xeon8375C
python3 certificates/bench/run_speed.py M2Pro /path/to/SMHasher3 certificates/bench/M2Pro --gate
python3 certificates/bench/collect.py
```

Stop this task's compute jobs before timings. The runner waits for no SMHasher3
process before each cell; the M2 additionally requires one-minute load below
4.5. It runs all five names in each of three passes. Both reported averages
are independently selected medians, with every individual 1–31-byte result
retained. Deviations over 15% are flagged. Benchmark seed expansion matches
the shipped port's key prefix; it does not instantiate the ideal uniform-key
probability space of the theorem.

The M2 build reused unchanged archived test/control objects to minimize local
preparation. `bench/M2Pro/` records archive hashes, source comparisons, compile
commands, and the quiet preparation gate. The final AVX-512-only edit is absent
from its active preprocessed source; `final-header-equivalence.json` certifies
identity, so its completed vectors, UBSan run, and timings apply unchanged.
`bench/Xeon8375C-before-unsigned512/` is an excluded earlier timing batch.
