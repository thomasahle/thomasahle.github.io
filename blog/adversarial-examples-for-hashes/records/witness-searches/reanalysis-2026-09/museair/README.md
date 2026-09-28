# MuseAir v0.3: an 18-byte every-seed family

On the 17..32-byte path the state before finalization is (i, j) = read16(bytes 0..15) ^ T(tail) ^
S(seed, len), where read16 is a bijection of the first 16 bytes (i = lu32(0)<<32 | lu32(12),
j = lu32(4)<<32 | lu32(8)) and T is the public product term of the tail. For every 2-byte tail t, the
head read16^-1(I ^ T(t)) gives the same state: 2^16 members at 18 bytes (256 at 17 bytes), colliding
in all four SMHasher3 variants (64/128-bit, standard/bfast) for every seed.

Example members (memory order), from `mc_museair.cpp`:

    fca3e54749629f45c010373f7f561c670000
    fca31973f8f89f45da3bac007f561c670100

| program | implementation | result | log |
|---|---|---|---|
| `mc_museair.cpp` (x86-64) | SMHasher3 `museair.cpp` (F89F1683, C61BEE56, D3DFE238, 27939BF1) | 18 B, 65,536 members: 65,536/65,536 seeds in each of the four variants; 17 B, 256 members: 65,536/65,536 | `logs/mc_museair_xeon_s16.txt` |
| `indep_museair_a5_murmur.cc` (M2) | the same SMHasher3 source, unmodified, behind a 3-file stand-in for the SMHasher3 headers (`smh3shim/`); family regenerated independently; verification values recomputed | 65,536 members, 4097/4097 seeds (4096 random + seed 0) in each variant | `logs/indep_museair_a5_murmur_m2.txt` |

Build and run:

    c++ -O2 -std=c++17 mc_museair.cpp -o mc_museair && ./mc_museair 16
    # independent check (fetch a5hash.h, MurmurHash3.cpp, MurmurHash3.h; see ../SOURCES.md)
    c++ -O2 -std=c++17 -I. -Ismh3shim indep_museair_a5_murmur.cc MurmurHash3.cpp -o indep_museair_a5_murmur
    ./indep_museair_a5_murmur 12

The same program also checks the a5hash-128 and MurmurHash3 families (see `../a5hash-128/`,
`../murmurhash3/`).
