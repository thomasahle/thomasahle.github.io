#!/bin/sh

python3 -m pip install --upgrade pip
python3 -m pip install jinja2

./build.sh || exit 1

echo "\n\nDeploying..."

rm -rf dist
mkdir dist
echo "thomasahle.com" > dist/CNAME
cp -r static dist
cp -r papers dist
cp -r blog dist
cp compiled/*.html dist
cp compiled/*.pdf dist
cp compiled/blog/*.html dist/blog
cp -r compiled/teaching dist/teaching

rm -rf compiled

# We must add dist before we can sub-tree it off
git add dist
git commit -m "Deploying" --quiet || exit 1

# Use subtree push to send it to the master branch on GitHub.
# git subtree push --prefix website origin master
#git push origin `git subtree split --prefix website gh-pages`:master --force
git subtree split --prefix dist -b split
git push origin split:master --force

#git checkout master
#mv website .website
#rm -rf *
#mv .website/* .
#rmdir .website

#git add .
#git commit -m "update"
#git push
#git checkout gh-pages


