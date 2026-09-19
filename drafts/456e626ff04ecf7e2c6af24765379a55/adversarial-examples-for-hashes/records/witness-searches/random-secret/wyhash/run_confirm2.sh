#!/bin/bash
cd "$(dirname "$0")"
until [ -f DONE_CONFIRM_A ]; do sleep 15; done
T="nice -n 10 taskset -c 24-31 ./wyrs"
set -- len24_w0M 38ae2f8efa92bafc796ada2e8cdcc1f30ce3f58993dd4f2e c751d071056d4503796ada2e8cdcc1f30ce3f58993dd4f2e
$T pair $2 $3 35 1 302 8 > confirm_len24_w0M_mode1_2p35.log 2>&1
touch DONE_CONFIRM_2
