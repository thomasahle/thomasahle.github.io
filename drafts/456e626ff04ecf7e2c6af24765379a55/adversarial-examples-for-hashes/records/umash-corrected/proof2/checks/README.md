# Exact checks for PROOF2.md

Run on `thomas-ahle@hardware.normalcomputing.net`, in
`~/agents/umash-goal2`, with:

```sh
bash checks/run_xeon.sh
```

The script uses `nice -n 10 taskset -c 40-47`, caps OpenMP at eight threads,
and sets an address-space limit of 31,250,000 KiB (32 GB). It needs
Python 3.12 and a C++17 compiler with OpenMP. No third-party Python package
is needed. No enumeration was run on the editing Mac.

- `certify_open_ph_enh.py` is the theorem certificate. It constructs the mask
  set by two complete algorithms, checks patterns against literal scaled
  congruences, enumerates all 60 shuffler/valuation sums, and verifies the
  divisor, tag-only and small-valuation primary constants. Its output is
  `open_ph_enh_certificate.json`; the concise log is `certificate.log`.
- `validate_quadratics.cpp` independently enumerates the new modular
  quadratic bit bound and the actual NH XOR counts at small widths. Its
  complete scope and totals are in `validation.json`. These are algebra
  checks, not a proof by extrapolation.
- `verify_certificate.py` independently checks mask completeness from the
  exact identity `sum |P(d)|*2^(64-popcount(d)) = 8q+72`, recomputes the
  weights as rational numbers, solves the shuffler inverses by geometric
  series, and checks all 60 sums. It also literally tests the quadratic
  interval lemma with both signs of the leading coefficient. The output is
  `independent_verification.json`.
- `ph_enh_candidates.py`, `small_r_candidates.py`, `improve_candidates.py`,
  and `quadratic_candidates.py` record the development of the finite sums.
  The first two approaches failed some thresholds; their bounds are not
  counterexamples. The final certificate is self-contained and imports
  none of these exploratory programs.
- `ph_degree_candidates.py` explores a primary PH degree/pivot relaxation.
  It does not prove the sharp primary bound. Its largest bound for equal
  nonzero coordinate differences is 157943/q, so this relaxation is still
  much too loose.
- `enh_high_probe.cpp` exactly counts a specified width-12 family on the
  complete low-equality grid. Its increments are the nonzero multiples of
  256, with seven common tags and five fixed high XOR offsets. The output
  `enh_high_probe.json` reports maxima only over this declared family. Build
  and run it with the same affinity, niceness and limits as the main script.

The exact theorem is a uniform upper bound over all message pairs satisfying
`OpenPHENH`. No JSON upper bound is represented as an attained collision
count. `SharpPrimaryProjection` remains open.
