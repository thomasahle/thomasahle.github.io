# foldhash-fast: standalone fixed-pair reproduction

foldhash 0.2.0, native 64-bit little-endian path. EXTENSION of the paper’s wyhash/rapidhash complement-both-operands differential, now at L = 1. Orson Peters’ 2024 known/fixed-seed attacks are prior art for the folded-multiply family, not this differential.

## Build and run

```sh
cc -O2 -std=c11 -pthread -o foldhash_verify foldhash_verify.c
./foldhash_verify 20 1 0xc0ffee1234567890
```

Requires a C11 compiler with `unsigned __int128`, POSIX threads and a 64-bit little-endian host (the loads use native byte order). The smoke commands use one worker and a deterministic seed, read no external files, and write only stdout/stderr. Reference checks run before measurement and must pass. `run_2p20.txt` records our local smoke run. Zero new hits at this scale is expected for an approximately 2^-26.6 event and does not reproduce or refute the historical rate.

## Implementation and validation

The original independent verifier is copied unchanged from scratchpad/vkf1/foldhash_verify.c (verify:key-free:1). It reimplements the short and long paths, seed expansion and finish, and checks baked-in outputs from the unmodified real Rust crate `foldhash = "=0.2.0"`. See the source header for exact coverage. These checks establish consistency on the reference cases, not equivalence for every possible input.

## Pair and seed model

```
m  = 0000000000000000
m′ = ffffffffffffffff
```

Both are eight bytes; L = 1. The first and last 8-byte loads alias, so complementing w complements both keyed fold operands. Equal fast accumulators remain equal through vector/string framing and the quality final fold. The all-ff byte string is not UTF-8; the article gives a separately verified valid-UTF-8 pair.

The scored model samples the per-hasher seed and six shared words independently uniformly. This is an idealized 448-bit secret model, not `RandomState::default()` and not the SMHasher3 mapping of one S into both seeds. The source’s sampling PRNG and thread partition are deterministic when given the documented seed. This uniform model does not by itself prove an ordering of collision rates relative to every other seed distribution.

## Historical sample and reproduction

Selected count: **2757 / 2^38**, from the VERIFIED synthesis. `historical-run.txt` preserves the original output. The quality total combines four separate 2^34 streams (168 + 198 + 170 + 160 = 696); do not replace this with one 2^36 stream and expect the same count. The original large-run commands are:

```sh
./foldhash_verify 38 24 0xc0ffee1234567890
```

These take substantially longer than the smoke run. Thread count and RNG seed affect the deterministic stream. The fast log has 2757 in all five counters; the quality log has equal fast/quality raw/vector counters in all four chunks and zero quality-only hits. The extra model-1 result in the quality log is not included in its selected count.

Arguments: `[log2 seeds] [threads] [base seed]`; defaults are 30, 24 and 0xc0ffee1234567890.

## Scope, attribution and disclosure

Algorithm: Orson Peters, foldhash, tag v0.2.0 ([source](https://github.com/orlp/foldhash/tree/v0.2.0)); original source and Zlib license are in `../../sources/foldhash-0.2.0/`. The unchanged verifier source is an independent translation, not the upstream implementation. The standalone archive also includes a copy of the upstream notice as `FOLDHASH-LICENSE` in this directory.

No every-seed pair was verified within a fixed byte-slice key type. The separately verified cross-key-type ambiguities in the article are outside this score and are not tested by this C byte-pair harness. The discarded from_u64(u64::MAX) absorbing-state claim is not used.

Disclosure: not yet reported to the maintainer.
