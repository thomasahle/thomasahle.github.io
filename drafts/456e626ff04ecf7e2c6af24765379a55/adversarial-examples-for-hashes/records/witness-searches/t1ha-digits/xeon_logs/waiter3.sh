#!/bin/bash
cd <xeon-work>/witness/t1ha-digits
while kill -0 2873506 2>/dev/null; do sleep 15; done
( time nice -n 10 taskset -c 24-31 ./dp5b 5 8 31.5 0/1 63 9 > outb_5low3.txt ) 2> timeb_5low3.txt
( time nice -n 10 taskset -c 24-31 ./dp5b 4T 8 30.5 > outb_4T.txt ) 2> timeb_4T.txt
( time nice -n 10 taskset -c 24-31 ./dp5b 5 8 30.5 0/1 15 > outb_5.txt ) 2> timeb_5.txt
touch BDONE
