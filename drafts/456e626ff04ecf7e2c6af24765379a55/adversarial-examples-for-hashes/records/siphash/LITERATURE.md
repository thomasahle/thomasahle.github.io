# SipHash fixed-pair collision claim — literature lens (2026-09-18)

Question: can the post's "unresolved claim" for SipHash-1-3 / SipHash-2-4 (bits = min_L log2(L/eps),
eps = Pr over a uniform 128-bit key that a FIXED pair collides on the 64-bit tag) be proved or refuted
from the published cryptanalysis?  Answer: NO on both sides.  No proof exists or can exist in the
universal-hashing sense (SipHash has no algebraic structure; its only security statement is a PRF
conjecture), and no published differential through any FULL SipHash-c-d with c>=1, d>=3 reaches
2^-64; the best published internal-collision characteristics are 2^-167 (1-x) and 2^-236.3 (2-x).

## Sources fetched (text in this directory)
- AB12  Aumasson, Bernstein, "SipHash: a fast short-input PRF", INDOCRYPT 2012.  aumasson_bernstein_siphash.{pdf,txt}
        https://www.aumasson.jp/siphash/siphash.pdf
- DMS14 Dobraunig, Mendel, Schläffer, "Differential Cryptanalysis of SipHash", SAC 2014, ePrint 2014/722.
        dms2014_eprint722.{pdf,txt}   https://eprint.iacr.org/2014/722
- XLSL19 Xin, Liu, Sun, Li, "Improved Cryptanalysis on SipHash", CANS 2019, LNCS 11829 pp. 61-79,
        doi:10.1007/978-3-030-31578-8_4.  PAYWALLED: only the abstract (via search snippet) was reachable.
- HY19/24 He, Yu, "Cryptanalysis of Reduced-Round SipHash", ePrint 2019/865; journal: Comput. J. 67(3):875-883, 2024,
        doi:10.1093/comjnl/bxad026.  heyu2019_eprint865.{pdf,txt}
- NSLL22 Niu, Sun, Liu, Li, "Rotational Differential-Linear Distinguishers of ARX Ciphers with Arbitrary Output
        Linear Masks", CRYPTO 2022, ePrint 2022/765.  niu2022_eprint765.{pdf,txt}
- CHNE24 Chakraborty, Hadipour, Nguyen, Eichlseder, "Finding Complete Impossible Differential Attacks on AndRX
        Ciphers and Efficient Distinguishers for ARX Designs", ToSC 2024(3), ePrint 2024/1359.  eprint2024_1359.{pdf,txt}
- SKSI25 Sasaki, Kurahara, Sakamoto, Isobe, "Forgery Attacks on SipHash", ACISP 2025, LNCS 15658,
        doi:10.1007/978-981-96-9095-4_1.  PAYWALLED: abstract only (springerprofessional.de mirror).
- Rust: issue rust-lang/rust#29754 (Aumasson comments 2015-11-12, 2016-03-04), PR #33940 (merged May 2016).

## What the designers claim (AB12, verbatim)
§3 "Expected strength": "SipHash-c-d with c >= 2 and d >= 4 is expected to provide the maximum PRF security
possible (and therefore also the maximum MAC security possible) for any function with the same key size and
output size. ... We define SipHash-c-d for smaller c and d to provide targets for cryptanalysis. Cryptanalysts
are thus invited to break SipHash-1-0, ... SipHash-1-1, ... SipHash-1-2, ... and so on."
"Security is also limited by the output size (64 bits). In particular, when SipHash is used as a MAC, an
attacker who blindly tries 2^s tags will succeed with probability 2^(s-64)."
"We comment that SipHash is not meant to be, and (obviously) is not, collision-resistant."  [unkeyed sense]
§5 designers' own characteristics (xor-linearised, Leurent ARX toolkit): 4 rounds 2^-134 with v3 guessed
every two rounds (Table 5.1, 6 rounds cumulative 2^-439); without guessing v3: 4 rounds 2^-159, 6 rounds
2^-498 (Table 5.2).  Truncated differentials: biases after 3 SipRounds, none detected after 4 with 2^30 samples.
"No vanishing characteristic exists for one SipRound"; no vanishing xor-linear characteristic for 2,3,4 rounds.
Internal collisions: generic 2^128 queries (256-bit state).
=> SipHash-1-3 is NOT covered by any claim in the design paper: c=1 is explicitly a cryptanalysis target.
The only "claim" for 1-3 is Aumasson's informal one (Rust #29754, quoted in the issue text): "we proposed
SipHash-2-4 as a (strong) PRF/MAC, and so far no attack whatsoever has been found ... However, fewer rounds
may be sufficient and I would be very surprised if SipHash-1-3 introduced weaknesses for hash tables."
and 2015-11-12: "SipHash-1-3 leaves 4 rounds between the last attacker-controlled input and the output.
There's a 'distinguisher' on 4 rounds in https://eprint.iacr.org/2014/722.pdf ... But you can't inject that
pattern in SipHash-1-3 because you don't control all the state. And even if you could inject that pattern
the bias wouldn't be exploitable anyway."

## Best published numbers
| variant | statement | probability / complexity | source |
|---|---|---|---|
| SipHash-1-x | internal collision, differential characteristic, 3 message blocks | 2^-167 (search stage 2^-169) | DMS14 Table 1/2, §5.1 |
| SipHash-2-x (= full 2-4) | internal collision, 1 message block; "best published characteristic for full SipHash-2-4" | 2^-236.3 (search stage 2^-238.9); previous best 2^-498 [AB12] | DMS14 Table 1/3 |
| 4-round finalization (2-4) | differential distinguisher (state difference in, biased tag) | 2^-35, complexity 2^35 | DMS14 §5.2 Table 4 |
| 4-round finalization | differential-linear, corr. theory 2^-12.45 / experiment 2^-6.03; 3 rounds 2^-2.19 / 2^-0.78 | NSLL22 Table 6, §6.2 |
| 4-round SipHash | cluster of 2^14 impossible-differential distinguishers (first ID result) | CHNE24 §5.1.5 Table 7 |
| SipHash-2-1 | truncated-differential distinguisher | 2^10 (ePrint) / 2^12 (journal) | HY19/24 |
| SipHash-2-2 | truncated-differential distinguisher (needs padding rule neglected) | 2^36 | HY19/24 |
| SipHash-2-1 | key recovery | 2^98 (ePrint); journal: ~97% of keys within 2^83 | HY19/24 |
| SipHash-1-x (revised constants) | RX-colliding characteristic, 1 block / 2 blocks | 2^-93.6 / 2^-160 | XLSL19 abstract (revised = rotation-friendly constants, not real SipHash) |
| SipHash-1-x, 2-x, 3-x, 4-x | updated/first internal-collision BOUNDS via SAT | numbers paywalled | SKSI25 abstract |
| SipHash-1-1, 1-0, 2-0 | first forgery via TAG collision through finalization ("feasible") | numbers paywalled | SKSI25 abstract |
| SipHash-2-4 related/chosen keys | internal collision, chosen key pair, out of model | 2^-169 char., pair in Table 7 | DMS14 App. A |
| SipHash-2-x (any d) | semi-free-start collision (attacker chooses state) | seconds | DMS14 App. A Tables 5,6 |

DMS14 on the meaning of these: "To improve this attack, characteristics are needed, which have a
probability higher than 2^-128 (in the case of SipHash). Otherwise, a birthday attack should be preferred."
"Although both characteristics do not have a probability higher than 2^-128, they are the best collision
producing characteristics published for SipHash so far." "these two results do not endanger the full
SipHash-2-4 function, which is still indistinguishable from a pseudo-random function."

## Reading against the post's metric
eps(m,m') = Pr_K[SipHash_K(m) = SipHash_K(m')].  A key-averaged differential (m xor m' -> 0 on the tag)
with probability p gives eps >= p (up to the usual Markov/independence heuristics).  Two routes to a tag
collision: (i) an internal (256-bit state) collision -> best 2^-167 for c=1, 2^-236.3 for c=2, far below
2^-64; (ii) a non-vanishing state difference that cancels in the 64-bit tag after the d finalization rounds
-> the only published results are SKSI25's tag-collision forgeries on 1-1, 1-0, 2-0; the abstract lists no
result for d>=2, and 1-3 keeps 4 SipRounds (1 compression + 3 finalization) after the last message word,
exactly the span for which the best known statistical structure is DMS14's 2^-35 distinguisher / NSLL22's
DL correlation 2^-6, neither of which is a collision.  Aumasson's own note: the 4-round pattern cannot be
injected because the attacker does not control the whole state.
=> No published fixed pair, differential or family gives eps > 2^-64 for SipHash-1-3 or SipHash-2-4.
=> No proof: a fixed-pair bound of the universal-hashing kind would require a theorem about an ARX
permutation with no algebraic structure; the strongest statement in print is the PRF conjecture of AB12
(for c>=2, d>=4 only) and Aumasson's informal comment for 1-3.  If the PRF conjecture holds with advantage
delta then eps <= 2^-64 + delta; that is a conjecture, not a bound.

## Correction for the post
The data.json rows cite AB12 "a fast short-input PRF" as the official claim for BOTH variants.  AB12's §3
claim covers only c>=2, d>=4; SipHash-1-3 is explicitly listed there as a cryptanalysis target.  The
honest source for the 1-3 "claim" is Aumasson's comment on rust-lang/rust#29754 (2015-11-12) and the
Rust std choice (PR #33940, merged 2016-05).  Suggested wording: see verdict in the structured output.
