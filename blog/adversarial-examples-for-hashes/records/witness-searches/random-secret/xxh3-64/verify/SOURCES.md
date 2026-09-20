# Upstream sources used by this verification lane

`xxhash.h` is the unmodified upstream header of xxHash v0.8.3 (tag `v0.8.3`), sha256
`17973c0dc49d9854ca26caa191f0e12f7a424b68858d9a78de3860d959d85e4b`; it is not copied into this
record. Fetch it beside `xxh3_verify.c` before building:

    curl -sSLO https://raw.githubusercontent.com/Cyan4973/xxHash/v0.8.3/xxhash.h
    sha256sum xxhash.h
