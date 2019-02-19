#!/bin/sh
mkdir compiled

python3 render_html.py templates/index.html > compiled/index.html
python3 render_tex.py templates/cv.tex > compiled/cv.tex

cp postdoc/statement.tex compiled/
cp postdoc/statement.bib compiled/

cd compiled
pdflatex cv.tex

pdflatex statement.tex
bibtex statement.aux
pdflatex statement.tex
pdflatex statement.tex
cd ..

