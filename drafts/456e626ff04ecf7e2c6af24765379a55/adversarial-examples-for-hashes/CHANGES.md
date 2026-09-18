# Changes: Go maphash, Abseil Hash and .NET Marvin

- Backed up `index.html`, `data.json` and `figure/profiles.json` with the UTC suffix
  `.bak-20260918T225319Z` before editing the live copy.
- Appended the three heuristic rows, sourced profiles and reader notes. Added
  both table entries, per-hash appendices, TOC links and the ecosystem-defaults
  paragraph beside the Rust/SipHash discussion.
- Preserved the audited caps: Go 25.0 bits (sample interval [24.61, 25.33]),
  Abseil 0 bits (same-length cap 1 bit), and Marvin 9.91 bits, with 32-bit output.
  Claims, seed models, pseudocode, literal pairs, derivations and scope are linked
  from each appendix.
- Imported the available completed Xeon timings: Go 17.66 B/cycle and 64.32
  small-key cycles; Abseil 8.22 / 20.67; Marvin 0.95 / 31.07. All three are
  `chart_eligible: true`. Each currently has one completed run; the timing lane
  is still running. M2 values remain blank. Go is explicitly Xeon-only because
  its amd64 score cannot be transferred to the ARM64 algorithm.
- Linked all three `verify/<id>/README.md` packages. Retained the Abseil and
  Marvin packages being supplied by the verification lane and packaged the
  audited Go transcription, runtime checker, vectors and measurement log.
- Clarified that byte slices are maphash inputs, not legal Go map keys; that .NET
  dictionaries draw independent seeds; and that five SwissTable seed bits limit
  nonzero collision probabilities, not force every arbitrary pair to collide.
  Go's exact sufficient trail is distinguished from its two sampled checks.
- Preserved all existing scientific rows and profiles, and the figure markup,
  controls, renderer, fallback and surrounding article structure. New content
  uses relative local links and contains no private workstation paths.

Validation: `figure/build.py` passed and regenerated all 24 SVG/PNG layouts,
inspection data and feature images. Data/profile digests agree. Playwright passed
at 375, 768, 1200 and 1620: no horizontal overflow, broken TOC targets, page errors
or failed local assets; profiles, expanded notes, host switching, keyboard controls
and no-JavaScript fallback work. Abseil and Marvin smoke verification passed.
The billion-trial experiments were not rerun during this page integration.

See [timing provenance and QA evidence](records/new-hashes/README.md).
