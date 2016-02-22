#!/bin/sh
pypy3 index.py > index.html
scp index.html ssh.itu.dk:~/public_html
git add .
git commit
git push

