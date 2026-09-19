import sys
sec = bytes.fromhex("b8fe6c3923a44bbe7c01812cf721ad1cded46de9839097db7240a4a4b7b3671fcb79e64eccc0e578825ad07dccff7221b8084674f743248ee03590e6813a264c3c2852bb91c300cb88d0658b1b532ea371644897a20df94e3819ef46a9deacd8a8fa763fe39c343ff9dcbbc7c70b4f1d8a51e04bcdb45931c89f7ec9d9787364eac5ac8334d3ebc3c581a0fffa1363eb170ddd51b7f0da49d3165526 29d4689e2b16be587d47a1fc8ff8b8d17ad031ce45cb3a8f95160428afd7fbcabb4b407e".replace(" ",""))
K=[int.from_bytes(sec[8*i:8*i+8],'little') for i in range(24)]
M=(1<<64)-1
def naf(d):
    digs=[]; i=0
    while d:
        if d&1:
            z=2-(d&3)  # 1 -> 1, 3 -> -1
            digs.append((i,z)); d-=z
        d>>=1; i+=1
    return digs
for o in range(8):
    ka,kb=K[2*o],K[2*o+1]; D=(ka+kb+1)&M
    n=naf(D); w=len(n)
    base=bin(ka^((ka-D)&M)).count('1')
    print(f"o={o} K'={ka:016x} K''={kb:016x} D={D:016x} popcount(D)={bin(D).count('1')} NAF weight={w} base disagreement={base}")
