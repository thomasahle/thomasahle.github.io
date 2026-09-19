# Bounded aHash AES trail search

The selected pair is A_2 (56 bytes, L = 7), defined in [pairs.txt](pairs.txt) and checked against the upstream crate in the [independent verification report](../ahash-trail-verify/RESULTS.md). The [adoption memo](../MEMO.md) summarizes the bounded search and its limits.

The three-S-box trail starts at active byte positions {0,10}, continues through {1,3}, and cancels in bytes 32–35. The additive-lane differences (3, −14, 14, 17) reduce the carry penalty from about 0.69 to 0.38 bits. The independent crate measurement is 12,627 / 2^33 = 2^-19.376, giving log2(7/epsilon) = 22.18 bits, with 95% interval 22.16–22.21.

The pair generator and bounded trail work are preserved in [make_pairs.py](make_pairs.py), [trunc.py](trunc.py), [trunc2.py](trunc2.py), [valsearch2.py](valsearch2.py), [pairs.json](pairs.json), and the [C sampler](ahash_trail_sample.c). They describe a search family, not a proof of a global optimum.

A_1 is statistically tied with A_2; pooled counts 19,154 vs 18,929 over 2^34 keys give z = 1.15. The real 64-byte B_0 in [pairs_B.txt](pairs_B.txt) has a similar collision rate but a 22.38-bit cap because L = 8. Line 2 of [confirm_pairs.txt](confirm_pairs.txt) is a stale **56-byte** negative control; its label has been corrected to `B_0_STALE56B`, leaving the bytes intact. It is not the real B_0.

The explicit colliding key for A_2 is in the [2^33 upstream log](../ahash-trail-verify/records/confirm_33_seed78.log). Its public `with_seeds` arguments are `a9438ebec17194e4 0bb688b615fedc93 499051d9d7fdc675 9b696bdcaa2076a5`, giving `21a01e6ddf40c84a`. The earlier key beginning `711096b4` belongs to the historical pair.

The maintained [reproduction package](../../../verify/rust-ahash/README.md) supplies A_2, A_1, B_0, deterministic Xeon reruns, and both C and pinned native Rust programs.
