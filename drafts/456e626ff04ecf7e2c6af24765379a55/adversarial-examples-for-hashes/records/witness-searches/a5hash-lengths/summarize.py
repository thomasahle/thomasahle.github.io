# Parse logs/<len>_<v>.log into one table sorted by score.
import re, glob, os, sys, math
rows=[]
for f in sorted(glob.glob(os.path.join(sys.argv[1] if len(sys.argv)>1 else 'logs','*_*.log'))):
    b=os.path.basename(f)[:-4]
    if not re.match(r'^\d+_\d+$',b): continue
    L,v=map(int,b.split('_')); s=open(f).read()
    m=re.search(r'(EXACT|BEAM) mode: W\* = (0x[0-9a-f]+)\s+weight (\d+).*?Pr\[S1_init = W\*\] = 2\^(-?[\d.]+)',s)
    sc=re.search(r'score log2\(L/eps\) with L=(\d+) words = ([\d.]+) bits',s)
    ver=re.search(r'(EXHAUSTIVE|SAMPLED) verification: class seeds tested (\d+), full-hash collisions (\d+), S1_init mismatches (\d+)',s)
    ctl=re.search(r'control: (\d+) collisions in (\d+)',s)
    pq=re.findall(r'\(p,q\)=\((\d+),(\d+)\) cost=(\d+) free=(\d+)\+(\d+) e=(\d+)',s)
    S=re.search(r'S=(\d+) determined',s)
    t=re.search(r'real\t(\S+)',s)
    if not m:
        rows.append((999,L,v,'-','-','-','-','-','-','-','(no result: %s)'%(s.strip().splitlines()[-1][:60] if s.strip() else 'empty'),'-'))
        continue
    rows.append((float(sc.group(2)),L,v,m.group(1),m.group(2),int(m.group(3)),S.group(1),m.group(4),
                 '%s/%s'%(ver.group(3),ver.group(2)) if ver else '-', ('%s/%s'%(ctl.group(1),ctl.group(2)) if ctl else '-'),
                 ','.join('(%s,%s)c%s'%(a,b,c) for a,b,c,_,_,_ in pq), t.group(1) if t else '-'))
rows.sort(key=lambda r:(r[0],r[1]))
print("%-6s %8s %3s %6s %-18s %7s %3s %9s %-22s %-14s %-14s %s"%('bits','len','v','method','W*','weight','S','log2Pr','class collide/tested','control','(p,q)cost','time'))
for r in rows: print("%-6s %8d %3d %6s %-18s %7s %3s %9s %-22s %-14s %-14s %s"%tuple([('%.2f'%r[0]) if r[0]!=999 else '-']+list(r[1:])))
