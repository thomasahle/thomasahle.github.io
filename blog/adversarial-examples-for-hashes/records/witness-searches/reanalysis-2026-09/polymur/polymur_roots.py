# Roots of the PolymurHash difference polynomial for the 8-byte vs 7-byte pair,
# and which roots lie in the key set K reachable by polymur_init_params.
from sympy import Poly, symbols, GF, factorint
p = 2**61 - 1
M1 = bytes.fromhex("2c336f948f6f6c99"); M2 = bytes.fromhex("aa373a8eeff85a")
le = lambda b: int.from_bytes(b, "little")
m0 = le(M1[:7])            # load(buf) & 2^56-1 ; m1 = same (offset (8-7)/2 = 0)
m2 = le(M1) >> 8           # load(buf+len-8) >> 8
mm = le(M2)                # 7-byte load
x = symbols("x")
H8 = (x**2 + m0) * (x**7 + m0) + (x + m2) * (x**3 + 8)
H7 = (x + mm) * (x**2 + 7)
D = Poly(H8 - H7, x, modulus=p)
print("degree", D.degree())
# roots in F_p: gcd with x^p - x computed by repeated squaring mod D
def polypowmod(e):
    r = Poly(1, x, modulus=p); b = Poly(x, x, modulus=p)
    while e:
        if e & 1: r = (r * b).rem(D)
        b = (b * b).rem(D); e >>= 1
    return r
g = D.gcd(polypowmod(p) - Poly(x, x, modulus=p))
print("number of distinct roots in F_p:", g.degree())
roots = [int(r) % p for r in g.ground_roots().keys()]
facs = list(factorint(p - 1).keys())
print("p-1 primes", facs)
M = 2**64 - 1
def red(v): return ((v & M) & p) + ((v >> 61) & M) if v < 2**128 else None
def red611(v): return (v & p) + (v >> 61)
def xr(v): return (v & p) + (v >> 61)
kstar = 391303342709703922
for r in sorted(roots):
    gen = all(pow(r, (p - 1) // q, p) != 1 for q in facs)
    k = xr(r)
    k2 = xr(red611(k * k)); k3 = red611(k * k2); k4 = red611(k2 * k2); k7 = xr(red611(k3 * k4))
    ok7 = k7 < (1 << 60) - (1 << 56)
    print(f"root {r}: generator={gen} k7={k7} k7_ok={ok7} in_K={gen and ok7} is_k*={r == kstar}")
    assert D.eval(r) % p == 0
