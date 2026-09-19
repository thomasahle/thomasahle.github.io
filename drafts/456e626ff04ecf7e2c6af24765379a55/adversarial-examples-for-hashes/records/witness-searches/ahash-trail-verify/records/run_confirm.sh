#!/bin/bash
# reproduction: native Rust (upstream ahash 0.8.12), rs4 key model, 8 threads pinned to cores 24-31
cd <xeon-work>/witness/ahash-trail/native-verify
nice -n 10 taskset -c 24-31 ./target/release/ahash_trail_verify pairs_confirm.txt 31 77 8 > confirm_31_seed77.log 2>&1
nice -n 10 taskset -c 24-31 ./target/release/ahash_trail_verify pairs_confirm.txt 33 78 8 > confirm_33_seed78.log 2>&1
touch DONE
