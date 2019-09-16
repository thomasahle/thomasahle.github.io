#!/bin/sh
mkdir compiled

python3 render_html.py data templates/index.html > compiled/index.html
cp compiled/index.html .

python3 render_html.py pcpp_data templates/pcpp.html > compiled/pcpp.html

python3 render_tex.py templates/cv.tex > compiled/cv.tex
python3 render_tex.py templates/cv_simons.tex > compiled/cv_simons.tex

cd compiled
pdflatex cv*.tex
pdflatex cv_simons.tex
cd ..

