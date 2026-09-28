#!/usr/bin/env python3
"""Construct an explicit UMASH-128 collision on the reference implementation.

Medium (9..16 byte) path: for a fixed message pair and OH/ENH keys, lane i of
the fingerprint outputs finalize(f_i^2 * x_i + f_i * y_i mod 8p), where 8p =
2^64-8 and (x_i,y_i) are lane i's Horner input words (from umash_words).  Lane i
collides for multiplier f_i iff

        f_i^2 * dx_i + f_i * dy_i == 0   (mod 8p),   dx_i,dy_i = word diffs.

Since gcd(8,p)=1 (p=2^61-1), CRT splits this into mod p (a single nonzero root
f* = -dy/dx) and mod 8 (a residue condition).  The accepted multiplier set is
[1,p-1], which pins f to f* mod p; the pair collides on lane i iff f* also meets
the mod-8 condition.  We randomise (messages, keys, seed) until BOTH lanes' f*
qualify, then hand the explicit f0,f1 to umash_verify (the real hash)."""
import os, random, subprocess, sys

HERE = os.path.dirname(os.path.abspath(__file__))
P = (1 << 61) - 1
EIGHTP = (1 << 64) - 8            # = 8*p
CC = os.environ.get("CC", "cc")

UMASH_DIR = os.environ.get("UMASH_DIR", os.path.join(HERE, "..", "upstream"))  # umash.c / umash.h at 9709e11

def build():
    # umash_words.c #includes umash.c directly (needs the static inlines).
    subprocess.run([CC, "-O2", "-std=c11", "-I", UMASH_DIR,
                    os.path.join(HERE, "umash_words.c"),
                    "-o", os.path.join(HERE, "umash_words")], check=True)
    # umash_verify.c uses the public API; link umash.c.
    subprocess.run([CC, "-O2", "-std=c11", "-I", UMASH_DIR,
                    os.path.join(HERE, "umash_verify.c"),
                    os.path.join(UMASH_DIR, "umash.c"),
                    "-o", os.path.join(HERE, "umash_verify")], check=True)

def words(msg16, oh0, oh1, lrc0, lrc1, seed, n):
    hexmsg = msg16.hex()
    r = subprocess.run([os.path.join(HERE, "umash_words"), hexmsg,
                        f"{oh0:x}", f"{oh1:x}", f"{lrc0:x}", f"{lrc1:x}", f"{seed:x}", str(n)],
                       capture_output=True, text=True, check=True)
    return [int(t, 16) for t in r.stdout.split()]

def lane_root(dx, dy):
    """Return f in [1,p-1] colliding this lane, or None. dx,dy are mod-8p diffs."""
    dxp, dyp = dx % P, dy % P
    if dxp == 0:
        return None                              # degenerate; skip
    fstar = (-dyp * pow(dxp, P - 2, P)) % P       # unique mod-p root, f != 0
    if fstar == 0:
        return None
    # mod-8 condition at f = fstar
    if (fstar * fstar * (dx % 8) + fstar * (dy % 8)) % 8 != 0:
        return None
    # full check mod 8p
    if (fstar * fstar * dx + fstar * dy) % EIGHTP != 0:
        return None
    return fstar

def try_one(rng):
    n = 16
    m1 = bytes(rng.randrange(256) for _ in range(16))
    # differ in a few bytes
    m2 = bytearray(m1)
    for _ in range(rng.randint(1, 6)):
        i = rng.randrange(16); m2[i] ^= rng.randrange(1, 256)
    m2 = bytes(m2)
    if m1 == m2:
        return None
    oh0 = rng.randrange(1 << 64); oh1 = rng.randrange(1 << 64)
    lrc0 = rng.randrange(1 << 64); lrc1 = rng.randrange(1 << 64)
    seed = rng.randrange(1 << 64)
    w1 = words(m1, oh0, oh1, lrc0, lrc1, seed, n)
    w2 = words(m2, oh0, oh1, lrc0, lrc1, seed, n)
    dx0 = (w1[0] - w2[0]) % EIGHTP; dy0 = (w1[1] - w2[1]) % EIGHTP
    dx1 = (w1[2] - w2[2]) % EIGHTP; dy1 = (w1[3] - w2[3]) % EIGHTP
    f0 = lane_root(dx0, dy0); f1 = lane_root(dx1, dy1)
    if f0 is None or f1 is None:
        return None
    return dict(m1=m1, m2=m2, n=n, f0=f0, f1=f1,
                oh0=oh0, oh1=oh1, lrc0=lrc0, lrc1=lrc1, seed=seed)

def verify(s):
    r = subprocess.run([os.path.join(HERE, "umash_verify"), s["m1"].hex(), s["m2"].hex(),
                        str(s["n"]), f"{s['f0']:x}", f"{s['f1']:x}",
                        f"{s['oh0']:x}", f"{s['oh1']:x}", f"{s['lrc0']:x}", f"{s['lrc1']:x}",
                        f"{s['seed']:x}"], capture_output=True, text=True)
    return r.returncode, r.stdout.strip()

def main():
    build()
    rng = random.Random(int(sys.argv[1]) if len(sys.argv) > 1 else 20260924)
    tries = 0
    while True:
        tries += 1
        s = try_one(rng)
        if s is None:
            continue
        rc, out = verify(s)
        print(f"[found after {tries} tries]")
        print(f"  M1   = {s['m1'].hex()}")
        print(f"  M2   = {s['m2'].hex()}")
        print(f"  f0   = {s['f0']}  (0x{s['f0']:x})")
        print(f"  f1   = {s['f1']}  (0x{s['f1']:x})")
        print(f"  oh0  = 0x{s['oh0']:016x}  oh1 = 0x{s['oh1']:016x}")
        print(f"  lrc0 = 0x{s['lrc0']:016x}  lrc1= 0x{s['lrc1']:016x}  seed=0x{s['seed']:016x}")
        print("  real umash_fprint says:")
        for line in out.splitlines():
            print("    " + line)
        print(f"  verifier rc = {rc} ({'COLLISION CONFIRMED' if rc==0 else 'FAILED'})")
        if rc == 0:
            break

if __name__ == "__main__":
    main()
