# aHash 0.8.12 (AES path): lane-tied echoes, few-way multicollisions

The post's 424-byte pair E is a two-S-box "lane echo" in one AES lane: a one-byte difference from the tail is
cancelled by an InvMixColumns column in the first loop block (difference-table entry 4/256), and a second
injection four blocks later is cancelled the same way. This package places the same echo in all four AES lanes
of a 448-byte message and lets each lane independently take the M or M' bytes of the pattern: 2^4 = 16 messages.

Generator (`build_tie` in `ahash_mc.c`, and `build_tie` in `native-crate-check/src/main.rs`): 448 zero bytes; for
lane j = 0..3 with choice bit c_j, at offset o = 16j + 8:

    m[384 + o]          = {0x03, 0x08}[c_j]                      (tail injection)
    m[o .. o+3]         = {7c067e7b, 81088180}[c_j]              (block 0 column)
    m[256 + o]          = {0x08, 0x03}[c_j]                      (block 4 injection)
    m[320 + o .. o+3]   = {80088280, 7d067d7b}[c_j]              (block 5 column)

Key model: four independent uniform RandomState words (`with_seeds(a, b, c, d)`), byte slices hashed with
`hash_one(&[u8])` (length, then bytes).

## Results

| event | this transcription, 2^31 keys (x86 AES-NI) | native crate 0.8.12, 2^30 keys (x86, target-feature aes) |
|---|---|---|
| one echo (slot 0 pair) | 484,058 = 2^-12.115 | 242,644 = 2^-12.112 |
| echoes in lanes 0 and 2 together (4-way set {0,1,4,5}) | 446,660 = 2^-12.231 | 224,041 = 2^-12.227 |
| echoes in lanes 1 and 3 together (4-way set {0,2,8,10}) | 446,245 = 2^-12.233 | 222,952 = 2^-12.234 |
| echoes in lanes 0 and 1 together | 120 = 2^-24.09 | - |
| all 16 messages equal | 104 = 2^-24.30 [2^-24.59, 2^-24.02] | 47 = 2^-24.45 |

Lanes 0 and 2 succeed under one key condition (joint rate 2^-12.23, almost the single-echo rate), and so do
lanes 1 and 3; the two groups are independent. So 4 messages collide for about 2^-12.2 of keys and 16 for about
2^-24.3. Longer messages would allow more independent groups (4^r messages at about 2^-12.3 r); only r <= 2 is
measured here.

**Witness** (16-way): internal key words 9644eff90e6144b3 7c41347a2bb5039f 3c026049b1eee2d8 38f6b49792da5af9,
i.e. `RandomState::with_seeds(0xd36cce1f36b157c4, 0xc21552b51f5c0ff3, 0xfcae49fe7892b205, 0x07726122279d53ee)`:
all 16 messages hash to 0x5593e3264efc6b5b (transcription and native crate agree). Members 0 and 15 in memory order
are printed in `logs/native_crate_2p26.txt`.

## Build and run

    cc -O2 -std=c11 -maes -pthread ahash_mc.c -o ahash_mc -lm          # arm64: -march=armv8-a+crypto
    ./ahash_mc tie 31 8 0x77ad        # checks 0x3BF4383B and pair E, then the 16-message family
    ./ahash_mc pairE 32 8 0x77ac      # pair E: 970,320 / 2^32 = 2^-12.112

    cd native-crate-check
    RUSTFLAGS="-C target-feature=+aes,+sse2,+ssse3" cargo build --release
    ./target/release/ahcheck 30       # x86-64 only: aHash 0.8.12 uses AES on ARM only with nightly-arm-aes

Expected output is in `logs/`.
