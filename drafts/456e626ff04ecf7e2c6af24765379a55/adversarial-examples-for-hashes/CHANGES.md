# Final rows pass — 2026-09-18

Updated the live working copy in place. Initial backups are `index.html.bak-20260918T195703Z`, `data.json.bak-20260918T195703Z`, and `figure/profiles.json.bak-20260918T195703Z`; the original builder is also saved as `figure/build.py.bak-20260918T195703Z`.

## Rows and evidence

- Added **HalftimeHash24 (fixed by us)** to Table 2 and both chart hosts: 192-bit output, requested **83.27-bit** label for `6804·2^-96`, Xeon **14.12 B/cycle / 79.42 cycles/hash**, M2 **5.55 / 48.63**, three-run medians. Included control ratios, the distance-three encoder, terminal length word, old witness's zero collisions in `2^36` keys at each width, and our not-yet-upstream `fixed-24byte-core` provenance. The source distinguishes the exact 83.2678325743-bit collision-bound value from its stronger **96-bit normalized score** and stack-safe bound; the appendix makes this distinction explicit while retaining the requested plot label. The construction badge is **✓ construction (M1–M4)**; executable refinement is still partial. The shipped 24-byte refutation remains untouched in Table 1.
- Added **ChainHash v2 (ours), 1 KB blocks**, adjacent pairing, to both chart hosts: Xeon **24.86 B/cycle / 104.34 cycles/hash** (best of two, independently by metric), M2 **11.19 / 89.85** (median of three accepted runs). The ideal bound is `(max(1,ceil(L/128))+2)/2^64` with 137 independent key words. Model A uses 80 random bytes; the row retains a conservative **≥61-bit** model A guarantee. The appendix supplies the adjacent-pair root-degree argument for this conservative envelope, distinguishes it from the ideal 62.415-bit result, includes the instruction-count comparison, and explains the optional batching and changed digests. Lean badge: **✓ (paper function); v2 re-indexing pending**.
- Renamed the original 256-byte strided row **ChainHash (paper)** and retained every numerical cell and all timing metadata. It remains in Table 2 and the accessible table, but is unplotted: each host has exactly one 64-bit ChainHash point, v2. The new M2 report was already present, so polling/fallback was unnecessary. Its same-binary paper control is 22.29 B/cycle; this control does not replace the existing paper-row timing of 22.80.
- Added **ChainHash-128 (ours), 512 B blocks** with strided pairing, both hosts, **≥125 bits · model A**, 160 random bytes / 20 64-bit words (82 expanded words), and ideal `(n+2)/2^128` / 126.415-bit scope. Xeon **8.16 B/cycle / 165.06 cycles/hash**; M2 **10.28 / 161.84**. The optional adjacent report existed but still said measurements in progress and marked M2 incomplete; it supplied no completed recommended default on both hosts. Therefore the requested strided fallback was selected. Its snapshot is retained under [records/chainhash/128-adjacent/](records/chainhash/128-adjacent/REPORT.md). The requested **— not checked (Lean lane queued)** badge remains, with a linked update explaining that the lane has since started and its end-to-end builds/audits remain incomplete.
- Poly1305 and GHASH now say **proved (audited)** and **✓ checked (ideal key)**. Scores remain **103** and **127**; GHASH explicitly uses the single-stream AAD-only theorem, with **126.4** reserved for the cited two-stream envelope. Each row states the 64-bit-seeded-wrapper caveat; the benchmark paragraph states it once for all seeded wrappers. The audit's seed-zero GHASH collision is linked. Existing timings are unchanged.
- Updated completed Lean lanes: CLHASH, Polymur cardinality, BRW/eight-lane recurrence, multiply-shift, paper ChainHash/model A integration, and HighwayHash. The BRW lane's `STATUS.md` is COMPLETE with the lead's accepted whole-bit-floor interpretation, although its copied `LEAN_BRW_STATUS.md` retains the earlier literal-exact-score PARTIAL wording; row links use `STATUS.md` and state the correct scope. UMASH remains partial. The NH/Halftime lane status arrived during integration: integer NH Task A is checked, but the combined lane remains partial because executable refinement is unfinished; row scopes state both facts.
- **HighwayHash source-status change:** both the supplied `lean-highway/LEAN_HIGHWAY_STATUS.md` and the later COMPLETE `lean-goal/highway-bridge/STATUS.md` now prove the uniform-key bridge, exact `56165·2^184` full-key count, all three output bounds, and score enclosure. Following the later instruction to honor COMPLETE lanes, the appendix says this is a Lean theorem. It does not repeat the superseded claim that the bridge is unproved. No collision number or Table 1 cell was changed.

Cited evidence is published under [records/](records/) and mirrored as requested under [out/records/](out/records/), including status files and available linked proof/source material. New article links and new-profile links are relative. Existing external citations are preserved.

## Generated chart and preservation

Updated canonical [data.json](data.json) and [figure/profiles.json](figure/profiles.json), then ran from the website root:

```sh
python3 blog/adversarial-examples-for-hashes/figure/build.py
```

Regenerated both hosts' desktop, compact and mobile SVG/PNG views, `figure/data.json`, and `feature.svg` / `feature.png`. Each host has **38 plotted results**. The builder's landmark annotation positions were adjusted to fit the new rows; both ChainHash annotations identify model A. Scientific point coordinates and axis limits were not shifted. The M2 CLHASH landmark was removed to make space; its point remains plotted and inspectable.

The figure section's markup, host controls, selector, inspector, keyboard help, fallback, caption and notes are unchanged. The page still loads `figure/chart.css` and `figure/chart.js`. No renderer, page script or stylesheet was added or edited. Existing scientific fields (`speeds`, `bits`, `score`, `score_lower_guarantee`, `output_bits`, `bound`, `key_words_64bit`, `smhasher3`) compare equal for every original row. Table 1 is unchanged. Original Table 2 cells outside the explicitly requested names, Lean labels, audit status and scope changes are preserved.

## Verification

- `figure/build.py`: PASS; built-in checks reject overlapping annotation boxes, labels obscuring other points, out-of-range data, missing profiles and missing result explanations.
- Playwright at **375, 768, 1200 and 1620 pixels**, both hosts: PASS. Checked all new and classic profiles, exactly one 64-bit ChainHash point per host, keyboard Enter/Escape/arrow navigation, animated and interrupted host changes, TOC targets including new appendix entries, and static fallback with JavaScript disabled.
- **Zero console/page errors, failed requests or HTTP asset errors.** Analytics was disabled in the local QA browser. No document overflow or label overlaps at any checked width. New relative article links resolve.
- Compared figure markup, script tags and stylesheet tags against the original backup; all unchanged. Visually inspected generated figures and responsive screenshots.
- Results and screenshots: [out/final-rows-qa/checks.json](out/final-rows-qa/checks.json), [375 px](out/final-rows-qa/375-m2.png), [768 px](out/final-rows-qa/768-m2.png), [1200 px](out/final-rows-qa/1200-m2.png), [1620 px](out/final-rows-qa/1620-xeon.png).

No commit or deployment was made. This pass integrates the supplied proof reports; it does not rerun Lean builds or benchmarks.
