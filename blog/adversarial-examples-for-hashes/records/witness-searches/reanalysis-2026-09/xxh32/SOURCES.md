# Sources

| file | pinned source | sha256 |
|---|---|---|
| `upstream/xxhash.h` (needed by `loop_xxmur.c`, not copied) | https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h (tag v0.8.3, commit e626a72) | `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b` |
| XXH32 body transcribed in `xxh32_witness.c` | https://github.com/fwojcik/smhasher3/blob/7ad8939d/hashes/xxhash.cpp | `4bef3cc80fff913b0f47be3f73b03119f24287df337d12b08634fee6f9d02240` |
| MurmurHash2 bodies in `loop_xxmur.c` | https://github.com/fwojcik/smhasher3/blob/7ad8939d/hashes/murmurhash2.cpp | `7a70fffa9709105ed5dd03e69d829575b97d8727c5d28e12c7faf55f9a29c278` |

Fetch the header before building `loop_xxmur.c`:

    mkdir -p upstream && curl -sL -o upstream/xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    shasum -a 256 upstream/xxhash.h   # 17973c0d...5e4b

SMHasher3 verification values checked at startup (registry in the files above): XXH-64 `0x8F8224C4`, XXH-32 `0x6FD78385`, MurmurHash2-64 `0x1F0D3804`, MurmurHash2-32 `0x27864C1E`, MurmurHash2a `0x7FBD4396` (little-endian verification values; the xxHash values use the canonical big-endian output bytes, as SMHasher3 does).
