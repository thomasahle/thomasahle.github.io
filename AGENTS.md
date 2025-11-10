# Repository Guidelines

## Project Structure & Module Organization
Content for the site is described in Python modules (`data.py`, `blog_data.py`, `pcpp_data.py`) that populate `Vars` objects consumed by the render scripts. Templates live in `templates/`, LaTeX-heavy posts in `tex4ht/`, and raw blog material in `blog/` and `papers/`. Static assets belong in `static/` or `feature_imgs/`, while course handouts stay in `teaching/`. Keep generated `compiled/` output ignored so sources remain canonical.

## Build, Test, and Development Commands
- `pip3 install jinja2` — minimal runtime dependency.
- `bash build.sh` — full site build: renders HTML, converts LaTeX blog posts, copies assets, and produces PDFs under `compiled/`.
- `python3 render_html.py <module> <template> > compiled/<path>.html` — ad-hoc regeneration while editing a single page (e.g., `data templates/index.html`).
- `python3 render_tex.py templates/cv.tex > compiled/cv.tex && (cd compiled && pdflatex cv.tex)` — iterate on CV templates without running the whole build.

## Coding Style & Naming Conventions
Python follows PEP 8 with four-space indentation, descriptive dataclass names, and snake_case attributes to keep template bindings predictable. Keep shared constants on `Vars` to avoid diverging APIs between `data` modules. Template blocks and file names stay lowercase with hyphens (e.g., `templates/blog/post.html`), and image filenames should match the slug referenced in `Vars.papers[*].img`.

## Testing Guidelines
We rely on build success as the primary regression check. Run `bash build.sh` before every commit; watch for LaTeX warnings and verify the newly generated HTML/PDF inside `compiled/`. When touching blog math, open the produced `.html` plus the `.log` files to confirm no missing references. Add representative sample data to the relevant `*_data.py` module so rendering still covers each branch.

## Commit & Pull Request Guidelines
Follow the existing history style: `<area>: concise present-tense summary` (e.g., `Stirling post: cite Robbins (1955)`). Keep subjects under ~72 characters and group related edits logically. Each PR should describe the rendered change, list touchpoints (templates, data modules, assets), include screenshots or PDF diffs, and call out any new build requirements. Double-check that `compiled/` stays absent from the diff.

## Deployment & Asset Tips
`build.sh` writes `compiled/CNAME` for thomasahle.com; leave that string untouched unless the domain changes. Add media to `feature_imgs/` or `static/` and reference them via relative paths so the deployment CDN picks them up automatically. If you add a new LaTeX article, mirror the `tex4ht/build_*` pattern and extend the script so CI agents can reproduce the conversion without manual intervention.
