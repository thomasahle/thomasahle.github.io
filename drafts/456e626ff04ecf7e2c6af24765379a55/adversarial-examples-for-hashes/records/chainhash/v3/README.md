# ChainHash v3 publication record

This snapshot supplies the page’s ChainHash v3 point on both hosts.

- [Specification](SPEC_v3.md), [written collision proofs](THEOREM_v3.md),
  and [release changelog](CHANGELOG.md) come from the ChainHash repository.
  The changelog was at the repository root; the two specification documents
  were in `docs/`.
- [Design memo](MEMO.md) records the alternatives and adversarial proof review.
  It predates the implementation measurements; its estimates and open next
  steps are historical. Use the specification and measurement report for the
  shipped definition and results.
- [Measurement report](REPORT.md) and [runs and same-binary controls](speeds_chainhash_v3.json)
  come from the ChainHash-Horner benchmark run. Its SMHasher3 measurements,
  rather than its separate standalone harness, supply the chart coordinates.
- [Default header](chainhash3.h), [original header](chainhash.h),
  [schedule check](schedule.c), and [original finalizer derivation](SEEDED_THEOREMS.md)
  support relative links in the copied documents.
- All 29 raw logs referenced by the benchmark JSON are included under `out/`;
  supplied raw SHA-256 values were verified before copying.

Local machine paths in command records were replaced with portable
`experiment/` paths. Document links were adjusted to this archive; the design
memo’s implementation assignment was made tool-neutral. Numeric observations,
aggregation rules, binary hashes and raw logs are unchanged.

The displayed model A score is 63.0 bits, with the Frobenius root-count lemma
included. Both natural-language key-model proofs were reviewed; the v3 Lean
lane is running. This archive does not transfer v1’s completed Lean proof or
200/200 statistical-suite result to v3.

Browser checks and screenshots are in [visualqa/](visualqa/).
