#!/usr/bin/env python3
"""Structured candidate (o, w0, w1) blocks around the fairness-pass pair (K0, ~K1)."""
import sys
sec = bytes.fromhex(
 "b8fe6c3923a44bbe7c01812cf721ad1cded46de9839097db7240a4a4b7b3671fcb79e64eccc0e578825ad07dccff7221"
 "b8084674f743248ee03590e6813a264c3c2852bb91c300cb88d0658b1b532ea371644897a20df94e3819ef46a9deacd8"
 "a8fa763fe39c343ff9dcbbc7c70b4f1d8a51e04bcdb45931c89f7ec9d9787364eac5ac8334d3ebc3c581a0fffa1363eb"
 "170ddd51b7f0da49d3165526 29d4689e2b16be587d47a1fc8ff8b8d17ad031ce45cb3a8f95160428afd7fbcabb4b407e".replace(" ",""))
assert len(sec)==192
K=[int.from_bytes(sec[8*i:8*i+8],'little') for i in range(24)]
M=(1<<64)-1
out=set()
def add(o,w0,w1,tag): out.add((o,w0&M,w1&M,tag))
fam=sys.argv[1] if len(sys.argv)>1 else "all"
for o in range(8):
    ka,kb=K[2*o],K[2*o+1]
    add(o,ka,~kb,"base")
    add(o,ka,kb,"same-sign")          # (k, k') vs complement, control
for o in (0,1):
    ka,kb=K[2*o],K[2*o+1]
    if fam in("all","xor"):
        for i in range(64):
            add(o,ka^(1<<i),~kb,"u1")
            add(o,ka,(~kb)^(1<<i),"v1")
            for j in range(64):
                add(o,ka^(1<<i),(~kb)^(1<<j),"uv1")
    if fam in("all","add"):
        for t in range(-2048,2049):
            add(o,ka+t,~(kb-t),"t")   # shifted-seed family: constants (ka+t, kb-t)
        for j in range(64):
            for sgn in (1,-1):
                add(o,ka+sgn*(1<<j),~(kb-sgn*(1<<j)),"t2j")
        for t in range(-256,257):
            add(o,ka+t,~kb,"a0")      # move w0 only
            add(o,ka,~kb+t,"a1")      # move w1 only
for (o,w0,w1,tag) in sorted(out):
    print(f"{o} {w0:016x} {w1:016x} {tag}")
