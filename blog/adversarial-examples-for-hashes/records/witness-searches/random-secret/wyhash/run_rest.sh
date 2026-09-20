#!/bin/bash
# Driver: wyhash final v4.3 under the random-secret model, 8 cores (taskset 24-31), nice 10.
cd "$(dirname "$0")"
T="nice -n 10 taskset -c 24-31 ./wyrs"
$T batch batch_struct.txt 30 1 201 8 > struct.log 2>&1
$T batch batch_xlen.txt 28 1 202 8 > xlen.log 2>&1
$T batch batch_site24.txt 29 1 203 8 > site24.log 2>&1
$T batch batch_site32.txt 29 1 204 8 > site32.log 2>&1
$T batch batch_sweep_w.txt 29 1 205 8 > sweep_w.log 2>&1
$T batch batch_sweep_b.txt 27 1 206 8 > sweep_b.log 2>&1
touch DONE_ALL
