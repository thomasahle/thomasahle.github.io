#!/bin/sh
mkdir compiled
python3 render_html.py data templates/index.html > compiled/index.html
mkdir compiled/blog
python3 render_html.py blog_data templates/blog/index.html > compiled/blog/index.html

# Latex blog things
cd tex4ht
latex --interaction=batchmode --output-directory=build termo_linalg.tex
cd build
cp ../termo_linalg.tex .
cp ../termo.bib .
cp ../myconfig.cfg .
bibtex termo_linalg
htlatex termo_linalg.tex "myconfig" " -cunihtf -utf8"
cd ../..
cp tex4ht/build/termo_linalg.{html,css} compiled/blog

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
