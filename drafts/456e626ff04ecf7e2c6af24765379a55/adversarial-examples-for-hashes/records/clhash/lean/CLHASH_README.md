# CLHASH lane — unverified draft

Read `../STATUS.md` before treating any result as proved. The assigned CPUs
104–111 do not exist on the supplied host, which exposes CPUs 0–95.
The first build failed in `taskset`, before Lean started.

The parent project's sources, cached build, and existing audits were copied
with `cp -a`, including its 6.3 GB `.lake` directory. Do not rebuild Mathlib.

New files:

- `CLHashBytes.lean`: adjacent pairs, 128-word blocks, active tail pairs,
  byte encoding, and draft CLNH stream proofs.
- `CLHashModulus.lean`: the exact polynomial `X^127 + X + 1`.
- `generate_clhash_certificate.py`: creates `CLHashCertificate.lean` on the
  Xeon; a single theorem checks 127 Frobenius squaring identities and a
  Bezout identity. The generator has not yet run.
- `CLHashField.lean`: draft irreducibility and polynomial-basis field proofs.
- `CLHashAlgorithm.lean`: the ideal hash itself, lazy polynomial accumulation,
  final CLNH product, length product, and explicit requested propositions.
- `MakeCLHashAudit.py`: prepares signatures and `#print axioms` for every new
  declaration. Its output has not been generated or checked.

After an available CPU allocation is supplied, on the Xeon:

```bash
cd ~/agents/lean-clhash
# Set CPUSET to the replacement allocation supplied by the lane owner.
CPUSET="$CPUSET" bash lean/build.sh
```

The default remains `104-111`; the script fails immediately when that set is
unavailable. It always uses `nice -n 10` and `LEAN_NUM_THREADS=8`.
The sources still need debugging and the probability/composition proofs still
need writing. A successful source build alone would not establish
`ProvenHashes.CLHash.CollisionBound`.

`Audit.txt`, `FullAudit.txt`, `ChainHashAudit.txt`, and `BuildVerification.txt`
are inherited evidence for the parent ChainHash lane. They do not audit or
validate the CLHASH additions.
