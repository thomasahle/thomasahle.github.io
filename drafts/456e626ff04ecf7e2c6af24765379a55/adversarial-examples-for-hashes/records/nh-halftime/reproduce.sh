#!/usr/bin/env bash
# Run on the Xeon from <xeon-work>/lean-nh-halftime; never compile on the Mac.
set -euo pipefail
cd -- "$(dirname -- "$0")"
source env.sh
export LEAN_NUM_THREADS=8
run() { nice -n 10 taskset -c 88-95 "$@"; }
mkdir -p evidence
exec > >(tee evidence/reproduction.log) 2>&1
cd lean
run lake build
run lake build vectorCheck
run python3 halftime_audit.py
run lake env lean HalftimeAudit.lean > HalftimeAudit.txt
run python3 halftime_audit.py --check
bash build.sh
cd ../refinement
run g++ -std=c++17 -O2 -march=native vectors.cpp -o vectors-simd
run ./vectors-simd > vectors-simd.txt
run g++ -std=c++17 -O2 -DSCALAR vectors.cpp -o vectors-scalar
run ./vectors-scalar > vectors-scalar.txt
run g++ -std=c++17 -O2 -march=native -fwrapv -DUPSTREAM vectors.cpp -o vectors-upstream
run ./vectors-upstream > vectors-upstream.txt
run python3 compare_vectors.py
run ../lean/.lake/build/bin/vectorCheck vectors-simd.txt
run g++ -std=c++17 -O2 -march=native encoder.cpp -o encoder
run ./encoder > encoder-cpp.txt
run ../lean/.lake/build/bin/vectorCheck encoders > encoder-lean.txt
run python3 check_rank.py
run g++ -std=c++17 -O2 seeds.cpp -o seeds
run ./seeds > seeds-cpp.txt
run ../lean/.lake/build/bin/vectorCheck seeds > seeds-lean.txt
cmp seeds-cpp.txt seeds-lean.txt
printf 'PASS: 36000 expanded key words agree (four seeds, 9000 words each)\n'
run clang++ -std=c++17 -O1 -march=native -fsanitize=undefined -fno-sanitize-recover=all vectors.cpp -o vectors-ubsan
run ./vectors-ubsan > vectors-ubsan.txt
cmp vectors-simd.txt vectors-ubsan.txt
printf 'PASS: fixed header UBSan and SIMD/Lean output agreement\n'
bash check-upstream-ub.sh
run g++ -std=c++17 -O2 -Wno-ignored-attributes upstream-encoder-witness.cpp -o upstream-encoder-witness
run ./upstream-encoder-witness
sha256sum headers/*.hpp > source-sha256.txt
sha256sum vectors-*.txt encoder-*.txt seeds-*.txt > vector-sha256.txt
printf 'Reproduction complete. See STATUS.md for claims that remain unproved.\n'
