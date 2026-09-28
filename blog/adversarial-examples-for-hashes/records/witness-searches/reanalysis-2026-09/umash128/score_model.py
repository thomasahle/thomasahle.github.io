#!/usr/bin/env python3
"""UMASH-128 score model: reproduce the published envelope E(L) and the
Lean coefficient-1 bound, then compute min_L log2(L/eps) on several domains.

Model (from the reference C, umash.c):
  - Two independent 61-bit multipliers f0, f1, uniform on the accepted set
    (mask to 61 bits; reject 0 and p); |accepted| = M.
  - Per lane, the polynomial recurrence is
      acc <- f^2*(acc + oh0) + f*oh1   (mod p = 2^61-1),
    so over B blocks a fixed pair's difference is a polynomial in f of
    degree <= 2B; a lane collides iff its f is a root.
  - Envelope constants taken verbatim from the post's exact-arithmetic
    certificate (certify_fingerprint.py).
"""
from fractions import Fraction as F
from decimal import Decimal, localcontext

Q = 1 << 64
P = (1 << 61) - 1
M = P - 2                       # multiplier_denominator in the certificate
C1, C2 = 3125, 2 * 604**2       # 604**2 = max PH-root count per lane block group
J = 345763417 * 4096            # 1416246956032
A = F(C1 + C2, Q - 561)
B = F(J, Q * (Q - 561))

def rho(L):
    return min(F(1), F(2 * ((L + 31) // 32), M))

def envelope(L):
    """Proven UPPER bound on eps for length L words (post's E(L))."""
    if L == 1:
        return F(1, Q * (Q - 561))
    r = rho(L)
    return B * (1 - r) ** 2 + A * r * (1 - r) + r * r

def lean_bound(L):
    """Machine-checked coefficient-1 bound: ceil(L/2^23)^2 / 2^83."""
    h = (L + (1 << 23) - 1) >> 23
    return F(h * h, 1 << 83)

def rounded_81_128(L):
    """Paper 'rounded quadratic': (81/128) * ceil(L/2^23)^2 / 2^83."""
    h = (L + (1 << 23) - 1) >> 23
    return F(81 * h * h, 128 * (1 << 83))

def log2(x):
    x = F(x)
    with localcontext() as ctx:
        ctx.prec = 60
        return Decimal(x.numerator).ln() / Decimal(2).ln() - Decimal(x.denominator).ln() / Decimal(2).ln()

def score(bound, L):
    return log2(F(L) / bound(L))

def min_score(bound, Lmax):
    """min over L in [1,Lmax] of log2(L/bound(L)), scanning the structural
    breakpoints (L=1,2, and step*2^e +/- small around each octave)."""
    cand = {1, 2, Lmax}
    for e in range(0, 66):
        for step in (32, 1 << 23):
            for d in (-1, 0, 1):
                v = step * (1 << e) + d
                if 1 <= v <= Lmax:
                    cand.add(v)
    best = None
    for L in sorted(cand):
        s = score(bound, L)
        if best is None or s < best[0]:
            best = (s, L)
    return best

domains = {
    "64 MiB  (L=2^23 words)": 1 << 23,
    "2^46 words (~512 TB)":   1 << 46,
    "full byte domain (2^61 words)": (1 << 61) - (1 << 23) + 1,
    "all lengths (saturation)":      (1 << 65),
}
print("=== envelope E(L)  (proven upper bound; the plotted lower bound) ===")
for name, Lmax in domains.items():
    s, L = min_score(envelope, Lmax)
    print(f"  {name:34s}  min score = {float(s):.6f}  at L={L}")

print("\n=== Lean coefficient-1 bound (machine-checked) ===")
for name, Lmax in domains.items():
    s, L = min_score(lean_bound, Lmax)
    print(f"  {name:34s}  min score = {float(s):.6f}  at L={L}")

print("\n=== paper 81/128 rounded-quadratic ===")
for name, Lmax in domains.items():
    s, L = min_score(rounded_81_128, Lmax)
    print(f"  {name:34s}  min score = {float(s):.6f}  at L={L}")

print("\n=== point comparison at the plotting cap L=2^46 ===")
L = 1 << 46
print(f"  envelope   eps = {float(envelope(L)):.6e}  score = {float(score(envelope,L)):.8f}")
print(f"  lean c=1   eps = {float(lean_bound(L)):.6e}  score = {float(score(lean_bound,L)):.8f}")
print(f"  81/128     eps = {float(rounded_81_128(L)):.6e}  score = {float(score(rounded_81_128,L)):.8f}")
print(f"  ratio lean/envelope eps = {float(lean_bound(L)/envelope(L)):.4f}  (= 2^{float(log2(lean_bound(L)/envelope(L))):.4f})")
