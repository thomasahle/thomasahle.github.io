# PolymurHash 2.0: an attained 2^-53.90 pair under the shipped seeding

The ideal-key proof (Peters) takes k uniform on the restricted key set K. The library never draws k that way:
`polymur_init_params(k_seed, s_seed)` walks k_seed forward by a public constant and accepts the first value whose
exponent e = (v >> 3) | 1 passes a coprimality filter and whose k^7 is small enough; k = 37^e. Accepted
exponents therefore receive unequal weight: the number W(k) of 64-bit k_seed values that lead to k is the
number of values v with (v >> 3) | 1 = e (16) plus the rejected runs in front of each of them.

For the key k* = 391303342709703922 (exponent e* = 0x08890268f3958b33), W(k*) = 1096 (runs of 73, 73, 73, 74,
74, 74, 66 x 9 and 61), against a mean of about 97.6 over K. Pick a pair whose difference polynomial vanishes at
k*: every one of the 1096 seeds collides, and nothing else does except with negligible probability.

    M  = 2c336f948f6f6c99   (8 bytes, memory order)
    M' = aa373a8eeff85a     (7 bytes)
    polymur_init_params(k_seed = 0x8545715fd36ce9e9, s_seed = 0xd95d3b0f3d7ebc54):
        polymur_hash(M, 8, p, 0) = polymur_hash(M', 7, p, 0) = 0x5335b33210496b34

Exactness. On the 8..21-byte path (M) and the <= 7-byte path (M') no 128-bit product overflows and the final mix
is a bijection with a common s and tweak, so a collision happens exactly when the degree-9 difference polynomial
D(k) = (k^2+m0)(k^7+m0) + (k+m2)(k^3+8) - (k+m)(k^2+7) vanishes mod p = 2^61 - 1. D has 3 roots in F_p; only k*
is a generator of F_p^* with small k^7, so only k* is reachable (`polymur_roots.py`). Hence, for uniform
(k_seed, s_seed), epsilon = 1096 / 2^64 = 2^-53.902 exactly, and the pair (L = 1) caps the score at 53.90 bits.

Counts (`logs/polymur_witness.txt`): SMHasher3 verification 0x0722B1A7 reproduced with the unmodified header;
replaying the rejection loop from the explicit k_seed accepts after 42 steps at e*; all 1096 preimage seeds
collide (random s_seed each); 0 of 2,000,000 uniform (k_seed, s_seed) controls collide.

The 54.23-bit figure is a proof for uniform k on K, a distribution no public constructor produces. A lower
bound in the shipped seeding would need max_k W(k), which is not established here.

## Build and run

    curl -LO https://raw.githubusercontent.com/orlp/polymur-hash/a7cc6b00051b4b579d718a4f26428098580029ec/polymur-hash.h
    cc -O2 -std=c11 -o polymur_witness polymur_witness.c
    ./polymur_witness                 # about one second
    python3 polymur_roots.py          # needs sympy
