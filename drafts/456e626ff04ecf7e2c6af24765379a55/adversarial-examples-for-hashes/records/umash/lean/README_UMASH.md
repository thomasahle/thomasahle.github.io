# UMASH Lean lane

This is a partial formalization. The authoritative completion ledger is
`STATUS.md`; `UMASHObligations.lean` defines missing propositions and does
not assume them. Numeric inequalities in `UMASHConstants.lean` are arithmetic
certificates, not proofs of collision probabilities for the corresponding cases.

The workspace was copied with `cp -a` from `<xeon-work>/lean-classic` to
`<xeon-work>/lean-umash`, retaining its `.lake` build cache and its existing
`.lake/packages` symlink into `<xeon-work>/lean-hash/lean/.lake/packages`.
Lean is 4.24.0; Mathlib is pinned by the manifest to
`f897ebcf72cd16f89ab4577d0c826cd14afaafc7`.

On the designated Xeon, run `bash reproduce.sh`. All Lean and certificate
computation uses `nice -n 10 taskset -c 56-63` and `LEAN_NUM_THREADS=8`.
The Mac mirror omits `.lake` and compiled artifacts. To reproduce from the mirror,
copy a parent lane with its cache into another Xeon workspace and transfer the
sources there; do not rebuild Mathlib. No build is run on the Mac.

`check_incremental.py` builds each `-- CHECKPOINT` lemma prefix and restores the
complete source even on failure. Its logs are under `logs/`. Regeneration of the
prime and mask certificate source is optional: `generate_prime.py` (SymPy) and
`generate_masks.py` (Python standard library) run on the Xeon. Generated values
are witnesses; only the Lean kernel proofs establish their properties.

`CheckModel.lean`/`check_model.py` compare the literal model to the supplied
Python reference at 20 boundary-sensitive lengths with three word/key profiles,
including wrapped additions and multipliers near p. Both output components use
separate multipliers. This is a smoke check, not a formal equivalence proof
or an experiment estimating collision rates.

The 34 OH words are uniform and independent in block-case propositions. The
full hash uses the uniform subtype of pairwise distinct words and independent
uniform polynomial multipliers in `{2,...,p-1}`. No information-theoretic claim
about Salsa20 expansion or production key preparation is made.

`hash128` uses two multipliers, matching the C architecture. The separate
`referenceFingerprint` definition deliberately models the Python API's shared
multiplier; no 83-bit theorem is asserted for that variant.
