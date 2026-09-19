# Exact checks for PROOF3.md

All computation runs on
`<xeon-host>:<xeon-work>/umash-goal3`.
From that directory:

```sh
bash checks/run_xeon.sh
```

The script requires Python 3.12 and a C++17/OpenMP compiler. It uses
`nice -n 10 taskset -c 40-47`, eight OpenMP threads, and a 31,250,000 KiB
(32 GB) address-space limit. It refuses to run on macOS. There are no
third-party Python dependencies.

- `certify_primary.py` constructs the production-width mask and function
  tables, checks all numerical upper bounds and exact score inequalities,
  and writes `primary_certificate.json` and `prefix_weights.h`.
- `validate_primary.cpp` checks the new scaled low-XOR/convolution bounds,
  grouped lifting fibres, and additive-product uniqueness. Its exact
  scopes and counts are in `validation.json`.
- `verify_certificate.py` independently reads the certificate, proves mask
  completeness from the count of all congruent pairs, reconstructs the
  function classes, literally checks scaled functions, and checks the
  envelope arithmetic. Its output is `independent_verification.json`.
- `manifest.py` records sources/certificate hashes and the run environment.
- `probe_counts.py` is the exploratory calculation retained for provenance.
  It is not imported by any proof check; in particular its common-PH
  weighted ENH calculation is not needed for the theorem in PROOF3.md.

The production-width numbers are uniform **upper bounds**, not attained
collision counts. Scaled enumerations are exact only over their declared
families. No published-constant counterexample is asserted.
