# CityHash64 v1.1.1 and FarmHash64 NA: a 16-byte every-seed family, and a long-input differential

## 1. Shortest flooding family: 65,536 sixteen-byte inputs, one unseeded value

`CityHash64WithSeed(m, s) = HashLen16(CityHash64(m) - k2, s)` and `CityHash64WithSeeds(m, s0, s1) =
HashLen16(CityHash64(m) - s0, s1)`; FarmHash64 NA has the same wrapper and the same 9..16-byte path. So
inputs with one unseeded value collide for every seed and both seed interfaces.

On the 9..16-byte path, with a = w0 + k2, b = w1, mul = k2 + 2·len:
c = ror(b, 37)·mul + a, d = (ror(a, 25) + b)·mul, out = HashLen16(c, d, mul). `HashLen16` inverts in
closed form: for a fixed target H and any 64-bit z there is exactly one (c, d). The remaining single
64-bit equation in a is solved with z3 (`city16_solve.py`). About 2^64 sixteen-byte inputs share each
value; 65,602 were produced for H = 0x0f55c17e5eed1616, and the first 65,536 distinct ones are in
`city16_members.txt.gz` (hex, memory order, one per line). Examples:

    d73194b9d3f216a51455b33a85eb5311
    40bde31d7e9d2b39e2ff5d30bb368ee9

Checks:

| program | implementation | result | log |
|---|---|---|---|
| `mc_city.cpp`, `mc_farm.cpp` (x86-64) | SMHasher3 `cityhash.cpp`, `farmhash.cpp` (verification values 5FABC5C5, EBC4A679, 5438EF2C) | all 65,536 share the full 64-bit output: City WithSeed 65,536/65,536 seeds, WithSeeds 1024/1024; Farm NA WithSeed 65,536/65,536, NA WithSeeds 1024/1024, UO WithSeed 1024/1024 | `logs/mc_city16_xeon_s16.txt`, `logs/mc_farm16_xeon_s16.txt` |
| `indep_city_farm.cc` (M2) | upstream google/cityhash `city.cc` and google/farmhash `farmhash.cc`, unmodified | unseeded value shared 65,536/65,536 (City and farmhashna); all members equal for 4096/4096 seeds in each of City WithSeed, City WithSeeds, farmhashna WithSeed, farmhashna WithSeeds | `logs/indep_city_farm_m2.txt` |

Build and run:

    python3 -m pip install z3-solver          # only for regenerating members
    python3 city16_solve.py 0f55c17e5eed1616 1000 8200 part0.txt
    c++ -O2 -std=c++17 mc_city.cpp -o mc_city && ./mc_city city16_members.txt 16
    c++ -O2 -std=c++17 mc_farm.cpp -o mc_farm && ./mc_farm city16_members.txt 16
    # independent check (fetch city.cc, city.h, citycrc.h, farmhash.cc, farmhash.h; see ../SOURCES.md)
    c++ -O2 -std=c++17 -I. indep_city_farm.cc city.cc farmhash.cc -o indep_city_farm
    gunzip -k city16_members.txt.gz && ./indep_city_farm city16_members.txt 12

Expected: `RESULT: PASS` / `PASS` with the counts above.

## 2. Long inputs: a rotation differential in WeakHashLen32

In a 64-byte loop chunk, add d = 0x0f0f0f0f0f0f0f0f to word 0 and subtract it from word 3 (or words 4
and 7). The only surviving term cancels when rotr(A + d, 44) - rotr(A, 44) = -d, where A is the public
running sum; this holds for 0.886 of random A. The condition is public, so for known content a slot is
used only when it holds, and then the pair collides for every seed. Works in any loop block except
block 0 and the last 64 bytes, from 192 bytes up.

| check | CityHash64 | FarmHash64 NA |
|---|---|---|
| random content, one slot (words 0/3), 256 B / 1 KiB / 64 KiB / 1 MiB | 0.8841 / 0.8851 / 0.8854 / 0.880 | 0.8848 / 0.8842 / 0.8834 / 0.898 |
| explicit 1024-byte pair | 65,536/65,536 seeds | 65,536/65,536 seeds |
| 2^16 cube at 1064 bytes (16 slots whose public condition holds) | all members equal for 256/256 seeds | 256/256 seeds |

Program `city_long.cpp`, `farm_long.cpp` (single files, SMHasher3 source verbatim):
`c++ -O2 -std=c++17 city_long.cpp -o city_long && ./city_long`. Logs: `logs/city_long_xeon.txt`,
`logs/farm_long_xeon.txt`.
