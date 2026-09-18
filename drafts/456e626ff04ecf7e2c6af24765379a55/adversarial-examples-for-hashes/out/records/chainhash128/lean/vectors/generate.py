from pathlib import Path
import random
r=random.Random(128512)
lengths=sorted(set([0,1,2,7,8,9,15,16,17,31,32,33,47,48,49,63,64,65,127,128,129,255,256,257,511,512,513,1023,1024,1025,1535,1536,1537,4095,4096,4097]+[r.randrange(10000) for _ in range(50)]))
rows=[]
for model in [0,1]:
 for case,n in enumerate(lengths):
  count=10 if model else 41
  limbs=[r.getrandbits(64) for _ in range(2*count)]
  msg=[r.randrange(256) for _ in range(n)]
  rows.append(' '.join(map(str,[model,n]+limbs+msg)))
 for edge in [0,1,2,2**64-1]:
  for n in [0,1,16,63,64,65,511,512,513,1025]:
   count=10 if model else 41
   rows.append(' '.join(map(str,[model,n]+[edge]*(2*count)+[255]*n)))
Path('vectors/corpus.txt').write_text('\n'.join(rows)+'\n')
print(len(rows),'vectors generated')
