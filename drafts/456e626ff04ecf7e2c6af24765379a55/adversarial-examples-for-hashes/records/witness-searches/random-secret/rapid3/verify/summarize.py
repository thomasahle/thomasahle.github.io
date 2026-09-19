#!/usr/bin/env python3
"""Parse logs/*.txt ("count C / 2^N keys") and print per-run and pooled Garwood 95% intervals."""
import glob, math, os, re, sys
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from ci import garwood
runs = {}
for f in sorted(glob.glob(os.path.join(os.path.dirname(os.path.abspath(__file__)), "logs", "*.txt"))):
    txt = open(f).read()
    m = re.search(r"count (\d+) / 2\^(\d+) keys", txt)
    if not m: continue
    tag = os.path.basename(f)[:-4]
    runs[tag] = (int(m.group(1)), 2 ** int(m.group(2)))
def fmt(c, T, L):
    lo, hi = garwood(c)
    r = c / T
    return (f"{c:5d} / 2^{math.log2(T):.2f}  rate 2^{math.log2(r):.3f} [2^{math.log2(lo/T):.3f}, 2^{math.log2(hi/T):.3f}]"
            f"  cap(L={L}) {math.log2(L)-math.log2(r):.2f} [{math.log2(L)-math.log2(hi/T):.2f}, {math.log2(L)-math.log2(lo/T):.2f}]")
L = {"pairA": 4, "pair24": 3, "pairD": 6}
for tag, (c, T) in runs.items():
    name = tag.split("_")[1]
    print(f"{tag:28s} {fmt(c, T, L[name])}")
print()
groups = {
    "pair A, seed+secret random, matched sizes (2^30+2^34+2^35)": ["10_pairA_random_2p30", "20_pairA_random_2p34", "30_pairA_random_2p35"],
    "pair 24, seed+secret random, matched sizes (2^30+2^34+2^35)": ["11_pair24_random_2p30", "21_pair24_random_2p34", "31_pair24_random_2p35"],
    "pair A, seed+secret random, all splitmix runs incl. 2^37": ["10_pairA_random_2p30", "20_pairA_random_2p34", "30_pairA_random_2p35", "41_pairA_random_2p37"],
    "pair 24, seed+secret random, all splitmix runs incl. 2^37": ["11_pair24_random_2p30", "21_pair24_random_2p34", "31_pair24_random_2p35", "42_pair24_random_2p37"],
    "pair D, seed+secret random, 2^30+2^34": ["12_pairD_random_2p30", "40_pairD_random_2p34"],
}
for name, tags in groups.items():
    tags = [t for t in tags if t in runs]
    if not tags: continue
    c = sum(runs[t][0] for t in tags); T = sum(runs[t][1] for t in tags)
    print(f"{name}: {fmt(c, T, L[tags[0].split('_')[1]])}")
