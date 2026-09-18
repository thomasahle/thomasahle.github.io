# Three ecosystem hash rows

The canonical scientific rows are in [data.json](../../data.json): `go-maphash`,
`abseil-hash`, and `dotnet-marvin`. Each links its standalone verification package.

[Timings](speeds_new_hashes.json) are a snapshot of completed runs from the timing
lane. At integration, each new hash had one completed Xeon 8375C run. The lane's
status remains `running`; these are not finished two-run aggregates. M2 timings
were not available and remain blank. No other host's timing was substituted.
Go's 25-bit score is specific to amd64 AES and must not be plotted with ARM64 speed.

The snapshot retains backend names, binary hashes, selected metrics, run dates,
load records and checks. Raw timing and sanity outputs are under `evidence/`.
In `verification.txt`, workstation paths were replaced by input basenames;
measurement results were not changed. Implementation verification is not a full
SMHasher3 suite verdict, which remains unavailable for these rows.

[Browser checks](qa/report.json) cover 375, 768, 1200 and 1620 pixels, both host
views, profiles and expanded notes, keyboard navigation, TOC targets, asset
loading and the static no-JavaScript fallback. The screenshots show the Xeon view:
[375](qa/375-xeon-chart.png), [768](qa/768-xeon-chart.png),
[1200](qa/1200-xeon-chart.png), [1620](qa/1620-xeon-chart.png).
The [motion report](qa/motion-report.json) records animated host changes and reduced motion.

The local verification smoke runs are [Abseil](qa/abseil-smoke.txt) and
[Marvin](qa/marvin-smoke.txt). Abseil checks all three pairs under all 32 table
seeds and 2^16 sampled seeds; Marvin checks its vectors, recorded collision seeds
and a 2^20 sample. These smoke tests do not replace the larger audited experiments.
