# mx3 v3, MUM and mir: 10-byte every-seed families and key-free pairs at every length

All three absorb message words as public terms (a public bijection, or a public 64x64 -> 128 fold),
combined with the seed only by XOR or addition. Fixing the public part fixes the state for every seed.

- mx3 v3: state = seed·C^(n+1) + public terms, each word entering as g(w) = (wC ^ (wC >> 39))C, a
  bijection. At 10 bytes choose the 2-byte tail t freely and solve word 0 from g(w0)·C + g(t) = T:
  exactly 2^16 members.
- MUM (exact 128-bit fold, the 64-bit default): the state is mum(seed+10, p_start) ^ mum(w0, p0) ^
  mum(t, p_tail); solve mum(w0, p0) = X ^ mum(t, p_tail), using mum(w, p) = w·p mod 2^64-1 (or one more).
- mir_hash (relaxed = exact fold): r = seed + 10; r ^= mum(w0, p1); r ^= mum(t, p2); then a seeded
  finalizer. Same solve with p1.
- Inexact folds (SMHasher3 `mum3.inexact`, `mir.inexact` = `mir_hash_strict`): keep only members whose
  inexact and exact folds agree; this needs a 3-byte tail, so 11 bytes.
- Every length >= 9: change the last byte and re-solve one earlier word whose public term is XORed (or,
  for mx3, added with an invertible coefficient) into the same state.

Examples (memory order):

    mx3 v3   8bfb57c33b76a9660000  c57e5558458bbaba0100    (mc_mx3.cpp)
    MUM v3   8299c118702bca980000  0d33410a0cb32d5c0100    (mc_mummir.cpp)
    mir      2705a6374fa8dc940000  ed94f0ed6e3dd8470100    (mc_mummir.cpp)

| program | implementation | result | log |
|---|---|---|---|
| `mc_mx3.cpp` (x86-64) | SMHasher3 `mx3.cpp` (verification 7B287B65) | 10 B family: 65,536/65,536 seeds; every length 9..512: 504/504 lengths, 1024/1024 seeds each | `logs/mc_mx3_xeon_s16.txt` |
| `mc_mummir.cpp` (x86-64) | SMHasher3 `mum_mir.cpp` (8BD72B8C, 0AD998DF, 00A393C8, 422A66FC) | MUM exact 10 B (unroll 3 and 4): 65,536/65,536 seeds each; mir exact 10 B: 65,536/65,536; 11 B families for MUM exact+inexact and mir exact+inexact: 65,536/65,536 each; every length 9..400: MUM 392/392, mir 392/392 | `logs/mc_mummir_xeon_s16.txt` |
| `indep_mx3_mum_mir.cc` (M2) | upstream `mx3.h`, `mum.h` (default build, 595c091, 16-word unroll on arm64), `mir-hash.h`, unmodified; families regenerated independently | 10 B families mx3, MUM, mir: 4096/4096 random seeds + seed 0 each; every length 9..1100: mx3 1092/1092, MUM 1092/1092, mir 1092/1092 (256 seeds per length) | `logs/indep_mx3_mum_mir_m2.txt` |

Build and run:

    c++ -O2 -std=c++17 mc_mx3.cpp -o mc_mx3 && ./mc_mx3 16
    c++ -O2 -std=c++17 mc_mummir.cpp -o mc_mummir && ./mc_mummir 16
    # independent check (fetch mx3.h, mum.h, mir-hash.h; see ../SOURCES.md)
    c++ -O2 -std=c++17 -I. indep_mx3_mum_mir.cc -o indep_mx3_mum_mir && ./indep_mx3_mum_mir 12 1100
