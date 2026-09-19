#!/bin/bash
cd <xeon-work>/witness/t1ha-digits
while [ ! -f F1_DONE ]; do sleep 15; done
nice -n 10 taskset -c 24-31 ./cond2 c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408 28 8 > cond_F60_2p28.txt 2>&1
( time nice -n 10 taskset -c 24-31 ./direct_class c4c0cc2284cd239ede36a18fa6 fbc31866049ec02dea0c4e2be580d3cf a5845081f808f44a 2080000118003408 36 8 4242 > direct_F60_2p36.txt ) 2> direct_F60_2p36.time
touch F60_DONE
