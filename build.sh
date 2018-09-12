#!/bin/sh
mkdir compiled

python3 render_html.py templates/index.html > compiled/index.html
python3 render_tex.py templates/cv.tex > compiled/cv.tex

cd compiled
pdflatex cv.tex
cd ..

