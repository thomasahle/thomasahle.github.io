"""Exact bitwise carry count for the Pair E class (MIT, 2026)."""
from collections import defaultdict
C2=0x7ab1006b26f9eb64
C3=0x21233394220b8457
T=0x5b9234ff04f56f3d
mask=0x21233494220c8459
value=0x0000008000008001
# State (carry in seed+C3, whether any mask constraint was violated).
# At bit i, test the target bit using both possible seed bits, then propagate carry.
states={(0,False):1}
for i in range(64):
    nxt=defaultdict(int)
    for (carry,bad),count in states.items():
        for bit in (0,1):
            total=bit+((C3>>i)&1)+carry
            if (((C2>>i)&1)^bit^(total&1)) != ((T>>i)&1):continue
            violated=bool((mask>>i)&1 and bit!=((value>>i)&1))
            nxt[(total>>1,bad or violated)]+=count
    states=nxt
count=sum(states.values())
assert count==2**43
assert not any(bad and n for (carry,bad),n in states.items())
assert bin(mask).count("1")==21 and value&~mask==0
# All solutions satisfy the mask and their count equals the mask class size.
# Hence the arithmetic condition and the mask define exactly the same class.
print(f"Exact Pair E class: {count} = 2^43 seeds; density 2^-21; mask equivalence PASS")
