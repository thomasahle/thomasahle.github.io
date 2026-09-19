#!/bin/bash
cd <xeon-work>/witness/t1ha-digits-verify/t1ha-erthink
M="c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408"
for R in 90210 31337; do
  ( time nice -n 10 taskset -c 24-31 ./vu uniform $M 36 8 $R ) > ../uniform_F60_2p36_rng$R.txt 2>&1
  touch ../uniform_F60_2p36_rng$R.done
done
touch ../all.done
