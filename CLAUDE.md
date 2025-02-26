# CLAUDE.md - Website Development Guide

## Build Commands
- `./build.sh` - Builds the project to the `compiled` directory
- `python3 render_html.py data templates/index.html > compiled/index.html` - Render specific HTML template
- `python3 render_tex.py templates/cv.tex > compiled/cv.tex` - Render specific LaTeX template
- `pdflatex --interaction=batchmode file.tex` - Convert LaTeX to PDF
- `pip install jinja2` - Install required dependency

## Deployment
- Deployment happens automatically via GitHub Actions when pushing to the master branch
- The workflow is defined in `.github/workflows/deploy.yml`
- Manually trigger deployment with the "Actions" tab on GitHub

## Project Structure
- `templates/` - Jinja2 HTML/LaTeX templates
- `data.py` - Main data models and website content
- `blog_data.py` - Blog-specific content
- `pcpp_data.py` - Teaching materials data
- `static/` - Static assets (CSS, JS, images)
- `feature_imgs/` - Images for featured content
- `abstracts/` - Paper abstracts
- `papers/` - PDF files of papers

## Code Style Guidelines
- **Python**: Follow PEP 8 conventions
- **Imports**: Group standard library, then third-party, then local imports
- **Data Models**: Use Python dataclasses for structured data
- **HTML/Templates**: Use Jinja2 templating for all HTML generation
- **LaTeX**: Use custom Jinja2 environment with modified delimiters
- **File Structure**: Maintain clear separation between data, templates, and static files
- **Documentation**: Document all classes with purpose and usage
- **Error Handling**: Use exceptions for unexpected cases and validation