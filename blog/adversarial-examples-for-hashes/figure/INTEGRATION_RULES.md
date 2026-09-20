# Chart integration rules

The chart is generated; update `data.json` and `figure/profiles.json`, run `figure/build.py`, never edit the figure section’s markup or add a renderer; the page must keep loading `figure/chart.css` and `figure/chart.js`.

Run from the website root:

```sh
python3 blog/adversarial-examples-for-hashes/figure/build.py
```

`figure/README.md` is the contract. Keep its host buttons, hash selector, inspector, keyboard help, static image fallback, caption and notes intact. Do not add plotting CDNs, output-width drop lines, dashed reference lines, or root-level chart assets.

Every plotted variant needs a sourced profile and result explanation. Unscored or untimed rows stay unplotted; never invent scores, substitute another host’s timing, or shift scientific coordinates. Resolve build checks by adjusting landmark annotations or axis limits.

The build regenerates both hosts’ desktop, compact and mobile SVG/PNG views, `figure/data.json`, and `feature.svg`/`feature.png`. Verify the actual page at 375, 768, 1200 and 1620 pixels, including animation, profiles, keyboard navigation, fallback, asset loading and the TOC.
