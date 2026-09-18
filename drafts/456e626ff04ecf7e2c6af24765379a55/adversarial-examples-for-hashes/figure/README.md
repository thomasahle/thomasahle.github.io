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
keyboard navigation, host selection, and a score-scale selector. `chart.css` styles only the controls.
No plotting CDN is needed. The HTML includes a static image fallback.

Both hosts share fixed axes within each scale. Speed uses a logarithmic axis.
The score defaults to linear (0–128 ticks). Choosing “Logarithmic” uses
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

The default assets are `m2.svg` / `xeon.svg`; alternate views use `-sqrt`, `-quadratic`, or `-log`,
followed by `-mobile` / `-compact` when needed. The builder generates all 24
layouts and keeps `feature.svg/png` on the default linear M2 view.
The HTML wraps controls and chart in `.figure-stage`; keep `#score-scale` and
`.figure-hosts` intact. Controls sit in the upper-right corner on desktop and
below the heading on smaller screens. The webpage hides the SVG’s static host
heading where controls replace it; standalone figures and print retain it.
Download links are intentionally absent from the page.

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

The article must load `figure/chart.css` and `figure/chart.js`; root-level
`chart.css` / `plot.js` belong to a different renderer and must not replace these
references during a content update. After scientific updates, regenerate with
`figure/build.py` so generated assets, point positions, classifications, and profiles
stay synchronized. Output-width drop lines and dashed reference lines are not
part of this design.
