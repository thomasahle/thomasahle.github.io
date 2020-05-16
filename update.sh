#!/bin/sh

git add .
git commit
git push

python3 -m pip install --upgrade pip
python3 -m pip install jinja2

./build.sh

rm -rf __pycache__

git checkout master
# compiled will be in working space, not repo
git checkout gh-pages -- static
git checkout gh-pages -- papers
git checkout gh-pages -- abstracts
git checkout gh-pages -- blog
mv compiled/*.html .
mv compiled/*.pdf .
rm -rf compiled
git add .
git commit -m "update"
git push
git checkout gh-pages

#scp compiled/index.html ssh.itu.dk:~/public_html/
#scp compiled/pcpp.html ssh.itu.dk:~/public_html/teaching/pcpp2019/index.html
#scp compiled/*.pdf ssh.itu.dk:~/public_html/
#scp -r static/ ssh.itu.dk:~/public_html/


