# Upstream sources used by this lane

`xxhash.h` (searcher, `rs128.c`) and `xxhash.h` / `xxhash.c` (verifier, `verify/verify_rs128.c`,
`verify/witness_check.c`) are the unmodified upstream files of xxHash v0.8.3 (tag `v0.8.3`):
`xxhash.h` sha256 `17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`,
`xxhash.c` sha256 `5c3591fe6e6c86a619eb26760e9520e37a6fd5152882ab5ad93f912e2a855966`. They are not
copied into this record; fetch them beside the programs before building:

    curl -sSL -o xxhash.h https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    curl -sSL -o xxhash.c https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.c
