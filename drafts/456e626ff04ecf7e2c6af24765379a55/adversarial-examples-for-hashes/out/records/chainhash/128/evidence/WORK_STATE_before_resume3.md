# ChainHash-128 resume state

Updated 2026-09-18 16:06 UTC. Work only in this scratch directory and the
explicit remote scratch tree. Do not rerun finished tests or measurements.

## Completed

- SPEC.md: default 512-byte blocks (W=32, S=1), ideal collision bound
  (n+2)/2^128; ideal score 126.4150374993, model-A at-most score 125.
  Includes proof transfer, model A, two independent ChainHash-64s and Lean
  extension requirements. No new Lean machine-checking is claimed.
- chainhash128.h: portable bit-serial reference, scalar PCLMUL, VPCLMUL256,
  ARM PMULL/PMULL2 pinned with asm, optional four-product schoolbook,
  both block sizes, explicit key models, self-test.
- Xeon: all C variants, 12,000 random inputs/backend + 32 long + arithmetic,
  monomials and guarded tails pass. All vectors match Python. Clang ASan/
  UBSan passes (initial GCC missing-runtime link failure retained separately).
  SMHasher3 new registrations pass Sanity.
- Mac: PMULL 512/256, schoolbook and portable all pass the same full C suite.
  Every vector file matches the independent oracle and Xeon. ChainHash-128,
  256-byte variant and UMASH-128 pass combined-binary Sanity. All 18 preflight
  launches had no SMHasher3 and load <3. Header/adapter hashes match both
  built binaries. Never rebuild these or rerun these completed tests.
- Mac probe done: recurrence 27.747431 estimated cycles, scalar CLMUL
  reciprocal throughput 0.349810. Raw evidence/M2Pro/probe.json.
- Xeon build/test evidence copied here: evidence/Xeon8375C/. Mac native
  evidence is evidence/M2Pro/. REPORT.md now includes Mac correctness.

## Running / waiting

Remote: hardware.normalcomputing.net:~/agents/speedbench-ch128/
- work/: delivered sources and scripts; source/: copied SMHasher3;
  build/: combined binary and probe; evidence/: correctness/build logs;
  out/Xeon8375C/: timing results.
- All 22 Speed runs and the probe are complete, copied locally and validated.
  Strict five-second >=95% idle sample on chosen logical CPU AND SMT sibling,
  then taskset, was enforced before each run. Do not run any more Xeon jobs.

Mac: local foreground tool session 18104 runs benchmarks/run_mac.py, now
in its child run_gated.py. Log evidence/mac-runner.log. Nineteen of 22 Speed
runs complete: every ChainHash variant and UMASH-128 have both passes.
Only komihash, rapidhash and XXH3-128 run 2 remain. It waits before each next
run for no SMHasher3 process and load <3. Do not start another runner.
Current source/build: smhasher3/ and build-mac/, copied from the existing
../m2-rerun/{smhasher3,build-fixed}; no baseline/canonical file was modified.

Both runners check again after 5 seconds when a gate fails. New local scripts
log failed gates at most once/minute (successful gates always saved), to
reduce filesystem/indexer activity. Xeon active runner has the previous
logging cadence, which does not load the Mac. Resume support skips complete,
hash-verified runs; run_mac skips successful preflight steps.

## Finish

1. Continue waiting for the required gates; do not weaken or bypass them.
   Poll completed row counts, not just last log lines. No sub-agents.
2. Remote timing evidence and probe are already copied. DO NOT overwrite
   speed complete.json with the remote probe-only completion file.
3. Run python3 benchmarks/collect.py --require-complete. It verifies 22
   runs/host, both repetitions, one binary, raw-file hashes, all 31 lengths,
   eight alignments and launch gates. Partial progress JSON currently exists;
   it is not the finished deliverable.
4. Run python3 benchmarks/render_results.py to fill REPORT's placeholder.
   Add final interpretation of 256 vs 512, scalar vs wide/schoolbook, controls,
   dual-64 costs, measured ceilings and shared-machine variation. Inspect
   report numbers against JSON. The Mac timer is calibrated estimated cycles;
   Xeon uses TSC ticks. Do not treat them as identical physical cycle units.
5. Audit artifacts/evidence, update this file to complete and deliver links.

Current header SHA256:
329f3181135ffba9816577bc5292f03bfbc6b7bfc6f9be041e0ac3cedd9f5481

Latest checkpoint: Xeon ALL 22 Speed runs and its gated probe are complete,
copied locally, and validated by collect.py. Scalar CLMUL reciprocal rate
0.828611; VP256 rate 1.657187; recurrence latency 21.765236 TSC ticks.
Mac has 19/22 runs complete and remains gated between runs. The latest
collector selects default128 10.28 B/cycle, 161.84 small cycles on Mac;
Xeon 8.16 B/cycle, 165.06 small cycles. These ChainHash repeats are final.

A throughput-probe concern was found: Mac eight-chain rate approximately equals
latency/8, so it may be limited by dependencies. benchmarks/probe.cpp now
measures 16 and 24 ARM chains as well; evidence/probe-original.cpp preserves
the old source. Helper benchmarks/refine_probe_mac.py is ALREADY RUNNING in
tool session 62512, waiting for all 22 Mac hashes before gated build + short
probe. Do not start a second copy. Log evidence/mac-probe-refinement.log.
It writes probe-refined.json and probe-refinement-complete.json in M2Pro.
collect.py --require-complete now also requires that plateau check. Original
Mac probe.json remains intact. REPORT/README explain the refinement.
