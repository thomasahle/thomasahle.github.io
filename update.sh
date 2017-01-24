#!/bin/sh
python3 -m pip install --upgrade pip
python3 index.py > output.html
scp output.html ssh.itu.dk:~/public_html/index.html
scp style.css ssh.itu.dk:~/public_html/style.css
scp script.js ssh.itu.dk:~/public_html/script.js
rm output.html
git add .
git commit
git push

