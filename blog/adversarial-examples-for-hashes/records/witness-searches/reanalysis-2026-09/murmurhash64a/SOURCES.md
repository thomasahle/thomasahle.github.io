# Sources

| file | pinned source | sha256 |
|---|---|---|
| `upstream/xxhash.h` (needed by `loop_xxmur.c`, not copied) | https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h (tag v0.8.3, commit e626a72) | `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b` |
| MurmurHash2_32, MurmurHash2_64 (= MurmurHash64A), MurmurHash2A bodies in `murmurhash2_witness.c`, `murmurhash2a_witness.c`, `loop_xxmur.c` | https://github.com/fwojcik/smhasher3/blob/7ad8939d/hashes/murmurhash2.cpp | `7a70fffa9709105ed5dd03e69d829575b97d8727c5d28e12c7faf55f9a29c278` |
| original MurmurHash2 / MurmurHash64A / MurmurHash2A | https://github.com/aappleby/smhasher/blob/07bb4de10a63e8cc2e1724865454eba635742383/src/MurmurHash2.cpp | `8cdec1fed73de41e4ee91ab78fe5ba76110582263064dcdcc0b770a25c0573f3` |
| libstdc++ `_Hash_bytes` (transcribed in `libstdcxx_check.c`) | https://github.com/gcc-mirror/gcc/blob/9e01c24ca797dfdc8e15d5678b2c3d5cb87b4c93/libstdc++-v3/libsupc++/hash_bytes.cc (lines 134-163) | `cad3ed6168eb8643141a56e052d09ee0770c97744cc1788f143d4a2515593b19` |
| libstdc++ default seed `0xc70f6907` (`_Hash_impl::hash`) | https://github.com/gcc-mirror/gcc/blob/9e01c24ca797dfdc8e15d5678b2c3d5cb87b4c93/libstdc++-v3/include/bits/functional_hash.h (lines 214-218) | `b9076516de5cf39ee39581055c07c360ba675517f456d153b45c64eccf14ceab` |

Fetch the header before building `loop_xxmur.c`:

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    shasum -a 256 upstream/xxhash.h   # 17973c0d...5e4b

SMHasher3 verification values checked at startup (registry in the files above): XXH-64 `0x8F8224C4`, XXH-32 `0x6FD78385`, MurmurHash2-64 `0x1F0D3804`, MurmurHash2-32 `0x27864C1E`, MurmurHash2a `0x7FBD4396` (little-endian verification values; the xxHash values use the canonical big-endian output bytes, as SMHasher3 does).
