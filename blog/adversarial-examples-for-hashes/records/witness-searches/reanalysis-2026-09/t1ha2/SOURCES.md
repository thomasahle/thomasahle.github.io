# Upstream sources (not copied)

t1ha by Leonid Yuriev (zlib license), repository https://github.com/erthink/t1ha (archived mirror;
primary now on GitFlic), commit 00eb779b6c042ccd831ec2f1ae757409c73f39f6 (`v2.1.4-10-g00eb779`). The
t1ha2 core is unchanged from tag v2.1.4 to this commit.

| file | sha256 |
|---|---|
| t1ha.h | f0b19e9a63338ea2d8e7c6c01380118f14c64e8c3e58923ac356df8932331c73 |
| src/t1ha2.c | ce411391f0fe1b23e5e61d720d1fb849000c48f390305a35fae8e6e397c8b900 |
| src/t1ha_bits.h | c60b0e68a48b37d126d6a8c37d10161adfcb34f249030926bbd224949f56c4ab |
| src/t1ha2_selfcheck.c | 32276b92f9175110dbc05127a61ab00bcb5160317a2a7567521f6bcc26658a66 |
| src/t1ha_selfcheck.c | 02446ed952423161035eb216ffd879307df77ebca1fcf8810e2aa5b3ecb9d981 |
| src/t1ha_selfcheck.h | 57c1a96d200af7e1d1c38afbc8a2fea813bf66599f8ded6d3eb9319e24a1d526 |

`t1ha2_core.h` is an independent port written from the SMHasher3 port `hashes/t1ha.cpp`; it
reproduces the SMHasher3 verification values of t1ha2_atonce (0x8F16C948) and t1ha2_atonce128
(0xB44C43A1) at startup.
