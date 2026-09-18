# Fix for the HalftimeHash24 encoder, key sizing, and length handling

A patch is available on the local `fixed-24byte-core` branch, based on
`fix-neon-dispatch`. The branch can be offered for review; it has not been pushed.
`FIX.diff` contains the header changes.

Encode3, Encode4, and Encode5 captured a stale input pointer in `DistributeRaw`,
advanced by individual blocks instead of rows, and included parity rows in the
first parity loop. The patch uses a referenced row pointer and stops at the last
data row. Generator matrices extracted by running the header now pass exhaustive
GF(2) rank checks for every survivor subset, proving packet distances 3, 4, and 5.
Encode2 retains distance 2. All 20,160 single-bit Encode3 input flips across the
four block widths change exactly three encoded packets.

The entropy helper now counts the key prefix actually required by the fixed
nine-level key layout, the live forest roots, and the padded tail. Its argument
is a byte length. Guard-page tests pass with the reported allocation and fault
when one word is removed, for all widths and output sizes at 592 boundary cases.

The three-word core also needs a length tag: the previous core mapped an empty
message and one zero byte to identical padded input for every key. The patch
reserves a separate Toeplitz tail pool, right-aligns the raw tail, and appends the
original byte length as the final scalar NH word. Its key position is fixed across
lengths. This preserves the 24-byte output and gives an unequal-length bound of
`2^-96`; simply appending at a variable key position would not establish the same
fresh-key argument. The terminal-word proof has also been checked in Lean.

The tabulation helpers now use valid flat-array indexing for all 24 byte tables
used by a Style wrapper. EHC entropy loads likewise avoid reshaping the caller's
array. The scalar parts of SIMD horizontal sums explicitly use unsigned arithmetic,
so the checks do not need `-fwrapv`. The parent branch's NEON dispatch repair is
retained.

The Style wrappers still use Encode2 and the original length tables. Their outputs
are unchanged: 40,000 comparisons against the parent header agree. The fixed raw
24-byte outputs intentionally change. Tests cover scalar, SSE2, AVX2, AVX-512, and
NEON on 10,000 random inputs per configuration, plus UBSan on both hosts.

The repaired construction has collision bound
`(h+2)^2(h+5) * 2^-96`. Its usual `h=16` expression is 83.27 collision-bound bits;
the length-normalized eight-byte-word-cap score is 96 bits. The retained stack
still restricts the supported length domain to fewer than `19,173,961` complete
groups and has actual height at most 7. The patch does not claim exabyte-size
support. The fixed Style wrappers retain their sharp normalized score of 63 bits.

`THEOREM.md`, `certificates/`, and `REPORT.md` contain the exact domain, key layout,
proof mapping, regression results, and two-host SMHasher3 measurements. The branch
and these reproducible checks are available for incorporation or review.
