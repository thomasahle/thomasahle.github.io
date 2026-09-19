# Round-4 exact proof certificates

The result is [../PROOF4.md](../PROOF4.md): primary full-hash coefficient
**58**, below the published 64, in the stated ideal reference key model.
The proof uses the literal reduction modulo 8p and does not prove the
stronger `SharpPrimaryProjection` premise with numerator 162.

Run on the Xeon, from the project root:

```sh
bash checks/run_xeon.sh
```

Requirements: Python 3.12, a C++17 compiler with OpenMP, Linux `taskset`.
No third-party Python packages are used. The script rejects non-Linux
execution, pins every computation and compilation to CPUs 40-47 at nice 10,
uses at most eight OpenMP threads, and limits address space to 32 GB.
All numbers used as certificates are integers or exact rational numbers.
Decimal scores are display values only.

Proof dependencies and outputs:

| Files | Scope |
|---|---|
| `exact_common.py`, `prepare_masks.py`, `mask_certificate.json`, `masks.h` | Complete 64-bit mask/pattern set; disjoint ordered-pair completeness check |
| `ph_low_table.cpp`, `ph_low_table.json` | Every small PH valuation, primitive degree, parity-leading position and valuation cancellation; complete binary-trie maximum |
| `verify_ph_low.cpp`, `ph_low_independent.json` | Independently constructed literal monomial rows and complete explicit-centre maximum for every relevant even prefix |
| `certify_ph_linear.py`, `ph_linear_certificate.json` | One-coordinate PH changes: low and high tables |
| `certify_ph_high.py`, `ph_high_certificate.json` | All 119 degree/type cases, both top bits; impossible target degrees explicitly excluded |
| `certify_enh_weights.py`, `enh_certificate.json` | Exact forced-pattern weights and all 851 nonzero-mask terms for ENH with a common PH product |
| `certify_tail.py`, `tail_certificate.json` | All 56 valuation tables and every 16-residue/wrap/tag context for the weighted final ENH case |
| `verify_certificates.py`, `independent_verification.json` | Independent mask completeness, literal high matrices, weights, affine signatures and brute-force 16-residue solvers |
| `validate_new.cpp`, `validation.json` | Exhaustive scaled low-product counts; literal linearized rows/ranks; wrap-rectangle range checks |
| `certify_envelope.py`, `envelope_certificate.json` | Exact all-message constants, rational score inequalities and boundary checks |
| `manifest.py`, `manifest.json` | Source/artifact SHA-256, machine, affinity, niceness and memory limit |

The completed validation visits 751,618,560 low operand pairs and checks
199,389,024 joint point inequalities, 683,426 exact minimum-valuation XOR
targets, 13,640 dense bounds, 3,719,641 literal linearized rows, 3,183,932
selected ranks and 20,852 wrap rectangles. All reports have zero failures.
The symbolic arguments in PROOF4, not extrapolation from these scaled
checks, establish the production-width probability bounds.

`exploratory/probe_*` and their logs/JSON are exploratory records and are not imported
by the proof scripts. Some preserve deliberately weaker or abandoned
relaxations. They are not counterexample certificates or all-pairs maxima.
Their original dependency was the preceding round's `certify_primary.py`.

The certificate runner was executed on
`<xeon-host>:<xeon-work>/umash-goal4`.
The proofs and results are mirrored locally; the Mac ran no enumerations.
