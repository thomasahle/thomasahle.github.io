# Sources

| file | origin | pin | sha256 |
|---|---|---|---|
| `umash.c`, `umash.h`, `umash_long.inc` (not copied) | https://github.com/backtrace-labs/umash | 9709e1123c753e7ef34bc560cbdefac6fd33fe47 | umash.c `aad7d50dcca9d8939a67452788fe7028357a76b125ee7f11d50b622269789409`, umash.h `a0a2a69bdd59a172fd090a7f321d94eecf718b64b63a6ef5962f31c409401291` |

    for f in umash.c umash.h umash_long.inc; do
      curl -LO https://raw.githubusercontent.com/backtrace-labs/umash/9709e1123c753e7ef34bc560cbdefac6fd33fe47/$f; done

`solver/` is the program that found the key (it expects the three files in `../upstream/` or `$UMASH_DIR`).
