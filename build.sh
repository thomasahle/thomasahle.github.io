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

# For https
echo "thomasahle.com" > compiled/CNAME

cp -r feature_imgs compiled
cp -r teaching compiled
cp -r static compiled
python3 render_html.py pcpp_data templates/pcpp.html > compiled/teaching/pcpp2019/index.html

python3 render_tex.py templates/cv.tex > compiled/cv.tex
python3 render_tex.py templates/cv_ac.tex > compiled/cv_ac.tex

cd compiled
# Run pdflatex with better error handling
echo "Running pdflatex on cv.tex..."
pdflatex cv.tex > cv.log 2>&1 || {
  echo "===== ERROR: PDF generation for cv.tex failed ====="
  echo "Last 20 lines of cv.log:"
  tail -n 20 cv.log
  echo "===== End of cv.log excerpt ====="
}

echo "Running pdflatex on cv_ac.tex..."
pdflatex cv_ac.tex > cv_ac.log 2>&1 || {
  echo "===== ERROR: PDF generation for cv_ac.tex failed ====="
  echo "Last 20 lines of cv_ac.log:"
  tail -n 20 cv_ac.log
  echo "===== End of cv_ac.log excerpt ====="
}

# Verify PDFs were created successfully
if [ ! -f cv.pdf ] || [ ! -f cv_ac.pdf ]; then
  echo "ERROR: PDF generation failed - one or more PDF files are missing!"
  ls -la
  exit 1
else
  echo "PDF generation completed successfully."
fi
cd ..

#pdfjam compiled/cv_ac.pdf postdoc/statement.pdf --outfile compiled/ta_cv_statement.pdf
#pdfjoin -output postdoc/combined.pdf compiled/cv_ac.pdf postdoc/statement.pdf 
#convert compiled/cv_ac.pdf postdoc/statement.pdf postdoc/combined.pdf
