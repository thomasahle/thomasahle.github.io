#!/bin/bash
cd "$(dirname "$0")"
nice -n 10 taskset -c 24-31 ./wyrs batch batch_final.txt 30 1 207 8 > final.log 2>&1
touch DONE_FINAL
