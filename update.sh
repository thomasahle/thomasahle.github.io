#!/bin/sh

git add .
git commit || exit 1
git push

python3 -m pip install --upgrade pip
python3 -m pip install jinja2

./build.sh || exit 1

echo "\n\nDeploying..."

mkdir website
echo "thomasahle.com" > website/CNAME
cp compiled/*.html website
cp compiled/*.pdf website
cp -r static website
cp -r papers website
cp -r blog website

# Use subtree push to send it to the master branch on GitHub.
git subtree push --prefix website origin master

rm -rf compiled
rm -rf website

#git checkout master
#mv website .website
#rm -rf *
#mv .website/* .
#rmdir .website

#git add .
#git commit -m "update"
#git push
#git checkout gh-pages


