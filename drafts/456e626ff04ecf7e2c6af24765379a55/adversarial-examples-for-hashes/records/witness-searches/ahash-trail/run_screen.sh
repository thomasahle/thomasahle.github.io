#!/bin/bash
# usage: run_screen.sh pairs.txt log2N_per_proc seedbase outprefix   (8 processes on cores 24-31)
set -e
P=$1; L=$2; S=$3; O=$4
for i in 0 1 2 3 4 5 6 7; do
  nice -n 10 taskset -c $((24+i)) ./ahash_trail_sample $P $L $((S+i)) > $O.$i.log 2>&1 &
done
wait
python3 - "$O" <<'PY'
import sys,glob,math
o=sys.argv[1]; tot={}; N=0
for fn in sorted(glob.glob(o+'.*.log')):
    for line in open(fn):
        if line.startswith('#'):
            N+=2**int(line.split('2^')[1].split(';')[0]); continue
        lab,c,lg,_=line.split(); tot[lab]=tot.get(lab,0)+int(c)
print("pooled N = 2^%.3f = %d"%(math.log2(N),N))
for lab,c in sorted(tot.items(),key=lambda t:-t[1]):
    if c==0: print("%-24s %8d  rate  -inf"%(lab,c)); continue
    r=c/N; lo=r-1.96*math.sqrt(c)/N; hi=r+1.96*math.sqrt(c)/N
    print("%-24s %8d  log2 rate %.3f  95%% [%.3f, %.3f]"%(lab,c,math.log2(r),math.log2(lo),math.log2(hi)))
PY
