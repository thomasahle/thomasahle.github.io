# Publication figure

`build.py` reads the article's canonical `../data.json`. It generates both host
views, separate phone layouts, the PNG/SVG downloads, the inspection record,
and the article's `feature.svg` / `feature.png`.

```sh
python3 blog/adversarial-examples-for-hashes/figure/build.py
```

The build requires Matplotlib. It refuses overlapping annotation boxes,
annotations covering other points, and data outside the explicit axis limits.
Regenerate after changing measurements. If these checks fail, adjust the
hand-placed landmark annotations or axis limits; never move scientific points.

The browser imports these same SVG files; `chart.js` adds point inspection,
keyboard navigation, and host selection. `chart.css` styles only the controls.
No plotting CDN is needed. The HTML includes a static image fallback.

Both hosts share fixed axes. Host changes animate the same point IDs and their
landmark labels over 720 ms; leader lines move with them. Results unavailable on
the other host fade in/out. Interrupted transitions resume from their visible
positions, and reduced-motion preferences disable animation. The settled frame
is the downloaded SVG, with no change to its data or point positions.

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
`figure/build.py` so downloads, point positions, classifications, and profiles
stay synchronized. Output-width drop lines and dashed reference lines are not
part of this design.
