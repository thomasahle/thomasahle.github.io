# Poly1305 and GHASH: proof audit of the keyed hash families

Audited 2026-09-18. **The ideal-key bounds support the conservative table scores 103 and 127. They do not support those scores for the timed 64-bit-seeded wrappers.** Bernstein's constant 8 is valid for every additive differential target; equality alone permits 7. The AAD-only GHASH theorem holds after repairing the polynomial argument in the cited paper. Uniform AES keys do **not** establish exact uniformity of `AES_K(0)`.

All probabilities below concern a fixed distinct input pair chosen before sampling the hidden key, with the **entire 128-bit output** compared. They are information-theoretic statements. They are not multi-query authentication guarantees.

| Claim / family | Certified probability envelope at at most `8L` bytes | Score from this envelope | Verdict |
|---|---|---|---|
| Poly1305, uniform clamped `r`, Bernstein's ADU bound | `min(1, 8 ceil(L/2) / 2^106)` | **103**, unique minimizing `L=1` | **HOLDS**; harmless sign typo in printed proof |
| Same family, equality of full tags | `min(1, 7 ceil(L/2) / 2^106)` | **106−log2(7) = 103.1926450779**, `L=1` | **HOLDS**; optional collision-specific improvement, not a correction required by Theorem 3.3 |
| GHASH with one AAD byte string, empty ciphertext, uniform field key | `(ceil(L/2)+1) / 2^128` | **127**, unique minimizing `L=1` | **HOLDS**, on the valid length-encoding domain |
| Cited GHASH proof used for two independently padded streams of total size at most `8L` | Safe envelope `(ceil(L/2)+2) / 2^128` | **128−log2(3) = 126.4150374993**, `L=1` | **HOLDS WITH CORRECTION (constant)** to the degree accounting; the printed argument does not prove the smaller general bound |
| Timed `poly1305-hash` and GMAC-based `ghash` registrations, uniform 64-bit seed | Ideal-key envelopes above are impossible for this family on the stated domain | **103 / 127 must not be advertised as guarantees for these registrations**; their actual metric is at most **65**, not a certified lower bound | **GAP** in transferring the proof; the proposed transfer is refuted by §17 |

Here `L≥1` is an **integer** number of 8-byte words. Empty and partial-word strings are admitted beneath each cap. A score calculated from an upper bound on collision probability is a guaranteed lower bound on the metric defined using the actual worst-pair probability. Thus 103 is a conservative Poly1305 theorem score, not an exact determination of that family's optimal metric. No extra output-probability floor has been added. Probability clipping cannot lower any displayed minimum.

For GHASH every encoded bit length must be **strictly less than `2^64`**. For byte strings this means fewer than `2^61` bytes. One can restrict `L` to `1≤L<2^58` so every string beneath `8L` is legal, or intersect every cap with the legal domain. Neither choice changes the minimum at `L=1`. This audit does not silently wrap the length field for arbitrarily long strings.

## Sources and checks

The supplied PDFs are complete and readable: Bernstein has 18 pages, the GCM specification 43, and the security paper 21. Bernstein PDF pages 8–9 and the GHASH lemma on PDF page 14 were rendered and visually checked, including the displayed signs and summation limits. Text extraction and all compilation, rendering, and arithmetic checks ran on the Xeon. The Mac only handled editing, transfers, and viewing the resulting pages.

- Bernstein, [The Poly1305-AES message-authentication code, final 2005-03-29 version](https://cr.yp.to/mac/poly1305-20050329.pdf), §§2–3, Theorems 3.1–3.3; [supplied PDF](sources/poly1305-20050329.pdf).
- [RFC 8439 §2.5](https://www.rfc-editor.org/rfc/rfc8439.html#section-2.5), particularly clamping, block markers, and final addition; [supplied text](sources/rfc8439.txt).
- McGrew–Viega, [GCM specification](https://csrc.nist.gov/CSRC/media/Projects/Block-Cipher-Techniques/documents/BCM/Proposed-Modes/gcm/gcm-spec.pdf), §§2.2–2.5; [supplied PDF](sources/gcm-spec.pdf).
- McGrew–Viega, [The Security and Performance of GCM, full version](https://eprint.iacr.org/2004/193), Appendix A, Lemma 2, PDF pages 14–15; [supplied PDF](sources/gcm-security.pdf).
- [Timed wrapper](classic_openssl.cpp), [benchmark report](REPORT.md), OpenSSL's [Poly1305 documentation](https://docs.openssl.org/3.5/man7/EVP_MAC-Poly1305/) and [GMAC documentation](https://docs.openssl.org/3.5/man7/EVP_MAC-GMAC/).
- The versioned [OpenSSL 3.5.5 Poly1305 implementation](https://raw.githubusercontent.com/openssl/openssl/openssl-3.5.5/crypto/poly1305/poly1305.c) was additionally fetched and checked; [local copy](sources/openssl-3.5.5-poly1305.c). Its 32- and 64-bit C initializers explicitly apply the mask.

Source SHA-256 values:

```text
poly1305-20050329.pdf  1c9240a5eb55d38af333888604f6b5f0473917c1402b80099dca5cbc207075c0
gcm-spec.pdf          8cec9d63d2e28400773fff488c7aa9fcecb9636d50d994baca3cc025aa23df95
gcm-security.pdf      124a178054c4e93e73b641358572a84ff3f0f8c6fa2203228cfcbaf02ae65f84
rfc8439.txt           25bef70fbf7a07ff45c2fe4cb7c6ce954eac687413d8610603268b4e4415324c
openssl-3.5.5-poly1305.c
                     2e4b983e23977763c706c2f60f1f19fc241c54fd5f93e798246af24fa727c8d7
classic_openssl.cpp   24e018a229f9bcd1e211f5456d1e26c64a6a8228265c51a3a5ab7d2eb68801b1
```

## Poly1305

**1. Exact content and quantifiers of Theorem 3.3 — HOLDS.**

In Bernstein's notation `H_r` is already the **128-bit projected hash**, not the full field residue. The theorem on PDF page 8 states:

> Let m, m′ be distinct messages, each having at most L bytes. Let g be a 16-byte string. Let R be a subset of {0, 1, …, 2^130−6}. Then there are at most 8⌈L/16⌉ integers r ∈ R such that H_r(m) = H_r(m′) + g.
>
> Consequently, if #R = 2^106, and if r is a uniform random element of R, then H_r(m) = H_r(m′) + g with probability at most 8⌈L/16⌉/2^106.

The displayed group addition is integer addition modulo `2^128`, with the strings interpreted little-endian. The theorem is for **every fixed** pair and **every fixed** `g`. It is not just a zero-target collision statement, not an XOR-difference statement, and not a claim for a key-dependent target. Its counting assertion permits **any** subset `R` of the canonical field representatives; clamping enters only when identifying the sampled subset and its cardinality. The source's `L` counts bytes. Below it is renamed `B` to avoid confusion with the write-up's word count.

**2. Byte encoding, including unequal lengths — HOLDS.**

Set `q=2^128`, `p=2^130−5=4q−5`. Split a message into consecutive chunks of 16 bytes, except for a possible shorter final chunk. For a chunk `b` of `j` bytes, `1≤j≤16`, set

`c(b) = Σ_{i=0}^{j−1} b[i] 256^i + 256^j`.

This is precisely appending byte `0x01` before integer interpretation. A complete 16-byte block also gets the marker at bit 128; the marker is not omitted for an exact multiple of 16. There is no extra empty block. The empty string has no chunks and polynomial zero. With `d` chunks define

`P_m(X) = c_1 X^d + c_2 X^(d−1) + … + c_d X` over `F_p`.

Each coefficient satisfies `256^j ≤ c(b) < 2·256^j ≤ 2^129 < p`. These disjoint ranges determine `j`; the low base-256 digits then recover all bytes. In particular, a zero byte appended to a short message changes its marker position.

If chunk counts differ, the higher-degree polynomial has a nonzero leading coefficient. If counts agree, equality of field coefficients implies equality of their integer representatives below `p`, hence equality of every chunk. Therefore distinct byte strings give distinct polynomials, including empty versus nonempty strings. Merely comparing coefficient lists without this nonzero-leading-coefficient argument would not suffice to distinguish all lengths.

**3. Prime-field root count — HOLDS.**

Bernstein proves primality of `p` in Theorem 3.1 using Pocklington. The accompanying Lean project proves the same concrete primality claim with a Lucas certificate, so primality is not an unproved hypothesis there.

For any field target `u`, `D_u=P_m−P_m′−u` is nonzero: the distinct-message difference has a nonzero coefficient at a **positive** power, which subtracting a constant cannot erase. Its degree is at most `n=max(d,d′)≤ceil(B/16)`. A root `a` permits division by `X−a`; induction on degree gives at most `n` distinct roots over the field. Restricting the roots to any subset `R` cannot increase that count. This argument remains valid at large degrees; the resulting probability bound is then capped at one.

**4. Projection and its explicit fibres — HOLDS; printed sign corrected, constant unchanged.**

Write `a=P_m(r) mod p` and `b=P_m′(r) mod p`, each in `[0,p−1]`. The integer difference `z=a−b` lies in

`[−p+1,p−1] = [−4q+6,4q−6]`.

Represent the fixed output difference by `g∈[0,q−1]`. Projected equality with difference `g` implies

`z=g+kq`, where `k∈{−4,−3,−2,−1,0,1,2,3}`.

Thus the bad-key event is contained in the union of at most eight **explicit field equations**

`P_m(r)−P_m′(r) = (g+kq) mod p`.

Each equation has at most `n` roots by Claim 3. No uniformity of `a`, `b`, or their difference is assumed. The interval has `8q−11` integers; a congruence class in it contains at most eight elements.

Equivalently, let `t` be the canonical field representative of `P_m(r)−P_m′(r)`. The ordinary difference is either `t` or `t−p`. Consequently `t` belongs to one of two fixed residue classes modulo `q`: `g` or `g+p`. Each residue class has at most **four** representatives below `p`, explicitly `v,v+q,v+2q,v+3q` after choosing its least residue `v` and discarding representatives outside `[0,p−1]`. This explains both the individual fibre count **4** and the general differential count **8**.

It would be wrong to claim that equality after projection is simply the event that the canonical **field difference** projects to zero: subtracting canonical representatives can wrap by `p`, and `p` is not a multiple of `q`. It would also be wrong to multiply an ordinary AU bound by a small fibre size without a bound for every relevant target. The invalid UMASH-style inference is avoided here because the event is explicitly covered by key-independent field targets, and **every** target produces a nonzero polynomial with a root-count bound. We do not bound a fibre of an unrelated intermediate variable or condition on a quantity depending on the same key.

The printed proof starts with `H_r(m)=H_r(m′)+g` but then writes `b−a≡g`; that should be `a−b≡g`, or equivalently `b−a≡−g`. Theorem 3.2 also reverses a coefficient-difference sign without affecting the zero/divisibility argument. These sign slips do not change any root count or constant. The Lean proof consistently uses `a−b`.

For collisions, `g=0`, the signed lifts are exactly `−3q,−2q,−q,0,q,2q,3q`. The endpoints `±4q` are outside the interval. The same proof therefore gives **`7n/|R|`** for full-output equality. This improvement does not assert that seven admissible roots can always be attained, or that the resulting metric lower bound is optimal.

**5. Clamping and the probability denominator — HOLDS for the ideal sampler.**

The mask clears the top four bits of bytes 3, 7, 11, 15 and the bottom two bits of bytes 4, 8, 12: `16+6=22` distinct bits. Exactly 106 bits remain. An equivalent explicit description is

`R={a + 2^34 b + 2^66 c + 2^98 d : 0≤a<2^28; 0≤b,c,d<2^26}`.

The four bit intervals do not overlap, so this parametrization is injective; all values are below `p`. Hence `|R|=2^(28+26+26+26)=2^106`. A genuinely uniform 128-bit word, after masking, is uniform on `R`, since each result has exactly `2^22` raw preimages. Dividing the count in Claim 4 by `2^106` proves Bernstein's probability bound. Full-field sampling and nonzero-only sampling are not required.

**6. The pad, weak keys, and authentication scope — HOLDS, with qualifications.**

Let `h_r(m)` denote the canonical full field residue. The RFC tag is

`T_(r,s)(m) = (h_r(m)+s) mod q`.

For every fixed `r,s`, two tags are equal exactly when the corresponding projected hashes are equal. In fact the same `s` cancels in every additive differential. Thus `s` contributes no factor `2^-128` to collision probability. It need not be uniform or independent of `r` for this cancellation; uniform `(r,s)` yields the same bound by averaging out `s`.

There **is** a Poly1305 weak key: `r=0` is admissible and sends every polynomial to zero, so every tag equals `s`. It is included with probability `2^-106` in the ideal key space. Other special values, such as `r=1`, are also included in the root count. No weak-key exclusion is used. The wrapper does not derive Poly1305's multiplier through AES; the field key is the clamped `r` itself. The original paper's nonce/AES pad construction and the RFC's one-time-MAC security setting are separate from this fixed-pair hash theorem.

**7. Word metric — HOLDS as a certified bound, not an exact Poly1305 optimum.**

For messages of at most `8L` bytes, `n≤ceil(8L/16)=ceil(L/2)`. Bernstein's envelope gives

`log2(L / ε_bound(L)) = 103 + log2(L / ceil(L/2))`

before clipping. Since `ceil(L/2)≤L`, with equality only at the positive integer `L=1`, its minimum is **103**. At every even positive `L`, this un-clipped expression equals 104. Odd `L>1` give values strictly between 103 and 104.

The seven-lift collision envelope gives instead

`106−log2(7) + log2(L / ceil(L/2))`,

whose minimum is **103.1926450779** at `L=1`. Either envelope is valid; retain **103** if the table is explicitly reporting Bernstein's conservative published guarantee. Use the latter number only with the collision-specific bound clearly stated. Neither formula determines the actual worst-pair Poly1305 collision probability exactly.

**8. Timed Poly1305 value computation — HOLDS; uniform-key assertion — GAP / false.**

Lines 14–25 of `classic_openssl.cpp` expand the 64-bit seed with four sequential SplitMix64 outputs, serialized little-endian, to 32 bytes. Lines 40–41 **already clamp `r` in the wrapper**. OpenSSL also applies the standard mask; that second clamping is idempotent. The last 16 bytes supply `s`.

`EVP_MAC_update` receives exactly the input bytes and `len`; the empty case skips the update and finalizes normally. There is no application length prefix, nonce, extra record framing, or ChaCha20-Poly1305 AEAD framing. The output is the RFC's full 16-byte little-endian tag. This value-level description matches the analysed function for the derived key, subject to the normal trust in the OpenSSL implementation; it is not a C or assembly verification.

The input to EVP is **not a uniformly random 32-byte string**: it is deterministic seed expansion and has already been masked. Even the ideal masked `(r,s)` space has `2^234` possibilities, whereas this wrapper has at most `2^64` key tuples. In particular its `r` cannot be uniform on `2^106` values. The report correctly distinguishes this distribution issue. The stronger impossibility in Claim 17 rules out transferring the ideal envelopes to uniform seeds.

The thread-local EVP context is allocated/fetched once, but every hash call expands the seed, initializes the key, processes the bytes, and finalizes. Small-input timings therefore include Poly1305 initialization and clamping/provider setup.

## GHASH and GMAC

**9. Correct single-stream theorem — HOLDS.**

Let `H` be uniform over all `2^128` elements of `F=GF(2^128)`, including zero. Let `A,A′` be fixed distinct byte strings with bit lengths below `2^64`. Pad only the final partial data block with zero bytes on its right; append the block

`ell_A = [8·|A|]_64 || [0]_64`,

where each 64-bit integer is big-endian. For `d=ceil(|A|/16)` padded data blocks, the GHASH polynomial is

`G_A(X)=A_1 X^(d+1) + A_2 X^d + … + A_d X^2 + ell_A X`.

For empty AAD, the sole length block is zero and the polynomial is zero. For every fixed full-width XOR difference `t` and every pair with at most `n` data blocks each,

`Pr_H[G_A(H) xor G_A′(H)=t] ≤ min(1,(n+1)/2^128)`.

The collision case is `t=0`. Addition and subtraction both equal XOR in this field. The GCM bit convention interprets blocks as polynomial-basis field elements, not as integer casts into a characteristic-two field.

**10. Encoding injectivity and root count — HOLDS.**

If the byte lengths differ, the coefficient of `X` differs: serialization of bit length is injective on the stated range, and data coefficients occur only at powers at least two. Thus the difference polynomial is nonzero even if zero padding causes some data blocks to match.

If lengths agree, chunk boundaries, chunk counts, and final padding widths agree. Distinct bytes then give a distinct padded data block. Its field coefficient is distinct because 128-bit block interpretation is bijective. In particular `[01]` and `[01 00]` have the same zero-padded data block but different length coefficients, so they cannot be confused as formal polynomials.

Subtracting a fixed target changes only the constant coefficient. Hence `G_A−G_A′−t` remains a nonzero polynomial of degree at most `n+1`. The field root count gives at most `n+1` keys, and division by the **full** field size gives the stated probability. For `t=0`, `H=0` is indeed a root for every message pair and is included. Discarding that root while keeping denominator `2^128` would be unjustified.

**11. Audit of the cited Lemma 2 — HOLDS WITH CORRECTION (constant) for its two-stream accounting; the requested single-stream constant is unchanged.**

The security paper prints a truncated-output AXU envelope `ceil(l/w+1)·2^-t` for two inputs whose lengths sum to at most `l` bits. Its proof introduces `p=max(m+n,m′+n′)`, where each stream is separately padded. The root-count reasoning is sound only after repairing the polynomial and encoding argument:

1. Its equation (8) sums only through `p`, omitting the listed length block at index `p+1`. Its increasing exponents and trailing zero alignment also do not match the specified Horner recurrence when lengths vary. The polynomial in Claim 9 fixes the AAD-only case by placing the length block at power one and aligning earlier coefficients by their distance from the end.
2. Distinct `(A,C)` pairs need not have distinct concatenations `A||C`; different splits can produce the same concatenation. The two encoded lengths, rather than that concatenation assertion, distinguish such pairs.
3. With two independently partial streams, the actual degree is `ceil(a/w)+ceil(c/w)+1`. The inequality bounding it by `ceil((a+c)/w)+1` is false. One byte of AAD plus seven bytes of ciphertext already use two padded data blocks and a length block, degree at most three, although the printed expression gives two. This identifies a proof error; it does not establish that this particular pair attains three roots.
4. Root counting gives a probability **at most** the degree divided by field size, not equality as the prose states.

A valid general cap is `ceil((a+c)/w)+2`; the two-stream length tuple supplies the same injectivity argument. For the timed wrapper `C` is always empty, so the exact block accounting is `ceil(|A|/16)+1` and its **127** score survives these repairs. The report must retain that single-stream qualification.

**12. Word metric — HOLDS, 127.**

Substituting `n=ceil(L/2)` gives

`log2(L / ε_bound(L)) = 128 + log2(L / (ceil(L/2)+1))`.

The denominator is at most `2L`, with equality only at `L=1`. Thus the envelope minimum is **127**. At `L=1` this is also attained by an actual fixed pair for standard GHASH: compare the empty message with a one-byte nonzero message. Their difference is `aH²+ell H`, with `a,ell` nonzero, and has exactly the two distinct roots `0` and `ell/a`. Consequently ideal single-stream GHASH's actual metric is exactly 127 on this domain. This tightness observation is an algebraic audit result; the Lean project proves the requested upper probability bound, not this separate tightness example.

**13. Timed GMAC output and fixed-IV mask — HOLDS.**

Lines 48–65 initialize `EVP_MAC` GMAC with AES-128-GCM, two SplitMix64 outputs as the 16-byte AES key, and **twelve zero IV bytes**. The IV is fixed, not generated from later seed outputs. All message bytes are AAD and ciphertext is empty. With this 96-bit IV,

`H=E_K(0^128)`, `J0=0^96 || 0^31 || 1`,

`tag_K(A)=GHASH_H(A,empty) xor E_K(J0)`.

For each fixed `K`, the same mask is used on both messages. XOR cancellation therefore makes tag collisions **exactly** GHASH collisions at `H=E_K(0)`. Correlation between the pad and `H` is immaterial for this equality. The wrapper returns all 16 bytes in canonical GMAC tag order, and both registration pointers produce that same byte string. There is no additional application framing; the standard GHASH padding and length block are part of GMAC.

The context is retained, but AES-128 key initialization is performed on every call. The report's small-input timing includes seed expansion, EVP/cipher parameter handling, AES key setup, generation of `H`, provider GHASH setup, and the AES mask computation. It must not be described as the cost of a bare GHASH recurrence with a preinstalled field key. This is a GMAC-based GHASH throughput proxy.

**14. “Uniform AES key implies exactly uniform H” — GAP.**

AES being a permutation says that **for fixed `K`**, the map `X↦E_K(X)` is bijective. The required assertion concerns the different map `K↦E_K(0)`. No cited result establishes that this map is balanced or bijective for concrete AES-128. Uniform 128-bit AES keys therefore do not justify the exact information-theoretic GHASH denominator by that argument.

More explicitly, let `mu(h)=#{K:E_K(0)=h}`. With uniform AES-128 keys and a bad-root set `S`, the exact probability is `Σ_{h∈S} mu(h)/2^128`, bounded by `|S| max_h mu(h)/2^128`. Replacing this by `|S|/2^128` requires an additional distribution result. Sampling an ideal uniform random permutation does give a uniform image of zero; a computational AES/PRP reduction is another possible model, but neither is an unconditional statement about concrete AES keys. The timed wrapper has the further 64-bit-seed restriction.

**15. Zero key, length limits, and tag truncation — HOLDS with the stated domain.**

`H=0` maps every GHASH input to zero and is included with probability `2^-128`. Excluding it changes the sampled family and denominator. A weak-key caveat is not an extra error term to add to a root count that already counts it.

The injectivity proof uses the full, non-wrapping 64-bit length field. No claim is made beyond its legal range. Full-output equality is essential to the bound proved here. Equality of truncated tags is a larger event; the full-width estimate must not be reused unchanged. A separate AXU lifting argument can yield a truncation bound, but it is not needed for these full-output registrations and is not part of the Lean theorem. Fixed-IV collision analysis also does not establish secure repeated-IV authentication.

## What the seed distribution changes

**16. A sampler substitution requires its own proof — GAP for both wrappers.**

The field root bound controls how many **field keys** are bad, not the probability of that set under every distribution. A valid general replacement is the root count times the maximum point probability of the actual evaluation key (and times the explicit target count for Poly1305). SplitMix64 cannot manufacture independent uniform 106- or 128-bit keys from 64 random bits. Correct output arithmetic, statistical sanity checks, or an AES mask do not supply that missing sampling theorem.

**17. The advertised tiny envelopes for the 64-bit-seeded family are impossible — GAP, with a counterargument and a checked example.**

There are `2^128` strings of exactly 16 bytes and one additional empty string. Fix any one seed of either 128-bit-output wrapper. By pigeonhole, two of these `2^128+1` inputs have the same output. Fix that pair; it does **not** depend on the subsequently sampled random seed. Uniform sampling of a 64-bit seed hits the chosen seed with probability `2^-64`. Hence the worst-pair collision probability at `L=2` satisfies

`epsilon_seeded(2) ≥ 2^-64`.

This contradicts both proposed transfers (`8/2^106` for Poly1305 and `2/2^128` for GHASH at `L=2`). It also gives an **upper** bound of 65 on the actual seeded-family score: `min_L log2(L/epsilon_seeded(L)) ≤ log2(2·2^64)=65`. This is not a positive universality guarantee of 65 bits and does not determine either wrapper's exact score.

For GHASH the existential argument was supplemented with an explicit fixed pair using seed zero. Let `ell=128||0` be the length block for a 16-byte AAD message. For nonzero `H`, choose its data block `A=ell/H`; then `A H²+ell H=0`, equal to the empty message's GHASH. The independent bit-serial field calculation and OpenSSL GMAC both passed on the Xeon:

```text
seed                 0
AES key              afcd1d7b39a820e2f465b9a16a9e786e
H                    5ab7e3eeba6fcda080aea7991c570f53
message 0            empty
message 1 (16 bytes)  9542346ddbc5acc607ff4c5823a106d2
full tag of both      1aaef5a74263c4591b70adb3bfbd6dbe
```

See [reproduction script](lean/CheckSeededGHASH.py) and [recorded result](lean/SeededGHASH.json). The script uses the wrapper's exact seed-to-key expansion and the same OpenSSL 3.5.5 GMAC API path through the command-line frontend; it does not pretend to be a new SMHasher timing or a formal C verification. Once these two messages are fixed, their random-seed collision probability is at least `2^-64`, irrespective of whether other seeds also collide.

## Publication and formal verification verdict

**18. Scores to publish and scope of Lean — HOLDS for the ideal families.**

Use **103 for ideal Poly1305** if retaining the published bound, and **127 for ideal AAD-only GHASH**. State the full-key sampling assumptions next to the table. The sharper Poly1305 collision envelope permits **103.1926450779** if desired and explicitly labelled. Describe the measured implementations as seeded Poly1305 and a GMAC-based GHASH proxy with setup included; their measurements do not certify those ideal-key scores.

The [Lean status](LEAN_CLASSIC_STATUS.md) and [source mirror](lean/README.md) document compiled byte-string encoding, Horner evaluation, field root counting, the explicit projection targets, the concrete prime, clamped-set cardinality and probability, and the full tag bounds. The formal constants are **8** for Bernstein's ADU theorem, **7** for Poly1305 tag equality, and **n+1** for single-stream GHASH. There are no admitted proof holes or additional axioms. The finite-field result is representation-independent over `GaloisField 2 128`; it does not verify OpenSSL assembly, AES pseudorandomness, or the seeded samplers. Those boundaries are explicit rather than hidden hypotheses about encoding injectivity or root counts.
