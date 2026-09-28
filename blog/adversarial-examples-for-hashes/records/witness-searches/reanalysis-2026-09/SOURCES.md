# Upstream sources used by the programs in these folders

The `mc_*.cpp`, `*_long.cpp` and `kf_abseil.cpp` programs are single files that already contain the
SMHasher3 platform header and the verbatim SMHasher3 `hashes/*.cpp` source of the hash they test, plus
a driver; they need nothing else. The `indep_*` programs include unmodified upstream files, fetched
from these pinned URLs (sha256 of the file as used):

| file | URL | sha256 |
|---|---|---|
| city.cc | https://raw.githubusercontent.com/google/cityhash/f5dc54147fcce12cefd16548c8e760d68ac04226/src/city.cc | 9cd2e21da0d6131b1756f059c6f269c204ee4ecd28c7f4c487376efe2336696b |
| city.h | https://raw.githubusercontent.com/google/cityhash/f5dc54147fcce12cefd16548c8e760d68ac04226/src/city.h | 0a65d60f1742e215e1cedd7fa081895c2bee7afdbe56b86870e63fd0536c5297 |
| citycrc.h | https://raw.githubusercontent.com/google/cityhash/f5dc54147fcce12cefd16548c8e760d68ac04226/src/citycrc.h | 4be7527f517b6744e70b3ae163125e908b5f5d660ed53740a33b93a6663bf7dd |
| farmhash.cc | https://raw.githubusercontent.com/google/farmhash/9d99331eb762e9ee22fd97d15a594525ad98310a/src/farmhash.cc | 13751fdbc92b7a08670956d5350d288b8fd30f0f1eb83e1b19059d6435a4ac51 |
| farmhash.h | https://raw.githubusercontent.com/google/farmhash/9d99331eb762e9ee22fd97d15a594525ad98310a/src/farmhash.h | f7c569f85c035f73705a4b9025a80ebb1c4772fb6305fd4469f7e0629a898dcd |
| mx3.h | https://raw.githubusercontent.com/jonmaiga/mx3/48924ee743d724aea2cafd2b4249ef8df57fa8b9/mx3.h | c65f1f3870ed35e06aa1dd8962132366eb7686b2aca2801035e0f8e60ecf8675 |
| mum.h | https://raw.githubusercontent.com/vnmakarov/mum-hash/595c091da6872da6a4f0a3852291c83aa37ea7eb/mum.h | a7008e07a8912fa8fbd7c949fcaf4498c7792c9a35bbe3f39b4233dddff7aaea |
| mir-hash.h | https://raw.githubusercontent.com/vnmakarov/mir/a8ab7c31cd5f9b23b77d84c60b3d83e62d9d304c/mir-hash.h | 7099eef3af2dd99f9710e5552a6dcca95dd1170f5345555ef97b9ddeb3a37971 |
| a5hash.h (v5.25) | https://raw.githubusercontent.com/avaneev/a5hash/10492dbe8c34ae043ccbab3fe4b16a2a26eebbc6/a5hash.h | fca1b4b9be5ee4f5861b509563bb0643c81f83464be6656852afc6df39a3452a |
| MurmurHash3.cpp | https://raw.githubusercontent.com/aappleby/smhasher/07bb4de10a63e8cc2e1724865454eba635742383/src/MurmurHash3.cpp | 30f121ed155ebf336af398aabb7d8d157afdfafc8d981e7b48d2a1ceb4b63e4e |
| MurmurHash3.h | https://raw.githubusercontent.com/aappleby/smhasher/07bb4de10a63e8cc2e1724865454eba635742383/src/MurmurHash3.h | 9af2003e3885c841fa0ac08cb5d5f31541ee9d793f8a9c4a4f771f43fb0d6f41 |
| museair_smhasher3.cpp | SMHasher3 `hashes/museair.cpp` (MuseAir v0.3, CC0), copied in `museair/` | 59c1a4276cd3ec263f53739c326008002b513ce6d8c0d84c4702ae61565dfd90 |
| abseil-cpp | https://github.com/abseil/abseil-cpp tree 73d2688 (= LTS 20260817.0); `absl/hash/internal/hash.cc` f2b6084b…857e, `hash.h` 65f71f11…e288, `absl/base/options.h` 7c869bc3…a9b6 | |

`config.h` in `city-farm/` is a six-line stand-in for cityhash's autoconf output (little-endian host).

Hosts: `<xeon-host>` is an Intel Xeon Platinum 8375C (x86-64, AES-NI, SSE4.2); "M2" is an Apple M2
(arm64, macOS, Apple clang). `<scratch>` stands for a working directory.
