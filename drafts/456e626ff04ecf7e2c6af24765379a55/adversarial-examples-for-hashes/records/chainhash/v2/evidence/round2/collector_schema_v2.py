#!/usr/bin/env python3
"""Collect existing evidence; never launches a benchmark."""
import argparse, hashlib, json, math, pathlib, statistics
from run_speeds import parse
root=pathlib.Path(__file__).resolve().parent.parent
def read(p): return json.loads((root/p).read_text())
def lines(p):
    return [json.loads(s) for s in (root/p).read_text().splitlines() if s.startswith('{')]
def sha(p): return hashlib.sha256((root/p).read_bytes()).hexdigest()

ap=argparse.ArgumentParser()
ap.add_argument('--speed-dir',default='evidence/smhasher')
args=ap.parse_args()
execution=read(str(pathlib.Path(args.speed_dir)/'execution.json'))
assert len(execution['runs'])==10
assert execution['binary_sha256']==execution['binary_sha256_after']
assert sha('chainhash_x86.h')==execution['header_sha256']
names=['chainhash-x86','komihash','rapidhash','XXH3-64','HalftimeHash-512']
old={'komihash':(7.35,27.49),'rapidhash':(10.67,27.58),'XXH3-64':(19.69,30.16),'HalftimeHash-512':(19.29,85.74)}
hashes={}
for name in names:
    runs=sorted((r for r in execution['runs'] if r['name']==name),key=lambda r:r['run'])
    assert [r['run'] for r in runs]==[1,2]
    for r in runs:
        assert r['returncode']==0 and sha(r['raw_file'])==r['raw_sha256']
        assert max(r['gate']['busy_percent'].values())<5
        parsed=parse((root/r['raw_file']).read_text())
        assert all(parsed[k]==r[k] for k in parsed), 'Speed record differs from raw output'
    selected=max(r['bulk'] for r in runs); small=min(r['small_cycles'] for r in runs)
    row=dict(bulk_b_per_tsc=selected,small_cycles_1_to_31=small,runs=runs)
    if name in old:
        row.update(baseline_bulk=old[name][0],baseline_small=old[name][1],bulk_control_ratio=selected/old[name][0],small_control_ratio=small/old[name][1])
    hashes[name]=row
# Stable metric names for the goal-loop consumer. These summaries come from
# the same verified two-pass measurements as the detailed `hashes` records.
xeon={name:dict(
    bulk_bytes_per_cycle=row['bulk_b_per_tsc'],
    small_key_cycles=row['small_cycles_1_to_31'],
    bulk_passes_bytes_per_cycle=[r['bulk'] for r in row['runs']],
    small_key_passes_cycles=[r['small_cycles'] for r in row['runs']],
    raw_speed_files=[r['raw_file'] for r in row['runs']],
) for name,row in hashes.items()}
winner=hashes['chainhash-x86']['bulk_b_per_tsc']
comparisons={name:winner/hashes[name]['bulk_b_per_tsc'] for name in names[1:]}
phase1=[]
for r in lines('evidence/micro_l1.jsonl'):
    if 'name' not in r: continue
    r.update(median_b_per_tsc=statistics.median(r['b_per_tsc_trials']),best_b_per_tsc=max(r['b_per_tsc_trials']))
    phase1.append(r)
ports=[]
for r in lines('evidence/micro_ports.jsonl'):
    if 'name' not in r: continue
    r['median_tsc']=statistics.median(r['tsc_per_instruction_or_pair']);ports.append(r)
port={r['name']:r['median_tsc'] for r in ports}
ceilings={
    'ph_xmm_products_only':16/port['vpclmul_xmm'],
    'ph_ymm_products_only':32/port['vpclmul_ymm'],
    'ph_zmm_products_only':64/port['vpclmul_zmm'],
    'nh32_two_lanes_multiply_only':64/(2*port['vpmuludq_zmm']),
    'nh32_two_lanes_eight_vector_ops_model':64/(8*port['vpaddq_zmm']),
    'ifma52_two_lanes_multiply_only':104/(4*port['vpmadd52luq_zmm']),
    'ifma52_packed_twelve_vector_ops_model':104/(12*port['vpaddq_zmm']),
}
tuning=[]
for b in (256,512,1024,2048,4096,8192):
    data=[r for r in lines(f'evidence/bench{b}.jsonl') if 'len' in r]
    for r in data:
        r['median_cycles']=statistics.median(r['cycles'])
        r['median_b_per_tsc']=r['len']/r['median_cycles']
    tuning.append(dict(block_bytes=b,measurements=data))
verification={}
for name in ('test','sanitize','portable','cpp'):
    text=(root/f'evidence/{name}.txt').read_text()
    assert 'PASS cases=16692 random_inputs=12000' in text
    assert read(f'evidence/{name}_gate.json')['returncode']==0
    verification[name]=dict(passed=True,raw_file=f'evidence/{name}.txt',sha256=sha(f'evidence/{name}.txt'))
for side,code in [('le','363953CA'),('be','61C632F7')]:
    p=f'evidence/sanity-{side}.txt';text=(root/p).read_text()
    assert f'0x{code} ...... PASS' in text and 'FAIL' not in text
    assert read(f'evidence/sanity-{side}_gate.json')['returncode']==0
    verification['sanity_'+side]=dict(passed=True,verification=code,raw_file=p,sha256=sha(p))
result=dict(
    schema_version=2,variant='chainhash-x86-v1',host='Xeon Platinum 8375C',
    target_met=all(r['bulk']>=20 for r in hashes['chainhash-x86']['runs']) and all(winner>hashes[n]['bulk_b_per_tsc'] for n in ('XXH3-64','HalftimeHash-512')),
    units=dict(bulk='bytes per invariant TSC cycle',small='TSC cycles per hash'),
    protocol='SMHasher3 --test=Speed; fixed 262144-byte Average, best of two; independent minimum small Average, lengths 1..31',
    selection=dict(candidate='D: adjacent carry-less PH',block_bytes=1024,S=1,key_words=137,key_bytes=1096),
    theorem=dict(key_model='137 independent uniform 64-bit words',epsilon='(max(1,ceil(L/128))+2)/2^64',L_unit='8-byte words',per_word_score=64-math.log2(3),new_lean_theorem_machine_checked=False,seed_expansion_covered=False),
    phase1=phase1,instruction_microbenchmarks=ports,optimistic_instruction_budget_b_per_tsc=ceilings,
    ceiling_scope='Measured instruction throughputs and stated port-demand models; not impossibility bounds over all algorithms. Inner loops exclude chain/finalizer; IFMA loops exclude carry normalization.',
    speed_dir=args.speed_dir,xeon=xeon,
    block_size_tuning=tuning,hashes=hashes,chainhash_to_current_control_ratios=comparisons,
    chainhash_to_previous_optimized=dict(chainhash_256=winner/15.40,chainhash_1k=winner/16.36),
    verification=verification,execution=execution,provenance=read('evidence/provenance.json'),
    mac=dict(new_benchmarks_run=False,new_compilations_run=False,new_result=None),
)
assert result['target_met']
(root/'speeds_chainhash_x86.json').write_text(json.dumps(result,indent=2)+'\n')
print(json.dumps({'target_met':result['target_met'],'bulk':winner,'small':hashes['chainhash-x86']['small_cycles_1_to_31'],'ratios':comparisons,'ceilings':ceilings},indent=2))
