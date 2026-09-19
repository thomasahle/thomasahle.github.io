# Publication figure

`build.py` reads the article's canonical `../data.json`. It generates both host
views, linear, square-root, quadratic, and logarithmic score options, separate phone layouts, PNG/SVG
assets, the inspection record,
and the article's `feature.svg` / `feature.png`.

```sh
python3 blog/adversarial-examples-for-hashes/figure/build.py
```

The build requires Matplotlib. It refuses overlapping annotation boxes,
annotations covering other points, and data outside the explicit axis limits.
Regenerate after changing measurements. If these checks fail, adjust the
hand-placed landmark annotations or axis limits; never move scientific points.

The browser imports these same SVG files; `chart.js` adds point inspection,
keyboard navigation and host selection. The score-scale selector is hidden. `chart.css` styles only the controls.
No plotting CDN is needed. The HTML includes a static image fallback.

Both hosts share fixed axes within each scale. Speed uses a logarithmic axis.
The published chart defaults to square-root spacing, with ticks showing the
original bit scores. The Y-axis controller is hidden. The retained “Logarithmic”
assets use
base-2 logarithmic spacing above 1 bit and a linear segment from 0 to 1
(Matplotlib `symlog`, `linthresh=1`, `linscale=0.5`). Ticks show the original
scores: 0, 1, 2, 4, 8, 16, 32, 64, 128. Never drop zero-score results or replace
them with a positive epsilon. This is a display transform of the bit score,
which is itself logarithmic; no scientific value is changed.

Square root uses position proportional to √score; quadratic uses score². Both
retain zero and keep 0 and 128 at the same display heights as the linear view.
Ticks always report bits, not transformed values. Power-scale labels start
from the linear layout and are fitted in display coordinates with fixed
clearance from other labels and points; final overlap checks still apply.

Host and score-scale changes animate the same point IDs and their
landmark labels over 720 ms; leader lines move with them. Results unavailable on
the other host fade in/out. Interrupted transitions resume from their visible
positions, and reduced-motion preferences disable animation. The settled frame
uses the generated SVG, with no change to its data or point positions.

The browser defaults to `m2-sqrt.svg` / `xeon-sqrt.svg`. Unsuffixed `m2.svg` /
`xeon.svg` retain the linear exports; other views use `-quadratic` or `-log`,
followed by `-mobile` / `-compact` when needed. The builder generates all 24
layouts and keeps `feature.svg/png` on the default square-root M2 view.
The HTML wraps controls and chart in `.figure-stage`; keep `#score-scale` and
`.figure-hosts` intact; keep the `.figure-scale` label hidden and its selected
option set to square root. Host controls sit in the upper-right corner on desktop and
below the heading on smaller screens. The webpage hides the SVG’s static host
heading where controls replace it; standalone figures and print retain it.
Download links are intentionally absent from the page.

Labels show hash names only, with an asterisk for sampled caps. Scores and key
models remain in point accessibility text, hover details, and hash profiles.
Labels are deliberately selective. All eligible measurements remain plotted and
available through the selector. Hollow markers are unresolved claims; solid teal
markers are audited lower guarantees, and rust markers are witness upper caps.
M2 uses the corrected benchmark records. Bounds and key models come from the current scientific data; GHASH timings use an OpenSSL GMAC proxy.

`profiles.json` contains sourced authorship, design background, original project
links, and short result explanations for every plotted variant. Its `sources`
paths are relative to the article; upstream links describe the original project,
while each row's `code_url` identifies the implementation studied in this post.
Edit profiles here and rerun `build.py` to update the inspector. The build checks
that every plotted variant has a profile and a result explanation, and records
the profiles file's SHA-256 alongside that of the scientific data. Profile text
does not change chart coordinates, measurements, or bound classifications.

## Reader-facing prose

Profiles are authored explanations, not copies of scientific audit records.
Keep their three jobs distinct:

- `background`: introduce the hash's purpose, history and relevant variant.
- `results[id]`: explain the finding and its practical meaning in ordinary
  language. State important limitations without burying the finding.
- `reader_notes[id]`: explain the evidence, key choice and scope separately.
  These supply the inspector's expandable notes. Every variant needs all three
  fields (`evidence`, `key`, `scope`); the build rejects missing explanations.

Do not replace these fields with `qualification`, `key_model`, proof-status
strings or theorem text from `../data.json`. Do not repeat the background as the
result. A non-specialist should understand each profile without knowing the
hash already. Explain terms such as lanes, wrappers or model A when needed;
prefer the actual assumption (“160 independently random key bytes”) to an
internal model name. Keep exact formulas and implementation details in the
linked appendix/specification. Retain authorship, original project links,
version limits, proof-versus-code distinctions and key-distribution caveats.

When scientific records change, update these explanations deliberately and
rebuild; never synchronize prose by copying audit notes. Check at least one
proved, claimed and measured profile in the browser, including the expanded
notes. Keep the main article at four paragraphs before and four after the chart;
the remaining sections are extra reading with plain-language introductions.

The article must load `figure/chart.css` and `figure/chart.js`; root-level
`chart.css` / `plot.js` belong to a different renderer and must not replace these
references during a content update. After scientific updates, regenerate with
`figure/build.py` so generated assets, point positions, classifications, and profiles
stay synchronized. Output-width drop lines and dashed reference lines are not
part of this design.

The builder also refreshes the existing historical-pair context paragraph from the selected XXH3-64 record, keeping the current rate and its relative provenance link synchronized. The controls, inspector, static fallback, caption and surrounding notes retain their structure.
