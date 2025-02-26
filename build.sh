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
mkdir -p compiled/blog

# First create build directory and copy files
mkdir -p tex4ht/build
cp tex4ht/termo_linalg.tex tex4ht/build/
cp tex4ht/termo.bib tex4ht/build/
cp tex4ht/myconfig.cfg tex4ht/build/

# Change to build dir and run the processing
cd tex4ht/build
latex --interaction=nonstopmode termo_linalg.tex
if [ $? -ne 0 ]; then
  echo "LaTeX processing failed"
  cd ../..
  exit 1
fi

bibtex termo_linalg
if [ $? -ne 0 ]; then
  echo "BibTeX processing failed"
  cd ../..
  exit 1
fi

latex --interaction=nonstopmode termo_linalg.tex
latex --interaction=nonstopmode termo_linalg.tex
htlatex termo_linalg.tex "myconfig" " -cunihtf -utf8"
if [ $? -ne 0 ]; then
  echo "HTML conversion failed"
  cd ../..
  exit 1
fi

# Check if the output files exist
if [ ! -f termo_linalg.html ] || [ ! -f termo_linalg.css ]; then
  echo "HTML output files not generated"
  cd ../..
  exit 1
fi

# Copy the output files to compiled/blog
cp termo_linalg.html ../../compiled/blog/
cp termo_linalg.css ../../compiled/blog/
cd ../..

# Files have already been copied to compiled/blog
fi

# For https
echo "thomasahle.com" > compiled/CNAME

cp -r feature_imgs compiled
cp -r teaching compiled
cp -r static compiled
python3 render_html.py pcpp_data templates/pcpp.html > compiled/teaching/pcpp2019/index.html

python3 render_tex.py templates/cv.tex > compiled/cv.tex
python3 render_tex.py templates/cv_ac.tex > compiled/cv_ac.tex

cd compiled
# Run pdflatex and ignore exit codes - PDFs are still generated despite errors
pdflatex --interaction=batchmode cv.tex || true
pdflatex --interaction=batchmode cv_ac.tex || true

# Verify PDFs were created successfully
if [ ! -f cv.pdf ] || [ ! -f cv_ac.pdf ]; then
  echo "ERROR: PDF generation failed - one or more PDF files are missing!"
  # Exit with error code if PDFs are missing
  [ -z "$IGNORE_PDF_CHECK" ] && exit 1
else
  echo "PDF generation completed successfully."
fi
cd ..

#pdfjam compiled/cv_ac.pdf postdoc/statement.pdf --outfile compiled/ta_cv_statement.pdf
#pdfjoin -output postdoc/combined.pdf compiled/cv_ac.pdf postdoc/statement.pdf 
#convert compiled/cv_ac.pdf postdoc/statement.pdf postdoc/combined.pdf
