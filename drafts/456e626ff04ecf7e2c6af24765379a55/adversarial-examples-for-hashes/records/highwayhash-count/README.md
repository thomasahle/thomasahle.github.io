# trail/exact-cond2 — the three-packet HighwayHash trail has an EXACT probability

Target: the pair from `angles/weak-keys-init` (three 32-byte packets, differences in lane 0 only,
`m1 = d131b3012a2a1924 0^24 | d132b3b42a2a1924 0^24 | 0^32`, `m2 = m1 + (2^8 | -(2^9+2^24) | 2^8)` in lane 0),
which collides in the full 1024-bit state after packet 3 (hence in HighwayHash-64/128/256 and under any suffix).

## Result (proved)

Let condition (1) be hi32(key[0]) = 0xdbe6d5d5 (exact probability 2^-32).  Then, with lo32(key[0]) and
key[1] uniform (key[2], key[3] arbitrary: lanes 2,3 cannot reach lanes 0,1 before finalization),

    Pr[ full-state collision | (1) ]  >=  Pr[ E_2 | (1) ]  =  235 * 239 / 2^40  =  56165 / 2^40  =  2^-24.2226

exactly, where E_2 is the packet-2 multiplier condition lo32(v1[0]) - hi32(v0[0]) = 2^8 (mod 2^32).
Over a uniformly random 256-bit key:

    eps  >=  235 * 239 / 2^72  =  56165 / 2^72  =  2^-56.2226           (vs. the measured 2^-56.3)

The "top-byte skew" buys exactly 56165/256 = 219.4 = 2^7.78 over a uniform 32-bit difference (2^-32).
In the paper's metric bits = min_L log2(L/eps) with L in 8-byte words this is log2(12) + 56.2226 = 59.81 at L = 12.

Every side condition of the trail is either automatic or absorbed into the count:
- the +2^8 stays in byte 1 of v1[0] at packet 1: lo32(v1[0]) is the CONSTANT 0x10e82046 under (1);
- the zipper moves never carry: byte 5 of v0[0] after packet 1 is byte 1 of h, which is 0xbe or 0xbf for
  EVERY key in the class (h = 0x81d3be10 + W1[7]*2^16 + W1[6] + gamma mod 2^32, gamma in {0,1,2});
  so the side condition "byte1(h) != 0xff" of the previous panel holds with probability 1;
- the packet-2 subtraction leaves exactly -2^8 in byte 1 of lo32(v1[0]) with no borrow: under E_2,
  x = h + 2^8 as integers (h < 2^32 - 2^8 because byte 3 of h is 0x81 or 0x82) and byte1(x) = byte1(h)+1 != 0;
- the byte-5 cancellation in v0[0] is modular (+2^40 - 2^40), no condition;
- packet 3 cancels exactly (v1[0] += mul0[0] + p3 with equal mul0[0]).
The multiplier products x*h and (x-2^8)(h+2^8) (mod 2^32 operands) coincide iff x - h = 2^8 (mod 2^32),
including the wrap-around cases (lemma.tex, step 2).

The count.  E_2 is equivalent to four byte equations (lemma.tex, step 3):
  (iv') e0 = 0  [0x10 + W1[6] + gamma <= 255]  and  W1[7] >= 0x15      (the top byte 0x4d of D0)
  (i)   U0[3] = 0xca + W1[6] + gamma (mod 256)
  (iii) U0[2] = 0xeb + W1[7]         (mod 256)
  (ii)  U1[4] = 0x9d + [h0 >= 0x46]  (mod 256)
where W1 = (init1[1] ^ rot32(key[1])) + init0[1], U0 = v0[0] after packet 1, gamma = ca + cz is the carry
count of a + 0xcb0ef593 + Zc.  (ii) is a one-time pad: byte 0 of lo32(W1) (a bijective image of byte 0 of
hi32(key[1])) enters it through a byte bijection and nothing else, so (ii) costs exactly 2^-8 (the single
delicate case lo32(W1) = 299f31xx, where the carry c1 into hi32(W1) depends on that byte, is handled
explicitly).  The remaining count M = #{(a, W1[4], W1[6], W1[7]) : (i),(iii),(iv')} = 2^24 * 235 * 239 is proved
by a 16-bit window argument (step 5) and confirmed by two independent computations (below).

## Files

- `c/highwayhash.{c,h}`, `c/highwayhash_test.c` — byte-identical copies of tools/bench/adversarial/vendor/highwayhash/c/
  (`hh_test` prints "Test success" on the 65 published vectors + the 33-byte vector).  Never modified; all programs
  `#include "c/highwayhash.c"` unmodified so the static `Update`/`Reset` are visible.
- `common.h` — the pair, the explicit packet-1 formulas (`formula_p1`), zipper terms copied verbatim, PRNG.
- `formula_check.c` — formulas vs. reference state on 10^6 class keys.
- `byte_eqs_check.c` — the byte system (i)-(iv') vs. the reference D0 == 0x4d000000, key by key (random + constructed keys).
- `constructed.c` — the pad/reduction claims on constructed keys through the reference (tests A-D).
- `count_M.c` — exact M by a 2^32 dynamic count; `brute_w4 <w4...>` re-counts, for each listed W1[4], over ALL 2^32
  values of a with the real-arithmetic predicate (3 threads); the proof says every w4 gives 256*256*235*239 = 3,680,829,440.
  (`brute`, the full 2^40 loop, was started and stopped after 5 min: at load average ~300 it would not finish in the budget.)
- `sample.c` — uniform class keys through the reference (3 threads), full-state collision count.
- `lemma.tex` — the lemma and proof.
- `dbg_special.c` — scratch used to understand the exceptional triple (kept for the record).
- outputs: `formula_check.txt`, `byte_eqs_check.txt`, `constructed.txt`, `count_M.txt`, `count_M_brute.txt`,
  `count_M_brute_w4.txt`, `sample30_seed11.txt`, `sample31_seed12.txt`.

## Reproduce (M2 Pro, 3 threads max; load average ~200-300 during these runs)

```
cd <this dir>
clang -O2 -std=c11 -I. c/highwayhash.c c/highwayhash_test.c -o hh_test && ./hh_test        # Test success
clang -O2 -std=c11 -I. formula_check.c -o formula_check && ./formula_check 1000000 7        # 0 mismatches
clang -O2 -std=c11 -I. byte_eqs_check.c -o byte_eqs_check && ./byte_eqs_check 3 1000000    # 0 mismatches
clang -O2 -std=c11 -I. constructed.c -o constructed && ./constructed 5 65536                # A-D all 0 violations
clang -O3 -std=c11 -I. count_M.c -o count_M -lpthread -lm && ./count_M                      # M = 942292336640 (49 s)
./count_M brute_w4 00 0a 0b ff 7c                                                            # 5 x 2^32, each == 3680829440
clang -O3 -std=c11 -I. sample.c -o sample -lpthread -lm && ./sample 30 11                   # 2^30 class keys, 160 s
./sample 31 12                                                                               # 2^31 class keys
```

## Measurements (all through the unmodified reference)

See the output files; summary:
- formula_check (10^6 class keys): 0 mismatches between the explicit formulas and the reference for
  v0[0], v1[0], v0[1], v1[1], mul0[0], mul1[0], mul0[1], mul1[1] after packet 1; the packet-1 difference pattern
  (v0[0] + 2^40, v1[0] + 2^8 + 2^24, everything else equal) held for all 10^6 keys; byte 1 of h in {be, bf} for all;
  D0 top byte histogram 4c: 39.3 %, 4d: 56.5 %, 4e: 4.2 %, 4f: 0.02 %.
- byte_eqs_check: (i)&(ii)&(iii)&(iv') == [D0 == 4d000000] on 10^6 random class keys (0 true) and on 999,936
  constructed keys (3,331 true; expected 3,347): 0 mismatches.
- constructed (65,536 tuples): for 56,197 a-side-solvable (a, K1l, w3, w2, w1) tuples, exactly one of the 256
  values of W1[0] collides (0 exceptions); with W1[6] perturbed, none collides; for uniformly random tuples the
  number of colliding W1[0] equals the a-side predicate (0 mismatches); the exceptional triple (29,9f,31)
  behaves as the proof says (0 violations in 3,496 x 2 alignments).
- count_M: M = 942,292,336,640 = 2^24 * 235 * 239 (DP over 2^32 values of lo32(v0[0]) collapsing W1[4] through one carry);
  brute_w4: for W1[4] in {00, 0a, 0b, ff, 7c} (including both sides of the only W1[4] threshold, 0x0b) the direct count over
  all 2^32 values of a equals 3,680,829,440 = 2^16 * 235 * 239 exactly: all five match (count_M_brute_w4.txt, 120 s wall).
- sample 2^30 (seed 11): 51 full-state collisions, all 51 also HighwayHash64 collisions; expected 54.85 +- 7.41.
- sample 2^31 (seed 12): 98 full-state collisions, all 98 also HighwayHash64 collisions (expected 109.70 +- 10.47).
- Pooled own sampling: 149 collisions in 3 * 2^30 class keys (expected 164.5 +- 12.8), observed rate 2^-24.366.
- Previous panel (independent code): 614 / 1.26e10 class keys (expected 643.6 +- 25.4 under the proved rate),
  26 / 2^29 (expected 27.4).

## What is proved vs. measured vs. conjectured

Proved (lemma.tex): Pr[E_2 | (1)] = 235*239/2^40 exactly; E_2 => full-state collision; hence eps >= 56165/2^72.
Measured: the rates above, consistent with the proved value.
Not proved: that no OTHER trail collides (so "=" for eps is a conjecture; the previous panel hashed every key in a
2^28 class sample and found no collision outside E_2).  Whether a different constant c2 (top byte 4e or 4f) or a
different low-24-bit target could do slightly better is not analysed here; the measured conditional top-byte
distribution (previous panel: 4d 85.6 %, 4e 13.9 %, 4f 0.5 % given low24 < 2^12) bounds any such gain by ~17 %.
