# UMASH bound update — 2026-09-19

Updated the live copy in place. Backups taken before editing:

- `index.html.bak-20260919T004649Z`
- `data.json.bak-20260919T004649Z`
- `figure/profiles.json.bak-20260919T004649Z`

## Content and data

- Updated the article, both UMASH table rows, 1 GB details and hover titles, canonical certified fields, and figure profiles to the reviewed 53.38-bit envelope. Both length conventions attain the score at L = 2.
- Recorded A = 3125/(2^64−561), the separate short-input bound, and the coarser 423·⌈L/512⌉/2^61 form. The 53.38 score comes from the tighter envelope, which UMASH-128 also inherits by event inclusion.
- Updated 1 KB / 1 MB / 1 GB collision-bound values to 52.36 / 47.93 / 37.9999 bits and the crossover to L = 5633 words (about 44 KB).
- Recorded PH-differing constant 1123, ENH-only constant 3125, both reviewed joint closures (<2^-105 and <2^-90), and the sole remaining sharp-primary gap: C = 3125 versus sufficient C ≤ 255 (12.25×, 3.6 bits).
- Explained that PROOF2's reviewed tag-only bound discharges PROOF3's recorded dependency. Kept the independent 46.96-bit fallback without that import explicit.
- Linked the proofs, reviews and checks from all three rounds using relative paths. Historical proof/review records retain their original statements; the article explains their combined current status.
- Retained the hollow 55/83 claim markers, measurements and coordinates. The quadratic 83-bit claim remains neither proved nor refuted. The reference-model, key-distribution and incomplete Lean/production-code verification qualifications remain explicit.
- Wrote [one consolidated issue #40 comment](ISSUE40_COMMENT.md), with relative links to the public post's records. This is a draft; it was not posted.

## Build and verification

- `python3 figure/build.py` passed, regenerating 24 SVGs, 24 PNGs, `figure/data.json`, and `feature.svg/png`. The builder's annotation and axis checks passed.
- Verified the figure markup, non-UMASH table rows, unrelated canonical data, all claimed chart scores, and all benchmark measurements against the backups; they are unchanged.
- Exact-rational arithmetic reproduced the least integer coefficient 423 and the crossover between 5632 and 5633 words. Computed score: 53.382991772348035; collision bits at 1 KB / 1 MB / 1 GB: 52.36111161774774 / 47.93279675114643 / 37.99993282084113.
- Checked for superseded scores, constants and open-case wording in the page, canonical data, profiles and generated inspection data. Remaining `604` matches are unrelated geometry, hashes or measurement digits; `37.99` occurs only as the prefix of the new `37.9999` value. PH+ENH and two-word mentions describe closed cases.
- Checked that the new article and issue-draft record links resolve locally and use relative paths.
- Playwright at **375, 768, 1200 and 1620 px** checked both hosts, animation, both UMASH profiles and expanded notes, a proved and a measured profile, hover, keyboard navigation, TOC navigation, pinned bounds, asset loading and horizontal overflow. Also checked reduced motion and the no-JavaScript static fallback. Screenshots were visually inspected.
- Initial browser runs completed the functional checks but reported analytics beacons aborted at page close. The final harness replaces only the external tracking script with an empty response; page and chart assets load normally.

The reproducible [browser script](records/umash-corrected/page-pass/check.cjs), [check report](records/umash-corrected/page-pass/checks.json), and viewport screenshots are under `records/umash-corrected/page-pass/`.
