#!/bin/sh
# reduced boundary-byte sweeps at 2^22 seeds; waits for the len32/byte23 2^24 sweep (marker: 255 lines)
cd /home/thomas-ahle/agents/witness/muse-v2
while [ "$(wc -l < logs/mode_len32_byte23.txt)" -lt 255 ]; do sleep 10; done
for cell in "19 18" "19 17" "18 16" "25 24" "22 21"; do
  set -- $cell
  nice -n 10 taskset -c 24-31 ./mode $1 $2 22 8 > logs/mode_len$1_byte$2.txt 2>&1
done
touch logs/modes.done
