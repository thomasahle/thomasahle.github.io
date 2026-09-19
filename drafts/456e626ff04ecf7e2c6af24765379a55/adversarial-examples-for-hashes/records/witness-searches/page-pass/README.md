# Page-pass validation

[Change log](../../../CHANGES.md), [responsive and interaction checks](visualqa.json), [scientific consistency checks](consistency-checks.txt), [Xeon package build](package-build-xeon.txt), and [deterministic make check](make-check-xeon.txt).

`visualqa.cjs` uses Playwright from the provided `scratchpad/codex/visualqa/node_modules` via `NODE_PATH`. Serve the website root locally and set `QA_BASE` if its URL differs from the default. PNGs alongside this file show the actual page at 375, 768, 1200 and 1620 pixels, both hosts, refreshed browser demos, and the static fallback.
