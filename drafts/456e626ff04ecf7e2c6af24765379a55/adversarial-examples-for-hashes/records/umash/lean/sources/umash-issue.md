**This is not a collision report.** I have no pair of inputs that collides more often than the whitepaper's bounds allow. It is a question about one step of the published proof, with the partial results I could establish and some possible repairs.

### The step

`umash_reference.py`, lines 366–381 (whitepaper PDF p. 6, dated 2022-02-23):

> In all cases, the probability of collisions between different blocks is at most 2^-63. However, we feed the 128-bit compressed output to the polynomial hash as two values in F = Z/(2^61 − 1)Z, which loses slightly more than 6 bits of entropy. In total, the first-level block compressor introduces collisions with probability less than ≈ 2^-57.

The fingerprint proof (lines 744–770) uses the same move: 2^-99 raw, "≈ 2^-87 once we take into account the entropy lost to F".

I can follow everything before and after this sentence. The raw OH bound holds (I get 1/2^64 for equal chunk counts and under 2/2^64 for unequal counts). The polynomial root count holds. What I cannot derive is the sentence itself: that reducing each 64-bit half modulo p = 2^61 − 1 costs only the fibre size of the reduction.

### Why the inference does not follow as written

An almost-universal bound controls one event, exact equality of the raw 128-bit outputs. After reduction the two outputs collide whenever each half's integer difference is a multiple of p, which is a union over many difference targets. A many-to-one map does not in general multiply a collision bound by its fibre size, because probability mass that sits off the diagonal before the map can land on the diagonal after it.

Small counterexample to the general claim: five equiprobable keys, 6-bit outputs, H(x) = u and H(y) = u + 31 for u in {1, 2, 4, 8, 16}. The five XOR differences 33, 35, 39, 47, 63 are distinct, so the family is 1/5-XOR-universal; after reduction modulo 31 the outputs are equal for every key, although the largest fibre has size 3.

The obstacle for UMASH specifically is an algebra mismatch. PH is XOR-linear, so what is available is XOR-universality (every fixed XOR difference has probability ≤ 2^-64). Reduction modulo p is a statement about integer differences. The two are linked through carries: with D = x ⊕ y, the integer difference is x − y = 2(x ∧ D) − D. So "x ≡ y (mod p)" is equivalent to "x ⊕ y ∈ S" for a specific set S of masks (the supports of signed-digit representations of the multiples ap, |a| ≤ 8). S is computable exactly:

| word size | p | masks in S |
|---|---|---|
| 8 | 31 | 68 |
| 12 | 509 | 231 |
| 64 | 2^61 − 1 | 852 |

### What I could prove

- If some PH chunk differs in the compared block: condition on all keys but one word of that chunk's key pair; the XOR of the two OH outputs is injective in that word, so at most |S|² = 852² key values give both halves congruent. This gives a rigorous per-block bound 852²/2^64 ≈ 2^-44.5. Valid, but ~19.5 bits of loss rather than ~6.
- Pairs with different block counts, blocks with different chunk counts, and short-vs-long comparisons: these cases go through at the advertised strength (the final ENH key pair is fresh relative to the shorter input, and the point-mass bound (2q − 1)/q² does the rest).
- The natural repair, "for every setting of the other keys at most ~128 values of the free key word collide after reduction", is false. Exact fibre solving finds settings with 183 and 242 colliding values of the free word, and both are realized through the unmodified `umash.c` on 32-byte messages (seed chosen so the tag is zero; ENH products q + 1 and 2^61 (q + 1), which factor into valid 64-bit inputs). These are conditional counts on rare values of the other keys, not an attack.
- Exact scaled models (w = 8, 10, 12 with p = 31, 127, 509, all keys enumerated): the largest all-key increase found over the raw AU bound is a factor of about 4, i.e. about 2 bits (about 3 bits against the tighter equal-chunk-count bound), against the ~6 bits the sentence budgets. The worst toy family lifts to 64 bits and provably satisfies the primary bound. These are searched families, not maxima over all pairs.

### What remains open

1. Equal-length pairs where every PH chunk agrees and only the ENH data or the tag differs. The PH terms are then a common XOR mask on both outputs; the ENH theorem is an integer-additive differential statement; I do not have a lemma that carries an additive bound through a common XOR mask and then through reduction modulo p.
2. For the fingerprint, the joint cases with equal checksums (the 2^-99 raw argument, and the linear-elimination step at lines 676–731, provide raw XOR targets, not the projected differential equations).

So the missing statement is: a bound B₁(s) on the probability that the two reduced coefficient sequences fed to the polynomial hash coincide (equal block counts, unequal block counts, short/long), and its joint counterpart B₁₂(s) for the fingerprint. With those, `Pr[collision] ≤ B₁ + (1 − B₁)·d/(p − 2)` and the fingerprint analogue complete the proof. Have I missed an argument in the paper or the code comments that supplies them?

### Repair strategies I can see

Proof side:
- Prove the projection lemma for the two open cases. The signed-digit set S (852 masks per half) and the ADU property of NH are the ingredients; the ENH-only case is where I got stuck.
- Alternatively publish the weaker constants that do follow (the 852² route, extended to the ENH case if it can be), which would put the headline nearer 2^-44 per block than 2^-57.

Implementation side (each restores a clean end-to-end proof at some cost):
- Evaluate the second-level polynomial over GF(2^64) with carry-less multiplication instead of over F_p. The coefficients are then the raw 64-bit halves, there is no reduction step, and XOR-universality composes directly: a collision is either a raw OH collision (≤ 2/2^64 per block) or a root of a nonzero polynomial over GF(2^64). PCLMULQDQ/PMULL are already required by PH.
- Make the first level integer-additive throughout (NH/ENH for every chunk). Then each half's integer difference is covered by the NH differential theorem, and reduction modulo p becomes a union over a few hundred additive targets, each ≤ 1/q, so the ~2^-55 headline follows in a page. This is essentially the UMAC/VHASH structure and gives up part of PH's speed.

### Reproduction

The mask enumeration (`algebra.py`), the exact fibre solver (`lift_ph.py`), the production check against unmodified `umash.c` (`production_fibres.c`, counts 183 and 242), and the scaled exact models are small scripts; I am happy to attach them or push them to a repository if that helps. The proof audit was machine-assisted (two independent automated passes plus a third pass that attempted the lemma), and I have checked the statements above against the reference source myself.

I intend to describe this in a write-up comparing published hash bounds. It will say exactly what is stated here: the advertised UMASH bounds are unproved at one step, not contradicted, and no collision is claimed. I would value your review before that goes out, and will correct anything I have misread.
