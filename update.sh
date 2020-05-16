#!/bin/sh

git add .
git commit
git push

python3 -m pip install --upgrade pip
python3 -m pip install jinja2

./build.sh
echo "\n\nMoving to master"

mkdir website
echo "thomasahle.com" > website/CNAME
cp compiled/*.html website
cp compiled/*.pdf website
cp -r static website
cp -r papers website
cp -r blog website
rm -rf compiled

git checkout master
mv website .website
rm -rf *
mv .website/* .
rmdir .website

git add .
git commit -m "update"
git push
git checkout gh-pages

#scp compiled/index.html ssh.itu.dk:~/public_html/
#scp compiled/pcpp.html ssh.itu.dk:~/public_html/teaching/pcpp2019/index.html
#scp compiled/*.pdf ssh.itu.dk:~/public_html/
#scp -r static/ ssh.itu.dk:~/public_html/


