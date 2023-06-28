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

# This command splits the subdirectory dist from the current repository into
# a new branch called split. The --prefix flag specifies the directory to be
# split, in this case, dist. The -b flag creates a new branch with the given name.
git subtree split --prefix dist -b split

# This command pushes the newly created split branch to a remote repository
# called origin. The split:master part specifies that the local branch split
# should be pushed to the remote branch master. The --force flag is used to
# force the update of the master branch in the remote repository, overwriting
# any existing commits
git remote add ghpages git@github.com:thomasahle/thomasahle.github.io.git
git push ghpages split:master --force




