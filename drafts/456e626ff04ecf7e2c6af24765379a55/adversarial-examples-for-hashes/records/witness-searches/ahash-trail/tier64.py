import math,json
src=open('valsearch2.py').read()
src=src.replace("if t12<16: continue","if t12<16: continue\n                    if t12<16: continue").replace("if tier<32: continue","if tier<64: continue")
src=src.replace("allres=[]","allres=[]\nimport sys\nsys.argv=['x','-40']")
# run with THRESH=-40 for tier 64 only
src=src.replace("THRESH=float(sys.argv[1]) if len(sys.argv)>1 else -19.85","THRESH=-40.0")
exec(src.split("allres=[]")[0])
for name,T in TRAILS.items():
    res=search(name,T)
    print(name,"tier-64 instances with a sum-lane-feasible d-assignment:",len(res))
    for r in res[:5]: print("  %.3f"%r['logp'],r['x1'],r['x2'],r['x3'],r['ds'])
