# Reproduce the corrected projection proof

The proof is in [../PROOF.md](../PROOF.md). These programs use exact integer
counts and Python `Fraction`; no floating-point estimate certifies a bound.
Dependencies are Python 3 (standard library), a C++17 compiler with OpenMP,
`nice`, and `taskset`.

From the editing workspace, transfer the checks and run on the specified Xeon:

```bash
rsync -a ./checks/ thomas-ahle@hardware.normalcomputing.net:agents/umash-projection-goal/checks/
ssh thomas-ahle@hardware.normalcomputing.net \
  'cd ~/agents/umash-projection-goal && bash checks/run_xeon.sh > checks/run.log 2>&1'
rsync -a thomas-ahle@hardware.normalcomputing.net:agents/umash-projection-goal/checks/ ./checks/
```

`run_xeon.sh` refuses to run on another host. Each computation uses
`nice -n 10 taskset -c 56-63`; OpenMP is capped at eight threads, and
address space at 32,000,000,000 bytes. The jobs run sequentially.

## Certificate contents

| File | What it certifies |
|---|---|
| `triangular.py`, `triangular.json` | Signed-support recurrence; all 852 real-word masks and their admissible patterns; ENH and PH constants; exact all-message rounding; 84 explicit scaled fibre checks covering 1,489,152 key pairs. |
| `certify_constants.py`, `constants.json` | Independent carry-automaton enumeration; every small-model congruent pair and pattern at the six declared parameters; exact rational inequalities; full factorization/order certificate for primality of `2^61-1`, with witness 37. |
| `exhaustive_lift.cpp`, `exhaustive_lift.json` | All 3,624 minimum-valuation increment pairs at widths 3--6, all operand pairs, and three tag/mask profiles: 35,779,968 pair/profile visits; 175,758 projected-collision visits; zero failures. |
| `manifest.py`, `manifest.json` | SHA-256 hashes of source and result files; Xeon hostname, affinity, niceness, thread cap, memory limit, Python and compiler versions. |
| `run.log` | Actual exact-certificate and exhaustive-check output. |

The C++ check's three profiles are `(tag,tag',M_high)=(0,0,0)`,
`(q-1,0,floor(q/3))`, and `(q/2-1,q/2,q-1)`. It checks all increments
subject to `delta!=0` and `v2(delta)<=v2(epsilon)`, treating zero epsilon
as infinite valuation. Swapping operands covers the other orientation.
It checks every actual low-equal high-projected event in these profiles
against both the necessary congruence and uniqueness of its fibre.

The 84 Python cases use the seven increment pairs listed literally in
`triangular.py`, at widths 4, 5, 6, and 8, with the same three profiles.
Some smallest-width parameter choices repeat a pair; the advertised
1,489,152 count is visits, not a count of distinct parameterized key pairs.

The small enumerations validate the algebra; they do not extrapolate a
64-bit collision frequency. The 64-bit proof follows from the uniform
fibre lemma and the complete finite mask count. No counterexample is
claimed. `SharpPrimaryProjection` and `OpenPHENH` remain open.
