#!/bin/bash
# Independent verification batch for xxh3-128 under the random-secret model (runs on the Xeon, cores 24-31, nice 10).
# Reproduce: curl -sSL -o xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
#            sha256sum xxhash.h  -> 17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b
#            cc -O2 -std=gnu11 -pthread -o verify_rs128 verify_rs128.c -lm
set -u
cd "$(dirname "$0")"
mkdir -p logs
P="nice -n 10 taskset -c 24-31"
run() { name=$1; shift; echo "== $name: $* start $(date -u +%FT%TZ)" >> logs/batch.log; $P ./verify_rs128 "$@" > logs/$name.txt 2> logs/$name.err; echo "== $name: exit $? $(date -u +%FT%TZ)" >> logs/batch.log; }
run selftest selftest
run F_secret_2p34_r1001      secret     34 1001 8 row.txt
run F_secretseed_2p33_r1002  secretseed 33 1002 8 row.txt
run F_seed_2p32_r1003        seed       32 1003 8 row.txt
run family_secret_2p32_r1004 secret     32 1004 8 family.txt
touch logs/BATCH_DONE
