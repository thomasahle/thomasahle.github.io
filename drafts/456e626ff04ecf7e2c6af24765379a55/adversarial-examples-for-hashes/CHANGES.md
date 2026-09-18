# ChainHash v3 page pass — 2026-09-18 UTC

Updated the live article in place. Before editing, saved
`index.html.bak-20260918T233900Z` and `data.json.bak-20260918T233900Z`.
The applicable integration rules were in `figure/INTEGRATION_RULES.md`;
there was no root `INTEGRATION_RULES.md`. Followed those rules and
`figure/README.md`.

- Added `chain-v3`, “ChainHash v3 (ours),” as the only 64-bit ChainHash
  chart point on both hosts: Xeon 28.31 B/cycle, M2 Pro 26.26 (median of
  three), at ≥63 bits under model A. Included the 64-byte random key,
  39-word paper model, construction, PolymurHash lineage, evaluation
  independence, Frobenius root-count scope and running Lean status.
- Made the original paper and x86 v2 rows chart-ineligible, preserving
  their numerical data and both existing HTML table rows byte-for-byte.
  The entire ChainHash-128 data row and its HTML rows are unchanged.
- Updated the profile, introductory discussion, reading notes, key-size
  discussion, speed section, recommendations, appendix and TOC. v3 is the
  shipped default in `chainhash3.h`; the original remains in `chainhash.h`.
  Explicitly stated that digests changed. Removed obsolete host-layout
  explanations. Preserved four main paragraphs before and after the chart.
- Added v3 to both tables, including its 1 GB bound: model A uses
  `(2^22+32)/2^64`, approximately 42.0000 bits. Short-input costs are explicit:
  155.14 cycles/hash on Xeon and 87.49 on M2, versus original-function
  controls of 103.00 and 72.57. Historical rows retain historical timings.
- Archived the benchmark report, measurements and controls, specification,
  theorem, changelog and design memo in [records/chainhash/v3/](records/chainhash/v3/).
  Added supporting files for relative document links and all 29 referenced
  raw logs. Verified supplied raw hashes. The changelog source was at the
  ChainHash repository root, not in `docs/`. Private command paths were
  normalized; numeric records, binary hashes and raw logs are unchanged.
- Updated the builder’s ChainHash landmark IDs and labels, regenerated all
  24 SVG/PNG layouts, inspection data and feature images, and advanced the
  page’s figure asset revision. The existing renderer, host controls,
  selector, keyboard help and fallback remain in use.

Validation:

- `python3 figure/build.py` passed all layouts and annotation checks.
- Generated data has `chain-v3` at (26.26, 63) on M2 and (28.31, 63) on
  Xeon, with neither historical 64-bit ChainHash variant plotted.
- Playwright at 375, 768, 1200 and 1620 pixels passed both host views,
  animation, proved and claimed profiles with expanded notes, keyboard
  navigation, TOC navigation, table-cell checks, and horizontal-overflow
  checks. Also inspected the sampled XXH3-64 profile and expanded notes.
  No-script image fallback and reduced-motion switching passed.
- No browser page errors or asset failures with analytics stubbed. The
  initial run’s only network failures were aborted Google Analytics beacons.
  Screenshots of both host charts, v3 profiles and appendix were visually
  reviewed across the four widths.
- New local HTML and Markdown links resolve. New records contain no private
  filesystem paths or tool attribution. Historical table rows were compared
  against the backups. Source/profile hashes match the generated manifest.

[Browser results](records/chainhash/v3/visualqa/checks.json) ·
[Reproducible Playwright check](records/chainhash/v3/visualqa/check.cjs) ·
[Screenshots](records/chainhash/v3/visualqa/).
