#!/bin/sh

git add .
git commit
git push

python3 -m pip install --upgrade pip
python3 -m pip install jinja2

./build.sh

git checkout master
# compiled will be in working space, not repo
#git checkout gh-pages -- compiled
git checkout gh-pages -- static
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


