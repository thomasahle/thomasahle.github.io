cd records/fairness-pass/foldhash-fast-verify
( nice -n 10 taskset -c 8-15 ./foldhash_verify 36 8 0xc0ffee1234567890 > c_2p36.log 2>&1; echo EXIT $? >> c_2p36.log )
( /usr/bin/time -v nice -n 10 taskset -c 8-15 ./fhcheck/target/release/fhcheck 36 8 0x5eed5eed5eed5eed > rust_2p36.log 2>&1; echo EXIT $? >> rust_2p36.log )
touch DONE
