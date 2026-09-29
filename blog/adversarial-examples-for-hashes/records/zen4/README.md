# AMD EPYC 9R14 (Zen 4): the full speed panel

This directory holds the third host of the chart, an AMD EPYC 9R14 (Zen 4, Genoa). Every
timed chart row was re-run there with the public
[timing reproduction](https://github.com/thomasahle/hash-benchmark-reproduction), following
its README. That covers all 42 manifest rows and the six chart rows outside the manifest.
PolyXOR128's raw variant, the four HalftimeHash styles and a ChainHash backend comparison were
run alongside. The Xeon and M2 tabs are unchanged except for the ChainHash rows, which were
refreshed to the same revision ([records/chainhash/refresh-a0116ea](../chainhash/refresh-a0116ea/README.html)).

## Host

| | |
|---|---|
| CPU | AMD EPYC 9R14, family 25 model 17 (Zen 4); a 16-vCPU KVM guest, one thread per core |
| Caches | 1 MiB L2 per core; two 32 MiB L3s, one shared by CPUs 0–7 and one by CPUs 8–15 |
| ISA | AVX-512 (F, VL, BW, DQ, IFMA, VBMI), VPCLMULQDQ, VAES, GFNI, SHA-NI |
| Clock | TSC 2.600 GHz (`tsc_known_freq`, clocksource `tsc`). A busy core runs at 3.66 GHz (boost), measured with a dependent-add loop; a user-mode `perf_event_open` cycle counter confirms 1.0002 cycles per add. |
| OS | Rocky Linux 9.8, kernel 5.14.0-687 |
| Compiler | GCC 11.5.0 (Red Hat 11.5.0-14), `-O3 -march=native` (resolves to `znver4`); Rust 1.98.1 for PolyXOR128 (backend `hash_blocks_avx512`) |
| Pinning | `nice -n 10 taskset -c 8-15` for every timing, and for the builds |

SMHasher3's x86 timer counts TSC ticks. B/cycle on this tab therefore means bytes per
2.600 GHz tick. Wall-clock GB/s is 2.6 × B/cycle, and bytes per core cycle at 3.66 GHz is
0.71 × B/cycle. The printed GiB/s assumes 3.5 GHz, as on the other hosts. The Xeon's TSC runs
at 2.900 GHz, and the M2 uses a calibrated clock, so compare hashes within one tab.

## Protocol

This is the reproduction's x86 rule, the same as on the Xeon. Each row gets two complete
serial passes of unmodified `SMHasher3 NAME --test=Speed`. The cell takes the higher bulk
average (262144-byte keys, eight alignments) and, independently, the lower 1–31-byte average.

Each start waits until no other SMHasher3 process runs on CPUs 8–15. Every 5 s the runner
samples for overlapping runs; none was excluded. Another job ran on CPUs 0–7, the other L3,
during the measurements, so no machine-wide load threshold was applied. Load1 at launch
ranged 0.95–6.35 and is recorded per run.

The rows outside the manifest ran on the same binary with the same selection. There, a
whole-machine check discarded and repeated three runs that overlapped another SMHasher3
process (`evidence/extra/discarded.jsonl`). No bulk spread exceeds 2%. The largest small-key
spreads are 3.5% (HalftimeHash-512) and 3.2% (`polyxor-128.raw`), so no row needed the
>5% re-run.

The ChainHash pin of the reproduction moved to
[`a0116ea`](https://github.com/thomasahle/chainhash/tree/a0116ea2072c0d9605acc6b47f0dc9f9d57b0c58)
during the run. ChainHash does not enter any other row. The build was repeated at the new
pin, and VerifyAll was asserted: 0x66672BD6 and 0x1FCA728C, unchanged. The `chainhash` and
`chainhash-128` rows were then re-timed on that binary (`evidence/panel-chainhash`). Their
earlier values at `30c0111` are kept in `speeds_zen4.json#/meta`.

## Results

Verification is SMHasher3's VerifyAll value on this build. Sanity is the reproduction's
native Sanity record. The four Sanity failures (foldhash ×2, shipped HalftimeHash24 and
Marvin32) are the ones the reproduction README documents for x86. The two HalftimeHash24
wrappers have zero registered constants. Their computed values equal the Xeon's
(0x8A105352 shipped, 0x3F1372EA fixed).

| Row | Registration | Bulk B/tick | GB/s at TSC 2.6 GHz | 1–31 B cycles | Spread bulk / small | Verification | Sanity |
|---|---|---:|---:|---:|---|---|---|
| city | `CityHash-64` | 6.82 | 17.7 | 34.57 | 0.00% / 0.00% | 0x5FABC5C5 PASS | PASS |
| farm | `FarmHash-64.NA` | 6.74 | 17.5 | 34.48 | 0.00% / 0.00% | 0xEBC4A679 PASS | PASS |
| murmur | `MurmurHash3-128` | 2.95 | 7.7 | 36.88 | 0.00% / 0.00% | 0x6384BA69 PASS | PASS |
| mx3 | `mx3.v3` | 4.34 | 11.3 | 32.52 | 0.00% / 0.18% | 0x7B287B65 PASS | PASS |
| fasthash-64 | `fasthash-64` | 2.85 | 7.4 | 26.14 | 0.00% / 0.08% | 0xA16231A7 PASS | PASS |
| fasthash-32 | `fasthash-32` | 2.85 | 7.4 | 27.60 | 0.00% / 0.00% | 0xE9481AFC PASS | PASS |
| muse | `MuseAir` | 9.95 | 25.9 | 13.27 | 0.00% / 0.00% | 0xF89F1683 PASS | PASS |
| muse-v2 | `MuseAir-v2` | 7.33 | 19.1 | 19.92 | 0.14% / 0.05% | 0x7140CABC PASS | PASS |
| komi | `komihash` | 7.71 | 20.0 | 21.65 | 0.00% / 0.00% | 0x8157FF6D PASS | PASS |
| t1ha | `t1ha2-64` | 7.00 | 18.2 | 28.59 | 0.00% / 0.00% | 0x8F16C948 PASS | PASS |
| a5 | `a5hash` | 3.80 | 9.9 | 11.04 | 0.26% / 0.00% | 0xADDE79B3 PASS | PASS |
| a5wide | `a5hash-128` | 10.00 | 26.0 | 19.36 | 0.00% / 0.00% | 0x89406B11 PASS | PASS |
| rapid3 | `rapidhash` | 11.61 | 30.2 | 19.84 | 0.09% / 0.00% | 0x1FDC65EE PASS | PASS |
| foldhash-fast | `foldhash-fast` | 11.41 | 29.7 | 14.03 | 0.80% / 0.07% | 0xA167BA5E PASS | FAIL |
| foldhash-quality | `foldhash-quality` | 11.25 | 29.2 | 17.47 | 0.18% / 0.06% | 0xE269316E PASS | FAIL |
| mum | `mum3.exact.unroll3` | 8.80 | 22.9 | 17.30 | 1.97% / 0.06% | 0x8BD72B8C PASS | PASS |
| mir | `mir.exact` | 3.25 | 8.5 | 24.50 | 0.62% / 0.00% | 0x00A393C8 PASS | PASS |
| xxh3-64 | `XXH3-64` | 18.81 | 48.9 | 20.38 | 0.16% / 0.05% | 0x1AAEE62C PASS | PASS |
| xxh3-128 | `XXH3-128` | 18.79 | 48.9 | 23.66 | 0.80% / 0.04% | 0x288DAA94 PASS | PASS |
| highway | `HighwayHash-64` | 3.88 | 10.1 | 59.71 | 0.00% / 0.00% | 0xF3246108 PASS | PASS |
| spooky | `SpookyHash2-64` | 6.65 | 17.3 | 34.76 | 0.00% / 0.03% | 0x972C4BDC PASS | PASS |
| pengyhash | `pengyhash` | 6.15 | 16.0 | 52.48 | 0.00% / 0.02% | 0x861A1254 PASS | PASS |
| nmhash32 | `NMHASH` | 11.43 | 29.7 | 32.07 | 0.00% / 0.00% | 0x12A30553 PASS | PASS |
| nmhash32x | `NMHASHX` | 11.45 | 29.8 | 20.73 | 0.09% / 0.05% | 0xA8580227 PASS | PASS |
| gx | `gxhash-64` | 30.79 | 80.1 | 31.28 | 0.69% / 0.00% | 0x48F84240 PASS | PASS |
| ahash | `rust-ahash` | 0.78 | 2.0 | 72.07 | 0.00% / 0.03% | 0x3BF4383B PASS | PASS |
| siphash-1-3 | `SipHash-1-3` | 1.03 | 2.7 | 62.06 | 0.00% / 0.00% | 0x8936B193 PASS | PASS |
| siphash-2-4 | `SipHash-2-4` | 0.55 | 1.4 | 86.30 | 0.00% / 0.03% | 0x57B661ED PASS | PASS |
| halftime24 | `HalftimeHash24-shipped` | 15.96 | 41.5 | 54.51 | 0.06% / 0.02% | 0x8A105352 unverifiable (zero constant) | FAIL |
| go-maphash | `GoMapHash` | 22.38 | 58.2 | 74.12 | 0.00% / 0.00% | 0x710289AB PASS | PASS |
| abseil-hash | `AbseilHash-default` | 9.77 | 25.4 | 14.06 | 0.21% / 0.00% | 0x07203CDB PASS | PASS |
| dotnet-marvin | `Marvin32` | 1.14 | 3.0 | 22.00 | 0.00% / 0.05% | 0x306A5169 PASS | FAIL |
| polymur | `polymurhash` | 6.13 | 15.9 | 28.32 | 0.99% / 0.00% | 0x0722B1A7 PASS | PASS |
| poly1305 | `poly1305-hash` | 5.05 | 13.1 | 195.97 | 0.20% / 0.06% | 0xBD015C42 PASS | PASS |
| ghash | `ghash` | 8.91 | 23.2 | 1418.97 | 0.22% / 0.49% | 0x1F397201 PASS | PASS |
| umash | `UMASH-64` | 9.10 | 23.7 | 26.91 | 0.00% / 0.15% | 0x36A264CD PASS | PASS |
| umash128 | `UMASH-128` | 6.39 | 16.6 | 31.13 | 0.00% / 0.03% | 0x63857D05 PASS | PASS |
| clhash | `CLhash` | 10.55 | 27.4 | 31.23 | 0.00% / 0.06% | 0x2E554CB4 PASS | PASS |
| chainhash | `chainhash` | 35.09 | 91.2 | 62.46 | 0.00% / 0.03% | 0x66672BD6 PASS | PASS |
| halftime24-fixed | `HalftimeHash24-fixed` | 12.44 | 32.3 | 71.63 | 0.16% / 0.13% | 0x3F1372EA unverifiable (zero constant) | PASS |
| chainhash128 | `chainhash-128` | 19.32 | 50.2 | 116.78 | 1.79% / 0.02% | 0x1FCA728C PASS | PASS |
| polyxor | `polyxor-128` | 19.79 | 51.5 | 129.96 | 0.00% / 0.03% | 0xA9574CA8 PASS | PASS |
| (extra) | `wyhash` | 11.20 | 29.1 | 17.46 | 0.27% / 0.06% | 0x9DAE7DD3 PASS | — |
| (extra) | `XXH-64` | 4.14 | 10.8 | 35.71 | 0.00% / 0.00% | 0x8F8224C4 PASS | — |
| (extra) | `XXH-32` | 2.85 | 7.4 | 27.31 | 0.00% / 0.22% | 0x6FD78385 PASS | — |
| (extra) | `MurmurHash2-64` | 2.85 | 7.4 | 26.60 | 0.00% / 0.04% | 0x1F0D3804 PASS | — |
| (extra) | `MurmurHash2-32` | 1.42 | 3.7 | 22.52 | 0.00% / 0.00% | 0x27864C1E PASS | — |
| (extra) | `MurmurHash2a` | 1.42 | 3.7 | 25.80 | 0.00% / 0.08% | 0x7FBD4396 PASS | — |
| (extra) | `HalftimeHash-64` | 3.37 | 8.8 | 65.03 | 0.00% / 0.34% | 0xED42E424 PASS | — |
| (extra) | `HalftimeHash-128` | 11.35 | 29.5 | 63.92 | 0.18% / 0.42% | 0x952DF141 PASS | — |
| (extra) | `HalftimeHash-256` | 17.12 | 44.5 | 63.98 | 0.41% / 0.53% | 0x912330EA PASS | — |
| (extra) | `HalftimeHash-512` | 17.45 | 45.4 | 70.48 | 0.40% / 3.49% | 0x1E0F99EA PASS | — |
| (extra) | `polyxor-128.raw` | 19.83 | 51.6 | 96.43 | 0.00% / 3.15% | 0x23A6E8BB PASS | — |

## ChainHash backends on Zen 4

The headers dispatch to ZMM when AVX-512 is present. Forcing each x86 backend in one binary
(`speeds_zen4_chainhash_backends.json`):

| Registration | Bulk B/tick | 1–31 B cycles |
|---|---:|---:|
| chainhash (run-time dispatch) | 35.14 | 61.78 |
| chainhash.zmm | 35.15 | 61.75 |
| chainhash.ymm | 20.14 | 61.72 |
| chainhash.xmm | 10.07 | 61.79 |
| chainhash-128 (run-time dispatch) | 19.35 | 116.58 |
| chainhash-128.zmm | 19.38 | 127.77 |
| chainhash-128.ymm | 12.40 | 127.53 |
| chainhash-128.xmm | 6.19 | 127.94 |

ZMM is 1.75× (ChainHash) and 1.56× (ChainHash-128) faster than YMM in bulk on Zen 4. Zen 4
executes 512-bit VPCLMULQDQ as two 256-bit halves, but the wider loop still wins, so the ZMM
dispatch is right on this CPU.

The forced ChainHash-128 registrations pass the backend as a compile-time constant. Their
short-input cost (about 128 cycles) is higher than the dispatched entry point's 116.6; bulk
is unaffected.

## Files

| File | What it is |
|---|---|
| [speeds_zen4.json](speeds_zen4.json) | The 42 manifest rows: selected cells, both passes, spreads, load, Sanity records (reproduction `benchmark.py` output) |
| [speeds_zen4_extra.json](speeds_zen4_extra.json) | wyhash, XXH64, XXH32, MurmurHash64A, MurmurHash2, MurmurHash2A, the four HalftimeHash styles, `polyxor-128.raw`; with per-length 1–31 B costs |
| [speeds_zen4_chainhash_backends.json](speeds_zen4_chainhash_backends.json) | The backend comparison above, with `0a03c63` and PolyXOR128/XXH3-128 controls |
| [provenance.json](provenance.json) | CPU, clocks, kernel, compiler and flags, rustc, patch hashes, binary SHA-256s, pin set, load |
| [verifyall-30c0111.txt](verifyall-30c0111.txt), [verifyall-a0116ea.txt](verifyall-a0116ea.txt) | VerifyAll of the two reproduction builds |
| `evidence/panel/`, `evidence/panel-chainhash/`, `evidence/extra/`, `evidence/backends/` | Raw `--test=Speed` output per row and pass (`NAME.runN.txt`), run and gate records, Sanity logs |
| [bench/](bench/clockprobe.c) | The TSC and core-clock probe with its before and after output, and the provenance collector |
| [merge_epyc.py](merge_epyc.py) | Writes these cells into the article's `data.json` and `records/speeds.json` |

Reproduce with the repository's README (`--host EPYC9R14 --cpus 8-15 --nice 10`). The extra
rows use `records/chainhash/refresh-a0116ea/bench/run_speed.py` on the same binary.
