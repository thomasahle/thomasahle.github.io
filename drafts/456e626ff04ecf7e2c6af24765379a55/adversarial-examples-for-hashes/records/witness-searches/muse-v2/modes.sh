#!/bin/sh
# exhaustive single-byte XOR at the boundary bytes, 2^24 seeds each, modal (i,j) difference
for cell in "32 23" "32 31" "25 24" "18 16" "19 18" "19 17" "32 16" "32 24" "22 16" "22 21"; do
  set -- $cell
  nice -n 10 taskset -c 24-31 ./mode $1 $2 24 8 > logs/mode_len$1_byte$2.txt 2>&1
done
touch logs/modes.done
