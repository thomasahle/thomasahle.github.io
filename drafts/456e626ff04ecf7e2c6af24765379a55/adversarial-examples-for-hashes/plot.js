// Generated from data.json by .work/build_plot.py.
const HASH_POINTS = [
  {
    "id": "city",
    "name": "CityHash64 v1.1.1",
    "version": "v1.1.1",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "Public compression collides before the seed is applied.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 5.78,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/CityHash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "CityHash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "CityHash-64                    64                      Google CityHash64WithSeed",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 5.43,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/CityHash-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "CityHash-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "CityHash-64                    64                      Google CityHash64WithSeed",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 44.28,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/CityHash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "CityHash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "CityHash-64                    64                      Google CityHash64WithSeed"
      }
    },
    "anchor": "appendix-cityhash64",
    "label": "CityHash64",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. ",
    "official_claim": {
      "text": "not suitable for cryptography",
      "url": "https://github.com/google/cityhash/blob/master/README",
      "context": "Explicit noncryptographic disclaimer.",
      "provenance": "data/claims.json#/result/table/rows/8/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 205,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/8",
      "failed_tests": [
        "SeedZeroes",
        "SeedSparse",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/8/smhasher3_verdict"
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Public compression collides before the seed is applied."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      8,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "EXTENSION",
    "classification_reason": "Peters 2024 and Aumasson–Bernstein–Bosslet 2012 seed-last attack; newly constructed v1.1.1 witnesses.",
    "mechanism_family": "Murmur/City/Farm lineage (multiply–xorshift mixing)",
    "rurban_alias": null,
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit WithSeed seed; structural result also holds for all pairs of 64-bit seeds.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "farm",
    "name": "FarmHash64 NA v1.1",
    "version": "NA v1.1",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "Public compression collides before either secret seed is applied.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 5.76,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/FarmHash-64.NA/M2Pro",
        "host": "M2Pro",
        "registered_name": "FarmHash-64.NA",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "FarmHash-64.NA                 64                      FarmHash Hash64WithSeed (NA version)",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 5.43,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/FarmHash-64.NA/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "FarmHash-64.NA",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "FarmHash-64.NA                 64                      FarmHash Hash64WithSeed (NA version)",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 44.8,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/FarmHash-64.NA/M2Pro",
        "host": "M2Pro",
        "registered_name": "FarmHash-64.NA",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "FarmHash-64.NA                 64                      FarmHash Hash64WithSeed (NA version)"
      }
    },
    "anchor": "appendix-farmhash64",
    "label": "FarmHash64 NA",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. ",
    "official_claim": {
      "text": "not suitable for cryptography",
      "url": "https://github.com/google/farmhash/blob/master/README.md",
      "context": "Explicit noncryptographic disclaimer.",
      "provenance": "data/claims.json#/result/table/rows/9/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 205,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/9",
      "failed_tests": [
        "SeedZeroes",
        "SeedSparse",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/9/smhasher3_verdict"
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Public compression collides before either secret seed is applied."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      8,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "EXTENSION",
    "classification_reason": "Peters 2024 FarmHash attack reproduced; selected witness shortens the published construction to 8 bytes.",
    "mechanism_family": "Murmur/City/Farm lineage (multiply–xorshift mixing)",
    "rurban_alias": "FarmHash",
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit Hash64WithSeed seed; all-seed result also holds for Hash64WithSeeds.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "gx",
    "name": "gxhash-64 v3.5.0",
    "version": "v3.5.0",
    "family": "heuristic",
    "bits": 1.0,
    "bits_kind": "exact",
    "mechanism": "A cross-length public-compression collision survives every seed.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 0.67,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/gxhash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "gxhash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "gxhash-64                      64  g+portable          GxHash, lower 64 bits (ported from Rust)",
        "backend_note": "g+portable"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 18.28,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/gxhash-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "gxhash-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "gxhash-64                      64  sse2+aesni          GxHash, lower 64 bits (ported from Rust)",
        "backend_note": "sse2+aesni"
      },
      "smh_m2_small_cycles": {
        "value": 117.12,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/gxhash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "gxhash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "gxhash-64                      64  g+portable          GxHash, lower 64 bits (ported from Rust)"
      }
    },
    "anchor": "appendix-gxhash",
    "label": "gxhash-64",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. 15 versus 16 bytes. Equal-length 24-byte witness gives 1.585 bits; both verified.",
    "official_claim": {
      "text": "seeded (with seed randomization) to improve DOS resistance",
      "url": "https://github.com/ogxd/gxhash/blob/main/README.md",
      "context": "Same paragraph calls these basic safeguards and disclaims security against all attacks.",
      "provenance": "data/claims.json#/result/table/rows/11/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 226,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/11",
      "failed_tests": [
        "Sparse",
        "TwoBytes",
        "SeedZeroes",
        "SeedSparse",
        "Seed"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/11/smhasher3_verdict"
    },
    "score": {
      "calculated": 1.0,
      "display": 1.0,
      "display_text": "≤ 1",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(2) - (0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "A cross-length public-compression collision survives every seed.",
      "quote": "seeded … to improve DOS resistance"
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      15,
      16
    ],
    "pair_length_words": 2,
    "key_free": true,
    "status": "EXTENSION",
    "classification_reason": "Peters, gxhash issue #83 (2024), and purplesyringa’s seed-after-compression observation; shorter and cross-length witnesses.",
    "mechanism_family": "AES-round mixing",
    "rurban_alias": null,
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Uniform 64-bit seed; public-compression collision also defeats full 128-bit output.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "murmur",
    "name": "MurmurHash3 x64_128",
    "version": "x64_128",
    "family": "heuristic",
    "bits": 1.584962500721156,
    "bits_kind": "exact",
    "mechanism": "Invertible public word mixing injects a cancelling state difference for every seed.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 1.73,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/MurmurHash3-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "MurmurHash3-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "MurmurHash3-128               128                      MurmurHash v3, 128-bit version using 64-bit variables",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 2.73,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/MurmurHash3-128/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "MurmurHash3-128",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "MurmurHash3-128               128                      MurmurHash v3, 128-bit version using 64-bit variables",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 44.37,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/MurmurHash3-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "MurmurHash3-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "MurmurHash3-128               128                      MurmurHash v3, 128-bit version using 64-bit variables"
      }
    },
    "anchor": "appendix-murmurhash3",
    "label": "MurmurHash3 x64_128",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. ",
    "official_claim": {
      "text": "should be darn near bulletproof",
      "url": "https://github.com/aappleby/smhasher/wiki/MurmurHash",
      "context": "The author separately disclaims resistance to intentional collision generation (issue #73).",
      "provenance": "data/claims.json#/result/table/rows/10/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 163,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/10",
      "failed_tests": [
        "BIC",
        "Zeroes",
        "Sparse",
        "Permutation",
        "PerlinNoise",
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "SeedBlockOffset",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/10/smhasher3_verdict"
    },
    "score": {
      "calculated": 1.584962500721156,
      "display": 1.59,
      "display_text": "≤ 1.59",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(3) - (0)"
    },
    "key_bits": 32,
    "output_bits": 128,
    "hover": {
      "mechanism": "Invertible public word mixing injects a cancelling state difference for every seed."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      24,
      24
    ],
    "pair_length_words": 3,
    "key_free": true,
    "status": "EXTENSION",
    "classification_reason": "REPRODUCTION of the Aumasson–Bernstein–Bosslet 2012 top-bit mechanism; EXTENSION to a 24-byte x64_128 pair. Peters 2024 x86_32 reproduction is separate.",
    "mechanism_family": "Murmur/City/Farm lineage (multiply–xorshift mixing)",
    "rurban_alias": null,
    "claim_category": "statistical quality only",
    "key_model": "Uniform 32-bit API seed; all 2^32 seeds enumerated by the supplied panel.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "muse",
    "name": "MuseAir v0.3",
    "version": "v0.3",
    "family": "heuristic",
    "bits": 1.584962500721156,
    "bits_kind": "exact",
    "mechanism": "A zero public product erases a changed message word before secret mixing.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 9.41,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/MuseAir/M2Pro",
        "host": "M2Pro",
        "registered_name": "MuseAir",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "MuseAir                        64                      MuseAir v0.3",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 7.82,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/MuseAir/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "MuseAir",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "MuseAir                        64                      MuseAir v0.3",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 26.12,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/MuseAir/M2Pro",
        "host": "M2Pro",
        "registered_name": "MuseAir",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "MuseAir                        64                      MuseAir v0.3"
      }
    },
    "anchor": "appendix-museair",
    "label": "MuseAir",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. SMHasher3 version only. Supplied upstream audit records algorithm v2 / crate 0.6.0 and deprecated earlier versions.",
    "official_claim": {
      "text": "MuseAir is not affected by seed-independent attacks.",
      "url": "https://github.com/eternal-io/museair/blob/v0.3/README.md",
      "context": "Historical v0.3 claim, withdrawn by the upstream v2 update. (withdrawn by the author; deprecated v0.3 tested)",
      "provenance": "data/claims.json#/result/table/rows/6/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "PASS",
      "passed": 250,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/6",
      "failed_tests": [],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/6/smhasher3_verdict"
    },
    "score": {
      "calculated": 1.584962500721156,
      "display": 1.59,
      "display_text": "≤ 1.59",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(3) - (0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "A zero public product erases a changed message word before secret mixing.",
      "quote": "not affected by seed-independent attacks.",
      "claim_note": "v0.3; withdrawn"
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      17,
      17
    ],
    "pair_length_words": 3,
    "key_free": true,
    "status": "EXTENSION",
    "classification_reason": "Peters issue #3 (2026-07-12) predates these experiments. Earlier panel NEW label superseded by claims.json; 17-byte witness is an extension.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": null,
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Uniform 64-bit seed. Pair collides on both full 128-bit variants and both 64-bit variants.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "komi",
    "name": "komihash v5.27 / v5.34",
    "version": "v5.27 / v5.34",
    "family": "heuristic",
    "bits": 3.13512916189466,
    "bits_kind": "measured",
    "mechanism": "Related lanes let chosen message differences cancel for most seeds.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 8.14,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/komihash/M2Pro",
        "host": "M2Pro",
        "registered_name": "komihash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "komihash                       64                      komihash v5.27",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 7.35,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/komihash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "komihash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "komihash                       64                      komihash v5.27",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 24.5,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/komihash/M2Pro",
        "host": "M2Pro",
        "registered_name": "komihash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "komihash                       64                      komihash v5.27"
      }
    },
    "anchor": "appendix-komihash",
    "label": "komihash",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. This high-probability construction applies to lengths 64..127 bytes only; not an all-length probability.",
    "official_claim": {
      "text": "only with a secret seed to minimize the risk of collision attacks (hash flooding)",
      "url": "https://github.com/avaneev/komihash/blob/main/README.md",
      "context": "The README explicitly says it is not cryptographically secure.",
      "provenance": "data/claims.json#/result/table/rows/4/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "PASS",
      "passed": 250,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/4",
      "failed_tests": [],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/4/smhasher3_verdict"
    },
    "score": {
      "calculated": 3.13512916189466,
      "display": 3.14,
      "display_text": "≈ 3.14*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest",
      "derivation": "log2(8) - (-0.13512916189466012)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Related lanes let chosen message differences cancel for most seeds.",
      "quote": "only with a secret seed … collision attacks …"
    },
    "chart_eligible": true,
    "collision": {
      "display": "0.9106 measured",
      "log2_contribution": -0.13512916189466012,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured rate or class-density-weighted measured rate",
      "count": 3910946997,
      "trials": 4294967296
    },
    "speed_note": "SMHasher3 registers v5.27; the collision mechanism and pair were also checked on v5.34.",
    "pair_lengths_bytes": [
      64,
      64
    ],
    "pair_length_words": 8,
    "key_free": false,
    "status": "NEW",
    "classification_reason": "No earlier lane-tie construction identified in the supplied evidence.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "komihash",
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Uniform 64-bit UseSeed.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "spooky",
    "name": "SpookyHash V2-64",
    "version": "V2-64",
    "family": "heuristic",
    "bits": 6.129270753827899,
    "bits_kind": "measured",
    "mechanism": "The step-10 late-injection trail cancels in the 768-bit state before finalization; 275-byte messages give L=35.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 4.13,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/SpookyHash2-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "SpookyHash2-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "SpookyHash2-64                 64                      SpookyHash v2, 64-bit result",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 5.08,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/SpookyHash2-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "SpookyHash2-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "SpookyHash2-64                 64                      SpookyHash v2, 64-bit result",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 52.31,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/SpookyHash2-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "SpookyHash2-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "SpookyHash2-64                 64                      SpookyHash v2, 64-bit result"
      }
    },
    "anchor": "appendix-spookyhash",
    "label": "SpookyHash V2-64",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. The same pair collides on SpookyHash2-32 at the same measured rate, and on both 64-bit halves. SMHasher3 duplicates a 64-bit seed; native Hash32 takes a 32-bit seed. Exact half balance is unproved. The claimed 2^32 and independent-seed-word runs were not independently reproduced.",
    "official_claim": {
      "text": "When NOT to use it: if you have an opponent.",
      "url": "https://burtleburtle.net/bob/hash/spooky.html",
      "context": "An explicit warning against adversarial use, not a promise of security.",
      "provenance": "data/claims.json#/result/table/rows/13/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 244,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/13",
      "failed_tests": [
        "SeedBIC",
        "SeedBitflip"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/13/smhasher3_verdict"
    },
    "score": {
      "calculated": 6.129270753827899,
      "display": 6.13,
      "display_text": "≈ 6.13*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest",
      "derivation": "log2(35) - (-0.9999877368829325)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The step-10 late-injection trail cancels in the 768-bit state before finalization; 275-byte messages give L=35."
    },
    "chart_eligible": true,
    "collision": {
      "display": "about half the seeds; verifier 536875475/1073741823",
      "count": 536875475,
      "trials": 1073741823,
      "log2_contribution": -0.9999877368829325,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured full-state collisions in the duplicated 64-bit seed model; exact population balance unproved"
    },
    "pair_lengths_bytes": [
      275,
      275
    ],
    "pair_length_words": 35,
    "key_free": false,
    "status": "NEW",
    "classification_reason": "New late-injection cancellation in the supplied panel.",
    "mechanism_family": "ARX lanes (add–rotate–xor)",
    "rurban_alias": "Spooky32",
    "claim_category": "statistical quality only",
    "key_model": "Hash64 duplicates one uniform seed into both native seed words. Similar rate measured with independent 128-bit native seeds.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "ahash",
    "name": "aHash 0.8.12, AES path",
    "version": "0.8.12, AES path",
    "family": "heuristic",
    "bits": 22.454208096559523,
    "bits_kind": "measured",
    "mechanism": "An inverse-AES differential cancels in both the AES and shuffled-addition accumulators.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 0.67,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/rust-ahash/M2Pro",
        "host": "M2Pro",
        "registered_name": "rust-ahash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "rust-ahash                     64    portable          aHash (ported from Rust, AES-based version)",
        "backend_note": "portable C++ port"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 0.81,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/rust-ahash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "rust-ahash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "rust-ahash                     64       ssse3          aHash (ported from Rust, AES-based version)",
        "backend_note": "ssse3 C++ port"
      },
      "smh_m2_small_cycles": {
        "value": 146.39,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/rust-ahash/M2Pro",
        "host": "M2Pro",
        "registered_name": "rust-ahash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "rust-ahash                     64    portable          aHash (ported from Rust, AES-based version)"
      }
    },
    "anchor": "appendix-ahash",
    "label": "aHash AES",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. Score uses verifier pool 2616/2^31 in heur_panel synthesis. Original 1227/2^30 and second family 75/2^26 are compatible sampling estimates. Approximate 95% sampling half-width: ±0.06 bits.",
    "official_claim": {
      "text": "designed to prevent an adversary that does not know the key from being able to create hash collisions or partial collisions.",
      "url": "https://github.com/tkaitchuck/aHash/blob/master/FAQ.md",
      "context": "The project disclaims cryptographic security and limits intended use to in-memory hashmaps.",
      "provenance": "data/claims.json#/result/table/rows/12/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 242,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/12",
      "failed_tests": [
        "Sparse",
        "TwoBytes"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/12/smhasher3_verdict"
    },
    "score": {
      "calculated": 22.454208096559523,
      "display": 22.45,
      "display_text": "≈ 22.45*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest",
      "derivation": "log2(7) - (-19.646853174501917)",
      "approximate_95_half_width_bits": 0.06
    },
    "key_bits": 256,
    "output_bits": 64,
    "hover": {
      "mechanism": "An inverse-AES differential cancels in both the AES and shuffled-addition accumulators.",
      "quote": "prevent an adversary … create hash collisions …"
    },
    "chart_eligible": true,
    "collision": {
      "display": "about 2^-19.7 measured",
      "log2_contribution": -19.646853174501917,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured rate or class-density-weighted measured rate",
      "count": 2616,
      "trials": 2147483648
    },
    "pair_lengths_bytes": [
      56,
      56
    ],
    "pair_length_words": 7,
    "key_free": false,
    "status": "NEW",
    "classification_reason": "New inverse-AES and shuffled-sum trail in the supplied panel.",
    "mechanism_family": "AES-round mixing",
    "rurban_alias": "ahash64",
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Four independent uniform internal RandomState words in the validated C reproduction of the specified aHash 0.8.12 AES path; native Rust transfer is conditional on the operation and constructor mapping.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "t1ha",
    "name": "t1ha2_atonce-64 v2.1.4",
    "version": "v2.1.4",
    "family": "heuristic",
    "bits": 31.0,
    "bits_kind": "measured",
    "mechanism": "Cross-length carry patterns cancel for a structured seed class.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 6.09,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/t1ha2-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "t1ha2-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "t1ha2-64                       64       1Y+a0          Fast Positive Hash #2 (portable, 64-bit core)",
        "backend_note": "1Y+a0"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 5.82,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/t1ha2-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "t1ha2-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "t1ha2-64                       64       1Y+a2          Fast Positive Hash #2 (portable, 64-bit core)",
        "backend_note": "1Y+a2"
      },
      "smh_m2_small_cycles": {
        "value": 40.35,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/t1ha2-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "t1ha2-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "t1ha2-64                       64       1Y+a0          Fast Positive Hash #2 (portable, 64-bit core)"
      }
    },
    "anchor": "appendix-t1ha2",
    "label": "t1ha2_atonce-64",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. Cross-length 16/11-byte pair. Exact class density 2^-26 multiplied by measured conditional rate about 2^-4. Equal-length best claim not independently re-run; excluded.",
    "official_claim": {
      "text": "Great quality of hashing",
      "url": "https://raw.githubusercontent.com/erthink/t1ha/master/t1ha.h",
      "context": "The project explicitly says it is not suitable for cryptography.",
      "provenance": "data/claims.json#/result/table/rows/7/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 247,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/7",
      "failed_tests": [
        "SeedBlockOffset"
      ],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/7/smhasher3_verdict"
    },
    "score": {
      "calculated": 31.0,
      "display": 31.0,
      "display_text": "≈ 31*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest",
      "derivation": "log2(2) - (-30)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Cross-length carry patterns cancel for a structured seed class."
    },
    "chart_eligible": true,
    "collision": {
      "display": "2^-26 × (262144/2^22) ≈ 2^-30, estimated class contribution",
      "log2_contribution": -30,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured rate or class-density-weighted measured rate",
      "conditional_count": 262144,
      "conditional_trials": 4194304,
      "class_density_log2": -26
    },
    "pair_lengths_bytes": [
      16,
      11
    ],
    "pair_length_words": 2,
    "key_free": false,
    "status": "NEW",
    "classification_reason": "New carry-pattern construction in supplied panel.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "t1ha2_atonce",
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Uniform 64-bit one-shot API seed.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "a5",
    "name": "a5hash v5.21, 64-bit",
    "version": "v5.21, 64-bit",
    "family": "heuristic",
    "bits": 47.80910046980944,
    "bits_kind": "exact",
    "mechanism": "An exactly counted seed class erases a message word through multiplication.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 3.07,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/a5hash/M2Pro",
        "host": "M2Pro",
        "registered_name": "a5hash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "a5hash                         64                      a5hash v5.21, 64-bit version",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 3.17,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/a5hash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "a5hash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "a5hash                         64                      a5hash v5.21, 64-bit version",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 23.04,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/a5hash/M2Pro",
        "host": "M2Pro",
        "registered_name": "a5hash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "a5hash                         64                      a5hash v5.21, 64-bit version"
      }
    },
    "anchor": "appendix-a5hash-64",
    "label": "a5hash 64-bit",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. Exact sufficient seed class, not equality for total epsilon. Best verified score upper bound, not an exhaustive minimum over lengths. v5.25 not vector-checked.",
    "official_claim": {
      "text": "practically resistant to blinding multiplication attacks",
      "url": "https://github.com/avaneev/a5hash/blob/main/README.md",
      "context": "v5.25 wording; conditioned on a secret 64-bit seed and non-exposed outputs, and explicitly not a complete security evaluation. Tested code is v5.21. (v5.25 wording; v5.21 tested)",
      "provenance": "data/claims.json#/result/table/rows/5/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "PASS",
      "passed": 250,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/5",
      "failed_tests": [],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/5/smhasher3_verdict"
    },
    "score": {
      "calculated": 47.80910046980944,
      "display": 47.81,
      "display_text": "≤ 47.81",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(827) - (-38.11735695063816)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "An exactly counted seed class erases a message word through multiplication.",
      "claim_note": "v5.25 wording"
    },
    "chart_eligible": true,
    "collision": {
      "display": "≥ 118 × 2^-45 ≈ 2^-38.12",
      "log2_contribution": -38.11735695063816,
      "exact_contribution": true,
      "total_equality_proved": false,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      6615,
      6615
    ],
    "pair_length_words": 827,
    "key_free": false,
    "status": "NEW",
    "classification_reason": "New heavy seed-expansion value / zero multiplier construction. Stronger deep-panel witness supersedes 48.3-bit panel value.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "a5hash",
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Uniform 64-bit UseSeed.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "a5wide",
    "name": "a5hash v5.21, 128-bit",
    "version": "v5.21, 128-bit",
    "family": "heuristic",
    "bits": 1.584962500721156,
    "bits_kind": "exact",
    "mechanism": "A public zero-product operand erases a changed word for every seed.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 11.55,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/a5hash-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "a5hash-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "a5hash-128                    128                      a5hash v5.21, 128-bit version",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 7.93,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/a5hash-128/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "a5hash-128",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "a5hash-128                    128                      a5hash v5.21, 128-bit version",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 22.48,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/a5hash-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "a5hash-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "a5hash-128                    128                      a5hash v5.21, 128-bit version"
      }
    },
    "anchor": "appendix-a5hash-128",
    "label": "a5hash 128-bit",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. Additional variant, not one of the original ten entries.",
    "official_claim": {
      "text": "practically resistant to blinding multiplication attacks",
      "url": "https://github.com/avaneev/a5hash/blob/main/README.md",
      "context": "Same v5.25 statement also names a5hash128(); tested implementation is v5.21. (v5.25 wording; v5.21 tested)",
      "provenance": "data/claims.json#/result/table/rows/5/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "PASS",
      "passed": 250,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/5",
      "failed_tests": [],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/5/smhasher3_verdict"
    },
    "score": {
      "calculated": 1.584962500721156,
      "display": 1.59,
      "display_text": "≤ 1.59",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(3) - (0)"
    },
    "key_bits": 64,
    "output_bits": 128,
    "hover": {
      "mechanism": "A public zero-product operand erases a changed word for every seed.",
      "claim_note": "v5.25 wording"
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Exact sufficient event"
    },
    "pair_lengths_bytes": [
      17,
      17
    ],
    "pair_length_words": 3,
    "key_free": true,
    "status": "NEW",
    "classification_reason": "New public-product construction; deep-panel confirmation upgrades the earlier claimant-only variant.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": null,
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Uniform 64-bit UseSeed; both full output words collide.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "highway",
    "name": "HighwayHash-64, frozen",
    "version": "frozen",
    "family": "heuristic",
    "bits": 59.8075787461754,
    "bits_kind": "exact",
    "mechanism": "A weak-key absorption trail merges the full state before any finalizer.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "post/data.json#/heuristics",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 0.65,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/HighwayHash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "HighwayHash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "HighwayHash-64                 64    portable  CRYPTO  HighwayHash, 64-bit version",
        "backend_note": "portable"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 3.24,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/HighwayHash-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "HighwayHash-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "HighwayHash-64                 64       sse41  CRYPTO  HighwayHash, 64-bit version",
        "backend_note": "sse41"
      },
      "smh_m2_small_cycles": {
        "value": 258.64,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/HighwayHash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "HighwayHash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "HighwayHash-64                 64    portable  CRYPTO  HighwayHash, 64-bit version"
      }
    },
    "anchor": "appendix-highwayhash",
    "label": "HighwayHash-64",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. Conservative 96-byte headline witness. Exact sufficient-event contribution, not exact total collision probability. All 64/128/256 output widths inherit it. Original second-family capped sample confirmed pair but not exact numerator.",
    "official_claim": {
      "text": "HighwayHash is a strong pseudorandom function with security claims",
      "url": "https://github.com/google/highwayhash/blob/f8381f3/highwayhash/highwayhash.h#L87",
      "context": "Header claim; arXiv:1612.06257 presents statistical analysis, speed measurements and preliminary cryptanalysis, conditional on withstanding further analysis. Section 6.3 states the combined event is at most 1 in 2^64.",
      "provenance": "data/claims.json#/result/table/rows/14/official_claim_quote",
      "locator": "highwayhash/highwayhash.h:87, commit f8381f3"
    },
    "smhasher3": {
      "verdict": "PASS",
      "passed": 238,
      "total": 238,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "provenance": "data/claims.json#/result/table/rows/14",
      "failed_tests": [],
      "failed_tests_provenance": "data/claims.json#/result/table/rows/14/smhasher3_verdict"
    },
    "score": {
      "calculated": 59.8075787461754,
      "display": 59.81,
      "display_text": "≤ 59.81",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(12) - (-56.22261624545425)"
    },
    "key_bits": 256,
    "output_bits": 64,
    "hover": {
      "mechanism": "A weak-key absorption trail merges the full state before any finalizer.",
      "quote": "a strong pseudorandom function …"
    },
    "chart_eligible": true,
    "collision": {
      "display": "class contribution 56165/2^72 ≈ 2^-56.22",
      "log2_contribution": -56.22261624545425,
      "exact_contribution": true,
      "total_equality_proved": false,
      "interpretation": "Exact sufficient event",
      "exact_fraction": {
        "numerator": 56165,
        "denominator_power_of_two": 72
      }
    },
    "pair_lengths_bytes": [
      96,
      96
    ],
    "pair_length_words": 12,
    "key_free": false,
    "status": "EXTENSION",
    "classification_reason": "Extension of the designers’ cancellation analysis in arXiv:1612.06257 §6.3; its at-most-1-in-2^64 commitment is present in the paper.",
    "mechanism_family": "SIMD lane multiply (Google)",
    "rurban_alias": null,
    "claim_category": "keyed collision / DoS claim",
    "key_model": "Full uniform 256-bit HHKey, four independent 64-bit words.",
    "domain_short": "variable-length byte strings; selected witness only"
  },
  {
    "id": "wyhash",
    "name": "wyhash final v4.3",
    "version": "final v4.3",
    "family": "heuristic",
    "bits": 28.830074998557688,
    "bits_kind": "measured",
    "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/0",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/0",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching version: speed registration is wyhash v4.2; collision row is final v4.3.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching version: speed registration is wyhash v4.2; collision row is final v4.3.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching version: speed registration is wyhash v4.2; collision row is final v4.3.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "appendix-wyhash",
    "label": "wyhash final v4.3",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. Default public secret and a random 64-bit API seed; no secret-word randomization. The SMHasher3 row is non-strict v4.2: claims.json records the same wyhash() body as final v4.3, not identity of every historical header. Exact Poisson 95% interval for this witness-derived score: [27.9, 30.0].",
    "official_claim": {
      "text": "Both of them are not 64 bit collision resistant, but is about 62 bits (flyingmutant/Cyan4973/vigna)",
      "url": "https://github.com/wangyi-fudan/wyhash/blob/master/README.md",
      "context": "Default public secret and a random 64-bit API seed; no secret-word randomization. The SMHasher3 row is non-strict v4.2: claims.json records the same wyhash() body as final v4.3, not identity of every historical header.",
      "provenance": "records/claims.json#/result/table/rows/0/official_claim_quote",
      "verbatim_record": "**solid**:  wyhash passed SMHasher, wyrand passed BigCrush, practrand. | **salted**: We use dynamic secret to avoid intended attack. | Both of them are not 64 bit collision resistant, but is about 62 bits (flyingmutant/Cyan4973/vigna) | 1: wyhash is designed to be as fast as possible, not for security. in my basic operator _wymum(A^p1,B^p2), if A is set to p1 then wyhash is not dependent on B. However, this occurs with 2^-64 probability in natural data which can be almost ignored. When being intendly attacked wyhash surely break as you said (by setting some parameter to magic numbers).",
      "source_record": "README.md lines 20, 28, 45 (commit e4764a0b, 2026-03-23), https://github.com/wangyi-fudan/wyhash/blob/master/README.md; issue #15 owner comment 2019-06-09T20:01:27Z, https://github.com/wangyi-fudan/wyhash/issues/15#issuecomment-500240721"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 235,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "provenance": "records/claims.json#/result/table/rows/0",
      "failed_tests": [
        "Zeroes",
        "Permutation",
        "SeedZeroes"
      ],
      "version_note": "v4.2 registration; collision witness is v4.3"
    },
    "score": {
      "calculated": 28.830074998557688,
      "display": 28.8,
      "display_text": "≈ 28.8*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest tenth; sampling uncertainty",
      "derivation": "log2(4) - (-26.830074998557688)",
      "poisson_95_interval": [
        27.9,
        30.0
      ],
      "poisson_count": 9,
      "poisson_trials": 1073741824,
      "interval_scope": "Two-sided exact Poisson interval transformed to log2(L/p); uncertainty in this witness-derived quantity, not an interval for the unknown family score."
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
      "claim_note": "REPRODUCTION; verbatim excerpt"
    },
    "chart_eligible": true,
    "collision": {
      "display": "9/2^30 measured",
      "count": 9,
      "trials": 1073741824,
      "trials_log2": 30,
      "log2_contribution": -26.830074998557688,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured full-output collision rate"
    },
    "speed_note": "SMHasher3 v4.2 timings are omitted from the v4.3 collision row.",
    "pair_lengths_bytes": [
      32,
      32
    ],
    "pair_length_words": 4,
    "key_free": false,
    "status": "REPRODUCTION",
    "classification_reason": "Independent reproduction of the paper’s harness results.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "wyhash",
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit API seed (sampled reproducibly with xoshiro256**); fixed default public secret/constants",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied; startup witness and deterministic smoke check recorded",
      "directory": "verify/wyhash",
      "source": "verify/wyhash/wyhash_verify.c",
      "command": "cc -O2 -std=c11 -o wyhash_verify wyhash_verify.c -lm && ./wyhash_verify 20",
      "trials": 1048576,
      "rng": "fixed deterministic pseudorandom smoke stream; see per-hash README",
      "note": "Completed dependency additions in verify/README.md. The smoke check does not replace historical large counts."
    }
  },
  {
    "id": "rapid1",
    "name": "rapidhash v1",
    "version": "v1",
    "family": "heuristic",
    "bits": 28.415037499278842,
    "bits_kind": "measured",
    "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/3",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/3",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "rapidhash v1 is not registered; rapidhash denotes v3 in this benchmark.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "rapidhash v1 is not registered; rapidhash denotes v3 in this benchmark.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "rapidhash v1 is not registered; rapidhash denotes v3 in this benchmark.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "appendix-rapidhash-v1",
    "label": "rapidhash v1",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. SMHasher3 verdicts and speeds in the supplied record apply to v3, not v1. No v1 SMHasher3 speed is supplied. The author quotation is from the v1-era issue #10; the full claim record also contains later v3 wording. Exact Poisson 95% interval for this witness-derived score: [27.6, 29.4].",
    "official_claim": {
      "text": "Rapidhash focuses on quality and speed, not security so for now it will be kept as it is.",
      "url": "https://github.com/Nicoshev/rapidhash/issues/10",
      "context": "SMHasher3 verdicts and speeds in the supplied record apply to v3, not v1. No v1 SMHasher3 speed is supplied. The author quotation is from the v1-era issue #10; the full claim record also contains later v3 wording.",
      "provenance": "records/claims.json#/result/table/rows/1/official_claim_quote",
      "verbatim_record": "All functions pass all tests in both [SMHasher](https://github.com/rurban/smhasher/blob/master/doc/rapidhash.txt) and [SMHasher3](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/raw/rapidhash.txt).  \n[Collision-based study](https://github.com/Nicoshev/rapidhash/tree/master?tab=readme-ov-file#collision-based-hash-quality-study) showed a collision probability close to ideal.  \nOutstanding collision ratio when tested with datasets of 16B and 67B keys: | Altering the secrets would reduce the hash quality, as they have been chosen for performing 64-bit multiplications.\n\nRapidhash focuses on quality and speed, not security so for now it will be kept as it is. | Hey @hoxxep, this is a non-cryptographic function, it is not our intent to cover specific attacks.\n\nJust released V3, which hardens it a bit against this attack. Now the input length must be known, appending the secret may not work anymore.",
      "source_record": "README.md (master 1ae7842f) section '**Excellent**' lines 35-37, https://github.com/Nicoshev/rapidhash/blob/master/README.md; issue #10 closing comment by Nicoshev 2024-12-02, https://github.com/Nicoshev/rapidhash/issues/10; issue #25 closing comment 2025-05-27, https://github.com/Nicoshev/rapidhash/issues/25"
    },
    "smhasher3": {
      "verdict": "not supplied for v1; supplied record tests v3",
      "passed": null,
      "total": null,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "provenance": "records/claims.json#/result/table/rows/1",
      "failed_tests": []
    },
    "score": {
      "calculated": 28.415037499278842,
      "display": 28.4,
      "display_text": "≈ 28.4*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest tenth; sampling uncertainty",
      "derivation": "log2(4) - (-26.415037499278842)",
      "poisson_95_interval": [
        27.6,
        29.4
      ],
      "poisson_count": 12,
      "poisson_trials": 1073741824,
      "interval_scope": "Two-sided exact Poisson interval transformed to log2(L/p); uncertainty in this witness-derived quantity, not an interval for the unknown family score."
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
      "claim_note": "REPRODUCTION; v1-era author comment"
    },
    "chart_eligible": true,
    "collision": {
      "display": "12/2^30 measured",
      "count": 12,
      "trials": 1073741824,
      "trials_log2": 30,
      "log2_contribution": -26.415037499278842,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured full-output collision rate"
    },
    "pair_lengths_bytes": [
      32,
      32
    ],
    "pair_length_words": 4,
    "key_free": false,
    "status": "REPRODUCTION",
    "classification_reason": "Independent reproduction of the paper’s harness results.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "rapidhash",
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit API seed (sampled reproducibly with xoshiro256**); fixed default public secret/constants",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied; startup witness and deterministic smoke check recorded",
      "directory": "verify/rapidhash-v1",
      "source": "verify/rapidhash-v1/rapidhash_v1_verify.c",
      "command": "cc -O2 -std=c11 -o rapidhash_v1_verify rapidhash_v1_verify.c -lm && ./rapidhash_v1_verify 20",
      "trials": 1048576,
      "rng": "fixed deterministic pseudorandom smoke stream; see per-hash README",
      "note": "Completed dependency additions in verify/README.md. The smoke check does not replace historical large counts."
    }
  },
  {
    "id": "rapid3",
    "name": "rapidhash v3",
    "version": "v3",
    "family": "heuristic",
    "bits": 28.540568381362704,
    "bits_kind": "measured",
    "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/5",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/5",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 15.35,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/rapidhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "rapidhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "rapidhash                      64                      rapidhash v3, 64-bit",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 10.67,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/rapidhash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "rapidhash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "rapidhash                      64                      rapidhash v3, 64-bit",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 20.68,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/rapidhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "rapidhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "rapidhash                      64                      rapidhash v3, 64-bit"
      }
    },
    "anchor": "appendix-rapidhash-v3",
    "label": "rapidhash v3",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. SMHasher3 tests v3. The first-fold flaw persists in standard, -micro and -nano: each has 11/2^30 at both 32 B and 48 B. The 28.54-bit plotted cap uses L=4 at 32 B; the 48-byte score is 29.125530882083858. Timings shown are for standard v3; protected variants were not measured by this reproduction. Exact Poisson 95% interval for this witness-derived score: [27.7, 29.5].",
    "official_claim": {
      "text": "showed a collision probability close to ideal.",
      "url": "https://github.com/Nicoshev/rapidhash/blob/master/README.md",
      "context": "SMHasher3 tests v3. The first-fold flaw persists in standard, -micro and -nano: each has 11/2^30 at both 32 B and 48 B. The 28.54-bit plotted cap uses L=4 at 32 B; the 48-byte score is 29.125530882083858. Timings shown are for standard v3; protected variants were not measured by this reproduction.",
      "provenance": "records/claims.json#/result/table/rows/1/official_claim_quote",
      "verbatim_record": "All functions pass all tests in both [SMHasher](https://github.com/rurban/smhasher/blob/master/doc/rapidhash.txt) and [SMHasher3](https://gitlab.com/fwojcik/smhasher3/-/blob/main/results/raw/rapidhash.txt).  \n[Collision-based study](https://github.com/Nicoshev/rapidhash/tree/master?tab=readme-ov-file#collision-based-hash-quality-study) showed a collision probability close to ideal.  \nOutstanding collision ratio when tested with datasets of 16B and 67B keys: | Altering the secrets would reduce the hash quality, as they have been chosen for performing 64-bit multiplications.\n\nRapidhash focuses on quality and speed, not security so for now it will be kept as it is. | Hey @hoxxep, this is a non-cryptographic function, it is not our intent to cover specific attacks.\n\nJust released V3, which hardens it a bit against this attack. Now the input length must be known, appending the secret may not work anymore.",
      "source_record": "README.md (master 1ae7842f) section '**Excellent**' lines 35-37, https://github.com/Nicoshev/rapidhash/blob/master/README.md; issue #10 closing comment by Nicoshev 2024-12-02, https://github.com/Nicoshev/rapidhash/issues/10; issue #25 closing comment 2025-05-27, https://github.com/Nicoshev/rapidhash/issues/25"
    },
    "smhasher3": {
      "verdict": "PASS",
      "passed": 250,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "provenance": "records/claims.json#/result/table/rows/1",
      "failed_tests": []
    },
    "score": {
      "calculated": 28.540568381362704,
      "display": 28.5,
      "display_text": "≈ 28.5*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest tenth; sampling uncertainty",
      "derivation": "log2(4) - (-26.540568381362704)",
      "poisson_95_interval": [
        27.7,
        29.5
      ],
      "poisson_count": 11,
      "poisson_trials": 1073741824,
      "interval_scope": "Two-sided exact Poisson interval transformed to log2(L/p); uncertainty in this witness-derived quantity, not an interval for the unknown family score."
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
      "claim_note": "REPRODUCTION; verbatim excerpt"
    },
    "chart_eligible": true,
    "collision": {
      "display": "11/2^30 measured",
      "count": 11,
      "trials": 1073741824,
      "trials_log2": 30,
      "log2_contribution": -26.540568381362704,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured full-output collision rate"
    },
    "pair_lengths_bytes": [
      32,
      32
    ],
    "pair_length_words": 4,
    "key_free": false,
    "status": "REPRODUCTION",
    "classification_reason": "Independent reproduction of the paper’s harness results.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "rapidhash",
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit API seed (sampled reproducibly with xoshiro256**); fixed default public secret/constants",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/rapidhash-v3",
      "source": "verify/rapidhash-v3/rapidhash_v3_verify.c",
      "command": "cc -O2 -std=c11 -o rapidhash_v3_verify rapidhash_v3_verify.c -lm && ./rapidhash_v3_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "xxh3-64",
    "name": "XXH3-64 0.8.3",
    "version": "0.8.3",
    "family": "heuristic",
    "bits": 24.991571377929418,
    "bits_kind": "measured",
    "mechanism": "Complementing the first two words can preserve their high/low multiplication fold. The seven unchanged folds cancel from the equality test; the avalanche is a permutation.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/14",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/14",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 7.15,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/XXH3-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "XXH3-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "XXH3-64                        64      scalar          xxh3, 64-bit version",
        "backend_note": "scalar"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 19.69,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/XXH3-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "XXH3-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "XXH3-64                        64      avx512          xxh3, 64-bit version",
        "backend_note": "avx512"
      },
      "smh_m2_small_cycles": {
        "value": 25.3,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/XXH3-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "XXH3-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "XXH3-64                        64      scalar          xxh3, 64-bit version"
      }
    },
    "anchor": "appendix-xxh3-64",
    "label": "XXH3-64 0.8.3",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. The rate depends on the first two words. Changing bytes 16 onward changes the output, not the colliding-seed set. No practical flooding attack is established by this approximately 2^-21 fixed-pair rate. Approximate 95% sampling half-width: ±0.09 bits. This measurement establishes no rate or guarantee for independently randomized custom-secret APIs.",
    "official_claim": {
      "text": "xxHash is primarily designed for speed. It is labeled non-cryptographic, and is not meant to avoid intentional collisions (same digest for 2 different messages), or to prevent producing a message with a predefined digest.",
      "url": "https://github.com/Cyan4973/xxHash/blob/v0.8.3/doc/xxhash_spec.md",
      "context": "Uses the explicit 128-byte base-1143 pair from records/xxh3_128b_pairs.json; two fresh confirmation runs total 1030/2^31. The older 32-byte pair remains a secondary verifier case.",
      "provenance": "records/claims.json#/result/table/rows/2/official_claim_quote",
      "verbatim_record": "xxHash is primarily designed for speed. It is labeled non-cryptographic, and is not meant to avoid intentional collisions (same digest for 2 different messages), or to prevent producing a message with a predefined digest. | It's possible to provide any blob of bytes as a \"secret\" to generate the hash. This makes it more difficult for an external actor to prepare an intentional collision. | DISCLAIMER: There are known *seed-dependent* multicollisions here due to multiplication by zero, affecting hashes of lengths 17 to 240. However, they are very unlikely. Keep this in mind when using the unseeded XXH3_64bits() variant: As with all unseeded non-cryptographic hashes, it does not attempt to defend itself against specially crafted inputs, only random inputs. | Finally, xxHash provides its own [massive collision tester](https://github.com/Cyan4973/xxHash/tree/dev/tests/collisions), able to generate and compare billions of hashes to test the limits of 64-bit hash algorithms. On this front too, xxHash features good results, in line with the [birthday paradox].",
      "source_record": "doc/xxhash_spec.md (v0.8.3) line 40; xxhash.h (v0.8.3) lines 1208-1209 and 4707-4714 (comment prefix stripped; SMHasher3's copy at hashes/xxhash.cpp L591-598 is reflowed and reads 'multiplication by 0'); README.md (v0.8.3) lines 85-88. https://github.com/Cyan4973/xxHash/blob/v0.8.3/"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 223,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "provenance": "records/claims.json#/result/table/rows/2",
      "failed_tests": [
        "BIC",
        "Sparse",
        "PerlinNoise",
        "Bitflip",
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "SeedBlockOffset",
        "SeedBIC"
      ]
    },
    "score": {
      "calculated": 24.991571377929418,
      "display": 24.99,
      "display_text": "≈ 24.99*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest",
      "derivation": "log2(16) - (-20.991571377929418)",
      "approximate_95_half_width_bits": 0.09
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Complementing the first two words can preserve their high/low multiplication fold. The seven unchanged folds cancel from the equality test; the avalanche is a permutation."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1030/2^31 measured (504 + 526 in two fresh 2^30 runs)",
      "count": 1030,
      "trials": 2147483648,
      "log2_contribution": -20.991571377929418,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Pooled confirmation estimate; excludes screening, refinement and repeated-seed controls."
    },
    "pair_lengths_bytes": [
      128,
      128
    ],
    "pair_length_words": 16,
    "key_free": false,
    "status": "REPRODUCTION",
    "classification_reason": "Reproduces the paper fold differential with a newly generated 128-byte base and two fresh confirmation streams.",
    "mechanism_family": "NH-like stripe accumulators",
    "rurban_alias": "xxh3low",
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit API seed (sampled reproducibly with xoshiro256**); fixed default public secret/constants",
    "domain_short": "variable-length byte strings; selected witness only",
    "api": "XXH3_64bits_withSeed; default secret public; uniform 64-bit seed",
    "reproduction": {
      "status": "standalone program supplied; startup witness and deterministic smoke check recorded",
      "directory": "verify/xxh3-64",
      "source": "verify/xxh3-64/xxh3_64_verify.c",
      "command": "cc -O2 -std=c11 -o xxh3_64_verify xxh3_64_verify.c -lm && ./xxh3_64_verify 20",
      "trials": 1048576,
      "rng": "fixed deterministic pseudorandom smoke stream; see per-hash README",
      "note": "Completed dependency additions in verify/README.md. The smoke check does not replace historical large counts."
    }
  },
  {
    "id": "xxh3-128",
    "name": "XXH3-128 0.8.3",
    "version": "0.8.3",
    "family": "heuristic",
    "bits": 29.67807190511264,
    "bits_kind": "measured",
    "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/15",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/15",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 7.16,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/XXH3-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "XXH3-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "XXH3-128                      128      scalar          xxh3, 128-bit version",
        "backend_note": "scalar"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 19.82,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/XXH3-128/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "XXH3-128",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "XXH3-128                      128      avx512          xxh3, 128-bit version",
        "backend_note": "avx512"
      },
      "smh_m2_small_cycles": {
        "value": 30.97,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/XXH3-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "XXH3-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "XXH3-128                      128      scalar          xxh3, 128-bit version"
      }
    },
    "anchor": "appendix-xxh3-128",
    "label": "XXH3-128 0.8.3",
    "qualification": "Measured score cap with sampling uncertainty; better pairs may exist. Pair F sets w1=~w0 before complementing both words, preserving their sum. Both low64 and high64 are compared; this is a full 128-bit collision. Exact Poisson 95% interval for this witness-derived score: [28.5, 31.3]. This measurement establishes no rate or guarantee for independently randomized custom-secret APIs.",
    "official_claim": {
      "text": "xxHash is primarily designed for speed. It is labeled non-cryptographic, and is not meant to avoid intentional collisions (same digest for 2 different messages), or to prevent producing a message with a predefined digest.",
      "url": "https://github.com/Cyan4973/xxHash/blob/v0.8.3/doc/xxhash_spec.md",
      "context": "Pair F sets w1=~w0 before complementing both words, preserving their sum. Both low64 and high64 are compared; this is a full 128-bit collision.",
      "provenance": "records/claims.json#/result/table/rows/2/official_claim_quote",
      "verbatim_record": "xxHash is primarily designed for speed. It is labeled non-cryptographic, and is not meant to avoid intentional collisions (same digest for 2 different messages), or to prevent producing a message with a predefined digest. | It's possible to provide any blob of bytes as a \"secret\" to generate the hash. This makes it more difficult for an external actor to prepare an intentional collision. | DISCLAIMER: There are known *seed-dependent* multicollisions here due to multiplication by zero, affecting hashes of lengths 17 to 240. However, they are very unlikely. Keep this in mind when using the unseeded XXH3_64bits() variant: As with all unseeded non-cryptographic hashes, it does not attempt to defend itself against specially crafted inputs, only random inputs. | Finally, xxHash provides its own [massive collision tester](https://github.com/Cyan4973/xxHash/tree/dev/tests/collisions), able to generate and compare billions of hashes to test the limits of 64-bit hash algorithms. On this front too, xxHash features good results, in line with the [birthday paradox].",
      "source_record": "doc/xxhash_spec.md (v0.8.3) line 40; xxhash.h (v0.8.3) lines 1208-1209 and 4707-4714 (comment prefix stripped; SMHasher3's copy at hashes/xxhash.cpp L591-598 is reflowed and reads 'multiplication by 0'); README.md (v0.8.3) lines 85-88. https://github.com/Cyan4973/xxHash/blob/v0.8.3/"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 214,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "provenance": "records/claims.json#/result/table/rows/2",
      "failed_tests": [
        "BIC",
        "Sparse",
        "PerlinNoise",
        "Bitflip",
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "SeedBlockOffset",
        "SeedBIC"
      ]
    },
    "score": {
      "calculated": 29.67807190511264,
      "display": 29.7,
      "display_text": "≈ 29.7*",
      "direction": "estimated_upper_bound",
      "rounding": "nearest tenth; sampling uncertainty",
      "derivation": "log2(4) - (-27.67807190511264)",
      "poisson_95_interval": [
        28.5,
        31.3
      ],
      "poisson_count": 5,
      "poisson_trials": 1073741824,
      "interval_scope": "Two-sided exact Poisson interval transformed to log2(L/p); uncertainty in this witness-derived quantity, not an interval for the unknown family score."
    },
    "key_bits": 64,
    "output_bits": 128,
    "hover": {
      "mechanism": "The all-ones difference on both operands of the first message multiply-fold can give the same lo ^ hi, merging the state before the unchanged suffix.",
      "claim_note": "REPRODUCTION; verbatim excerpt"
    },
    "chart_eligible": true,
    "collision": {
      "display": "5/2^30 measured",
      "count": 5,
      "trials": 1073741824,
      "trials_log2": 30,
      "log2_contribution": -27.67807190511264,
      "exact_contribution": false,
      "total_equality_proved": false,
      "interpretation": "Measured full-output collision rate"
    },
    "pair_lengths_bytes": [
      32,
      32
    ],
    "pair_length_words": 4,
    "key_free": false,
    "status": "REPRODUCTION",
    "classification_reason": "Independent reproduction of the paper’s harness results.",
    "mechanism_family": "NH-like stripe accumulators",
    "rurban_alias": null,
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit API seed (sampled reproducibly with xoshiro256**); fixed default public secret/constants",
    "domain_short": "variable-length byte strings; selected witness only",
    "api": "XXH3_128bits_withSeed; default secret public; uniform 64-bit seed",
    "reproduction": {
      "status": "standalone program supplied; startup witness and deterministic smoke check recorded",
      "directory": "verify/xxh3-128",
      "source": "verify/xxh3-128/xxh3_128_verify.c",
      "command": "cc -O2 -std=c11 -o xxh3_128_verify xxh3_128_verify.c -lm && ./xxh3_128_verify 20",
      "trials": 1048576,
      "rng": "fixed deterministic pseudorandom smoke stream; see per-hash README",
      "note": "Completed dependency additions in verify/README.md. The smoke check does not replace historical large counts."
    }
  },
  {
    "id": "mum",
    "name": "MUM v3",
    "version": "v3",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "The public _mum(w, p0) term collides on two distinct 8-byte words before it is XORed into the seeded state.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/17",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/paper_rows.json#/17",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 7.15,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mum3.exact.unroll3/M2Pro",
        "host": "M2Pro",
        "registered_name": "mum3.exact.unroll3",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "mum3.exact.unroll3             64                      Mum-hash v3, unroll 2^3, exact mult",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 6.57,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mum3.exact.unroll3/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "mum3.exact.unroll3",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "mum3.exact.unroll3             64                      Mum-hash v3, unroll 2^3, exact mult",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 24.27,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mum3.exact.unroll3/M2Pro",
        "host": "M2Pro",
        "registered_name": "mum3.exact.unroll3",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "mum3.exact.unroll3             64                      Mum-hash v3, unroll 2^3, exact mult"
      }
    },
    "anchor": "appendix-mum",
    "label": "MUM v3",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. Exact multiplication, fixed public constants, pre-fix MUM v3. The key-free 8-byte pair collides for both 8-word and 16-word unroll variants. The plotted speed is mum3.exact.unroll3 (8 words), from the two-host benchmark. Its unroll4 timings are not substituted. This does not describe the later collision-prevention default.",
    "official_claim": {
      "text": "MUM hash passes **all** [SMHasher](https://github.com/aappleby/smhasher) tests",
      "url": "https://github.com/vnmakarov/mum-hash/blob/master/README.md",
      "context": "Exact multiplication, fixed public constants, pre-fix MUM v3. The key-free 8-byte pair collides for both 8-word and 16-word unroll variants. The plotted speed is mum3.exact.unroll3 (8 words), from the two-host benchmark. Its unroll4 timings are not substituted. This does not describe the later collision-prevention default.",
      "provenance": "records/claims.json#/result/table/rows/3/official_claim_quote",
      "verbatim_record": "* MUM hash passes **all** [SMHasher](https://github.com/aappleby/smhasher) tests\n  * For comparison, only 4 out of 15 non-cryptographic hash functions\n    in SMHasher passes the tests, e.g. well known FNV, Murmur2,\n    Lookup, and Superfast hashes fail the tests |   * MUM and VMUM are also *resistant* to preimage attack (finding a\n    key with a given hash) \n    * To make hard moving to previous state values we use mostly 1-to-1 one way\n      function `lo(x*C) + hi(x*C)` where C is a constant.  Brute force\n      solution of equation `f(x) = a` probably requires `2^63` tries.\n      Another used function equation `x ^ y = a` has a `2^64`\n      solutions.  It complicates finding the overal solution further |   It is easy to generate data which makes the 1st operand of `_vmum`\n  to be zero.  In this case whatever the second operand is, the hash\n  will be generated the same.  So an adversary can generate a lot of\n  data with the same hash",
      "source_record": "README.md (commit 0b4387cf, 2026-06-02) raw lines 38-41, 198-204, and top banner lines 7-10 ('# **Update (Nov. 28, 2025): Implemented collision attack prevention in VMUM and MUM-V3**'), https://github.com/vnmakarov/mum-hash/blob/master/README.md. Note lines 42-50 also say 'It fails on Perlin noise and bad seeds tests' (second bullet there is a sub-bullet in the source)."
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 214,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "provenance": "records/claims.json#/result/table/rows/3",
      "failed_tests": [
        "Cyclic",
        "SeedZeroes",
        "SeedBlockLen",
        "SeedBlockOffset"
      ]
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0.0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The public _mum(w, p0) term collides on two distinct 8-byte words before it is XORed into the seeded state.",
      "claim_note": "REPRODUCTION; verbatim excerpt"
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed); 2^20/2^20 in both unroll variants",
      "count": 1048576,
      "trials": 1048576,
      "trials_log2": 20,
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "interpretation": "Public-term equality proves the all-seed collision"
    },
    "speed_note": "SMHasher3 speeds are mum3.exact.unroll3 (8 words); the pair also covers unroll4 (16 words).",
    "pair_lengths_bytes": [
      8,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "REPRODUCTION",
    "classification_reason": "Independent reproduction of the paper’s harness results.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "MUM/mir",
    "claim_category": "statistical quality only",
    "key_model": "Uniform 64-bit API seed (sampled reproducibly with xoshiro256**); fixed default public secret/constants",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/mum",
      "source": "verify/mum/mum_verify.c",
      "command": "cc -O2 -std=c11 -o mum_verify mum_verify.c -lm && ./mum_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "pengyhash",
    "name": "pengyhash v0.3",
    "version": "v0.3",
    "family": "heuristic",
    "bits": 2.0,
    "bits_kind": "exact",
    "mechanism": "The seed never touches the 32-byte bulk loop, and the public block compression decouples into two independent 64-bit word-pair equations (equal w1+2*w0 and equal u1(w0)), so a ~2^32 Brent-rho search on u1(w0) gives two 32-byte messages with an identical 256-bit state entering the seeded finalization; z3 likewise maps a 32-byte block onto the pre-finalization state (1,0,0,0) of the byte 00.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/0/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/0/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 3.49,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/pengyhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "pengyhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "pengyhash                      64                      pengyhash v0.3",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 4.41,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/pengyhash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "pengyhash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "pengyhash                      64                      pengyhash v0.3",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 77.88,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/pengyhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "pengyhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "pengyhash                      64                      pengyhash v0.3"
      }
    },
    "anchor": "appendix-pengyhash",
    "label": "pengyhash v0.3",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. The attacked function is SMHasher3’s sequenced pengyhash v0.3, verification 0x861A1254, with a 64-bit seed. The task’s original v0.2 label and rurban’s v0.2 results concern an older 32-bit-seed implementation. Upstream v0.3 has a reported unsequenced modification; this result is pinned to the sequenced SMHasher3 form. v0.3 uses GPLv3; earlier versions used BSD 2-Clause.",
    "official_claim": {
      "text": "Version 0.3 passes these tests.",
      "verbatim_record": "Fast 64-bit non-cryptographic hash algorithm | ## v0.3 changes\n- remove usage of memcpy()\n- 64-bit seed\n- tweaked hash with new rotation constants\n- faster\n- switched to GNU GPLv3 license | Version 0.3 passes these tests.",
      "url": "https://github.com/tinypeng/pengyhash",
      "context": "The attacked function is SMHasher3’s sequenced pengyhash v0.3, verification 0x861A1254, with a 64-bit seed. The task’s original v0.2 label and rurban’s v0.2 results concern an older 32-bit-seed implementation. Upstream v0.3 has a reported unsequenced modification; this result is pinned to the sequenced SMHasher3 form. v0.3 uses GPLv3; earlier versions used BSD 2-Clause.",
      "source_record": "README.md line 2 (the entire official description; also the GitHub repository description) and lines 4-9 (the remainder of the 199-byte README), https://github.com/tinypeng/pengyhash; author's reply on issue #1 'Fail Avalanche, Seed Tests from demerphq/smhasher' (tinypeng, 2022-11-30T04:38:57Z, issue still open), https://github.com/tinypeng/pengyhash/issues/1. Implied quality claim: the author-bundled SMhasher_results.txt (first added 2020-08-21, updated -- not added -- in the v0.3 commit 036dab53: line 2 '--- Testing pengyhash \"pengyhash\" GOOD', 'Verification value 0x861A1254 ....... PASS', zero FAIL lines) and SMhasher_demerphq_results.txt ('# All Tests Passed. pengyhash passed all 195 tests run.'). No paper, design doc, security statement or collision claim of any kind exists.",
      "provenance": "records/remainder_rows.json#/rows/0/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 234,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "SeedBlockLen"
      ],
      "provenance": "records/remainder_rows.json#/rows/0"
    },
    "score": {
      "calculated": 2.0,
      "display": 2.0,
      "display_text": "≤ 2",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(4) - (0.0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The seed never touches the 32-byte bulk loop, and the public block compression decouples into two independent 64-bit word-pair equations (equal w1+2*w0 and equal u1(w0)), so a ~2^32 Brent-rho search on u1(w0) gives two 32-byte messages with an identical 256-bit state entering the seeded finalization; z3 likewise maps a 32-byte block onto the pre-finalization state (1,0,0,0) of the byte 00.",
      "claim_note": "NEW; verbatim excerpt; The attacked function is SMHasher3’s sequenced pengyhash v0.3, verification 0x861A1254, with a 64-bit seed. The task’s original v0.2 label and rurban’s v0.2 results concern an older 32-bit-seed implementation. Upstream v0.3 has a reported unsequenced modification; this result is pinned to the sequenced SMHasher3 form. v0.3 uses GPLv3; earlier versions used BSD 2-Clause."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 1073741824,
      "trials": 1073741824,
      "interpretation": "Public-state equality proves all-seed collision; corroborated by independent measurement"
    },
    "pair_lengths_bytes": [
      32,
      32
    ],
    "pair_length_words": 4,
    "key_free": true,
    "status": "NEW",
    "classification_reason": "No prior literature identified in the supplied checked record.",
    "mechanism_family": "ARX lanes (add–rotate–xor)",
    "rurban_alias": "pengyhash",
    "claim_category": "statistical quality only",
    "key_model": "64 (`uint64_t pengyhash(const void *p, size_t size, uint64_t seed);` pengyhash.h line 7; 32 in v0.1/v0.2). The seed enters only in the 6-round finalization (`s[1] += f[1] + seed` each round, with the 0..31-byte zero-padded tail f -- corrected from '<=32'); the 32-byte bulk loop is seed-free, so the 256-bit state after it is a public function of the message and its length (s0 = size). SMHasher3: 64.",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/pengyhash",
      "source": "verify/pengyhash/pengyhash_verify.c",
      "command": "cc -O2 -std=c11 -o pengyhash_verify pengyhash_verify.c -lm && ./pengyhash_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "nmhash32",
    "name": "nmhash32 v2",
    "version": "v2",
    "family": "heuristic",
    "bits": 4.993026634712533,
    "bits_kind": "exact",
    "mechanism": "A loop-round trail in lane 0 (words a_0 and b_0 XOR 0x80400000) keeps x+y unchanged when bit 22 of seed+64 is set, passes the odd 16-bit-lane multiplies as a top-bit / 0x2000 difference when no carry crosses bit 13, and the final round's fresh words absorb the resulting register differences; every xorshift step is a GF(2) bijection, so only the one add and the low-lane multiply are probabilistic.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/1/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/1/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 1.78,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/NMHASH/M2Pro",
        "host": "M2Pro",
        "registered_name": "NMHASH",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "NMHASH                         32      scalar          nmhash32 v2",
        "backend_note": "scalar"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 6.88,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/NMHASH/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "NMHASH",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "NMHASH                         32      avx512          nmhash32 v2",
        "backend_note": "avx512"
      },
      "smh_m2_small_cycles": {
        "value": 65.92,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/NMHASH/M2Pro",
        "host": "M2Pro",
        "registered_name": "NMHASH",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "NMHASH                         32      scalar          nmhash32 v2"
      }
    },
    "anchor": "appendix-nmhash32",
    "label": "nmhash32 v2",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. NMHASH32 v2 uses a 32-bit seed and returns 32 bits. Verification is 0x12A30553. The last algorithmic change was 2021-06-08; the 2021-06-09 commit changed the version label, and the December 2024 undefined-behaviour fix preserved verification values. The new timings use scalar code on M2 Pro and AVX-512 on Xeon; the reproduction is scalar.",
    "official_claim": {
      "text": "Both hashes are the same high quality, and pass the checking:",
      "verbatim_record": "32bit hash, the core loop is constructed of invertible operations, and the multiplications are limited to `16x16->16`. For better speed on short keys, the limitation is loosen to `32x32->32` multiplication in the `NMHASH32X` variant when hashing the short keys or avalanching the final result of the core loop. | Both hashes are the same high quality, and pass the checking:\n\n- [rurban/smhasher](https://github.com/rurban/smhasher), including LongNeighbors and BadSeeds\n- [demerphq/smhasher](https://github.com/demerphq/smhasher/)\n- [massive collision tester](https://github.com/Cyan4973/xxHash/tree/dev/tests/collisions), 1G, len=8,16,256",
      "url": "https://github.com/gzm55/hash-garage/blob/master/README.md",
      "context": "NMHASH32 v2 uses a 32-bit seed and returns 32 bits. Verification is 0x12A30553. The last algorithmic change was 2021-06-08; the 2021-06-09 commit changed the version label, and the December 2024 undefined-behaviour fix preserved verification values. The new timings use scalar code on M2 Pro and AVX-512 on Xeon; the reproduction is scalar.",
      "source_record": "README.md, section `NMHASH32`/`NMHASH32X` first paragraph and section '### Quality' (the only quality/collision statement in the repository), https://github.com/gzm55/hash-garage/blob/master/README.md (raw: https://raw.githubusercontent.com/gzm55/hash-garage/master/README.md). Design statement in rurban/smhasher issue #190 'Add NMHASH32' (gzm55, 2021-04-10): 'friendly to low profile hardware, using only narrow multiplications (16 x 16 -> 16), and 32bit addition, shift and xor'. No statement about cryptographic strength, seeded security or adversarial inputs anywhere (README, nmhash.h, LICENSE, issue #2, rurban issues #190/#194).",
      "provenance": "records/remainder_rows.json#/rows/1/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 172,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "BIC",
        "Zeroes",
        "Sparse",
        "Permutation",
        "TwoBytes",
        "PerlinNoise",
        "Bitflip",
        "SeedZeroes",
        "SeedBlockLen",
        "SeedBlockOffset",
        "SeedBitflip"
      ],
      "provenance": "records/remainder_rows.json#/rows/1"
    },
    "score": {
      "calculated": 4.993026634712533,
      "display": 4.994,
      "display_text": "≤ 4.994",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(8) - (-1.9930266347125327)"
    },
    "key_bits": 32,
    "output_bits": 32,
    "hover": {
      "mechanism": "A loop-round trail in lane 0 (words a_0 and b_0 XOR 0x80400000) keeps x+y unchanged when bit 22 of seed+64 is set, passes the odd 16-bit-lane multiplies as a top-bit / 0x2000 difference when no carry crosses bit 13, and the final round's fresh words absorb the resulting register differences; every xorshift step is a GF(2) bijection, so only the one add and the low-lane multiply are probabilistic.",
      "claim_note": "NEW; verbatim excerpt; NMHASH32 v2 uses a 32-bit seed and returns 32 bits. Verification is 0x12A30553. The last algorithmic change was 2021-06-08; the 2021-06-09 commit changed the version label, and the December 2024 undefined-behaviour fix preserved verification values. The new timings use scalar code on M2 Pro and AVX-512 on Xeon; the reproduction is scalar."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1078944392/2^32 (exhaustive)",
      "log2_contribution": -1.9930266347125327,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 1078944392,
      "trials": 4294967296,
      "interpretation": "Fixed-pair exhaustive count"
    },
    "pair_lengths_bytes": [
      64,
      64
    ],
    "pair_length_words": 8,
    "key_free": false,
    "status": "NEW",
    "classification_reason": "No prior literature identified in the supplied checked record.",
    "mechanism_family": "narrow-multiply lanes",
    "rurban_alias": "nmhash32",
    "claim_category": "statistical quality only",
    "key_model": "32 (`uint32_t seed`, nmhash.h line 593; author on issue #2: 'output size is fixed to 32bit. seed is also 32bit.'; SMHasher3 FLAG_HASH_SMALL_SEED). The seed enters only as sl = seed+len (9..255 B: four lanes y_j = sl ^ word; 0..8 B: seed+const added to the data and rotl(seed,5) XORed mid-mixer). For a uniform 32-bit seed, fixed-pair probabilities are integer multiples of 2^-32; nonzero probabilities are at least 2^-32, not at most that value.",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/nmhash32",
      "source": "verify/nmhash32/nmhash32_verify.c",
      "command": "cc -O2 -std=c11 -o nmhash32_verify nmhash32_verify.c -lm && ./nmhash32_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "nmhash32x",
    "name": "nmhash32x v2",
    "version": "v2",
    "family": "heuristic",
    "bits": 2.0,
    "bits_kind": "exact",
    "mechanism": "Delta = 0x08008008 on both words of an x-stream round leaves x^y unchanged; the y difference re-enters as rotl(Delta,4) = 0x80080080, which x ^= x>>12 turns into exactly 2^31 before the odd 32-bit multiply (an odd multiply preserves a top-bit XOR difference exactly), giving 0x80080000; the len-8 and len-4 tail words (bytes 20..23 ^= 0x80080000, bytes 24..27 ^= 0x80080080) cancel both registers before any seed-dependent operation.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/2/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/2/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 1.79,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/NMHASHX/M2Pro",
        "host": "M2Pro",
        "registered_name": "NMHASHX",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "NMHASHX                        32      scalar          nmhash32x v2",
        "backend_note": "scalar"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 6.88,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/NMHASHX/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "NMHASHX",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "NMHASHX                        32      avx512          nmhash32x v2",
        "backend_note": "avx512"
      },
      "smh_m2_small_cycles": {
        "value": 39.35,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/NMHASHX/M2Pro",
        "host": "M2Pro",
        "registered_name": "NMHASHX",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "NMHASHX                        32      scalar          nmhash32x v2"
      }
    },
    "anchor": "appendix-nmhash32x",
    "label": "nmhash32x v2",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. NMHASH32X v2 is a separate function from NMHASH32, with 32×32 short-key multiplication, a 32-bit seed, 32-bit output and verification 0xA8580227. It has its own row and witness. Its statistical failures and host-specific timings must not be merged with NMHASH32’s; these timings use scalar code on M2 Pro and AVX-512 on Xeon.",
    "official_claim": {
      "text": "Both hashes are the same high quality, and pass the checking:",
      "verbatim_record": "Both hashes are the same high quality, and pass the checking:\n\n- [rurban/smhasher](https://github.com/rurban/smhasher), including LongNeighbors and BadSeeds\n- [demerphq/smhasher](https://github.com/demerphq/smhasher/)\n- [massive collision tester](https://github.com/Cyan4973/xxHash/tree/dev/tests/collisions), 1G, len=8,16,256 | `NMHASH32X` is a variant of `NMHASH32`, which mixes short keys with 32x32->32 `MUL`, and improve the speed of short keys.",
      "url": "https://github.com/gzm55/hash-garage/blob/master/README.md",
      "context": "NMHASH32X v2 is a separate function from NMHASH32, with 32×32 short-key multiplication, a 32-bit seed, 32-bit output and verification 0xA8580227. It has its own row and witness. Its statistical failures and host-specific timings must not be merged with NMHASH32’s; these timings use scalar code on M2 Pro and AVX-512 on Xeon.",
      "source_record": "README.md section '### Quality', https://github.com/gzm55/hash-garage/blob/master/README.md; rurban/smhasher issue #194 'Add NMHASH32X variant of NMHASH32', opening post by gzm55, 2021-04-22, https://github.com/rurban/smhasher/issues/194. No security statement anywhere.",
      "provenance": "records/remainder_rows.json#/rows/2/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 150,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "BIC",
        "Zeroes",
        "Sparse",
        "Permutation",
        "TwoBytes",
        "PerlinNoise",
        "Bitflip",
        "SeedZeroes",
        "SeedBlockLen",
        "SeedBlockOffset",
        "Seed",
        "SeedBIC",
        "SeedBitflip"
      ],
      "provenance": "records/remainder_rows.json#/rows/2"
    },
    "score": {
      "calculated": 2.0,
      "display": 2.0,
      "display_text": "≤ 2",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(4) - (0.0)"
    },
    "key_bits": 32,
    "output_bits": 32,
    "hover": {
      "mechanism": "Delta = 0x08008008 on both words of an x-stream round leaves x^y unchanged; the y difference re-enters as rotl(Delta,4) = 0x80080080, which x ^= x>>12 turns into exactly 2^31 before the odd 32-bit multiply (an odd multiply preserves a top-bit XOR difference exactly), giving 0x80080000; the len-8 and len-4 tail words (bytes 20..23 ^= 0x80080000, bytes 24..27 ^= 0x80080080) cancel both registers before any seed-dependent operation.",
      "claim_note": "NEW; verbatim excerpt; NMHASH32X v2 is a separate function from NMHASH32, with 32×32 short-key multiplication, a 32-bit seed, 32-bit output and verification 0xA8580227. It has its own row and witness. Its statistical failures and host-specific timings must not be merged with NMHASH32’s; these timings use scalar code on M2 Pro and AVX-512 on Xeon."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 4294967296,
      "trials": 4294967296,
      "interpretation": "Public-state equality proves all-seed collision; corroborated by independent measurement"
    },
    "pair_lengths_bytes": [
      28,
      28
    ],
    "pair_length_words": 4,
    "key_free": true,
    "status": "NEW",
    "classification_reason": "No prior literature identified in the supplied checked record.",
    "mechanism_family": "narrow-multiply lanes",
    "rurban_alias": null,
    "claim_category": "statistical quality only",
    "key_model": "32 (same `uint32_t seed` API as NMHASH32; SMHasher3 FLAG_HASH_SMALL_SEED). For 9..255 B the y and b streams are affine in (seed, message) and the seed meets the x stream only through y. For a uniform 32-bit seed, fixed-pair probabilities are integer multiples of 2^-32; nonzero probabilities are at least 2^-32, not at most that value.",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/nmhash32x",
      "source": "verify/nmhash32x/nmhash32x_verify.c",
      "command": "cc -O2 -std=c11 -o nmhash32x_verify nmhash32x_verify.c -lm && ./nmhash32x_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "mx3",
    "name": "mx3 v3",
    "version": "v3.0.0",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "The pre-finalizer state is seed*C^(L+1) + (a seed-free sum of g(word)*C^k) with g(x) = ((x*C) ^ ((x*C)>>39))*C a public bijection and mix() a public bijective finalizer, so two messages with the same word count L collide independently of the seed once the last word is solved through g^-1; a 1-byte message and an 8-byte message both have L=1 and collide for every seed.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/3/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/3/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 4.07,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mx3.v3/M2Pro",
        "host": "M2Pro",
        "registered_name": "mx3.v3",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "mx3.v3                         64                      mx3 (revision 3)",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 4.82,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mx3.v3/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "mx3.v3",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "mx3.v3                         64                      mx3 (revision 3)",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 45.93,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mx3.v3/M2Pro",
        "host": "M2Pro",
        "registered_name": "mx3.v3",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "mx3.v3                         64                      mx3 (revision 3)"
      }
    },
    "anchor": "appendix-mx3",
    "label": "mx3 v3",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. This is mx3 v3.0.0, tag 48924ee7, with a 64-bit seed. rurban’s mx3 entry tests v1.0.0 (0x4DB51E5B) with a 32-bit seed cast to 64 bits; its GOOD label and older bad-seed result do not describe the attacked v3.",
    "official_claim": {
      "text": "* mx3::hash passes all [SMHasher](https://github.com/rurban/smhasher) tests",
      "verbatim_record": "Repo with non-cryptographic bit mixer, pseudo random number generator and a hash function. The functions were found semi-algorithmically as detailed in those posts: | * mx3::hash passes all [SMHasher](https://github.com/rurban/smhasher) tests | [Version 3](https://github.com/jonmaiga/mx3/releases/tag/v3.0.0) improves mx3::hash with better seeding and speed while maintaining the same good quality. | This work is to a large degree empirical without much theoretical attention. It would be interesting to know more of what properties and weaknesses mx3 has, so please don't hesitate to let me know!",
      "url": "https://github.com/jonmaiga/mx3",
      "context": "This is mx3 v3.0.0, tag 48924ee7, with a 64-bit seed. rurban’s mx3 entry tests v1.0.0 (0x4DB51E5B) with a 32-bit seed cast to 64 bits; its GOOD label and older bad-seed result do not describe the attacked v3.",
      "source_record": "README.md first paragraph (raw line 2), section 'Quality' third bullet, section 'Version 3', section 'Feedback', https://github.com/jonmaiga/mx3 (raw: https://raw.githubusercontent.com/jonmaiga/mx3/master/README.md); release v3.0.0 body 'Improves mx3::hash in terms of speed and seeding.'; PR #1 'Better seeding and faster stream mixer' (jonmaiga, 2022-04-13): '- Avoid obvious bad seeds when data length == seed'; blog http://jonkagstrom.com/mx3/index.html (2020-08-10): 'For hashing pass all SMHasher tests' (section 'Goals'); the quotes 'At first, the idea I outlined above...' and 'With this it passed all the SMHasher tests' sit inside the 'mx3::hash outline' section -- corrected. No collision-probability, universality or security claim anywhere; the Rust port mx3-rs (chfoo) says 'The crate is *not* intended for cryptographically secure purposes.'",
      "provenance": "records/remainder_rows.json#/rows/3/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 214,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "SeedBlockLen",
        "SeedBlockOffset"
      ],
      "provenance": "records/remainder_rows.json#/rows/3"
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0.0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "The pre-finalizer state is seed*C^(L+1) + (a seed-free sum of g(word)*C^k) with g(x) = ((x*C) ^ ((x*C)>>39))*C a public bijection and mix() a public bijective finalizer, so two messages with the same word count L collide independently of the seed once the last word is solved through g^-1; a 1-byte message and an 8-byte message both have L=1 and collide for every seed.",
      "claim_note": "NEW; verbatim excerpt; This is mx3 v3.0.0, tag 48924ee7, with a 64-bit seed. rurban’s mx3 entry tests v1.0.0 (0x4DB51E5B) with a 32-bit seed cast to 64 bits; its GOOD label and older bad-seed result do not describe the attacked v3."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 1073741823,
      "trials": 1073741823,
      "interpretation": "Public-state equality proves all-seed collision; corroborated by independent measurement"
    },
    "pair_lengths_bytes": [
      1,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "NEW",
    "classification_reason": "No prior literature identified in the supplied checked record.",
    "mechanism_family": "Murmur/City/Farm lineage (multiply–xorshift mixing)",
    "rurban_alias": "mx3",
    "claim_category": "statistical quality only",
    "key_model": "64 (`inline uint64_t hash(const uint8_t* buf, size_t len, uint64_t seed)`; v3 seeds via h = mix_stream(seed, len + 1), v1/v2 via h = seed ^ len). The seed enters only through the additive term C^(L+1)*seed of the pre-finalizer state, before a public bijective finalizer mix(). SMHasher3: 64.",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/mx3",
      "source": "verify/mx3/mx3_verify.c",
      "command": "cc -O2 -std=c11 -o mx3_verify mx3_verify.c -lm && ./mx3_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "mir",
    "name": "mir.exact / mir.inexact",
    "version": "arithmetic since 2019-04-09",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "Message words enter only through the public fold mum(v,p) = hi+lo of v*p = (v*p mod (2^64-1)) minus a carry, XORed into the seeded state; m' = p1^-1 mod (2^64-1) gives mum(m',p1) = 0 = mum(0,p1) in both the exact and the 32x32 'strict' fold, so the 8-byte messages 0 and m' reach the same state for every seed.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/4/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/4/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 1.79,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mir.exact/M2Pro",
        "host": "M2Pro",
        "registered_name": "mir.exact",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "mir.exact                      64                      MIR-hash, exact 128-bit mult",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 2.59,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mir.exact/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "mir.exact",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "mir.exact                      64                      MIR-hash, exact 128-bit mult",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 36.04,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/mir.exact/M2Pro",
        "host": "M2Pro",
        "registered_name": "mir.exact",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "mir.exact                      64                      MIR-hash, exact 128-bit mult"
      }
    },
    "anchor": "appendix-mir",
    "label": "mir.exact / mir.inexact",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. mir has no algorithm version number; its arithmetic is unchanged since April 2019. SMHasher3 mir.exact (0x00A393C8) and mir.inexact (0x422A66FC) are output-equivalent to upstream mir_hash and mir_hash_strict on little-endian hosts. The strict fold drops one cross-product carry; it does not drop every carry. The 64-bit seed is passed verbatim, including seed+len = 0; SMHasher3’s inexact seed-fixup is a harness option, not the API model here. The chart uses only mir.exact timings from the two-host benchmark.",
    "official_claim": {
      "text": "Simple high-quality multiplicative hash passing demerphq-smhasher,\n   faster than spooky, city, or xxhash for strings less 100 bytes.\n   Hash for the same key can be different on different architectures.\n   To get machine-independent hash, use mir_hash_strict which is about\n   1.5 times slower than mir_hash.",
      "verbatim_record": "Simple high-quality multiplicative hash passing demerphq-smhasher,\n   faster than spooky, city, or xxhash for strings less 100 bytes.\n   Hash for the same key can be different on different architectures.\n   To get machine-independent hash, use mir_hash_strict which is about\n   1.5 times slower than mir_hash. | File `mir-hash.h` is a general, simple,\n   high quality hash function used by hashtables",
      "url": "https://github.com/vnmakarov/mir/blob/master/mir-hash.h",
      "context": "mir has no algorithm version number; its arithmetic is unchanged since April 2019. SMHasher3 mir.exact (0x00A393C8) and mir.inexact (0x422A66FC) are output-equivalent to upstream mir_hash and mir_hash_strict on little-endian hosts. The strict fold drops one cross-product carry; it does not drop every carry. The 64-bit seed is passed verbatim, including seed+len = 0; SMHasher3’s inexact seed-fixup is a harness option, not the API model here. The chart uses only mir.exact timings from the two-host benchmark.",
      "source_record": "mir-hash.h lines 6-10 (file header comment), https://github.com/vnmakarov/mir/blob/master/mir-hash.h; README.md section 'Structure of the project code', lines 310-311, https://github.com/vnmakarov/mir/blob/master/README.md. No security, DoS, hash-flooding or cryptographic claim or disclaimer for mir_hash anywhere (the words crypt/secur/attack/collision/flood do not occur in mir-hash.h or the mir README). The parent MUM's README says 'MUM hash is a **fast non-cryptographic hash function**' (line 25), '[V]MUM is not designed to be a crypto-hash' (lines 237-238) and, at lines 1-2, '# **Update (Nov. 28, 2025): Implemented collision attack prevention in VMUM and MUM-V3**' (Issue #18), which did not touch mir_hash.",
      "provenance": "records/remainder_rows.json#/rows/4/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 232,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "Cyclic",
        "SeedZeroes",
        "SeedBlockOffset"
      ],
      "provenance": "records/remainder_rows.json#/rows/4"
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0.0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Message words enter only through the public fold mum(v,p) = hi+lo of v*p = (v*p mod (2^64-1)) minus a carry, XORed into the seeded state; m' = p1^-1 mod (2^64-1) gives mum(m',p1) = 0 = mum(0,p1) in both the exact and the 32x32 'strict' fold, so the 8-byte messages 0 and m' reach the same state for every seed.",
      "claim_note": "EXTENSION; verbatim excerpt; mir has no algorithm version number; its arithmetic is unchanged since April 2019. SMHasher3 mir.exact (0x00A393C8) and mir.inexact (0x422A66FC) are output-equivalent to upstream mir_hash and mir_hash_strict on little-endian hosts. The strict fold drops one cross-product carry; it does not drop every carry. The 64-bit seed is passed verbatim, including seed+len = 0; SMHasher3’s inexact seed-fixup is a harness option, not the API model here. The chart uses only mir.exact timings from the two-host benchmark."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 1073741824,
      "trials": 1073741824,
      "interpretation": "Public-state equality proves all-seed collision; corroborated by independent measurement"
    },
    "speed_note": "SMHasher3 speeds are mir.exact only; no mir.inexact timing is assigned.",
    "pair_lengths_bytes": [
      8,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "EXTENSION",
    "classification_reason": "Extension of the paper’s MUM public-term result to the MUM-derived mir_hash.",
    "mechanism_family": "mum family (64×64→128 multiply-fold)",
    "rurban_alias": "MUM/mir",
    "claim_category": "statistical quality only",
    "key_model": "64 (`uint64_t seed`, entering only as `uint64_t r = seed + len;`, mir-hash.h line 67); every message word enters only through the public fold mum(v, p_i) XORed into the state (p1 = 0x65862b62bdf5ef4d, p2 = 0x288eea216831e6a7). SMHasher3: 64 (mir.inexact carries seedfixfn excludeBadseeds for len+seed == 0, a harness fixup only).",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/mir",
      "source": "verify/mir/mir_verify.c",
      "command": "cc -O2 -std=c11 -o mir_verify mir_verify.c -lm && ./mir_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "fasthash-64",
    "name": "fasthash-64",
    "version": "08a25db2",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "Every message word passes through the public bijection mix() before meeting the seed, and messages of any two lengths a<b<=8 take exactly one (h ^= mix(v); h *= m) step after h = seed ^ len*m, so 7 zero bytes and the 8-byte word mix^-1(7m ^ 8m) collide for every seed; for equal lengths a 2^63 difference in mix(v) survives the odd multiply deterministically ((x ^ 2^63)*m = x*m ^ 2^63) and a second such word cancels it.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/5/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/5/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 1.33,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/fasthash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "fasthash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 1,
        "registration": "fasthash-64                    64                      fast-hash, 64-bit version",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 2.3,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/fasthash-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "fasthash-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "fasthash-64                    64                      fast-hash, 64-bit version",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 37.06,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/fasthash-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "fasthash-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "fasthash-64                    64                      fast-hash, 64-bit version"
      }
    },
    "anchor": "appendix-fasthash",
    "label": "fasthash-64",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately.",
    "official_claim": {
      "text": "Robust - Passes all tests of SMHasher(http://code.google.com/p/smhasher).",
      "verbatim_record": "The fast-hash is a simple, robust, and efficient general-purpose hash function. | Robust - Passes all tests of SMHasher(http://code.google.com/p/smhasher). | The fast-hash was tested using the SMHasher(http://code.google.com/p/smhasher), which is known as the \"DieHarder\" hash testing. The test results show that the fast-hash is a better choice than Google MurmurHash2 (slightly biased and slower than the fast-hash), Jenkins hash function (moderately biased and notably slower than the fast-hash), and a few other popular ones such as Bernstein, CRC, SDBM, FNV, and etc. | Yes, you can use any hardcoded integer number like 0xdeadbeef as the seed.",
      "url": "https://github.com/ztanml/fast-hash",
      "context": "The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately.",
      "source_record": "README.md lines 3, 6 and 67 ('## Results', 2012 original SMHasher; the embedded FastHash64 log has no FAIL lines), https://github.com/ztanml/fast-hash/blob/master/README.md; issue #1 'how do I seed fasthash ?' (lucasart, 2019-12-22), author reply by ztanml 2020-01-17 (the only seed guidance; no secrecy requirement), https://github.com/ztanml/fast-hash/issues/1. README line 20 cites O'Neill's PCG paper ('one of the best general-purpose integer hash functions', a PRNG-quality remark, Section 9). No statement about collision resistance, seed secrecy, hash flooding or cryptographic strength anywhere (README, headers, comments, issues #1-#3, PR #4). Third-party: rurban's harness comment fasthash.cpp line 37 '// security: if the system allows empty keys (len=3) the seed is exposed, the reverse of mix.'",
      "provenance": "records/remainder_rows.json#/rows/5/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 151,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "BIC",
        "Permutation",
        "PerlinNoise",
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "SeedBlockOffset",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "provenance": "records/remainder_rows.json#/rows/5"
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0.0)"
    },
    "key_bits": 64,
    "output_bits": 64,
    "hover": {
      "mechanism": "Every message word passes through the public bijection mix() before meeting the seed, and messages of any two lengths a<b<=8 take exactly one (h ^= mix(v); h *= m) step after h = seed ^ len*m, so 7 zero bytes and the 8-byte word mix^-1(7m ^ 8m) collide for every seed; for equal lengths a 2^63 difference in mix(v) survives the odd multiply deterministically ((x ^ 2^63)*m = x*m ^ 2^63) and a second such word cancels it.",
      "claim_note": "NEW; verbatim excerpt; The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 1073741824,
      "trials": 1073741824,
      "interpretation": "Public-state equality proves all-seed collision; corroborated by independent measurement"
    },
    "pair_lengths_bytes": [
      7,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "NEW",
    "classification_reason": "No prior literature identified in the supplied checked record.",
    "mechanism_family": "Murmur/City/Farm lineage (multiply–xorshift mixing)",
    "rurban_alias": "fasthash32 (derived)",
    "claim_category": "statistical quality only",
    "key_model": "64 for fasthash64 (`uint64_t fasthash64(const void *buf, size_t len, uint64_t seed);` fasthash.h line 50); upstream fasthash32 takes `uint32_t seed` (fasthash.h line 42), so nonzero fixed-pair probabilities are at least 2^-32 under a uniform native seed; SMHasher3 passes a 64-bit seed_t to both widths. The seed enters once, `uint64_t h = seed ^ (len * m);` (fasthash.c line 41 -- corrected from 40), before any message word; each 8-byte word (and the zero-padded tail) passes through the public bijection mix() (xorshift 23, *0x2127599bf4325c37, xorshift 47) and then h = (h ^ mix(v)) * m with odd m = 0x880355f21e6d1965; output mix(h).",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/fasthash",
      "source": "verify/fasthash/fasthash_verify.c",
      "command": "cc -O2 -std=c11 -o fasthash_verify fasthash_verify.c -lm && ./fasthash_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "fasthash-32",
    "name": "fasthash-32",
    "version": "08a25db2",
    "family": "heuristic",
    "bits": 0.0,
    "bits_kind": "exact",
    "mechanism": "Every message word passes through the public bijection mix() before meeting the seed, and messages of any two lengths a<b<=8 take exactly one (h ^= mix(v); h *= m) step after h = seed ^ len*m, so 7 zero bytes and the 8-byte word mix^-1(7m ^ 8m) collide for every seed; for equal lengths a 2^63 difference in mix(v) survives the odd multiply deterministically ((x ^ 2^63)*m = x*m ^ 2^63) and a second such word cancels it.",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/5/smhasher3_speed",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "records/remainder_rows.json#/rows/5/smhasher3_speed",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": 1.34,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/fasthash-32/M2Pro",
        "host": "M2Pro",
        "registered_name": "fasthash-32",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "fasthash-32                    32                      fast-hash, 32-bit version",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 2.3,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/fasthash-32/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "fasthash-32",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "fasthash-32                    32                      fast-hash, 32-bit version",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 38.65,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/fasthash-32/M2Pro",
        "host": "M2Pro",
        "registered_name": "fasthash-32",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "fasthash-32                    32                      fast-hash, 32-bit version"
      }
    },
    "anchor": "appendix-fasthash",
    "label": "fasthash-32",
    "qualification": "Exact sufficient contribution; other pairs or collision events may lower the score. The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately.",
    "official_claim": {
      "text": "Robust - Passes all tests of SMHasher(http://code.google.com/p/smhasher).",
      "verbatim_record": "The fast-hash is a simple, robust, and efficient general-purpose hash function. | Robust - Passes all tests of SMHasher(http://code.google.com/p/smhasher). | The fast-hash was tested using the SMHasher(http://code.google.com/p/smhasher), which is known as the \"DieHarder\" hash testing. The test results show that the fast-hash is a better choice than Google MurmurHash2 (slightly biased and slower than the fast-hash), Jenkins hash function (moderately biased and notably slower than the fast-hash), and a few other popular ones such as Bernstein, CRC, SDBM, FNV, and etc. | Yes, you can use any hardcoded integer number like 0xdeadbeef as the seed.",
      "url": "https://github.com/ztanml/fast-hash",
      "context": "The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately.",
      "source_record": "README.md lines 3, 6 and 67 ('## Results', 2012 original SMHasher; the embedded FastHash64 log has no FAIL lines), https://github.com/ztanml/fast-hash/blob/master/README.md; issue #1 'how do I seed fasthash ?' (lucasart, 2019-12-22), author reply by ztanml 2020-01-17 (the only seed guidance; no secrecy requirement), https://github.com/ztanml/fast-hash/issues/1. README line 20 cites O'Neill's PCG paper ('one of the best general-purpose integer hash functions', a PRNG-quality remark, Section 9). No statement about collision resistance, seed secrecy, hash flooding or cryptographic strength anywhere (README, headers, comments, issues #1-#3, PR #4). Third-party: rurban's harness comment fasthash.cpp line 37 '// security: if the system allows empty keys (len=3) the seed is exposed, the reverse of mix.'",
      "provenance": "records/remainder_rows.json#/rows/5/official_claim_quote"
    },
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 191,
      "total": 250,
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md",
      "failed_tests": [
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "provenance": "records/remainder_rows.json#/rows/5"
    },
    "score": {
      "calculated": 0.0,
      "display": 0.0,
      "display_text": "≤ 0",
      "direction": "upper_bound",
      "rounding": "upward",
      "derivation": "log2(1) - (0.0)"
    },
    "key_bits": 64,
    "output_bits": 32,
    "hover": {
      "mechanism": "Every message word passes through the public bijection mix() before meeting the seed, and messages of any two lengths a<b<=8 take exactly one (h ^= mix(v); h *= m) step after h = seed ^ len*m, so 7 zero bytes and the 8-byte word mix^-1(7m ^ 8m) collide for every seed; for equal lengths a 2^63 difference in mix(v) survives the odd multiply deterministically ((x ^ 2^63)*m = x*m ^ 2^63) and a second such word cancels it.",
      "claim_note": "NEW; verbatim excerpt; The unversioned fast-hash code is pinned to upstream commit 08a25db2 (2018-10-22). SMHasher3 verification is 0xA16231A7 at 64 bits and 0xE9481AFC at 32 bits. Its wrappers pass a 64-bit seed to both widths; upstream fasthash32 takes a 32-bit seed. The every-seed identity covers both models, which the reproduction program measures separately."
    },
    "chart_eligible": true,
    "collision": {
      "display": "1 (every seed)",
      "log2_contribution": 0.0,
      "exact_contribution": true,
      "total_equality_proved": true,
      "count": 1073741824,
      "trials": 1073741824,
      "interpretation": "Public-state equality proves all-seed collision; corroborated by independent measurement"
    },
    "pair_lengths_bytes": [
      7,
      8
    ],
    "pair_length_words": 1,
    "key_free": true,
    "status": "NEW",
    "classification_reason": "No prior literature identified in the supplied checked record.",
    "mechanism_family": "Murmur/City/Farm lineage (multiply–xorshift mixing)",
    "rurban_alias": "fasthash32",
    "claim_category": "statistical quality only",
    "key_model": "64 for fasthash64 (`uint64_t fasthash64(const void *buf, size_t len, uint64_t seed);` fasthash.h line 50); upstream fasthash32 takes `uint32_t seed` (fasthash.h line 42), so nonzero fixed-pair probabilities are at least 2^-32 under a uniform native seed; SMHasher3 passes a 64-bit seed_t to both widths. The seed enters once, `uint64_t h = seed ^ (len * m);` (fasthash.c line 41 -- corrected from 40), before any message word; each 8-byte word (and the zero-padded tail) passes through the public bijection mix() (xorshift 23, *0x2127599bf4325c37, xorshift 47) and then h = (h ^ mix(v)) * m with odd m = 0x880355f21e6d1965; output mix(h).",
    "domain_short": "variable-length byte strings; selected witness only",
    "reproduction": {
      "status": "standalone program supplied",
      "directory": "verify/fasthash",
      "source": "verify/fasthash/fasthash_verify.c",
      "command": "cc -O2 -std=c11 -o fasthash_verify fasthash_verify.c -lm && ./fasthash_verify 20",
      "trials": 1048576,
      "rng": "splitmix64-seeded xoshiro256**, one stream, default seed 1",
      "note": "Small package checks are separate from the historical large measurements."
    }
  },
  {
    "id": "horner",
    "name": "Horner / unrolled, GF(2^64)",
    "version": "GF(2^64); sequential timing",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "(L−1)/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": 1.3,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/21/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 2.3,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/21/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "Horner / unrolled",
    "qualification": "Fixed length over GF(2^64), full field output; uniform field key. The output floor applies at L=1.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/21/smhasher3"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "(L−1)/q"
    },
    "output_bits": 64,
    "key_words_64bit": "1",
    "bound": "(L−1)/q",
    "hover": {
      "scope": "Fixed length over GF(2^64), full field output; uniform field key. The output floor applies at L=1."
    },
    "proof_status": "proved (audited)",
    "domain": "Fixed length over GF(2^64), full field output; uniform field key. The output floor applies at L=1.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "polynomial over GF(2^k)",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "No: unshifted AU only.",
      "note": "Last-word XOR 2^63 leaves all low 63 bits equal for every key.",
      "audit_url": "records/AUDIT.md#11-horner-over-gf264"
    }
  },
  {
    "id": "brw",
    "name": "BRW, GF(2^64)",
    "version": "GF(2^64)",
    "family": "proven",
    "bits": 63.0,
    "bits_kind": "audited lower guarantee",
    "mechanism": "(2L−1)/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": 6.0,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/22/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 5.3,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/22/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "BRW",
    "qualification": "Fixed-length BRW envelope, 1≤L≤2^61−1. Minimum of this loose envelope at L=2^61−1; displayed ≥63 is rounded downward.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/22/smhasher3"
    },
    "score": {
      "calculated": 63.0,
      "display": 63.0,
      "display_text": "≥ 63",
      "direction": "lower_bound",
      "minimizer_L": "2305843009213693951",
      "formula": "(2L−1)/q"
    },
    "output_bits": 64,
    "key_words_64bit": "1",
    "bound": "(2L−1)/q",
    "hover": {
      "scope": "Fixed-length BRW envelope, 1≤L≤2^61−1. Minimum of this loose envelope at L=2^61−1; displayed ≥63 is rounded downward."
    },
    "proof_status": "proved (audited)",
    "domain": "Fixed-length BRW envelope, 1≤L≤2^61−1. Minimum of this loose envelope at L=2^61−1; displayed ≥63 is rounded downward.",
    "score_minimizer_L": "2305843009213693951",
    "chart_eligible": true,
    "mechanism_family": "polynomial over GF(2^k)",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "No general masking guarantee for raw BRW.",
      "note": "The shifted authenticator has a differential theorem; the raw evaluation row does not inherit it.",
      "audit_url": "records/AUDIT.md#12-brw-bernsteinrabinwinograd"
    }
  },
  {
    "id": "recurrence",
    "name": "Injective recurrence, one chain",
    "version": "one chain, GF(2^64)",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "ceil(L/2)/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": 4.1,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/3/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 10.1,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/3/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "Injective recurrence, one chain",
    "qualification": "Fixed padded length, injective field-word encoding; three independent uniform field keys.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/3/smhasher3"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "ceil(L/2)/q"
    },
    "output_bits": 64,
    "key_words_64bit": "3",
    "bound": "ceil(L/2)/q",
    "hover": {
      "scope": "Fixed padded length, injective field-word encoding; three independent uniform field keys."
    },
    "proof_status": "proved (audited)",
    "domain": "Fixed padded length, injective field-word encoding; three independent uniform field keys.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "polynomial over GF(2^k)",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "No general masking guarantee.",
      "note": "The last additive data word can leave a key-independent constant difference; the audited row is full-output AU.",
      "audit_url": "records/AUDIT_REVIEW.md#101-recurrence-injectivity-including-the-small-cases"
    }
  },
  {
    "id": "lanes",
    "name": "Injective recurrence, eight lanes",
    "version": "eight lanes, GF(2^64)",
    "family": "proven",
    "bits": 61,
    "bits_kind": "audited lower guarantee",
    "mechanism": "(ceil(ceil(L/2)/8)+7)/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": 23.8,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/4/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 17.9,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/4/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "Injective recurrence, eight lanes",
    "qualification": "The explicitly defined eight-lane field construction, with independent lane-combining key; fixed padded length.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/4/smhasher3"
    },
    "score": {
      "calculated": 61,
      "display": 61,
      "display_text": "≥ 61",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "(ceil(ceil(L/2)/8)+7)/q"
    },
    "output_bits": 64,
    "key_words_64bit": "4",
    "bound": "(ceil(ceil(L/2)/8)+7)/q",
    "hover": {
      "scope": "The explicitly defined eight-lane field construction, with independent lane-combining key; fixed padded length."
    },
    "proof_status": "proved (audited)",
    "domain": "The explicitly defined eight-lane field construction, with independent lane-combining key; fixed padded length.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "polynomial over GF(2^k)",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "No general masking guarantee.",
      "note": "The fixed-length lane-composition proof certifies full-output AU, not all XOR differences.",
      "audit_url": "records/AUDIT_REVIEW.md#101-recurrence-injectivity-including-the-small-cases"
    }
  },
  {
    "id": "polymur",
    "name": "PolymurHash 2.0",
    "version": "2.0",
    "family": "proven",
    "bits": 54.22679349755191,
    "bits_kind": "audited lower guarantee",
    "mechanism": "D(8L)/K0; K0 = 189729088763903999",
    "speeds": {
      "m2_gbps_16k": {
        "value": 19.7,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/7/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 16.2,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/7/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": 5.69,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/polymurhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "polymurhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "polymurhash                    64                      Polymur Hash (using polymur_init_params_from_seed)",
        "backend_note": "generic"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 4.97,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/polymurhash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "polymurhash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "polymurhash                    64                      Polymur Hash (using polymur_init_params_from_seed)",
        "backend_note": "generic"
      },
      "smh_m2_small_cycles": {
        "value": 32.31,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/polymurhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "polymurhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "polymurhash                    64                      Polymur Hash (using polymur_init_params_from_seed)"
      }
    },
    "anchor": "proofs",
    "label": "PolymurHash 2.0",
    "qualification": "Uniform k on the exact restricted set K; fixed tweak; full 64-bit output. The key-cardinality gap was resolved with a rigorous lower bound. Shipped seed distribution and almost-strong-universality claim remain uncertified.",
    "smhasher3": {
      "verdict": "PASS",
      "passed": 250,
      "total": 250,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/7/smhasher3",
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md"
    },
    "score": {
      "calculated": 54.22679349755191,
      "display": 54.2267,
      "display_text": "≥ 54.2267",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "D(8L)/K0; K0 = 189729088763903999"
    },
    "output_bits": 64,
    "key_words_64bit": "4 resident",
    "bound": "D(8L)/K0; K0 = 189729088763903999",
    "hover": {
      "scope": "Uniform k on the exact restricted set K; fixed tweak; full 64-bit output. The key-cardinality gap was resolved with a rigorous lower bound. Shipped seed distribution and almost-strong-universality claim remain uncertified."
    },
    "proof_status": "proved with correction: D(8L)/K0",
    "domain": "Uniform k on the exact restricted set K; fixed tweak; full 64-bit output. The key-cardinality gap was resolved with a rigorous lower bound. Shipped seed distribution and almost-strong-universality claim remain uncertified.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "polynomial over a prime field",
    "rurban_alias": null,
    "domain_short": "full 64-bit output; uniform restricted-set key",
    "masking": {
      "label": "Full-output certificate only.",
      "note": "The corrected certificate is AU only; the ASU claim remains uncertified.",
      "audit_url": "records/AUDIT_REVIEW.md#1-polymurhash-20"
    }
  },
  {
    "id": "umash",
    "name": "UMASH-64",
    "version": "64-bit",
    "family": "proven",
    "bits": 55,
    "bits_kind": "claimed",
    "mechanism": "ceil(n/4096)·2^-55 (n bytes; equivalently ceil(L/512)·2^-55)",
    "speeds": {
      "m2_gbps_16k": {
        "value": 40.7,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/8/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 32.9,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/8/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "not_registered",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/UMASH-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "UMASH-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": 11.33,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/UMASH-64/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "UMASH-64",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "UMASH-64                       64     hwclmul          UMASH-64 (which == 0)",
        "backend_note": "hwclmul"
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "not_registered",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/UMASH-64/M2Pro",
        "host": "M2Pro",
        "registered_name": "UMASH-64",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proof-audit-notes",
    "label": "UMASH-64",
    "qualification": "Uniform full key; n bytes = 8L. The published projection argument has a gap; the claimed bound could not be certified from the text as written.",
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 123,
      "total": 250,
      "failed_tests": [
        "BIC",
        "Zeroes",
        "Sparse",
        "Permutation",
        "PerlinNoise",
        "Bitflip",
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "SeedBlockOffset",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "provenance": "data/provable.json#/result/table/rows/8/smhasher3",
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md"
    },
    "score": {
      "calculated": 55,
      "display": 55,
      "display_text": "55 (claimed)",
      "direction": "claimed",
      "minimizer_L": null,
      "formula": "ceil(n/4096)·2^-55 (n bytes; equivalently ceil(L/512)·2^-55)"
    },
    "output_bits": 64,
    "key_words_64bit": "38",
    "bound": "ceil(n/4096)·2^-55 (n bytes; equivalently ceil(L/512)·2^-55)",
    "hover": {
      "scope": "Uniform full key; n bytes = 8L. The published projection argument has a gap; the claimed bound could not be certified from the text as written."
    },
    "proof_status": "claimed; gap in the published argument (see note)",
    "domain": "Uniform full key; n bytes = 8L. The published projection argument has a gap; the claimed bound could not be certified from the text as written.",
    "score_minimizer_L": null,
    "chart_eligible": true,
    "bits_claimed": 55,
    "bits_certified": 1,
    "bound_certified": "F64(1)=2^-64/(1−561/q); F64(L)=1 for L≥2",
    "mechanism_family": "NH family (multiply–add, Halevi–Krawczyk)",
    "rurban_alias": "umash",
    "domain_short": "full ideal key; claim unresolved",
    "masking": {
      "label": "Unresolved.",
      "note": "No certified end-to-end bound to transfer to masked outputs.",
      "audit_url": "records/AUDIT_REVIEW.md#2-umash-64-and-umash-128"
    }
  },
  {
    "id": "umash128",
    "name": "UMASH-128 fingerprint",
    "version": "128-bit fingerprint",
    "family": "proven",
    "bits": 83,
    "bits_kind": "claimed",
    "mechanism": "ceil(n/2^26)^2·2^-83 (n bytes)",
    "speeds": {
      "m2_gbps_16k": {
        "value": 23.9,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/9/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 19.7,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/9/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "not_registered",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/UMASH-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "UMASH-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": 6.02,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/UMASH-128/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "UMASH-128",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 2,
        "registration": "UMASH-128                     128     hwclmul          UMASH-128",
        "backend_note": "hwclmul"
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "not_registered",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/UMASH-128/M2Pro",
        "host": "M2Pro",
        "registered_name": "UMASH-128",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proof-audit-notes",
    "label": "UMASH-128 fingerprint",
    "qualification": "Uniform full key; claimed 83-bit score on the short range, 1 ≤ n ≤ 2^26 bytes. The published projection argument has a gap; the claimed bound could not be certified from the text as written.",
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 122,
      "total": 250,
      "failed_tests": [
        "BIC",
        "Zeroes",
        "Sparse",
        "Permutation",
        "TwoBytes",
        "PerlinNoise",
        "Bitflip",
        "SeedZeroes",
        "SeedSparse",
        "SeedBlockLen",
        "SeedBlockOffset",
        "Seed",
        "SeedAvalanche",
        "SeedBIC",
        "SeedBitflip"
      ],
      "provenance": "data/provable.json#/result/table/rows/9/smhasher3",
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md"
    },
    "score": {
      "calculated": 83,
      "display": 83,
      "display_text": "83 (claimed)",
      "direction": "claimed",
      "minimizer_L": null,
      "formula": "ceil(n/2^26)^2·2^-83 (n bytes)"
    },
    "output_bits": 128,
    "key_words_64bit": "38",
    "bound": "ceil(n/2^26)^2·2^-83 (n bytes)",
    "hover": {
      "scope": "Uniform full key; claimed 83-bit score on the short range, 1 ≤ n ≤ 2^26 bytes. The published projection argument has a gap; the claimed bound could not be certified from the text as written."
    },
    "proof_status": "claimed; gap in the published argument (see note)",
    "domain": "Uniform full key; claimed 83-bit score on the short range, 1 ≤ n ≤ 2^26 bytes. The published projection argument has a gap; the claimed bound could not be certified from the text as written.",
    "score_minimizer_L": null,
    "chart_eligible": true,
    "bits_claimed": 83,
    "bits_certified": 1,
    "bound_certified": "F128(1)=2^-128/(1−561/q); F128(L)=1 for L≥2",
    "mechanism_family": "NH family (multiply–add, Halevi–Krawczyk)",
    "rurban_alias": "umash",
    "domain_short": "≤64 MiB for claimed score",
    "masking": {
      "label": "Unresolved.",
      "note": "The raw compressor’s AXU theorem does not certify the projected full hash or its truncation.",
      "audit_url": "records/AUDIT_REVIEW.md#2-umash-64-and-umash-128"
    }
  },
  {
    "id": "clhash",
    "name": "CLHASH",
    "version": "64-bit variable-length family",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "1/q for L≤128; 2/q+(ceil(L/128)−1)/2^126 otherwise",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/10/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/10/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "not_registered",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/CLhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "CLhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": 11.56,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/CLhash/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "CLhash",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "CLhash                         64     hwclmul          Carryless multiplication hash, without -DBITMIX",
        "backend_note": "hwclmul"
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "not_registered",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/CLhash/M2Pro",
        "host": "M2Pro",
        "registered_name": "CLhash",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "CLHASH",
    "qualification": "Ideal Algorithm 4, byte strings shorter than 2^64 bytes, full independent key. The timed seed initializer is not certified by this theorem.",
    "smhasher3": {
      "verdict": "FAIL",
      "passed": 64,
      "total": 250,
      "failed_tests": [
        "Avalanche",
        "BIC",
        "Cyclic",
        "Sparse",
        "Permutation",
        "Text",
        "TwoBytes",
        "PerlinNoise",
        "Bitflip",
        "Seed*"
      ],
      "provenance": "data/provable.json#/result/table/rows/10/smhasher3",
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/3b619371/results/README.md"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "1/q for L≤128; 2/q+(ceil(L/128)−1)/2^126 otherwise"
    },
    "output_bits": 64,
    "key_words_64bit": "133",
    "bound": "1/q for L≤128; 2/q+(ceil(L/128)−1)/2^126 otherwise",
    "hover": {
      "scope": "Ideal Algorithm 4, byte strings shorter than 2^64 bytes, full independent key. The timed seed initializer is not certified by this theorem."
    },
    "proof_status": "proved (audited)",
    "domain": "Ideal Algorithm 4, byte strings shorter than 2^64 bytes, full independent key. The timed seed initializer is not certified by this theorem.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "NH family (multiply–add, Halevi–Krawczyk)",
    "rurban_alias": null,
    "domain_short": "byte strings <2^64 bytes; ideal keys",
    "masking": {
      "label": "Yes, ideal Algorithm 4: AXU.",
      "note": "For t selected bits, multiply the length-specific ε by 2^(64−t), clipped at 1. This does not cover CLHASH_BITMIX or seeded initialization.",
      "audit_url": "records/AUDIT.md#1-clhash-and-clnh"
    }
  },
  {
    "id": "chain256",
    "name": "ChainHash (ours), 256 B blocks",
    "version": "256 B blocks; W=32, S=1",
    "family": "proven",
    "bits": 62.415037499278846,
    "bits_kind": "audited lower guarantee",
    "mechanism": "(ceil(L/32)+2)/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": 67.7,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/1/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 37.6,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/1/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": 21.47,
        "source": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "source_short": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/chainhash-256/M2Pro",
        "host": "M2Pro",
        "registered_name": "chainhash-256",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "chainhash-256                  64     hwpmull          ChainHash (GF(2^64) carry-less PH with strided word pairing + three-key injective chain + degree-5 finalizer behind an additive twist), 256-byte blocks",
        "backend_note": "hwpmull"
      },
      "smh_xeon_bulk_Bpc": {
        "value": 14.42,
        "source": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "source_short": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/chainhash-256/Xeon8375C",
        "host": "Xeon8375C",
        "registered_name": "chainhash-256",
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": 1,
        "registration": "chainhash-256                  64     hwclmul          ChainHash (GF(2^64) carry-less PH with strided word pairing + three-key injective chain + degree-5 finalizer behind an additive twist), 256-byte blocks",
        "backend_note": "hwclmul"
      },
      "smh_m2_small_cycles": {
        "value": 75.09,
        "source": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "source_short": "SMHasher3 small keys 1-31 B, cycles per hash, M2 Pro",
        "url": "records/speeds.json",
        "record": "records/speeds.json#/chainhash-256/M2Pro",
        "host": "M2Pro",
        "registered_name": "chainhash-256",
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": 2,
        "registration": "chainhash-256                  64     hwpmull          ChainHash (GF(2^64) carry-less PH with strided word pairing + three-key injective chain + degree-5 finalizer behind an additive twist), 256-byte blocks"
      }
    },
    "anchor": "proofs",
    "label": "ChainHash (ours) | 256 B blocks",
    "qualification": "Byte strings shorter than 2^64 bytes; 41 independent field words and the audited finalizer. Timed SplitMix64 seed expansion is outside this theorem.",
    "smhasher3": {
      "verdict": "PASS",
      "passed": 200,
      "total": 200,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/1/smhasher3",
      "url": "https://gitlab.com/lobais/smhasher3/-/tree/mr/chainhash",
      "suite_note": "author fork; no --extra",
      "suite_context": "200 = without --extra plus the default SeedDifferential tier; author local combined tree, not the upstream 250-test suite."
    },
    "score": {
      "calculated": 62.415037499278846,
      "display": 62.41,
      "display_text": "≥ 62.41",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "(ceil(L/32)+2)/q"
    },
    "output_bits": 64,
    "key_words_64bit": "41",
    "bound": "(ceil(L/32)+2)/q",
    "hover": {
      "scope": "Byte strings shorter than 2^64 bytes; 41 independent field words and the audited finalizer. Timed SplitMix64 seed expansion is outside this theorem."
    },
    "proof_status": "proved (audited)",
    "domain": "Byte strings shorter than 2^64 bytes; 41 independent field words and the audited finalizer. Timed SplitMix64 seed expansion is outside this theorem.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "this paper",
    "rurban_alias": null,
    "domain_short": "byte strings <2^64 bytes; ideal keys",
    "masking": {
      "label": "Yes, ideal keys and the finalizer.",
      "note": "If e is the intermediate collision bound, masked collision ≤ e+(1−e)2^−t; the coarser 2^(64−t)ε bound is also valid.",
      "audit_url": "records/AUDIT.md#14-chainhash-complete-audit-of-the-stated-composition"
    }
  },
  {
    "id": "clnh",
    "name": "Carryless NH (CLNH)",
    "version": "fixed-length, GF(2^64)",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "1/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": 27.3,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/11/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "m2_gbps_512": {
        "value": 26.3,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/11/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "Carryless NH (CLNH)",
    "qualification": "Carryless NH at equal fixed padded length, independent word keys, irreducible field reduction (or full unreduced output). One-word zero padding is admitted; unpadded even lengths score 65. No unequal-length transfer is claimed.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/11/smhasher3"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "1/q"
    },
    "output_bits": 64,
    "key_words_64bit": "2 ceil(L/2)",
    "bound": "1/q",
    "hover": {
      "scope": "Carryless NH at equal fixed padded length, independent word keys, irreducible field reduction (or full unreduced output). One-word zero padding is admitted; unpadded even lengths score 65. No unequal-length transfer is claimed."
    },
    "proof_status": "proved (audited)",
    "domain": "Carryless NH at equal fixed padded length, independent word keys, irreducible field reduction (or full unreduced output). One-word zero padding is admitted; unpadded even lengths score 65. No unequal-length transfer is claimed.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "NH family (multiply–add, Halevi–Krawczyk)",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "Yes: AXU.",
      "note": "Multiply ε by 2^(r−t) for r-bit full output; use the stated reduced or unreduced variant.",
      "audit_url": "records/AUDIT.md#1-clhash-and-clnh"
    }
  },
  {
    "id": "nh",
    "name": "NH with 64-bit words",
    "version": "64-bit words, 128-bit output",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "1/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/16/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/16/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "NH with 64-bit words",
    "qualification": "Unsigned integer NH with 64-bit words at equal fixed padded length. One-word zero padding is admitted; unpadded pairs score 65. No unequal-length transfer is claimed.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/16/smhasher3"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "1/q"
    },
    "output_bits": 128,
    "key_words_64bit": "2 ceil(L/2)",
    "bound": "1/q",
    "hover": {
      "scope": "Unsigned integer NH with 64-bit words at equal fixed padded length. One-word zero padding is admitted; unpadded pairs score 65. No unequal-length transfer is claimed."
    },
    "proof_status": "proved (audited)",
    "domain": "Unsigned integer NH with 64-bit words at equal fixed padded length. One-word zero padding is admitted; unpadded pairs score 65. No unequal-length transfer is claimed.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "NH family (multiply–add, Halevi–Krawczyk)",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "Low bits only: integer ADU.",
      "note": "Reduction modulo 2^t costs at most 2^(r−t)ε, clipped at 1. Arbitrary bit subsets are not covered by this additive-group argument.",
      "audit_url": "records/AUDIT_REVIEW.md#41-the-nh-theorem-and-standard-toeplitz-theorem-hold"
    }
  },
  {
    "id": "multiply",
    "name": "Vector multiply-shift",
    "version": "fixed-length vectors",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "1/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/19/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": 16.5,
        "source": "paper harness, Apple M2 Pro",
        "record": "records/provable.json#/result/table/rows/19/speed_m2_gbps",
        "source_short": "paper harness / Apple M2 Pro",
        "url": "records/provable.json"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "Vector multiply-shift",
    "qualification": "Full-key vector multiply-add-shift; independent 128-bit coefficients including the additive key, fixed length. Named string wrappers do not inherit this theorem.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/19/smhasher3"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "1/q"
    },
    "output_bits": 64,
    "key_words_64bit": "2(L+1)",
    "bound": "1/q",
    "hover": {
      "scope": "Full-key vector multiply-add-shift; independent 128-bit coefficients including the additive key, fixed length. Named string wrappers do not inherit this theorem."
    },
    "proof_status": "proved (audited)",
    "domain": "Full-key vector multiply-add-shift; independent 128-bit coefficients including the additive key, fixed length. Named string wrappers do not inherit this theorem.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "multiply-shift (Dietzfelbinger)",
    "rurban_alias": null,
    "domain_short": "fixed-length words; timed code: ≤64 words, no partial bytes",
    "masking": {
      "label": "Specified high bits only.",
      "note": "The theorem selects high bits of the wide accumulator. Do not replace that operation by a low-bit mask on the accumulator. Subsets of the already selected pairwise-independent output are safe.",
      "audit_url": "records/AUDIT_REVIEW.md#62-vector-and-paired-constructions-completing-the-exercise"
    }
  },
  {
    "id": "tabulation",
    "name": "Simple tabulation",
    "version": "fixed-length byte keys",
    "family": "proven",
    "bits": 64,
    "bits_kind": "audited lower guarantee",
    "mechanism": "1/q",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/20/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Unavailable in supplied records",
        "record": "data/provable.json#/result/table/rows/20/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proofs",
    "label": "Simple tabulation",
    "qualification": "Simple tabulation with independent position-specific tables on fixed-length inputs; an 8-byte input is admitted. Composite string wrappers do not inherit this theorem.",
    "smhasher3": {
      "verdict": "not registered for this family",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "provenance": "data/provable.json#/result/table/rows/20/smhasher3"
    },
    "score": {
      "calculated": 64,
      "display": 64,
      "display_text": "≥ 64",
      "direction": "lower_bound",
      "minimizer_L": 1,
      "formula": "1/q"
    },
    "output_bits": 64,
    "key_words_64bit": "256c, c bytes",
    "bound": "1/q",
    "hover": {
      "scope": "Simple tabulation with independent position-specific tables on fixed-length inputs; an 8-byte input is admitted. Composite string wrappers do not inherit this theorem."
    },
    "proof_status": "proved (audited)",
    "domain": "Simple tabulation with independent position-specific tables on fixed-length inputs; an 8-byte input is admitted. Composite string wrappers do not inherit this theorem.",
    "score_minimizer_L": 1,
    "chart_eligible": true,
    "mechanism_family": "tabulation",
    "rurban_alias": null,
    "domain_short": "fixed-length only",
    "masking": {
      "label": "Yes: AXU / pairwise independence.",
      "note": "Any t output bits have collision probability 2^−t under independent table entries.",
      "audit_url": "records/AUDIT.md#9-simple-tabulation"
    }
  },
  {
    "id": "halftimehash",
    "name": "HalftimeHash, shipped 64-bit API",
    "version": "Style64/128/256/512",
    "family": "proven",
    "bits": 59.83007499855769,
    "bits_kind": "claimed",
    "mechanism": "(2^4 + h^2 + 2)·2^-64; h = floor(log₈(floor(n/144))), n bytes",
    "speeds": {
      "m2_gbps_16k": {
        "value": null,
        "source": "Not measured on the M2 Pro (deliberately skipped; the vendored NEON-fixed copy in tools/bench/adversarial/vendor/halftime_hash/ is ready). Upstream header does not compile on AArch64 without the macro-alias fix (arm_neon_fix.h).",
        "record": "records/provable.json#/result/table/rows/13/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "m2_gbps_512": {
        "value": null,
        "source": "Not measured on the M2 Pro (deliberately skipped; the vendored NEON-fixed copy in tools/bench/adversarial/vendor/halftime_hash/ is ready). Upstream header does not compile on AArch64 without the macro-alias fix (arm_neon_fix.h).",
        "record": "records/provable.json#/result/table/rows/13/speed_m2_gbps",
        "source_short": "unavailable"
      },
      "smh_m2_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      },
      "smh_xeon_bulk_Bpc": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "Xeon8375C",
        "registered_name": null,
        "binary_sha256": "03c2ff9f38cc564400eeb1906da90cb74ea6efcff2bea6c660626fdbe1571a99",
        "selected_run": null,
        "registration": ""
      },
      "smh_m2_small_cycles": {
        "value": null,
        "source": "No matching registration for this mathematical family.",
        "source_short": "unavailable",
        "url": "records/speeds.json",
        "record": null,
        "host": "M2Pro",
        "registered_name": null,
        "binary_sha256": "33670275b735a33de9384d74d970cac932e1d5019ab505f7564c42bc1532b541",
        "selected_run": null,
        "registration": ""
      }
    },
    "anchor": "proof-audit-notes",
    "label": "HalftimeHash",
    "qualification": "Derived claim for the shipped k = 2 core plus 64-bit tabulation fold, not a bound stated in the paper. At h = 0 (including 1 KB), 64 − log₂(18) ≈ 59.83 bits; the formula is applied below the > 1 KB design regime. No nontrivial bound for the shipped wrappers could be certified from the text as written; all four styles return 64 bits.",
    "smhasher3": {
      "verdict": "FAIL (all four styles)",
      "passed": null,
      "total": null,
      "failed_tests": [],
      "url": "https://gitlab.com/fwojcik/smhasher3/-/blob/5035a923/results/README.md",
      "style_counts": {
        "64": "214/250",
        "128": "202/250",
        "256": "200/250",
        "512": "190/250"
      }
    },
    "score": {
      "calculated": 59.83007499855769,
      "display": 59.83,
      "display_text": "≈ 59.83 (claimed)",
      "direction": "claimed",
      "minimizer_L": null,
      "formula": "(2^4 + h^2 + 2)·2^-64; h = floor(log₈(floor(n/144))), n bytes"
    },
    "output_bits": 64,
    "key_words_64bit": "8866 64-bit words (kEntropyBytesNeeded = 70928 bytes: 6144 words of tabulation tables + 2722 words for the k = 2 core sized for unbounded length), compiled from the upstream header (commit caf7924)",
    "bound": "(2^4 + h^2 + 2)·2^-64; h = floor(log₈(floor(n/144))), n bytes",
    "hover": {
      "scope": "Derived claim for the shipped k = 2 core plus 64-bit tabulation fold, not a bound stated in the paper. At h = 0 (including 1 KB), 64 − log₂(18) ≈ 59.83 bits; the formula is applied below the > 1 KB design regime. No nontrivial bound for the shipped wrappers could be certified from the text as written; all four styles return 64 bits."
    },
    "proof_status": "claimed; gap in the published argument (see note)",
    "domain": "Derived claim for the shipped k = 2 core plus 64-bit tabulation fold, not a bound stated in the paper. At h = 0 (including 1 KB), 64 − log₂(18) ≈ 59.83 bits; the formula is applied below the > 1 KB design regime. No nontrivial bound for the shipped wrappers could be certified from the text as written; all four styles return 64 bits.",
    "score_minimizer_L": null,
    "chart_eligible": true,
    "bits_claimed": 59.83007499855769,
    "bits_certified": 0,
    "bound_certified": "1 (only the trivial bound certified for the shipped wrappers)",
    "style_speeds": {
      "HalftimeHash-64": {
        "M2Pro": {
          "bulk_bytes_per_cycle": 4.56,
          "small_cycles": 48.61
        },
        "Xeon8375C": {
          "bulk_bytes_per_cycle": 2.5,
          "small_cycles": 90.11
        }
      },
      "HalftimeHash-128": {
        "M2Pro": {
          "bulk_bytes_per_cycle": 5.04,
          "small_cycles": 53.57
        },
        "Xeon8375C": {
          "bulk_bytes_per_cycle": 8.07,
          "small_cycles": 82.97
        }
      },
      "HalftimeHash-256": {
        "M2Pro": {
          "bulk_bytes_per_cycle": 4.14,
          "small_cycles": 57.38
        },
        "Xeon8375C": {
          "bulk_bytes_per_cycle": 15.7,
          "small_cycles": 77.66
        }
      },
      "HalftimeHash-512": {
        "M2Pro": {
          "bulk_bytes_per_cycle": 3.66,
          "small_cycles": 78.79
        },
        "Xeon8375C": {
          "bulk_bytes_per_cycle": 19.29,
          "small_cycles": 85.74
        }
      }
    },
    "mechanism_family": "NH family (multiply–add, Halevi–Krawczyk)",
    "rurban_alias": "halftime_hash128",
    "domain_short": "shipped 64-bit wrappers; claim unresolved",
    "masking": {
      "label": "Unresolved for shipped wrappers.",
      "note": "The independently keyed repaired construction is a different family.",
      "audit_url": "records/AUDIT_REVIEW.md#34-encoder-and-wrapper-qualifications-checked-against-the-source"
    }
  }
];

// Data is embedded above: opening the page never fetches data.json.
// Palette and typography are declared together in post.css.
(() => {
  'use strict';
  const axes = [{"key": "smh_m2_bulk_Bpc", "label": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro", "compact": "M2 Pro bulk \u00b7 B/cycle", "title": "SMHasher3 bulk, bytes/cycle, Apple M2 Pro<br>faster \u2192", "unit": "B/cycle", "type": "log"}, {"key": "smh_xeon_bulk_Bpc", "label": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C", "compact": "Xeon 8375C bulk \u00b7 B/cycle", "title": "SMHasher3 bulk, bytes/cycle, Intel Xeon 8375C<br>faster \u2192", "unit": "B/cycle", "type": "log"}];
  const families = [
    {key: 'proven', name: 'audited ideal-key bound (●)', color: 'proven'},
    {key: 'claimed', name: 'claimed bound, unresolved proof (○)', color: 'proven'},
    {key: 'heuristic', name: 'pair cap (◆; × = every seed; * = measured)', color: 'heuristic'},
  ];
  const format = value => Number(value.toFixed(2)).toString();
  const escape = value => String(value).replace(/[&<>"']/g, c => ({'&': '&amp;', '<': '&lt;', '>': '&gt;', '"': '&quot;', "'": '&#39;'}[c]));
  function init() {
    const chart = document.getElementById('bits-vs-speed');
    if (!chart) return;
    if (!window.Plot || !window.d3) {
      chart.innerHTML = '<p class="chart-status">The interactive chart could not load. <a href="#chart-table-details">Show as table</a> contains every value and source.</p>';
      return;
    }
    const css = getComputedStyle(chart);
    const colors = Object.fromEntries(['proven', 'heuristic', 'surface', 'ink', 'muted', 'grid'].map(key => [key, css.getPropertyValue('--chart-' + key).trim()]));
    const phone = () => window.matchMedia('(max-width: 600px)').matches;
    const ticksY = d3.range(0, 89, 8);
    let active = 0, svg, points = [], xScale, baseDomain, viewDomain;
    let hideTimer, resizeFrame, hoveredPoint, suppressFocus = false, drag;
    let lastWidth = 0, lastHeight = 0, dragMode = 'none', suppressClick = false;

    // Equal 10 px silhouettes, independent of D3's area-normalized symbols.
    const diamond = {draw(context, size) {
      const r = Math.sqrt(size / Math.PI);
      context.moveTo(0, -r); context.lineTo(r, 0);
      context.lineTo(0, r); context.lineTo(-r, 0); context.closePath();
    }};
    const times = {draw(context, size) {
      const r = Math.sqrt(size / Math.PI), a = r * 0.36;
      [[-r, -r + a], [-r + a, -r], [0, -a], [r - a, -r], [r, -r + a], [a, 0],
        [r, r - a], [r - a, r], [0, a], [-r + a, r], [-r, r - a], [-a, 0]]
        .forEach(([x, y], i) => i ? context.lineTo(x, y) : context.moveTo(x, y));
      context.closePath();
    }};

    // Preserve the native selector as the no-script fallback. The four buttons
    // use exactly the same options, including the paper's GB/s measurements.
    const selector = document.getElementById('speed-axis');
    const controls = document.createElement('div');
    controls.className = 'axis-control axis-buttons';
    controls.setAttribute('role', 'group');
    controls.setAttribute('aria-label', 'Speed measure');
    const buttons = axes.map((axis, index) => {
      const button = document.createElement('button');
      button.type = 'button';
      button.textContent = axis.compact;
      button.title = axis.label;
      button.dataset.axis = axis.key;
      button.setAttribute('aria-label', axis.label);
      button.setAttribute('aria-controls', chart.id);
      button.addEventListener('click', () => selectAxis(index));
      controls.append(button);
      return button;
    });
    selector.closest('label').hidden = true;
    selector.closest('label').after(controls);
    selector.addEventListener('change', () => selectAxis(axes.findIndex(axis => axis.key === selector.value)));


    const card = document.createElement('div');
    card.className = 'chart-tooltip';
    card.id = 'chart-tooltip';
    card.hidden = true;
    card.setAttribute('role', 'region');
    card.setAttribute('aria-label', 'Hash evidence');
    chart.after(card);

    function hideCard(restoreFocus = false) {
      clearTimeout(hideTimer);
      const point = hoveredPoint;
      hoveredPoint = undefined;
      if (restoreFocus && point && card.contains(document.activeElement)) {
        suppressFocus = true;
        point.node.focus({preventScroll: true});
        suppressFocus = false;
      }
      card.hidden = true;
      point?.node.setAttribute('aria-expanded', 'false');
    }
    const deferHide = () => {
      if (phone()) return;
      clearTimeout(hideTimer);
      hideTimer = setTimeout(() => {
        if (!card.matches(':hover') && !card.contains(document.activeElement) && document.activeElement !== hoveredPoint?.node) hideCard();
      }, 220);
    };
    card.addEventListener('pointerenter', () => clearTimeout(hideTimer));
    card.addEventListener('pointerleave', deferHide);
    card.addEventListener('focusout', deferHide);
    card.addEventListener('click', event => {
      const link = event.target.closest('a[href^="#"]');
      if (link) {event.preventDefault(); hideCard(); jump(link.hash.slice(1));}
    });
    document.addEventListener('keydown', event => {if (event.key === 'Escape') hideCard(true);});
    document.addEventListener('pointerdown', event => {
      if (!chart.contains(event.target) && !card.contains(event.target)) hideCard();
    });

    function tooltip(row, axis) {
      const measurement = row.speeds[axis.key];
      const link = (text, url) => url ? '<a href="' + escape(url) + '">' + text + '</a>' : text;
      const entry = (name, value) => '<dt>' + name + '</dt><dd>' + value + '</dd>';
      const smh = row.smhasher3;
      let verdict = smh.verdict;
      if (smh.passed !== null) {
        const failed = smh.failed_tests;
        verdict = smh.verdict + ' — ' + smh.passed + ' of ' + smh.total + ' tests passed';
        if (failed.length) verdict += '; failed categories: ' + failed.slice(0, 2).join(', ') + (failed.length > 2 ? ', …' : '');
      }
      if (smh.suite_note) verdict += ' (' + smh.suite_note + ')';
      const name = row.family === 'heuristic' && !row.name.includes(row.version) ? row.name + ' · ' + row.version : row.name;
      const rows = [];
      if (row.family === 'heuristic') {
        rows.push(entry('published wording', link('<em>“' + escape(row.official_claim.text) + '”</em>', row.official_claim.url) +
          (row.hover.claim_note ? ' (' + escape(row.hover.claim_note) + ')' : '')));
      } else {
        rows.push(entry(row.bits_kind === 'claimed' ? 'claimed bound' : 'audited bound', 'ε ≤ ' + escape(row.bound)));
      }
      rows.push(entry('SMHasher3', link(escape(verdict), smh.url)));
      rows.push(entry('speed', link(escape(format(measurement.value) + ' ' + axis.unit + ' · ' + measurement.source_short), measurement.url)));
      if (row.family === 'heuristic') {
        rows.push(entry('estimated cap / upper bound', escape(row.score.display_text + ' bits') +
          '<span class="tooltip-note">' + escape('pair: ' + row.pair_lengths_bytes.join('/') + ' B · L=' + row.pair_length_words +
          ' · key/output: ' + row.key_bits + '/' + row.output_bits + ' bits') + '</span>'));
        rows.push(entry('collision evidence', escape(row.collision.display)));
        rows.push(entry('mechanism', escape(row.mechanism)));
        rows.push(entry('evidence scope', escape(row.qualification)));
      } else {
        rows.push(row.bits_kind === 'claimed'
          ? entry('score (claimed)', escape(format(row.bits_claimed) + ' bits — published argument has a gap, ') + '<a href="#proof-gaps">see audit</a>')
          : entry('collision score', escape(row.score.display_text + ' bits; minimum at L=' + row.score_minimizer_L)));
        rows.push(entry('proof status', escape(row.proof_status)));
        rows.push(entry('scope', escape(row.domain)));
      }
      if (row.key_model) rows.push(entry('key model', escape(row.key_model)));
      if (row.domain_short) rows.push(entry('domain', escape(row.domain_short)));
      if (row.masking) rows.push(entry('masking', escape(row.masking.label)));
      if (measurement.backend_note) rows.push(entry('backend', escape(measurement.backend_note)));
      if (measurement.registered_name) rows.push(entry('timed variant', escape(measurement.registered_name +
        ' · run ' + measurement.selected_run + ' · binary ' + measurement.binary_sha256.slice(0, 12))));
      if (row.speed_note) rows.push(entry('timing scope', escape(row.speed_note)));
      return '<strong class="tooltip-name">' + escape(name) + '</strong><small class="tooltip-family">' + escape(row.mechanism_family) + '</small>' +
        (row.rurban_alias ? '<small class="tooltip-alias">rurban: ' + escape(row.rurban_alias) + '</small>' : '') + '<dl>' + rows.join('') +
        '</dl><a class="tooltip-section" href="#' + escape(row.anchor) + '">→ section</a>';
    }

    function showCard(point) {
      if (!point) return;
      clearTimeout(hideTimer);
      if (hoveredPoint !== point || card.hidden) {
        hoveredPoint?.node.setAttribute('aria-expanded', 'false');
        hoveredPoint = point;
        card.innerHTML = tooltip(point.row, axes[active]);
        card.hidden = false;
        point.node.setAttribute('aria-expanded', 'true');
      }
      positionCard();
    }
    function positionCard() {
      if (card.hidden || !hoveredPoint || phone()) return;
      const plot = chart.getBoundingClientRect();
      const figure = chart.parentElement.getBoundingClientRect();
      const x = plot.left - figure.left + hoveredPoint.px;
      const y = plot.top - figure.top + hoveredPoint.py;
      const right = Math.min(figure.width, innerWidth - figure.left) - 8;
      const left = Math.max(8, 8 - figure.left);
      let cx = x + 14;
      if (cx + card.offsetWidth > right) cx = x - card.offsetWidth - 14;
      let cy = y + 14;
      if (figure.top + cy + card.offsetHeight > innerHeight - 8) cy = y - card.offsetHeight - 14;
      card.style.left = Math.max(left, Math.min(cx, right - card.offsetWidth)) + 'px';
      card.style.top = Math.max(8 - figure.top, cy) + 'px';
    }
    window.addEventListener('scroll', positionCard, {passive: true});
    function jump(anchor) {
      const section = document.getElementById(anchor);
      if (!section) return;
      window.location.hash = anchor;
      section.setAttribute('tabindex', '-1');
      section.focus({preventScroll: true});
    }

    const label = (row, mobile) => {
      if (row.id.startsWith('chain')) return 'ChainHash (ours)\n' + row.name.split(', ')[1];
      const lines = [''];
      (row.label + (row.bits_kind === 'measured' ? '*' : '')).split(/\s+/).forEach(word => {
        if (lines.at(-1).length + word.length + 1 > (mobile ? 17 : 25) && lines.at(-1)) lines.push('');
        lines[lines.length - 1] += (lines.at(-1) ? ' ' : '') + word;
      });
      return lines.join('\n');
    };
    const overlaps = (a, b, pad = 1.5) => a.x < b.x + b.w + pad && a.x + a.w + pad > b.x &&
      a.y < b.y + b.h + pad && a.y + a.h + pad > b.y;
    function rect(node) {
      const box = node.getBoundingClientRect(), origin = svg.getBoundingClientRect();
      return {x: box.x - origin.x, y: box.y - origin.y, w: box.width, h: box.height};
    }

    // Measure the actual Plot.text glyphs before packing. Alternate the first
    // dx/dy by nearest-neighbour distance, then search nearby gaps. Only labels
    // move: coincident hashes keep their exact scientific coordinates.
    function placeLabels(width, height, margins) {
      const obstacles = [...svg.querySelectorAll('g[aria-label="y-axis tick label"] text, g[aria-label="x-axis tick label"] text, .x-title text, g[aria-label="y-axis label"] text, .reference-label text, .series-legend text')].map(rect);
      const markers = points.map(point => rect(point.node));
      const bounds = {left: margins.left + 4, right: width - 8, top: phone() ? 50 : margins.top - 18, bottom: height - margins.bottom + 20};
      points.forEach((point, index) => {
        point.index = index;
        const b = point.labelNode.getBBox();
        point.box = {x: b.x, y: b.y, w: b.width, h: b.height};
        const nearest = points.filter(other => other !== point).sort((a, b) =>
          Math.hypot(point.px - a.px, point.py - a.py) - Math.hypot(point.px - b.px, point.py - b.py))[0];
        point.nearest = nearest ? Math.hypot(point.px - nearest.px, point.py - nearest.py) : Infinity;
        const vertical = !nearest || Math.abs(point.px - nearest.px) >= Math.abs(point.py - nearest.py);
        point.preferred = point.nearest < 90 ? (vertical ? (index % 2 ? [0, 1] : [0, -1]) : (point.px < nearest.px ? [-1, 0] : [1, 0])) : [[0, -1], [0, 1], [-1, 0], [1, 0]][index % 4];
      });
      // Precompute marker-free candidates once; retries only change packing
      // order. Crowded labels prefer alternating sides of their neighbours.
      const choices = new Map(points.map(point => {
        const {w, h} = point.box, [dx, dy] = point.preferred;
        const preferredX = point.px - w / 2 + dx * (w / 2 + 10);
        const preferredY = point.py - h / 2 + dy * (h / 2 + 10);
        const candidates = [];
        const consider = (x, y) => {
          if (x < bounds.left || x + w > bounds.right || y < bounds.top || y + h > bounds.bottom) return;
          const box = {x, y, w, h};
          if (obstacles.some(other => overlaps(box, other)) || markers.some(other => overlaps(box, other, 4))) return;
          const cost = Math.hypot(x + w / 2 - point.px, y + h / 2 - point.py) ** 2 +
            Math.hypot(x - preferredX, y - preferredY) * 0.15;
          candidates.push({...box, cost});
        };
        consider(preferredX, preferredY);
        for (let distance = 10; distance <= 150; distance += 8) {
          for (const [sx, sy] of [[0, -1], [0, 1], [-1, 0], [1, 0], [-1, -1], [1, 1], [1, -1], [-1, 1]]) {
            consider(point.px - w / 2 + sx * (w / 2 + distance), point.py - h / 2 + sy * (h / 2 + distance));
          }
        }
        for (let y = bounds.top; y <= bounds.bottom - h; y += 4) {
          for (let x = bounds.left; x <= bounds.right - w; x += 4) consider(x, y);
        }
        return [point, candidates.sort((a, b) => a.cost - b.cost)];
      }));
      let packed, bestScore = Infinity;
      // On narrow screens a different packing order prevents small labels
      // from consuming the only slots large enough for two-line labels.
      for (let attempt = 0; attempt < 80; attempt++) {
        const priority = point => attempt === 0 ? -point.nearest : attempt === 1 ? point.box.w * point.box.h :
          point.box.w * point.box.h * (0.3 + ((Math.imul(point.index + 1, 2654435761) ^ Math.imul(attempt, 1597334677)) >>> 0) / 4294967296);
        const order = [...points].sort((a, b) => priority(b) - priority(a) || a.index - b.index);
        const placed = new Map();
        let score = 0;
        for (const point of order) {
          const candidates = choices.get(point);
          let box = candidates.find(candidate => [...placed.values()].every(other => !overlaps(candidate, other)));
          if (!box) {
            // Keep every point labelled even in unusually small containers.
            box = candidates.reduce((best, candidate) => {
              const penalty = [...placed.values()].filter(other => overlaps(candidate, other)).length * 1e9 + candidate.cost;
              return !best || penalty < best.penalty ? {...candidate, penalty} : best;
            }, null);
            score += box?.penalty || 1e9;
          }
          if (!box) box = {x: bounds.left, y: bounds.top, w: point.box.w, h: point.box.h};
          placed.set(point, box);
          score += box.cost || 0;
        }
        if (score < bestScore) {packed = placed; bestScore = score;}
        if (score < 1e9 && (!phone() || points.length < 12)) break;
      }
      for (const [point, box] of packed) {
        point.labelX = box.x - point.box.x;
        point.labelY = box.y - point.box.y;
        const targetX = Math.max(box.x, Math.min(point.px, box.x + box.w));
        const targetY = Math.max(box.y, Math.min(point.py, box.y + box.h));
        const distance = Math.hypot(targetX - point.px, targetY - point.py);
        point.guide = distance > 16;
        point.guideX = targetX; point.guideY = targetY;
        point.guideStartX = point.px + (targetX - point.px) * 7 / Math.max(1, distance);
        point.guideStartY = point.py + (targetY - point.py) * 7 / Math.max(1, distance);
      }
    }

    const pixels = value => ({value, scale: null});
    function labelMarks() {
      return [false, true].map(bold => Plot.text(points.filter(point => point.bold === bold), {
        x: pixels('labelX'), y: pixels('labelY'), text: 'text',
        textAnchor: 'start', lineAnchor: 'top', lineHeight: 1.05,
        fontWeight: bold ? 700 : 400, fontSize: phone() ? 13 : 14,
        fill: colors.ink, className: bold ? 'hash-labels-bold' : 'hash-labels',
      }));
    }
    function attachIdentities(interactive) {
      for (const [name, selected] of [
        ['points-proven', points.filter(point => point.row.family === 'proven' && point.row.bits_kind !== 'claimed')],
        ['points-claimed', points.filter(point => point.row.bits_kind === 'claimed')],
        ['points-heuristic', points.filter(point => point.row.family === 'heuristic' && !point.row.key_free)],
        ['points-key-free', points.filter(point => point.row.key_free)],
      ]) {
        [...(svg.querySelector(`.${name}`)?.children || [])].forEach((node, i) => {
          const point = selected[i]; point.node = node;
          node.dataset.pointId = point.row.id;
          node.dataset.speed = point.value; node.dataset.bits = point.row.bits;
          if (!interactive) return;
          node.setAttribute('tabindex', '0'); node.setAttribute('role', 'link');
          node.setAttribute('aria-label', point.row.name + ': ' + point.row.score.display_text + ' bits; ' + format(point.value) + ' ' + axes[active].unit + '. Jump to section.');
          node.setAttribute('aria-controls', card.id); node.setAttribute('aria-expanded', 'false');
          node.addEventListener('focus', () => {if (!suppressFocus) showCard(point);});
          node.addEventListener('blur', deferHide);
          node.addEventListener('keydown', event => {
            if (event.key === 'Enter') {event.preventDefault(); hideCard(); jump(point.row.anchor);}
            else if (event.key === ' ') {
              event.preventDefault(); showCard(point); card.querySelector('.tooltip-section').focus({preventScroll: true});
            }
          });
        });
      }
      for (const bold of [false, true]) {
        const selected = points.filter(point => point.bold === bold);
        [...(svg.querySelector(`.hash-labels${bold ? '-bold' : ''}`)?.children || [])].forEach((node, i) => {
          selected[i].labelNode = node; node.dataset.labelId = selected[i].row.id;
        });
      }
    }

    function draw() {
      hideCard();
      const mobile = phone(), axis = axes[active];
      const width = chart.clientWidth, height = mobile ? 620 : 520;
      if (!width) return;
      const margins = {left: mobile ? 46 : 58, right: 14, top: 86, bottom: 64};
      const chartRows = HASH_POINTS.flatMap(row => row.style_speeds
        ? Object.entries(row.style_speeds).map(([name, hosts]) => ({
          ...row, id: row.id + '-' + name.split('-').at(-1), name: name + ' (64-bit output)',
          label: 'HalftimeHash ' + name.split('-').at(-1),
          speeds: Object.fromEntries(Object.entries(row.speeds).map(([key, measurement]) => [key, {
            ...measurement,
            value: key === 'smh_m2_bulk_Bpc' ? hosts.M2Pro.bulk_bytes_per_cycle :
              key === 'smh_xeon_bulk_Bpc' ? hosts.Xeon8375C.bulk_bytes_per_cycle : measurement.value,
            source_short: key === 'smh_m2_bulk_Bpc' ? axes[0].label : axes[1].label,
            record: 'data.json#/proven/' + HASH_POINTS.filter(point => point.family === 'proven').findIndex(point => point.id === row.id) + '/style_speeds/' + name,
            url: 'data.json',
          }]))
        })) : [row]);
      const rows = chartRows.filter(row => row.chart_eligible && Number.isFinite(row.bits) && row.bits >= 0 && row.bits <= 88 && Number.isFinite(row.speeds[axis.key].value) && row.speeds[axis.key].value > 0);
      const scale = axis.type === 'log' ? Math.log10 : value => value;
      const unscale = axis.type === 'log' ? value => 10 ** value : value => value;
      const [min, max] = d3.extent(rows, row => scale(row.speeds[axis.key].value));
      const padding = (max - min) * 0.16;
      baseDomain = (axis.type === 'log' ? [min - padding, max + padding] : [max + padding, min - padding]).map(unscale);
      const domain = viewDomain || baseDomain;
      xScale = (axis.type === 'log' ? d3.scaleLog() : d3.scaleLinear()).domain(domain).range([margins.left, width - margins.right]);
      const yScale = d3.scaleLinear().domain([0, 88]).range([height - margins.bottom, margins.top]);
      points = rows.map(row => {
        const value = row.speeds[axis.key].value;
        return {row, value, px: xScale(value), py: yScale(row.bits), text: label(row, mobile), bold: row.id.startsWith('chain')};
      }).filter(point => point.px >= margins.left && point.px <= width - margins.right);
      points.forEach(point => {point.labelX = point.px; point.labelY = point.py;});
      const ticksX = axis.type === 'log' ? [0.1, 0.2, 0.5, 1, 2, 5, 10, 20, 50, 100, 200, 500, 1000].filter(value => xScale(value) >= margins.left + 12 && xScale(value) <= width - margins.right) : xScale.ticks(mobile ? 4 : 7);
      const legend = families.map((family, index) => ({text: family.name, x: width - 12, y: 14 + index * 22}));
      const axisTitle = (mobile ? axis.compact : axis.label) + '\n' + (axis.type === 'linear' && !mobile ? 'lower is better · reversed axis · ' : '') + 'faster → right';
      const spec = () => ({
        width, height, marginLeft: margins.left, marginRight: margins.right, marginTop: margins.top, marginBottom: margins.bottom,
        style: {fontFamily: css.fontFamily, fontSize: '14px', color: colors.ink, background: colors.surface},
        offset: 0, axis: null,
        x: {type: axis.type, domain, nice: false, label: axisTitle},
        y: {domain: [0, 88], nice: false, label: 'Collision score (bits)'},
        marks: [
          Plot.gridX({ticks: ticksX, stroke: colors.grid, strokeOpacity: 1}),
          Plot.gridY({ticks: ticksY, stroke: colors.grid, strokeOpacity: 1}),
          Plot.ruleY([64], {stroke: colors.muted, strokeDasharray: '2,4', strokeWidth: 1}),
          Plot.axisX({ticks: ticksX, tickSize: 0, tickPadding: 10, tickFormat: value => format(value), label: null, color: colors.muted}),
          Plot.axisY({ticks: ticksY, tickSize: 0, tickPadding: 8, labelAnchor: 'center', labelArrow: false, labelOffset: mobile ? 33 : 42, color: colors.muted}),
          Plot.text([{text: axisTitle, x: (margins.left + width - margins.right) / 2, y: height - 31}], {
            x: pixels('x'), y: pixels('y'), text: 'text', lineAnchor: 'top', lineHeight: 1.1, fill: colors.ink, className: 'x-title',
          }),
          Plot.text([{text: 'ideal for a 64-bit output', x: margins.left, y: yScale(64) - 9}], {
            x: pixels('x'), y: pixels('y'), text: 'text', textAnchor: 'start', lineAnchor: 'bottom', fill: colors.muted, className: 'reference-label',
          }),
          Plot.text(legend, {x: pixels('x'), y: pixels('y'), text: 'text', textAnchor: 'end', fill: colors.ink, className: 'series-legend'}),
          ...families.map((family, index) => Plot.dot([{x: width - 24 - measureText(family.name), y: 14 + index * 22}], {
            x: pixels('x'), y: pixels('y'), r: 5, symbol: family.key === 'heuristic' ? diamond : 'circle', fill: family.key === 'claimed' ? colors.surface : colors[family.color], stroke: family.key === 'claimed' ? colors.proven : '#fff', strokeWidth: 2, paintOrder: 'stroke', ariaHidden: true,
          })),
          Plot.link(points.filter(point => point.guide), {
            x1: pixels('guideStartX'), y1: pixels('guideStartY'), x2: pixels('guideX'), y2: pixels('guideY'),
            stroke: colors.muted, strokeOpacity: 0.4, strokeWidth: 0.75, className: 'label-guides',
          }),
          Plot.dot(points.filter(point => point.row.family === 'proven' && point.row.bits_kind !== 'claimed'), {
            x: 'value', y: point => point.row.bits, r: 5, fill: colors.proven, stroke: '#fff', strokeWidth: 2, paintOrder: 'stroke', className: 'points-proven',
          }),
          Plot.dot(points.filter(point => point.row.bits_kind === 'claimed'), {
            x: 'value', y: point => point.row.bits, r: 5, fill: colors.surface, stroke: colors.proven, strokeWidth: 2, className: 'points-claimed',
          }),
          Plot.dot(points.filter(point => point.row.family === 'heuristic' && !point.row.key_free), {
            x: 'value', y: point => point.row.bits, r: 5, symbol: diamond, fill: colors.heuristic, stroke: '#fff', strokeWidth: 2, paintOrder: 'stroke', className: 'points-heuristic',
          }),
          Plot.dot(points.filter(point => point.row.key_free), {
            x: 'value', y: point => point.row.bits, r: 5, symbol: times, fill: colors.heuristic, stroke: '#fff', strokeWidth: 2, paintOrder: 'stroke', className: 'points-key-free',
          }),
          ...labelMarks(),
        ],
      });
      // Both passes use Plot.text. The first is hidden just long enough to get
      // real SVG text bounds; the final spec uses the packed pixel anchors.
      svg = Plot.plot(spec()); svg.style.visibility = 'hidden'; chart.replaceChildren(svg);
      attachIdentities(false); placeLabels(width, height, margins);
      svg = Plot.plot(spec()); chart.replaceChildren(svg); attachIdentities(true);
      svg.setAttribute('role', 'group'); svg.setAttribute('aria-label', 'Hash collision score by ' + axis.label);
      svg.classList.add('hash-plot');
      bindPointer(margins, height);
      chart.dataset.axis = axis.key;
      chart.dataset.points = points.length;
      lastWidth = width; lastHeight = height;
    }
    const measurementCanvas = document.createElement('canvas');
    const measurementContext = measurementCanvas.getContext('2d');
    function measureText(text) {
      measurementContext.font = '14px ' + css.fontFamily;
      return measurementContext.measureText(text).width;
    }
    function selectAxis(index) {
      if (index < 0) return;
      active = index; viewDomain = undefined; selector.value = axes[active].key;
      buttons.forEach((button, i) => button.setAttribute('aria-pressed', String(i === active)));
      draw();
    }

    function hit(event) {
      const labelId = event.target.closest('[data-label-id]')?.dataset.labelId;
      if (labelId) return points.find(point => point.row.id === labelId);
      // Use the nearest coordinate even if an overlapping SVG mark is on top.
      const [x, y] = d3.pointer(event, svg);
      let closest, distance = 14;
      points.forEach(point => {
        const next = Math.hypot(point.px - x, point.py - y);
        if (next <= distance) {closest = point; distance = next;}
      });
      return closest;
    }
    function bindPointer(margins, height) {
      svg.addEventListener('pointermove', event => {
        if (drag) {
          drag.end = Math.max(margins.left, Math.min(lastWidth - margins.right, d3.pointer(event, svg)[0]));
          if (Math.abs(drag.end - drag.start) > 4) {
            drag.moved = true; hideCard();
            drag.selection.setAttribute('x', Math.min(drag.start, drag.end));
            drag.selection.setAttribute('width', Math.abs(drag.end - drag.start));
          }
          return;
        }
        if (event.pointerType === 'touch') return;
        const point = hit(event);
        svg.style.cursor = point ? 'pointer' : dragMode === 'pan' ? 'grab' : dragMode === 'zoom' ? 'crosshair' : 'default';
        if (point) showCard(point); else deferHide();
      });
      svg.addEventListener('pointerleave', deferHide);
      svg.addEventListener('click', event => {
        if (suppressClick) {suppressClick = false; return;}
        const point = hit(event);
        if (!point) return;
        if (phone()) {point.node.focus({preventScroll: true}); showCard(point);}
        else {hideCard(); jump(point.row.anchor);}
      });
      svg.addEventListener('dblclick', event => {
        if (!hit(event)) {viewDomain = undefined; draw();}
      });
      svg.addEventListener('pointerdown', event => {
        if (event.button !== 0 || event.pointerType === 'touch' || hit(event)) return;
        const [x, y] = d3.pointer(event, svg);
        if (x < margins.left || x > lastWidth - margins.right || y < margins.top || y > height - margins.bottom) return;
        event.preventDefault(); svg.setPointerCapture(event.pointerId);
        const selection = document.createElementNS('http://www.w3.org/2000/svg', 'rect');
        selection.setAttribute('y', margins.top); selection.setAttribute('height', height - margins.top - margins.bottom);
        selection.setAttribute('class', 'chart-selection'); svg.append(selection);
        drag = {start: x, end: x, selection, moved: false};
      });
      svg.addEventListener('pointerup', () => {
        if (!drag) return;
        const gesture = drag; drag = undefined; gesture.selection.remove();
        if (!gesture.moved) return;
        suppressClick = true;
        if (dragMode === 'zoom') viewDomain = [Math.min(gesture.start, gesture.end), Math.max(gesture.start, gesture.end)].map(xScale.invert);
        else if (dragMode === 'pan') viewDomain = xScale.range().map(x => xScale.invert(x - (gesture.end - gesture.start)));
        draw();
        // A replacement SVG may mean the browser does not emit a click.
        setTimeout(() => {suppressClick = false;}, 0);
      });
      svg.addEventListener('pointercancel', () => {drag?.selection.remove(); drag = undefined;});
    }
    function zoomBy(factor) {
      const [left, right] = xScale.range(), center = (left + right) / 2;
      viewDomain = [left, right].map(x => xScale.invert(center + (x - center) * factor));
      draw();
    }
    function saveBlob(blob, extension) {
      const url = URL.createObjectURL(blob), link = document.createElement('a');
      link.href = url; link.download = 'bits-vs-speed.' + extension; link.click();
      setTimeout(() => URL.revokeObjectURL(url), 1000);
    }
    function download(type) {
      const copy = svg.cloneNode(true);
      copy.style.removeProperty('cursor');
      const blob = new Blob([new XMLSerializer().serializeToString(copy)], {type: 'image/svg+xml;charset=utf-8'});
      if (type === 'svg') {saveBlob(blob, 'svg'); return;}
      const url = URL.createObjectURL(blob), image = new Image();
      image.onload = () => {
        const canvas = document.createElement('canvas');
        canvas.width = lastWidth * 2; canvas.height = lastHeight * 2;
        const context = canvas.getContext('2d');
        context.fillStyle = colors.surface; context.fillRect(0, 0, canvas.width, canvas.height);
        context.drawImage(image, 0, 0, canvas.width, canvas.height);
        URL.revokeObjectURL(url); canvas.toBlob(result => {if (result) saveBlob(result, 'png');});
      };
      image.onerror = () => URL.revokeObjectURL(url);
      image.src = url;
    }
    selectAxis(0);
    const onResize = () => {
      cancelAnimationFrame(resizeFrame);
      resizeFrame = requestAnimationFrame(() => {
        if (lastWidth !== chart.clientWidth || lastHeight !== (phone() ? 620 : 520)) draw();
        else positionCard();
      });
    };
    new ResizeObserver(onResize).observe(chart);
    window.addEventListener('resize', onResize);
  }
  if (document.readyState === 'loading') document.addEventListener('DOMContentLoaded', init, {once: true});
  else init();
})();
