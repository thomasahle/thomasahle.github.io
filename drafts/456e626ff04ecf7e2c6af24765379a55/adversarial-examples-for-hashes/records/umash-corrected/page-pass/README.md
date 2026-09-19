# UMASH page-pass browser checks

Serve the website root on port 8765, then run:

```sh
node records/umash-corrected/page-pass/check.cjs
```

Run from the article directory. Set `PLAYWRIGHT_MODULE` to the installed Playwright module path if necessary, and `PAGE_URL` to override the local page URL. Chromium must be installed for Playwright.

[checks.json](checks.json) records checks at 375, 768, 1200 and 1620 px, both chart hosts, profiles and expanded notes, hover, keyboard controls, TOC navigation, pinned bounds, overflow, page/asset errors, reduced motion and the static fallback. The external tracking script is replaced with an empty local response during the four viewport checks; the actual page and chart assets are loaded normally.

Screenshots use `<width>-m2-chart.png`, `<width>-xeon-chart.png`, `<width>-umash-profile.png`, and `<width>-appendix.png`; `375-static-fallback.png` records the no-JavaScript figure.

The content changes and arithmetic checks are described in [CHANGES.md](../../../CHANGES.md).
