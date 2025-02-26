#!/bin/sh
set -e  # Exit on error

# Create output directories
mkdir -p compiled/blog
mkdir -p compiled/teaching/pcpp2019
mkdir -p tex4ht/build

# Render HTML pages
python3 render_html.py data templates/index.html > compiled/index.html
python3 render_html.py blog_data templates/blog/index.html > compiled/blog/index.html

# Latex blog things - skip in CI environment if TEX4HT_SKIP is set
if [ -z "$TEX4HT_SKIP" ]; then
  cd tex4ht
  # Ensure build directory exists
  mkdir -p build
  
  # Copy the source files to build dir first
  cp termo_linalg.tex build/
  cp termo.bib build/
  cp myconfig.cfg build/
  
  # Change to build dir and run the processing
  cd build
  latex --interaction=batchmode termo_linalg.tex
  bibtex termo_linalg
  latex --interaction=batchmode termo_linalg.tex
  latex --interaction=batchmode termo_linalg.tex
  htlatex termo_linalg.tex "myconfig" " -cunihtf -utf8"
  cd ../..
else
  echo "Skipping tex4ht processing as TEX4HT_SKIP is set"
  # Create placeholder files for the CI build
  mkdir -p compiled/blog
  echo "<html><body><h1>Placeholder for termo_linalg content</h1></body></html>" > compiled/blog/termo_linalg.html
  echo "body { font-family: sans-serif; }" > compiled/blog/termo_linalg.css
fi

# Only copy from tex4ht/build if we didn't skip processing
if [ -z "$TEX4HT_SKIP" ]; then
  cp tex4ht/build/termo_linalg.html compiled/blog/
  cp tex4ht/build/termo_linalg.css compiled/blog/
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
