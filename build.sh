#!/bin/sh
set -e  # Exit on error

# Create output directories
mkdir -p compiled/blog
mkdir -p compiled/teaching/pcpp2019
mkdir -p tex4ht/build

# Render HTML pages
python3 render_html.py data templates/index.html > compiled/index.html
python3 render_html.py blog_data templates/blog/index.html > compiled/blog/index.html

# Latex blog things
cd tex4ht
# Ensure build directory exists
mkdir -p build
latex --interaction=batchmode --output-directory=build termo_linalg.tex
cd build
cp ../termo_linalg.tex .
cp ../termo.bib .
cp ../myconfig.cfg .
bibtex termo_linalg
htlatex termo_linalg.tex "myconfig" " -cunihtf -utf8"
cd ../..
cp tex4ht/build/termo_linalg.html compiled/blog/
cp tex4ht/build/termo_linalg.css compiled/blog/

# For https
echo "thomasahle.com" > compiled/CNAME

cp -r feature_imgs compiled
cp -r teaching compiled
cp -r static compiled
python3 render_html.py pcpp_data templates/pcpp.html > compiled/teaching/pcpp2019/index.html

python3 render_tex.py templates/cv.tex > compiled/cv.tex
python3 render_tex.py templates/cv_ac.tex > compiled/cv_ac.tex

cd compiled
pdflatex --interaction=batchmode cv.tex
pdflatex --interaction=batchmode cv_ac.tex
cd ..

#pdfjam compiled/cv_ac.pdf postdoc/statement.pdf --outfile compiled/ta_cv_statement.pdf
#pdfjoin -output postdoc/combined.pdf compiled/cv_ac.pdf postdoc/statement.pdf 
#convert compiled/cv_ac.pdf postdoc/statement.pdf postdoc/combined.pdf
