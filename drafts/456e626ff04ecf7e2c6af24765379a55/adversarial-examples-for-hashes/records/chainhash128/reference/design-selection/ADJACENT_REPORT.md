# ChainHash-128 adjacent — measurements in progress

The specification and Xeon comparison are complete. Both designs have
passed 12,000 random inputs against an independent bit-serial oracle on
Xeon and M2, including the latest overlapping-LD2 ARM A kernels. Cross-host
vectors match. All requested Sanity tests and product-stage assembly audits
pass. The three-pass final M2 timing sequence is in progress.

Both ideal-key families have the bound `(n+2)/2^128` and score
`128-log2(3) = 126.4150374993` bits. B uses independent PH64 keys and injects
the 64-bit length into both lanes, giving the exact block bound
`<= (2^-64)^2 = 2^-128`. See [SPEC.md](SPEC.md).

Two complete Xeon passes are finished: B reaches 8.90 B/cycle versus 7.03
for the fastest A variant, a 26.60% advantage. Three final M2 passes remain.
The Mac waits before every run for no SMHasher3
process and load1 <4.5. No gate is bypassed. The JSON records completed
observations and explicitly marks the M2 host incomplete.

Pilot measurements preceding the C++ registration type-isolation fix or
the latest ARM A kernel are excluded from final aggregation and retained
under `evidence/pilot`.
`REPORT_strided.md` preserves the supplied historical report.

The final report is generated after `benchmarks/collect.py --complete`
validates every run and provenance record.
