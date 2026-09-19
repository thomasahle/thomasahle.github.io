# SipHash fixed-pair collision claim: can it be resolved? (consolidated memo, 2026-09-18)

Question from the author: both SipHash rows (1-3 and 2-4) now appear as hollow "unresolved claim"
markers at 64 bits. Can the claim be resolved (proved or refuted)?

Three lenses ran in parallel: literature (no computation), feasibility (SAT search for XOR-differential
characteristics, Xeon), experiment (Monte-Carlo fixed-pair and differential screening, Xeon). They agree
on the verdict and on the reasons. Source material:

- literature/LITERATURE.md (verbatim quotes) + PDFs/text of AB12, DMS14 (ePrint 2014/722), He-Yu (2019/865),
  Niu et al. (2022/765), Chakraborty et al. (2024/1359); Rust issue #29754 and PR #33940.
- feasibility/ (sipchar.py CaDiCaL model, sip_pair.c, sip_seg.c, RUNLOG.md; logs on the Xeon under
  <xeon-work>/siphash-claim/feasibility/logs).
- experiment/ (sipdiff.c, gendiffs.py, run_phase*.sh, analyze.py, results.json, REPRODUCE.md, raw outputs).

## 1. The answer

No, and not for lack of budget. The post's metric is eps(m,m') = Pr over a uniform 128-bit key that a fixed
pair collides on the 64-bit tag, and a proof of eps <= c*2^-64 would be a universal-hashing theorem about a
keyed ARX permutation: SipHash has no algebraic structure to count agreements over (no polynomial, no
field), the design paper's only security statement is the conjecture that SipHash-c-d with c >= 2, d >= 4
"is expected to provide the maximum PRF security possible" (Aumasson-Bernstein 2012, sec. 3), and no proof
of that kind exists for any ARX PRF; a fixed-pair bound would follow from PRF security (eps <= 2^-64 + PRF
advantage) and from nothing weaker, so proving it means proving SipHash. Note also that the strict
statement eps <= 2^-64 is false by counting for every key (with >= 9-byte messages, each key has about
N^2/2^65 colliding pairs, so the worst pair has eps >= 2^-64(1-2^-8)); the most a proof could ever say is
"about 2^-64", which is what the row already says. SipHash-1-3 is not even covered by the designers'
conjecture (c=1 is explicitly listed as a cryptanalysis target); its only claim is Aumasson's 2015 remark on
rust-lang/rust#29754 that he "would be very surprised if SipHash-1-3 introduced weaknesses for hash tables",
on which Rust std adopted it (PR #33940, May 2016). A refutation, conversely, would be a pair with eps
measurably above 2^-64, i.e. an XOR differential through the full function (for 1-3: one compression
SipRound, the injection into v0, three finalization rounds, and cancellation in the 64-bit XOR
v0^v1^v2^v3). The best published objects are far from that: internal-collision characteristics of
probability 2^-167 for SipHash-1-x and 2^-236.3 for SipHash-2-x = full 2-4 (Dobraunig, Mendel, Schlaffer,
SAC 2014, Table 1; the designers' own was 2^-498), both below even the 2^-128 threshold at which DMS14 say a
characteristic starts to beat the birthday attack; everything since 2014 is a distinguisher on the isolated
4-round finalization (DMS14 2^-35; Niu et al. CRYPTO 2022 DL correlation 2^-6.03; Chakraborty et al. ToSC
2024 impossible differentials; He et al. ePrint 2026/2013 five rounds at correlation 2^-20.15), a result on
reduced d <= 2 (He-Yu 2019/2024: distinguishers 2^10-2^12 on 2-1, 2^36 on 2-2, key recovery on 2-1), on
modified constants (Xin et al. CANS 2019, RX characteristics 2^-93.6 / 2^-160 for a revised SipHash-1-x), or
on d <= 1 (Sasaki, Kurahara, Sakamoto, Isobe, ACISP 2025: tag-collision forgeries for 1-1, 1-0, 2-0 only;
the paper is paywalled and its abstract lists nothing for d >= 2). What we measured confirms the picture
from below: the experiment lens screened 10,517 XOR message differences (all 1-bit, all 2-bit, 59/122
structured) at 8 and 16 bytes for 1-1, 1-2, 1-3 and 2-4 with 2^22-2^24 random keys each, re-ran ~20
candidates per cell at 2^28 keys both as key-averaged differentials and as genuine fixed pairs (0 vs D,
ff..ff vs ff..ff^D), and found zero 64-bit collisions in about 1.9e11 pair evaluations per variant and no
output bias (max |z| 2.96 against a Bonferroni threshold of 3.67); the per-pair 95% floor is 2^-26.4, 37.6
bits short of 2^-64, a gap Monte-Carlo cannot close (2^66 keys for a 3-hit expectation). The harness is
calibrated: it reproduces the designers' 3-round bias (SipHash-1-1, z = -960 at the MSB of the message word,
1.54x excess low-16 collisions, and the bit63 > bit62 > bit23 ranking) and the 4-round vanishing point, and
even SipHash-1-0 (2 SipRounds, wildly broken: low-16 collision rate 2^-7) produces no exact 64-bit collision
in 3.6e10 evaluations, because the XOR of four lanes rarely cancels exactly. The feasibility lens's SAT model
(Lipmaa-Moriai XOR model, CaDiCaL, ~4 CPU-min per job) shows no single-block SipHash-1-3 collision
characteristic of weight <= 29 (1-2: > 28; 1-1: > 26), with the Matsui pieces P1 = 18 exact, P2 > 11,
P3 > 13 and a two-block prefix of weight only 8 (Delta0 = 2^63, Delta1 = 0x1080100000), so the entire
barrier for two-block messages is the 3 finalization rounds plus the output-XOR condition, whose cost is
unknown but is bounded below by nothing useful yet. Net: neither side is reachable; "unresolved" is the
honest status and is what the post shows.

## 2. Text for the post (rows, hover, profile)

One factual correction first. data.json currently cites AB12's "a fast short-input PRF" as the
official_claim for BOTH rows. That claim is stated only for c >= 2, d >= 4; SipHash-1-3 has no claim in the
design paper. Change the 1-3 row's official_claim to Aumasson's comment on rust-lang/rust#29754
(text: "I would be very surprised if SipHash-1-3 introduced weaknesses for hash tables",
url: https://github.com/rust-lang/rust/issues/29754), and keep the AB12 claim for 2-4.

Chart note (replace the SipHash sentence in #chart-additions-note):

  SipHash-1-3 and SipHash-2-4 are plotted as unresolved claims at their 64-bit output width. No proof of
  the fixed-pair bound exists for any ARX PRF, and no refutation is known: the best published collision
  characteristics are 2^-167 for SipHash-1-x and 2^-236.3 for SipHash-2-4 (Dobraunig, Mendel and Schlaffer,
  2014), and our own search sees nothing above 2^-26.4 per pair.

score.display_text (both rows): "64 bits · PRF claim (no theorem; best known characteristic 2^-167 / 2^-236.3)"
  (1-3 gets 2^-167, 2-4 gets 2^-236.3.)

hover, SipHash-1-3:
  SipHash-1-3: 64-bit output, plotted as a claim. Not covered by the designers' PRF conjecture (which
  states c >= 2, d >= 4); rests on Aumasson's 2015 remark that he "would be very surprised if SipHash-1-3
  introduced weaknesses for hash tables" (rust-lang/rust#29754), the basis of Rust std's choice. No proof,
  no known pair: best published collision characteristic 2^-167 (DMS 2014); our 2^28-key fixed-pair search
  found no collision and no bias, floor 2^-26.4 per pair.

hover, SipHash-2-4:
  SipHash-2-4: 64-bit output, plotted as a claim. The designers conjecture maximal PRF security
  (Aumasson-Bernstein 2012), which would give about 2^-64 per fixed pair; no proof exists for any ARX PRF.
  No known pair: best published characteristic for the full function 2^-236.3 (DMS 2014); our 2^28-key
  fixed-pair search found no collision and no bias, floor 2^-26.4 per pair.

qualification (shared, replaces the current text):
  Plotted as an unresolved claim at its 64-bit output width. SipHash is an ARX PRF design; a fixed-pair
  collision bound would follow only from its unproven PRF security (eps <= 2^-64 + PRF advantage), and no
  such bound has been proved for any ARX PRF. No refutation is known either: the best published
  differential characteristics are 2^-167 for an internal collision in SipHash-1-x and 2^-236.3 for full
  SipHash-2-4 (Dobraunig, Mendel and Schlaffer, SAC 2014), the 2025 tag-collision forgeries reach only
  SipHash-1-1, 1-0 and 2-0 (Sasaki, Kurahara, Sakamoto and Isobe, ACISP 2025), and the remaining results
  are distinguishers on four or five finalization SipRounds. Our own search over 10,517 single-bit,
  two-bit and structured message differences at 8 and 16 bytes, 2^24 to 2^28 random keys each and fixed
  pairs re-tested at 2^28 keys, found no collision and no output bias with a detection floor of 2^-26.4 per
  pair, 37 bits short of the claim; the same harness reproduces the known 3-round bias and 4-round
  vanishing point, so the null result is about the function, not the harness. If some pair collided with
  probability 2^-63 or more it would be a 2^64-query distinguisher contradicting the PRF claim, so the
  plotted width is exactly as strong, and as unproven, as SipHash itself. The SMHasher3 wrapper expands a
  64-bit seed into the key, so the timed function is a 2^64-member subfamily.

appendix-siphash paragraph (the published post still says "neither is plotted"; the draft plots both):
  Both use an add-rotate-xor permutation keyed by 128 bits and target a stronger problem than universal
  hashing: an adaptive attacker who learns earlier outputs. There is no universality theorem to score and
  no colliding pair to cap, so both are plotted as hollow claims at 64 bits. SipHash-2-4 carries the
  designers' PRF conjecture [AB12, sec. 3]; SipHash-1-3 does not, and rests on Aumasson's 2015 opinion
  quoted in the Rust issue that adopted it [rust-lang/rust#29754, PR #33940]. Twelve years of cryptanalysis
  give internal-collision characteristics of 2^-167 (1-x) and 2^-236.3 (2-x) [DMS14], forgeries only for
  d <= 1 [SKSI25], and distinguishers on 4-5 finalization rounds [DMS14, NSLL22, CHNE24, HHNPW26]; my own
  search (records/siphash/) finds nothing above 2^-26.4 per pair and reproduces the known 3-round bias and
  4-round vanishing point.

Citations to add to the post's reference list: Aumasson, Bernstein, "SipHash: a fast short-input PRF",
INDOCRYPT 2012 (already present); Dobraunig, Mendel, Schlaffer, "Differential Cryptanalysis of SipHash",
SAC 2014, ePrint 2014/722; Sasaki, Kurahara, Sakamoto, Isobe, "Forgery Attacks on SipHash", ACISP 2025,
doi 10.1007/978-981-96-9095-4_1 (abstract only was read; say so); He, Yu, Comput. J. 67(3) 2024 / ePrint
2019/865; Niu, Sun, Liu, Li, CRYPTO 2022, ePrint 2022/765; Chakraborty, Hadipour, Nguyen, Eichlseder, ToSC
2024(3), ePrint 2024/1359; Xin, Liu, Sun, Li, CANS 2019 (abstract and ASK 2019 slides only). Label our
experiment as a null-result reproduction/extension per the cite-prior-work rule: the 3-round bias and
4-round vanishing point are reproductions of AB12 sec. 5; the fixed-pair floor is new but trivial.

Correction to one report: the experiment lens attributes DMS14 to "Dobraunig, Eichlseder, Mendel"; the SAC
2014 authors are Dobraunig, Mendel and Schlaffer. Use the latter.

## 3. Is a longer search worth running?

No, not to change the plot. Three separate reasons:

- Monte-Carlo: the observable floor at N keys is about log2(N) - 1.6 bits (2^-26.4 at 2^28). Reaching
  even 2^-40 needs 2^42 key trials per pair (~2 CPU-hours per pair on 8 Xeon cores for 1-3, a few pairs,
  fine) but that is still 24 bits above the claim; 2^-64 itself needs 2^66 trials. No sampling budget
  closes a 37-bit gap.
- Characteristic search (SAT): the only affordable question is a bound on the best single XOR
  characteristic, and UNSAT time grows ~1.4x per unit of weight. The useful runs are P3 (free input, 3
  finalization rounds, output-XOR condition; every characteristic for any message length costs >= 8 + P3)
  and P2 (2 rounds; one-block characteristics cost >= 18 + P2). Each is feasible to weight ~35-40 in hours
  to a day on the Xeon; a direct UNSAT proof at weight 63 for the full one-block model would take ~1.4^34 x
  100 s, i.e. years. Outcomes: if P3 >= 56 (unlikely; DMS14's free 4-round characteristic costs 35) all
  single characteristics fall below 2^-64, which would still not bound the differential (clustering) and so
  would still not be a proof; if P3 lands at 30-45 (likely) a two-block characteristic of weight 40-55
  probably exists, is unverifiable empirically (needs 2^45+ keys) and is far above 2^-64 anyway, so the
  marker does not move. Cost: 1-2 Xeon days, cores 24-31, nice 10. Value: one sentence ("no single-block
  XOR characteristic of weight <= W") for the appendix. Recommend running it only if the author wants that
  sentence; it is not on the path to resolution.
- Literature: the two paywalled papers (CANS 2019, ACISP 2025) could be obtained via a library or by
  writing to the authors; that costs nothing computational and would let the post cite the ACISP 2025
  updated 1-x internal-collision bound (medium-low confidence in the exact figure; the abstract's forgery
  list implies it stays above 2^-128). Worth a request, not a run.

## 4. Marker category

Keep the hollow "unresolved claim" marker, but the legend and the row text should distinguish two kinds
of hollowness, because they are currently conflated. UMASH's 55/83-bit points are "claimed, with a specific
gap in a published proof that could be closed tomorrow"; SipHash's are "cryptographic assumption, no proof
of this kind possible with current techniques, best known attack far below the claim". The honest
rendering, in decreasing order of effort:

(a) Minimal: keep one hollow marker, keep "unresolved claim" in the legend, and make the SipHash rows'
    score text say "PRF assumption; best known characteristic 2^-167 / 2^-236.3" (section 2). This is
    correct and costs nothing.
(b) Better: add a third claim sub-kind in data.json, e.g. bits_kind "assumed" with score.direction
    "assumption", rendered as the same hollow circle but with a different stroke (dashed) and a legend
    entry "cryptographic assumption (no proof expected; best known attack shown on hover)". The chart then
    says what the reader needs: UMASH = provable-in-principle gap, SipHash = assumption. Also fits any
    future PRF rows (e.g. BLAKE3-keyed, AES-based hashes) without special-casing.
(c) Not recommended: a solid marker or a numeric "attack" cap. The 2^-167 / 2^-236.3 figures are
    characteristics (lower bounds on eps for one differential), not a demonstrated pair, and a rust
    "witness" mark at 167 or 236 bits would read as if those were measured collision rates above the claim;
    they are below it and prove nothing about the worst pair.

Recommendation: (b) if the chart code can take a third hollow style cheaply (post.js already branches on
score.direction); otherwise (a). Either way the sentence in section 2 goes into the chart note.

## Files

- This memo: scratchpad/design/siphash-claim/MEMO.md
- Literature note: scratchpad/design/siphash-claim/literature/LITERATURE.md
- SAT model + run log: scratchpad/design/siphash-claim/feasibility/{sipchar.py,RUNLOG.md}
- Experiment: scratchpad/design/siphash-claim/experiment/{REPRODUCE.md,results.json,phaseA_summary.txt,
  phaseB_summary.txt,calib10_summary.txt}; Xeon mirror <xeon-work>/siphash-claim/experiment/
- Post rows to edit: <repos>/website/drafts/456e626ff04ecf7e2c6af24765379a55/adversarial-examples-for-hashes/data.json
  (ids siphash-1-3 at line ~12123 and siphash-2-4 at line ~12591: official_claim, qualification, hover,
  score.display_text), index.html #chart-additions-note and #appendix-siphash.
- No solver or sipdiff processes are left running on the Xeon.
