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
M2 timings remain explicitly provisional. Current data values are preserved.

`profiles.json` contains sourced authorship, design background, original project
links, and short result explanations for every plotted variant. Its `sources`
paths are relative to the article; upstream links describe the original project,
while each row's `code_url` identifies the implementation studied in this post.
Edit profiles here and rerun `build.py` to update the inspector. The build checks
that every plotted variant has a profile and a result explanation, and records
the profiles file's SHA-256 alongside that of the scientific data. Profile text
does not change chart coordinates, measurements, or bound classifications.
