# Independent verification: aHash 0.8 AES path

The claim is supported: **75 full 64-bit collisions in 67,108,864 (2^26) uniformly pseudorandom 256-bit keys**, giving **log2(rate) = -19.771181309504**. The reference verification value and the supplied example both pass. No network access, SMHasher3 framework compilation, or modification of `ref/` was used.

The standalone implementation is [verify.cpp](verify.cpp); the complete final execution log is [run.log](run.log). Every sampled collision, including its zero-based sample index, four keys, both outputs, and state-equality checks, is saved in [collisions.tsv](collisions.tsv). [analyze.py](analyze.py) audits those records and produces [analysis.json](analysis.json).

## Implementation and validation

I read both supplied source files and implemented the normal shuffled, little-endian AES variant, including every input-length branch and finalization. In `ref/rust-ahash.cpp:49–53`, SMHasher3's scalar seed maps to four words as `k[j] = PI2[j] XOR seed`, where:

```text
PI2 = 452821e638d01377 be5466cf34e90c6c c0ac29b7c97c50dd 3f84d5b5b5470917
```

That mapping is used only for the verification test. The collision experiment directly supplies four RNG outputs as `k0..k3`; it does not restrict keys to the scalar-seed family.

The ARM decrypt round is `AESIMC(AESD(value, zero)) XOR round_key`, implementing InvShiftRows, InvSubBytes, InvMixColumns, then AddRoundKey. The encrypt round similarly uses `AESMC(AESE(value, zero)) XOR round_key`. A separate portable byte implementation generates the AES S-box from finite-field inversion and the affine transformation, then implements the row permutations and column mixing explicitly.

Following `ref/Hashinfo.cpp:50–90`, I hashed the 256 messages of lengths 0 through 255, containing bytes `0..length-1`, with scalar seeds `256-length`; concatenated the 256 little-endian eight-byte outputs; hashed those 2,048 bytes with scalar seed zero; and interpreted the first four output bytes as little-endian.

| Validation | Result |
|---|---|
| Expected reference value | `0x3BF4383B` |
| ARM implementation | `0x3BF4383B` — PASS |
| Portable implementation | `0x3BF4383B` — PASS |
| Complete final verification hash, both implementations | `0x8D9D958B3BF4383B` |
| ARM versus portable AES rounds | Both directions agree on 4,096 random input/key pairs |
| ARM versus portable complete hashes | Agree for every length 0..2,048, with a separate random key at each length |
| Independent portable recheck of sampled collisions | All 75 agree; both 128-bit `enc` and `sum` states coincide |

## Messages and supplied example

The messages are distinct and each decodes to exactly 56 bytes:

```text
m1 = 40313233343536373839253b3c3d3e3f408142094445464748494a4b4c4d4e4f8d896d065c5d5e5f606162636465666768696a6b6c6d6e6f
m2 = 3e313233343536373839403b3c3d3e3f405e42204445464748494a4b4c4d4e4f727290085c5d5e5f606162636465666768696a6b6c6d6e6f

k0 = 711096b41e1b7d99
k1 = 59af3b7ae3b7d595
k2 = d2fb75eb5658a771
k3 = 5a0f91d8ed1ffe38

hash(m1) = 0x45EF7682D72B19B6
hash(m2) = 0x45EF7682D72B19B6
```

Both implementations produce these outputs. Each output's little-endian byte encoding is `b6192bd78276ef45`. The four supplied words are used directly as RandomState keys.

## Sampling result

The experiment used **one thread**, with no filtering, early differential tests, or weak-seed conditioning. Both complete hashes, including finalization, were evaluated for every sampled key and their full 64-bit outputs compared. The seed-class density is **1**, so the measured rate is already the unconditional total rate.

The RNG is xoshiro256**, initialized by four successive SplitMix64 outputs starting from the printed constant **`0x6A09E667F3BCC909`**. Its initial state is:

```text
63cfc62a2b097592 dc0746b419466aec 08264674f98aa19e 3ca4eb47b26de7ac
```

Each sample consumes four successive xoshiro outputs in `k0, k1, k2, k3` order. This is a deterministic, reproducible pseudorandom sample of the full four-word key distribution; no scalar SMHasher3 seed expansion is involved.

| Quantity | Result |
|---|---:|
| N | 67,108,864 |
| Full-output collisions | 75 |
| Rate | 0.0000011175870895385742 |
| log2(rate) | -19.771181309504 |
| Approximate 95% Wilson interval for log2(rate) | [-20.096995, -19.445368] |
| Expected collisions at the claimed 1,227 / 2^30 rate | 76.6875 |
| Expected collisions at 2^-19.69 | 79.3413 |

The observed count is consistent with both stated approximations. This finite experiment does not establish the exponent to hundredth-bit precision. The modulo-2^64 sums of all outputs were `5424d5742e87e5c0` for m1 and `4a08f80104dce35f` for m2.

## Why the collision happens

The source initializes `enc=(k0,k1)`, `sum=(k2,k3)`, and an unchanged finalization key equal to their XOR; each absorbed block updates `enc = AESDEC(enc, block)` and `sum = shuffle(sum) + block`, with two separate 64-bit additions (`ref/rust-ahash.cpp:125–138,203–231`). For 56 bytes, it absorbs the overlapping blocks `[0,16)`, `[16,32)`, `[24,40)`, and `[40,56)` (`:311–326`). After the first block, the AES states differ only at byte indices 0 and 10, by `7e` and `65`. Independently enumerating the inverse S-box gives transitions `7e→de` and `65→a5`, each with 4/256 inputs; InvShiftRows puts these differences into rows 0 and 2 of column 0, and InvMixColumns maps `(de,00,a5,00)` to `(00,df,8d,29)`. The second message-block difference cancels `df` and `29`, leaving only `8d` at state byte 2. The third inverse-S-box transition `8d→f7` has 2/256 inputs; the resulting column difference `(ff,fb,fd,0e)` is exactly canceled by bytes 8..11 of the third block. These three substitution constraints give the AES trail a probability of 2^-19: the first state is uniform, and unconstrained bytes leave the third active substitution's input uniform after fixing the first two. Separately, carry-dependent arithmetic conditions in the shuffled-sum path must also hold; they reduce the full-state collision rate below that AES-only rate. The example trace shows both accumulators equal immediately after the third block, and all 75 sampled collisions have equal accumulators before finalization. The fourth block is identical, and the finalizer uses the same unchanged key, so equal states necessarily give equal complete outputs.

## Exact reproduction commands

Run from this directory on the supplied Apple ARM host; the compiler was Apple clang 17.0.0. These are the commands for the final implementation and experiment:

```sh
shasum -a 256 ref/Hashinfo.cpp ref/rust-ahash.cpp > ref-before.sha256
clang++ -O3 -std=c++17 -mcpu=apple-m2 -Wall -Wextra -Wpedantic verify.cpp -o verify
./verify 67108864 > run.log
python3 analyze.py > analysis.json
shasum -a 256 -c ref-before.sha256
```

The reference integrity check returned `OK` for both files. Their SHA-256 digests are:

```text
fb0ad09280aae231050527b46d2f807381b62c9d49b36e0b046c117cefb33c4d  ref/Hashinfo.cpp
b3e927ec82e47a6868508a6e2b49f7c68d0f3328cd3ec496e25b669b18150245  ref/rust-ahash.cpp
```

The matching reference verification, independently reproduced example, and 75 independently rechecked full-output collisions support the claimed unconditional approximate rate.

VERDICT: CONFIRMED
