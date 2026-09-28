# Upstream source

Every program here includes the unmodified `xxhash.h` of xxHash v0.8.3 (tag `v0.8.3`), sha256
`17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`, built with `XXH_INLINE_ALL`.
It is not copied into this record; fetch it beside the programs before building:

    curl -sSL -o xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    shasum -a 256 xxhash.h    # 17973c0d...5e4b

The search programs check the SMHasher3 verification values (XXH3-64 `1AAEE62C`, XXH3-128
`288DAA94`) and `XXH_versionNumber() == 803` before measuring; the verifiers check
`XXH3_64bits("") = 2d06800538d394c2`.
