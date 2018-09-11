#!/bin/sh
python3 -m pip install --upgrade pip
mkdir compiled

python3 render_html.py templates/index.html > compiled/index.html
python3 render_tex.py templates/cv.tex > compiled/cv.tex

cd compiled
pdflatex cv.tex
cd ..

scp compiled/index.html ssh.itu.dk:~/public_html/
scp compiled/cv.pdf ssh.itu.dk:~/public_html/
scp -R static/ ssh.itu.dk:~/public_html/

rm compiled/*
rmdir compiled

git add .
git commit
git push

