# UMASH headline page verification

Both headline updates were checked in the live article with Chromium/Playwright
at widths 375, 768, 1200 and 1620 pixels. See [checks.json](checks.json) and
[check.cjs](check.cjs). Screenshots accompany this record.

At every width, both host views have solid proven UMASH markers at 56.18 and
83.99. Checks cover both expanded UMASH profiles, a further proved profile,
an unresolved SipHash profile and a measured profile, hover, keyboard controls,
host animation, TOC navigation, no page overflow, and the certified 1 GB column.
The no-JavaScript static fallback and reduced-motion host switch also pass.
No browser, asset, console or network errors were reported. The analytics
script was replaced with an empty response during the checks.

Run a static server at the website root on port 8765, then run `node check.cjs`.
Set `PLAYWRIGHT_MODULE` to the installed Playwright module if it is outside this
checkout; `PAGE_URL` can override the local article URL.

The chart build generated all 24 SVG and 24 PNG views, the inspection manifest,
and feature SVG/PNG. Its annotation/coordinate checks passed without changes to
`build.py` or `LABELS`. Scientific coordinates and timing records were preserved
except for the two requested UMASH scores and proof classifications.
