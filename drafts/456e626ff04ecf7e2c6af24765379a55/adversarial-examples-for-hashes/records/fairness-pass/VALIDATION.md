# Publication validation

- The shipped HighwayHash C program regenerated run_2p24.txt, run_2p26.txt and run_2p24_sm3.txt. Counts agree with the historical files; only the zero-count description changes.
- The bundled MuseAir cross-version Cargo program passes cargo check with all four path dependencies.
- The bundled aHash 0.8.12 native Cargo program passes cargo check. Its AES measurements are the copied Xeon runs; a local compile check does not repeat the x86 AES measurement.
- The 24 generated figure layouts pass the build's annotation/point-overlap checks.
- Playwright covers 375, 768, 1200 and 1620 pixels, both hosts, measured/claimed/proved profiles, expanded notes, keyboard navigation, host transitions, asset loading, document width and static fallback. See visualqa-results.json.
