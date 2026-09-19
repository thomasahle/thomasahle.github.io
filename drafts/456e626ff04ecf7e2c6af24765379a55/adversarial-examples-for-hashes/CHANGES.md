# UMASH headline bounds page pass — 2026-09-19

Updated the live article in place. Backups: `index.html.bak-20260919T024930Z`
and `data.json.bak-20260919T024930Z` (UTC).

- `data.json`: both UMASH rows are proven. UMASH-64 is plotted at 56.18 bits,
  with the max-envelope using 205 and 435 and the coefficient-58 headline.
  UMASH-128 is plotted at 83.99, displayed in the table as “≥ 84 bits on
  L ≤ 2^46” with an explicit rounding note. Its headline coefficient is 81/128;
  the finer envelope supplies the domain scores and 75.98-bit 1 GB value.
- `figure/profiles.json`: rewrote both results and their evidence, key and scope
  explanations; linked PROOF4/5, the four-lens verdicts and reproducibility
  checks. Partial Lean labels distinguish the machine-checked 46.52-bit
  envelope and ENH-only closure from the stronger verified paper proofs.
- `index.html`: updated the chart note, both table representations, 1 GB column,
  proof discussion, fingerprint length note, TOC labels, appendix and disclosure.
  Both headlines are proved, while the paper’s 162/q projection step remains
  unvalidated. The appendix explains the mod-8p mechanism, independent
  two-multiplier fingerprint route, closed joint cases, single-multiplier
  Python refutation, ideal full-key scope and Lean status. The old 53.38-bit
  score and 3125/q marginal remain only as proof history/dependency.
- `ISSUE40_COMMENT.md`: one consolidated follow-up draft, with relative links
  to public-post records. It has not been posted; disclosure says pending.
- Regenerated the 24 SVGs, 24 PNGs, `figure/data.json` and `feature.svg/png`
  exclusively through `figure/build.py`. No builder or landmark-label changes
  were needed. The existing renderer, controls, inspector, fallback, caption,
  keyboard help and four-paragraph introduction are retained.

Validation: `python3 blog/adversarial-examples-for-hashes/figure/build.py` passed.
Playwright passed at **375 / 768 / 1200 / 1620** on both hosts: solid markers,
expanded profiles (proved, claimed and measured), hover, keyboard navigation,
host animation, TOC, no page overflow, current 1 GB values and asset loading.
No-JavaScript fallback and reduced motion passed; no browser/network errors.
Screenshots were visually inspected for layout and text wrapping.
[Browser report and screenshots](records/umash-corrected/headline-page-pass/README.md).

Data integrity checks confirmed the generated data/profile hashes, all newly
linked UMASH record paths, and unchanged unrelated scientific rows and UMASH
timing records. No proof or benchmark computations were rerun for this page pass;
the supplied verdicts and records are the scientific basis. The linked Lean
completion report is retained as supplied; the page does not claim that the new
headline results have been machine-checked.
