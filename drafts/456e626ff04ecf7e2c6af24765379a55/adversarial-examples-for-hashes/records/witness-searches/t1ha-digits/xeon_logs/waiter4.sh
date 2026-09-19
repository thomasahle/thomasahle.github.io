#!/bin/bash
cd <xeon-work>/witness/t1ha-digits
while [ ! -f BDONE ]; do sleep 15; done
( time nice -n 10 taskset -c 24-31 ./dp5c 5 8 31.5 0/1 63 15 > outb_5low3b.txt ) 2> timeb_5low3b.txt
( time nice -n 10 taskset -c 24-31 ./dp5c 5 8 32.5 0/1 63 63 15 > outb_5low4.txt ) 2> timeb_5low4.txt
touch CDONE
