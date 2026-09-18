# UMASH projection lemma: partial results and remaining gap

**Verdict: open.** This investigation proves several of the missing cases, gives exact all-key counts for specified scaled families, and exactly lifts the worst family found to 64 bits. It does **not** prove the general 55-bit or 83-bit statements, or exhibit a counterexample to either statement. It does refute the proposed *uniform one-free-key-word* estimate with constant 128: actual production OH has conditional fibres of sizes 183 and 242.

The maximum over **all message pairs** was not computed. The numerical maxima below are over the explicitly specified search families. In particular, the all-key hill climb varies one changed PH chunk within two families, not arbitrary words in arbitrary blocks. The remaining computational omissions are listed in §8.

## 1. Model and source correspondence

Set `q = 2^64`, `p = 2^61 - 1`. Initially all 34 OH words are independent and uniform. Messages and the optional seed are fixed independently of these keys. Polynomial multipliers are independent of OH keys and each other.

Read `AUDIT_REVIEW.md` §2 in full before undertaking this analysis; `AUDIT.md` §§3–4 identifies the same composition problem. The disputed statements are in `umash_reference.py` **366–382**, **744–771**, and the more explicit fibre multiplication claim at **1176–1185**. The corresponding PDF passages are on pp. 6 and 11–12.

The executable definition is slightly richer than the displayed OH formula at reference **240–246**. Write

```
PH(a,b;k,l) = (a xor k) carryless_product (b xor l)
N = ((a+k) mod q) * ((b+l) mod q)
E_t(a,b;k,l) = (N mod q,
                 (((floor(N/q)+t) mod q) xor (N mod q)))
OH = xor of the nonfinal PH chunks, then xor E_t of the final chunk.
```

Here `t` is the high-word tag; the reference's 128-bit tag is `q*t`. In particular, **ENH XORs its low word into its high word**. This operation is in reference **1098–1106** and production `umash-src/umash.c` **541–568**. It must be included in a projection analysis. The map `T(lo,hi)=(lo,hi xor lo)` is invertible for raw equality, but it cannot simply be omitted before reduction modulo `p`.

The checksum, secondary shufflers, and block combination are reference **1092–1108**, **1112–1149**; production **588–649**. Each reduced block contributes low and high coefficients in that order to a polynomial with positive powers, reference **270–282**, **1193–1217**. Production performs a congruent calculation with a precomputed square and representatives modulo `8p`; projected coefficient equality is a sufficient condition for *formal equality modulo p*, not necessarily for equality of the full 64-bit result.

The reference samples its multiplier from `F_p \ {0,1}` and 34 distinct OH words, reference **846–863**. An IID OH-event bound can therefore be divided by

```
D = 1 - 561/q > 0.
```

This follows by conditioning IID words on distinctness and the union bound on 561 pairs. The C initializer actually permits multiplier 1, `umash.c` **1023–1032**; using the smaller denominator `p-2` below is conservative for a uniform multiplier on `F_p \ {0}` too. No information-theoretic distribution claim for a fixed-size Salsa20 seed is established here.

## 2. An exact signed-digit lemma

For general word size `w`, put `Q=2^w` and define

```
S(w,p) = { x xor y : 0 <= x,y < Q, x = y (mod p) }.
```

If `D=x xor y` and `t=x & D`, then, as ordinary integers,

```
x-y = 2t-D.
```

Thus the valid assignments to the differing bits for a given `D` are exactly

```
T(D) = { (D+a*p)/2 : |a| <= floor((Q-1)/p),
         D+a*p is nonnegative and even,
         ((D+a*p)/2) & D = (D+a*p)/2 }.
```

The bits outside `D` are unrestricted. This is the precise signed-digit condition; neither `x-y` nor its sign is replaced by a fixed XOR target.

An equivalent complete way to enumerate `S` is to enumerate signed binary representations of `a*p`. Let `R(n,w)` be their support masks, allowing digits `-1,0,+1`. The exact recurrence is

```
R(0,0) = {0}; R(n,0) = empty for n != 0
R(n,w) = empty if |n| >= 2^w
R(n,w) = {2s : s in R(n/2,w-1)}                         if n even
R(n,w) = {2s+1 : s in R((n-1)/2,w-1)
                    or R((n+1)/2,w-1)}                 if n odd.
```

Taking the union for `0 <= a <= floor((Q-1)/p)` suffices, since negating digits preserves their support. The recurrence is complete by inspecting the low digit. Conversely, each signed representation is realized by placing its positive digits in `x` and negative digits in `y`; equal bits can be added outside its support. Therefore this procedure computes exactly `S`, with no sampling or assumption on the distribution of `x`.

[algebra.py](toy/algebra.py) implements this recurrence and independently checks the resulting sets by enumerating all congruent word pairs at the three toy widths. Its exact output is:

| w | p | distinct masks in S | signed-support counts for a = 0,...,8, before union/deduplication |
|---:|---:|---:|---|
| 8 | 31 | 68 | 1,16,11,25,6,19,9,10,1 |
| 10 | 127 | 96 | 1,22,15,37,8,31,13,16,1 |
| 12 | 509 | 231 | 1,47,32,77,17,46,27,45,2 |
| 64 | 2305843009213693951 | **852** | 1,184,123,361,62,355,121,178,1 |

The 64-bit cardinality is a small exact computational certificate from this proved recurrence, not an extrapolation of the toy counts.

### 2.1 Consequence for a differing PH chunk

Condition on all other keys and one word of the selected PH key pair. Choose the unconditioned word so the corresponding multiplier difference is nonzero. The two complete OH values have the form

```
X(v) = u carryless_product v xor A
Y(v) = u' carryless_product (v xor delta) xor B,   u != u'.
```

Their XOR is

```
(u xor u') carryless_product v xor (u' carryless_product delta) xor A xor B.
```

This is injective in `v`, because binary polynomials form an integral domain. Simultaneous reduced equality of the two halves requires this XOR to belong to `S x S`. Hence there are at most `|S|^2` admissible keys, regardless of `A,B,delta,u,u'`. At 64 bits this proves

```
Pr[projected primary block collision] <= C/q,
C = 852^2 = 725904,
```

whenever an aligned differing block has a differing PH chunk. This is a valid use of XOR universality: the set of possible XOR targets has been explicitly counted. It is much too loose to certify the advertised primary constant.

[lift_ph.py](toy/lift_ph.py) also gives an exact version of this argument: enumerate the allowed half masks, divide the resulting binary polynomial by `u xor u'`, retain quotients of degree below 64 with zero remainder, then check both actual congruences. This exhausts possible keys without iterating through `2^64` values.

### 2.2 The desired conditional constant 128 is false

With `w=64`, the exact solver finds the following slices. Masks `A,B` are full 128-bit XOR constants, written as integers; all shown masks occupy only the low half.

| u | u' | delta | A | B | colliding v |
|---|---|---:|---:|---:|---:|
| `2^61+1` | `2^61+3` | 0 | 0 | 0 | **183** |
| `2^62+1` | `2^62+3` | 0 | 1 | `2^61` | **242** |

For these rows, `u xor u'=2`, so equality of the high residues forces the top bit of `v` to vanish. Only the finite low-word support set needs to be searched. The saved key lists and the polynomial-division enumeration certify completeness, as well as membership of the listed keys.

These slices can be realized by the actual compressor, including its ENH step. [production_fibres.c](toy/production_fibres.c) directly includes the supplied, unmodified `umash.c`, constructs two 32-byte messages, and fixes seed 32 so the tag is zero. It checks all 183 or 242 keys through `oh_varblock`.

For the second row the ENH products are `q+1` and `2^61*(q+1)`. The factorization

```
q+1 = 274177 * 67280421310721
```

and factors `274177*2^43`, `67280421310721*2^18` for the latter product give valid 64-bit inputs. After the ENH triangular XOR these products are precisely masks 1 and `2^61`. The first row uses a zero ENH product. Input words are adjusted for the fixed additive ENH keys.

All 33 fixed OH words are distinct and avoid every listed free-word key. The counts are therefore 183 and 242 both in the IID slice and in the distinct-word slice, whose denominators are respectively `q` and `q-33`. Raw OH equality occurs for **1** and **0** of these keys. Three deterministic full-hash checks at polynomial multipliers 2, 3, and 123456789 give respectively `[183,2,2]` and `[0,0,0]` collisions among the listed keys. Those three checks are implementation checks, **not** an estimate of the probability over polynomial keys.

These are counterexamples to a uniform conditional `128/q` lemma. They are **not** counterexamples to an unconditional `128/q` bound after averaging the other keys, and are not counterexamples to the `512/q = 2^-55` full-hash headline. The messages in this experiment are fixed, but almost all OH words are conditioned on particular values.

The same conditional patterns were counted at widths 8,10,12,16,32,64 with modulus `2^(w-3)-1`. The 242-key pattern has counts 18,26,34,50,114,242: growth in this conditional family is real at the evaluated widths. At `w=12`, that particular control modulus is 511 and is composite. At the **requested prime 509**, the same pattern has only 2 keys. Do not combine these two experiments into a claim of growing all-key loss at the requested primes.

## 3. Small bounds for length and short-input cases

These bounds are on formal polynomial identity, not just equality of equal-length coefficient vectors.

### 3.1 A projected point-mass bound

For independent uniform `a,b` in `[0,q)`, a prescribed nonzero integer product has at most `q-1` preimages: for each nonzero `a` there is at most one `b`. Product zero has `2q-1` preimages. The same statements hold for full carry-less products, by injectivity for a nonzero first factor.

A residue of a 64-bit word modulo `p` has at most 9 representatives. A specified pair of residues therefore gives at most 81 raw OH targets. Undoing a fixed XOR contribution, the ENH triangular XOR, and the tag is a bijection on those targets. Even if one inverse target is zero, their total probability is at most

```
U = (82q-81)/q^2 < 82/q.
```

This is valid fibre counting: it bounds the conditional mass of each *output point*, rather than multiplying an AU diagonal bound by a fibre size.

For a fresh checksum PH product, the most significant bit of the 128-bit product is zero. After a fixed XOR mask the high word lies in one specified half of the word range, with at most 5 representatives of a given residue. There are at most `9*5=45` targets. Thus a secondary output point has conditional probability at most

```
V = (46q-45)/q^2 < 46/q.
```

### 3.2 Unequal numbers of chunks in an aligned block

Condition on everything except the longer block's last ENH key pair. Those keys are unused by the shorter block. Its reduced output is fixed, and the longer block has a fixed XOR offset plus a fresh ENH product. The primary projected collision probability is at most `U`.

For the secondary, unequal chunk counts have checksum equality probability exactly `q^-2` under IID keys: the checksum difference contains a fresh independent 128-bit key pair. Outside that event, condition on the original keys and expose the fresh checksum-PH keys. The signed-support argument gives conditional secondary collision probability at most `C/q`. Consequently the following safe bounds hold for this aligned block:

```
B1 <= U
B2 <= q^-2 + C/q
B12 <= q^-2 + U*C/q.
```

No independence between the completed compressors was used.

### 3.3 Unequal numbers of blocks

Let the longer string have `n` blocks and the shorter one fewer than `n`. Their polynomials use only positive powers. Formal equality forces **both coefficients of the longer string's first block to be zero modulo p**, because their degrees `2n` and `2n-1` exceed the shorter degree. This explicitly handles vanished leading coefficients.

The point-mass argument gives `B1 <= U`. Conditional on all original OH keys, the secondary first-block value contains a fresh checksum PH product, giving `B2 <= V` and

```
B12 <= U*V.
```

Reusing the OH keys across later blocks does not invalidate these implications; we only bound a necessary event on the first block.

### 3.4 Short versus long

Undo the long finalizer on the short result and reduce it modulo `p`, obtaining a key-dependent constant `c`. The long polynomial has zero constant term, so an identically zero comparison polynomial requires `c=0` and all its positive coefficients zero.

For a fixed short message the short result is uniform on 64-bit words under IID keys: its length-selected noise word passes through permutations, reference **942–963**. The long finalizer is a permutation, reference **1245–1256**: in `F_2[R]/(R^64-1)`, its multiplier is `1+R^8+R^33`, which is 1 at `R=1`, and hence coprime to `(R+1)^64`. Thus `c=0` has probability `9/q`.

The two short fingerprint components use different noise words, reference **838–841**, **934–939**, so their outputs for one fixed short message are independent uniform words. Therefore

```
B1 <= 9/q,   B2 <= 9/q,   B12 <= 81/q^2.
```

This remains valid although the long message uses some of those same OH words: the zero-polynomial events imply the indicated short-output events; no independence from the long computation is asserted.

For completeness, distinct short strings of the same length never collide. Different short lengths have IID collision bounds `1/q` and `1/q^2`. For the fingerprint, the two distinct constraints on length-indexed noise words form a two-edge forest, even when one endpoint is shared. Expose its vertices from a root to get the two factors `1/q`. Divide these IID bounds by `D` for distinct OH words.

## 4. Where the general proof still stops

For equal block counts, distinct encoded strings have at least one differing aligned block tuple (expanded chunks, tag). Chunking and tagging are reference **1012–1058**, **1152–1158**; the tag recovers the final block's length and the chunks recover its bytes. Select such a block before sampling keys. Whole-polynomial identity implies the corresponding reduced block equality, so the following block-case ledger suffices to identify the remaining issues.

| Case in the chosen aligned block | B1 established here | B2 established here | B12 established here |
|---|---|---|---|
| Different chunk counts | `U` | `q^-2+C/q` | `q^-2+U*C/q` |
| Same counts, some PH chunk differs, checksums differ | `C/q` | `C/q` | `C^2/q^2` |
| Same counts/checksum, at least two PH chunks differ | `C/q` | `2C/q` | `C^2*2^29/q^2` |
| Same counts/checksum, a PH chunk and the ENH data differ | `C/q` | `2C/q` | only the marginal `C/q` here |
| Same counts, all PH chunks agree, ENH data differ | no small general bound here | `C/q` | only that secondary marginal here |
| Same expanded data, tag differs | no small general bound here | no small general bound here | no small general bound here |

All probability bounds may of course be clipped at 1. For the fifth row the checksum chunks necessarily differ, so the fresh checksum PH supplies the stated secondary bound even though the primary remains unresolved.

Here are the joint arguments and their limits, to avoid silently importing the disputed projection inference.

* If checksums differ, condition on all original OH keys. Their primary collision event is then fixed. The fresh checksum-PH keys give the conditional `C/q` secondary factor. This proves the second row.
* If checksums agree, their checksum-PH outputs cancel in raw XOR differences. A nonfinal shuffler is a lane-wise left shift by one composed with an invertible XOR-shift, or just the left shift. Its kernel on PH's 127-bit output space has size 2. Hence a differing PH chunk gives secondary XOR-target probability at most `2/q`, and projected marginal at most `2C/q`.
* With two differing PH chunks, the linear elimination at reference **676–731** works for every fixed pair of raw XOR targets. The kernel intersection has at most `2^29` possible differences; independence of the two chunk key pairs then gives `2^29/q^2`. Taking a union over `C^2` possible target pairs is valid. Its constant is `282896942250948820992`, far too large to certify 83 bits.
* With one PH and one ENH chunk differing, projection no longer forces the PH difference to be zero. The invertible `I+shuffler` equation determines a generally nonzero PH XOR difference and a generally nonzero ENH XOR difference. The ENH theorem in reference **226–238** is an **integer additive** differential theorem. It does not supply the needed XOR-target bound. The raw-zero argument at **627–656** therefore does not complete this projected case.
* If only ENH or its tag differs, a common PH XOR mask cannot be canceled through reduction modulo `p`. Neither a small cardinality for the residue fibres nor the integer NH differential bound alone resolves this.

Thus a small averaged PH projection theorem, an ENH theorem stable under the common PH mask, and a suitable joint theorem for the equal-checksum cases remain missing. The 183/242-key examples show that simply strengthening §2.1 to a uniform 128-key slice theorem is not a viable repair.

For the cases with bounds, let `r_s=min(1,2 ceil(s/256)/(p-2))`. The review's completion formulas apply:

```
Pr[primary collision] <= B1 + (1-B1)*r_s
Pr[fingerprint collision] <= B12 + (B1+B2)*r_s + r_s^2.
```

Under distinct-word conditioning divide the OH-event bounds `Bi` by `D`; the polynomial root term itself does not need that factor. Unequal-block, unequal-chunk, and short/long primary cases certify the advertised 55-bit envelope: `d <= 32 ceil(s/4096)` and the largest constant here is less than 82, leaving ample room below `512 ceil(s/4096)/q`.

For the fingerprint, the unequal-block, unequal-chunk, short/long, and differing-checksum-with-PH-difference cases also certify the advertised envelope on their respective pairs. At 64 MiB the exact rational calculations give upper bounds divided by `2^-83` of respectively approximately **0.500015259, 0.586545967, 0.500002146, 0.688045440**. Increasing `ceil(s/2^26)` preserves the quadratic envelope. These are **case-specific certificates**, not a certificate for the missing rows.

## 5. Scaled exact model and search

The primary and secondary literal definitions are in [model.py](toy/model.py). The main experiments use `w=8,10,12`, primes `31,127,509`, and IID word keys. A chunk is two words; toy full-block tags count words (`2n`) and the adjacent shorter tag is `2n-1`. These are scaled arithmetic/tagged-chunk experiments, not a byte-for-byte emulation of packing fractional-byte words at widths 10 and 12. Additional carry/wrap tag tests use arbitrary compressor tags and are marked separately; those tag pairs need not be obtainable from 2–4 chunk strings with a single fixed seed.

All probabilities in the main all-key experiments are **exact**, with integer numerators and denominators in the saved JSON/JSONL. Keys are not sampled. Some key sums are evaluated by exact linear algebra and XOR convolution, instead of visiting every tuple individually. For example, a four-chunk 12-bit count represents all `2^96` key tuples. Unused OH words integrate out.

The search includes:

1. One changed PH chunk `(0,0)->(d,0)` and `(0,0)->(d,d)`, all other chunks identical, at each of 2,3,4 chunks. At `w=8` every nonzero `d` in both families is enumerated. At 10 and 12 bits, structured starts and a deterministic hill climb change bits of `d` or switch families; each objective evaluation sums all keys exactly. It examines 390 and 507 distinct family/mode parameters during the hill climbs, respectively, in addition to the structured scans.
2. Only the ENH input changes by the top bit of one word, at 2,3,4 chunks; adjacent valid toy length tags and supplemental carry/wrap tags are also tested. The common PH prefix is integrated exactly, including the ENH low-to-high XOR.
3. Four specified 1-vs-2 chunk pairs at every requested width. Exact 2-vs-3 and 3-vs-4 all-zero pairs at `w=8`; these larger unequal cases were not computed at 10 or 12 bits.
4. A separate *conditional* PH search, exhaustively counting all remaining one-word keys for every tested slice. It exhausts zero-mask pairs `u<u'` with `delta=0` at widths 8 and 10, uses a structured set at 12, adds 100000 deterministic pseudorandom starting configurations at each width, and hill-climbs 64 starts. Randomness chooses candidate **pairs**, not keys for estimating probabilities.

### 5.1 Exact counting algorithms

For the one-word PH family, condition on the common other operand `b`. The raw difference is `d carryless_product b`. The selected bits of the first PH output are a linear function of the remaining key word. For the two-equal-word family, set `z=a xor b xor d`; then

```
raw difference = d carryless_product z
PH(a,b) = a carryless_product a xor a carryless_product (z xor d).
```

Squaring is linear over `F_2`, so this too is a linear image of a uniform word for fixed `z`. Retain only differences whose halves are in `S`. For each difference, Gaussian elimination computes the image on its differing bit positions. Every pattern in `T(Dlo) x T(Dhi)` yields an exact coset condition on the other chunks' common XOR contribution. Exhaustively enumerate each free PH/ENH key pair into that quotient, then convolve their distributions using an integer Walsh-Hadamard transform. Multiplicities account for every omitted key assignment. [exact.cpp](toy/exact.cpp) implements this calculation with 128-bit integer accumulators; the largest main denominator is `2^96`.

For an ENH top-bit change, either the low words agree exactly or their XOR is `q/2`. The latter cannot survive projection at these primes. Thus only a high-word common-mask marginal is needed. These marginals are obtained by exact prefix-product convolution and a ternary subset-sum table. Tag-only comparisons also keep the low word equal.

For unequal counts, product histograms sum the fresh final key pair exactly. At 1-vs-2 chunks the other key pair is directly enumerated and each residue fibre lifted. For 2-vs-3 and 3-vs-4, [unequal.cpp](toy/unequal.cpp) additionally integrates the common PH prefix via the exact mask marginals and the signed-support condition.

### 5.2 Results

Define loss consistently as

```
Pr[both reduced OH halves equal] / (2/2^w).
```

The tighter equal-chunk raw bound is `1/2^w`; losses against it are exactly twice this column. Both normalizations are saved. The denominator is an **AU bound**, not the observed raw collision probability.

| w | p | maximum over tested all-key pairs | exact projected probability of a maximizing 2-chunk pair | loss vs tighter equal-count raw bound |
|---:|---:|---:|---:|---:|
| 8 | 31 | **4.14208984375** | `138985472 / 4294967296` | 8.2841796875 |
| 10 | 127 | **4.035888671875** | `8667004928 / 1099511627776` | 8.07177734375 |
| 12 | 509 | **4.0263824462890625** | `553381789696 / 281474976710656` | 8.052764892578125 |

In all three searches a maximizing pair is `[(0,0),(0,0)]` versus `[(1,1),(0,0)]`, with the same tag. The largest tested loss decreases slightly with width; it neither approaches 64 nor exceeds 256 at width 12. This statement is restricted to the tested pairs. It is not an upper bound on untested pairs.

The conditional searches find 18,26,16 colliding free keys at the three requested widths. These correspond to losses 9,13,8 against `2/2^w`, but are **conditional quantities**, not rows in the preceding all-key table.

The largest ENH-top-bit losses against `2/2^w` are about 2.327853, 2.272416, 2.268007. The specified unequal-count pairs are much smaller. See [results.json](toy/results/results.json) for exact maxima by family and every scope qualification, and the individual JSONL files for the actual counts.

These scaled results alone can neither prove nor disprove the 64-bit claim.

## 6. Exact 64-bit lift of the worst toy family

There is a useful complete all-key calculation for the family `(0,0)->(1,1)` in one PH chunk, all other chunks identical. It applies directly to production OH at each chunk count `2 <= n <= 16`, with any common tag.

First, a still simpler family has a closed form. If only the first PH word changes by XOR 1, the difference is the other operand `b`, confined to the low half. For `b!=0`, put `r=v_2(b)`. Truncated carry-less multiplication by `b` maps the other uniform key word uniformly onto the subspace of words with low `r` bits zero. All positions in the support of `b` are therefore uniform. Reduced equality depends only on those positions, so any independent common XOR mask has no effect on its probability. Averaging over `b`, the map `(x,b)->(x,x xor b)` gives

```
Pr[projected collision] = sum_residue |fibre(residue)|^2 / q^2.
```

At the real modulus there are eight fibres of size 9 and the others have size 8, so this is exactly `8/q + 72/q^2`. [report.py](toy/report.py) checks this formula against every relevant scaled all-key count.

For the two-word family write `z=a xor b xor 1`, uniform and independent of `a`. The PH outputs have the same high word and low-word difference `z`; their first low word is

```
L_z(a) = a carryless_product a xor (z xor 1) carryless_product a  (mod X^64).
```

For any unchanged PH chunk, its low word is a truncated carry-less product of two uniform words. For the unchanged ENH chunk, its low word is an integer product modulo `q`; the tag and triangular XOR do not change it. **These two low-word distributions are identical.** Conditioning either product's first operand to have valuation `r` makes its output uniform on

```
V_r = { words whose low r bits are zero }.
```

The mixing probabilities are `2^(-r-1)` for `0<=r<w`, and `2^-w` for the zero operand. This follows from invertibility of the odd/unit factor, respectively in `Z/2^w Z` and `F_2[X]/(X^w)`.

The XOR of `k=n-1` such independent products is uniform on `V_r` in a mixture with weights

```
alpha_r = 2^(-kr) - 2^(-k(r+1)),  0 <= r < w
alpha_w = 2^(-kw),               V_w={0}.
```

Indeed, the XOR of independent uniform vectors in two nested subspaces is uniform in the larger one, and `Pr[min valuation >= r]=2^(-kr)`.

For each of the 852 possible `z`, restrict `L_z` to positions in `z`. Let `H_z` be its image. For each `r`, restrict `V_r` to those same positions. The exact conditional projected collision probability is

```
 sum_r alpha_r * | T(z) intersect (H_z + V_r) | / |H_z + V_r|.
```

Average over the uniform `z` by dividing the sum over the 852 admissible values by `q`. This is a finite calculation on binary vector spaces of dimension at most 64, with rational weights. [lift_family.py](toy/lift_family.py) implements precisely this formula. There is no enumeration or sampling of large random key tuples.

At **two chunks and 64 bits**, the exact projected probability is

```
8/q + 147/(2*q^2).
```

For each of 2 through 16 chunks the saved exact rational value is less than `9/q`. The 8-,10-,12-bit versions of this independently implemented method match all nine corresponding all-key C++ counts exactly.

Thus, for these actual one-block message families, full primary UMASH collision probability is at most

```
(9/q)/D + 2/(p-2) < 25/q < 2^-55.
```

This is an upper bound on full UMASH, obtained from an **exact projection probability** and the polynomial root theorem; it is not an exact count of full-hash collisions over every polynomial multiplier. It rules out the structural family maximizing the toy search as a counterexample to the advertised primary bound. It does not settle arbitrary message differences or the fingerprint.

## 7. Verification and reproduction

Heavy work ran on `hardware.normalcomputing.net`, in `~/agents/umash-lemma`, with `nice -n 10` and at most 48 computational threads. The Mac used one computational thread. No web access, package installation, or network operation other than SSH/SCP was used.

[run.sh](toy/run.sh) reproduces the computations on the Xeon. It uses only a C++17/C compiler, OpenMP, x86 carry-less multiplication/BMI2, and Python's standard library. There are no floating-point probability calculations in the enumerators; printed decimals are conveniences alongside exact counts.

Verification includes:

* The signed-support recurrence checked against every congruent word pair at all three requested widths.
* Thirty direct literal full-key comparisons against the linear/convolution algorithm at width 3, through four chunks, plus all ternary marginals through width 6: 1122 checks total.
* Twelve additional direct full-key checks of ENH, tag, and unequal-count cases, including 3-vs-4 chunks, matching the aggregated algorithms.
* Seven complete tiny fingerprint enumerations, including differing-checksum, PH/ENH equal-checksum, and two-PH equal-checksum cases. These are validation instances at widths 3 and 4, **not** an 83-bit certificate or a width-8/10/12 fingerprint search.
* Nine cross-checks of the exact low-word distribution lift against the main C++ all-key counts.
* The two exact real-word conditional key lists checked through the supplied production C, with pairwise-distinct OH words.
* Exact rational checks of the case-specific headline arithmetic.

The [results manifest](toy/results/results.json) and [source hashes](toy/SHA256SUMS) identify what was run. The upstream sources were not edited.

## 8. What remains unresolved

The all-pair `B1(s)` and the joint `B12(s)` required by `AUDIT_REVIEW.md` §2.3 remain open. In particular, this is not a proof with a merely adjusted numerical constant: the ENH-only projected case and the needed equal-checksum joint control are still missing.

The numerical work also has definite limits. It did not evaluate all arbitrary two-word PH changes, arbitrary ENH changes, or all message pairs; the hill climb was within the two explicitly stated PH families. Complete unequal 2-vs-3 and 3-vs-4 counts were only done at width 8, with smaller direct validation. Larger-width unequal experiments used 1-vs-2 chunks. The fingerprint was fully enumerated only in tiny validation models. No averaged all-key probability was calculated for the particular production message pair realizing the conditional 242-key slice.

What was tried therefore supports these precise conclusions: several omitted length cases can be closed; the naive small-constant conditional PH route fails on actual production algebra; the tested all-key losses are small; and the strongest toy family lifts to a provably harmless 64-bit primary family. **No production counterexample with probability greater than `ceil(s/4096)*2^-55`, and no refutation of the 83-bit fingerprint bound, has been established.**
