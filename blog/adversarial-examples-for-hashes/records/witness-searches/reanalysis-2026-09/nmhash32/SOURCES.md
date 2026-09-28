# Sources

| file | origin | pin | sha256 |
|---|---|---|---|
| `nmhash.h` (not copied; fetch it next to the programs) | https://raw.githubusercontent.com/gzm55/hash-garage/e022156ca866edc46e1aa79b2b6a466822b8b767/nmhash.h | hash-garage e022156ca8 (the post's pinned nmhash32 v2 source; verification 0x12A30553) | `3dccf61d2c81104f34d7cff5cc7a33686a998aeefe5d139fbcec13231e48c641` |

    curl -LO https://raw.githubusercontent.com/gzm55/hash-garage/e022156ca866edc46e1aa79b2b6a466822b8b767/nmhash.h
    shasum -a 256 nmhash.h

The search runs in `logs/*_xeon.log` used SMHasher3's scalar port of the same function
(hashes/nmhash.cpp, verification 0x12A30553); `nmhash32_mc.cpp` here includes the upstream header instead
and reproduces the same counts (`logs/exh512_m2.txt`, and the published pair's exhaustive count in `pair` mode).
