# Upstream sources used by this verification lane

`upstream/rapidhash_v3.h` is `rapidhash.h` at tag `rapidhash_v3` of github.com/Nicoshev/rapidhash,
sha256 `807874eeb23339eaa4b435fbadd33d46718c88e32f3eb67bab3eb965eecc626e`, included verbatim by
`verify_rapid3.c`. It is not copied into this record:

    mkdir -p upstream
    curl -sSL -o upstream/rapidhash_v3.h https://raw.githubusercontent.com/Nicoshev/rapidhash/rapidhash_v3/rapidhash.h
    sha256sum upstream/rapidhash_v3.h
