# xxh3-128 under the random-secret key model (2026-09-19)

STATUS: measurements launched on the Xeon at 08:22Z (batch `run_all.sh`, sequential, cores 24-31,
nice 10, shared with other nice-10 jobs at load ~55, so ~1.3 cores effective); results land in
`logs/<name>.txt` on the host under `agents/random-secret/xxh3-128/` and `logs/BATCH_DONE` marks
the end. Copy the logs into this directory when done (`scp -r <host>:agents/random-secret/xxh3-128/logs .`).
No rate below was measured yet when this file was written; the analysis section states what the
runs are expected to show and why.

## 1. Secret mechanism and size
* `XXH3_128bits_withSecret(data, len, secret, secretSize)`: caller-supplied secret, `secretSize >=
  XXH3_SECRET_SIZE_MIN = 136` bytes (1088 bits); default size `XXH3_SECRET_DEFAULT_SIZE = 192`
  bytes (1536 bits, used here). Seed is 0 on this path. `XXH3_generateSecret()` derives one from
  arbitrary seed material; `XXH3_generateSecret_fromSeed(seed)` gives only 64 bits (kSecret +/- seed,
  identical to `_withSeed`).
* `XXH3_128bits_withSecretandSeed(data, len, secret, secretSize, seed)`: for len <= 240 it IGNORES
  the custom secret and hashes with kSecret + seed (= the blog row's current model); for len > 240 it
  uses the custom secret and ignores the seed. So "seed AND secret both random" for short inputs
  exists only through the static `XXH3_128bits_internal(input, len, seed, secret, 192,
  XXH3_hashLong_128b_withSecret)` (reachable with XXH_INLINE_ALL), which is what api=secretseed runs.
  For 17..240 B the seed only shifts uniform secret words (S0+seed, S1-seed), so it adds nothing to a
  uniform secret: api=secret and api=secretseed are the same distribution of the keyed words.
* Key bits under the model: 1536 (192-byte secret) [+64 seed, redundant].

## 2. Analysis (what decides a 17..128-byte collision under a uniform secret)
For 32 B: acc.lo = 32*P1 + fold(w0^S0, w1^S1) ^ (w2+w3); acc.hi = fold(w2^S2, w3^S3) ^ (w0+w1);
fold(a,b) = lo(ab)^hi(ab); output = (aval(lo+hi), -aval(lo*P1 + hi*P4 + c)). P1-P4 has one trailing
zero, so the output collides iff (acc.lo, acc.hi) are equal OR both differ by exactly 2^63 ("twin").
Every message word is XORed with an independent uniform secret word before any non-linear step, so a
pair's rate depends only on the per-word XOR differences (d0,d1) and on whether the unkeyed sums
w0+w1, w2+w3 are preserved. Pair F (w1 = ~w0, both complemented) preserves w0+w1 = -1 and needs
fold(A,B) = fold(~A,~B) for A,B independent uniform. (~A)(~B) = AB + T - 2^64 (T+1) mod 2^128 with
T = A+B+1, so the event is "carry chain of lo(AB)+T equals borrow chain of hi(AB)-T", heuristically
(3/4)^64 = 2^-26.56 for uniform T -- pair-independent. Hence the expectation: the random-secret rate
of F is ~2^-26.6, the same as the default-secret row figure (165/2^34 = 2^-26.63), and every pair in
the class (sum -1 or 2^63-1 on one 16-byte block; twin classes 2^62-1, 2^63+2^62-1) has the same
rate. The xxh3-64 NAF trick (2^-10.47) needs the fixed default words and cannot transfer.
Short paths (1..16 B) are injective per length under a uniform secret (combinedl carries len;
g(x)=x+lo32(x)*(P32_2-1) is bijective; the 4..8 product by an odd constant is injective), and
cross-length pairs are ~2^-64. Long path (>240 B): swapping w and ~w in the same lane of two stripes
of one block keeps the unkeyed lane sums and needs lo32+hi32 of the two keyed words to agree:
Pr = (2/3)2^-32 = 2^-32.58 with L >= 31 words -> ~37.5 bits, worse than 2 + 26.6.
Prediction for (3): no fixed pair beats the complement family; recommended row score stays ~28.6.

## 3. Runs (all deterministic; `rs128` prints the SMHasher3 verification 0x288DAA94 first)
    cc -O2 -std=gnu11 -pthread -o rs128 rs128.c -lm      # xxhash.h v0.8.3 sha256 17973c0d...
    python3 gen.py                                        # regenerates F.txt family.txt long.txt controls.txt masks*.txt
    ./rs128 pair 34 11 8 secret F.txt          # (2) row pair F, fresh 192-B secret per key, 2^34 keys
    ./rs128 pair 33 12 8 secretseed F.txt      # (2) seed AND secret random, 2^33 keys
    ./rs128 pair 32 13 8 seed F.txt            # control: default secret + random seed (row model)
    ./rs128 pair 33 14 8 secret family.txt     # (3) complement classes, twin classes, 64/160-B, NAF control
    ./rs128 fold 30 17 8 masks1.txt            # (3) fold XOR-differential scan, 759 masks
    ./rs128 fold 28 18 8 masks2.txt            # (3) 500 distance-2 neighbours of all-ones
    ./rs128 pair 28 16 8 secret controls.txt   # (3) random + single-bit pairs, lengths 1..256
    ./rs128 pair 35 15 8 secret long.txt       # (3) long-path stripe swap / top-bit pairs, 248 B
    python3 ci.py <count> <N> 4                # exact Poisson 95% CI and cap bits for L=4
Resolution floors: a zero count in N trials means rate < 3.69/N at 97.5% (2^30 -> 2^-28.1).
