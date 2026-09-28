# MurmurHash3_x64_128: a 144-byte every-seed family (four choices per 16-byte block)

In each 16-byte block, h1 ^= g1(k1); h1 = rotl(h1, 27) + h2; h1 = 5·h1 + c; h2 ^= g2(k2); h2 =
rotl(h2, 31) + h1; h2 = 5·h2 + c', with g1, g2 public bijections. XOR differences 2^36 (h1 side) and
2^32 (h2 side) rotate onto bit 63, where addition and multiplication by 5 carry an XOR difference
exactly. So a block can absorb any incoming top-bit difference (a, b) and emit (r1 ^ b, r2 ^ r1 ^ b)
for any choice (r1, r2): four alternatives per block. Eight free blocks and one closing block
(r1 = b, r2 = 0) give 4^8 = 2^16 messages of 144 bytes with the same internal state, hence the same
128-bit output for every seed. An 8-byte tail can cancel an h1 difference but not an h2 difference,
which gives 2^17 members at 152 bytes.

Example members (memory order), from `mc_murmur.cpp` (144 bytes each; first 32 bytes shown):

    cb23ab8fca5c4f3c3d416b98c9d5de5cc67dff08b5fdface7d291055c777b4a6...
    6b83ad6d26e9f9733d416b98c9d5de5cc67dff0838b02c4c7d291015e7022c1d...

| program | implementation | result | log |
|---|---|---|---|
| `mc_murmur.cpp` (x86-64) | SMHasher3 `murmurhash3.cpp` (6384BA69) | 144 B, 65,536 members: 65,536/65,536 uint32 seeds and 1024/1024 64-bit seed_t seeds; 152 B, 131,072 members: 1024/1024; 48 B, 16 members, all 2^32 uint32 seeds: 4,294,967,296/4,294,967,296 | `logs/mc_murmur_xeon_k16_s16.txt`, `logs/mc_murmur_exhaustive_2p32_48B.txt` |
| `../museair/indep_museair_a5_murmur.cc` (M2) | upstream aappleby `MurmurHash3.cpp`, unmodified; family regenerated independently | 65,536 members, 4097/4097 seeds | `logs/indep_museair_a5_murmur_m2.txt` |

    c++ -O2 -std=c++17 mc_murmur.cpp -o mc_murmur && ./mc_murmur 16 16
    ./mc_murmur 4 8 0x0f55e7 0 4294967296      # exhaustive over all uint32 seeds, 16 members of 48 bytes
