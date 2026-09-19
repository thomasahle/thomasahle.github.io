# Exact checks for PROOF5

Run on the specified Xeon, from the project directory:

```sh
bash checks/run_xeon.sh
```

The runner pins all computation to CPUs 40--47 with nice 10 and a 32 GB
address-space limit. It uses Python 3.12, GCC, and the supplied unmodified C
source. It needs no external Python packages. Do not run the computations
on the Mac.

- `certify_fingerprint.py`: exact mask and subcase-b counts, imported numeric
  ceilings, exhaustive case-ledger maxima, rational envelope comparisons,
  score minima certificates, and the exact shared-multiplier bad fibre.
- `verify_certificate.py`: independent certificate reader; no generator
  imports. Mask completeness by disjoint word-pair counts, a separately
  expanded quadratic, exact endpoint comparisons, and integer-power scores.
- `c_bridge.c`, `check_c_model.py`: C/reference model comparisons using two
  separate multipliers, all actual PH shufflers, and scaled exhaustive kernel
  checks. These are model checks, not collision-rate sampling.
- `manifest.py`: SHA-256 hashes of artifacts, sources and imported data.

Outputs are `fingerprint_certificate.json`, `independent_verification.json`,
`model_checks.json`, `manifest.json`, and corresponding logs. Shared libraries
are rebuilt by the runner and are not proof certificates.

Probability lemmas from the verified earlier rounds are dependencies with
their hypotheses restated in PROOF5. The new checks do not relabel old table
arithmetic as a fresh proof of those lemmas, and do not re-run their large
exhaustive experiments. Every numerical constant newly used in this round
is checked exactly. Decimal logarithms are display-only.
