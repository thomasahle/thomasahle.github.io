#!/usr/bin/env python3
"""Validate the recorded evidence; never substitute requested for completed trials."""
import datetime, hashlib, json, pathlib
root = pathlib.Path(__file__).resolve().parents[2]
def read(path): return json.loads((root/path).read_text())
def events(path): return [json.loads(s) for s in (root/path).read_text().splitlines()]
def sha(path): return hashlib.sha256((root/path).read_bytes()).hexdigest()
def result(path):
    rows = events(path)
    assert rows[0]['event'] == 'start' and rows[0]['rng_kat']
    assert rows[-1]['event'] == 'result', path
    assert rows[-1]['completed'] == rows[0]['requested'], path
    assert rows[-1]['collisions'] == 0, path
    return rows[0],rows[-1]

header = sha('halftime-hash.hpp')
for copy in ['HalftimeHash-fork/halftime-hash.hpp','HalftimeHash24-fixed.hpp']:
    if (root/copy).exists(): assert header == sha(copy)
rank = read('certificates/rank-certificate.json')
assert rank['header_sha256'] == header
assert [e['minimum_distance'] for e in rank['encoders']] == [2,3,4,5]
assert [e['all_subsets_checked'] for e in rank['encoders']] == [128,512,1024,512]
flips = events('certificates/logs/flips.jsonl')
assert [r['b'] for r in flips] == [1,2,4,8]
assert sum(r['single_bit_flips'] for r in flips) == 20160
assert all(r['single_bit_flips'] == r['distances']['3'] == 1344*r['b'] for r in flips)
guard = events('certificates/logs/guard.jsonl')
assert len(guard) == 592
assert all(r['exact_status'] == 0 and r['minus_one_word_status'] == -11 for r in guard)
object_equivalence = read('certificates/logs/final-object-equivalence.json')
assert object_equivalence['final_header_sha256'] == header
assert len(object_equivalence['unchanged_functions']) == 3
plan = read('certificates/logs/witness-parts.json')
witness = []
for b in [1,2,4]:
    p = next(p for p in plan['parts'] if p['b'] == b)
    first = events(p['part1_log'])
    checkpoint = p['part1_checkpoint']
    assert checkpoint in first and checkpoint['collisions'] == 0
    assert checkpoint['completed'] == p['part1_counted_completed']
    start,last = result(p['part2_log'])
    total = checkpoint['completed'] + last['completed']
    assert total == 2**36 and last['b'] == b and last['condition'] == 0
    witness.append({'b':b,'completed':total,'collisions':0,
                    'parts':[{'log':p['part1_log'],'completed':checkpoint['completed'],'checkpoint_only':True},
                             {'log':p['part2_log'],'completed':last['completed'],'checkpoint_only':False}],
                    'final_header_applicability':'Identical function bytes, relocations and constant sections; see final-object-equivalence.json.'})
start,last = result('certificates/logs/witness-b8-final.jsonl')
assert last['b'] == 8 and last['condition'] == 0 and last['completed'] == 2**36
witness.append({'b':8,'completed':last['completed'],'collisions':0,
                'parts':[{'log':'certificates/logs/witness-b8-final.jsonl','completed':last['completed'],'checkpoint_only':False}],
                'final_header_applicability':'Fresh complete run of witness-final, compiled against the final header.'})
condition = []
for b in [1,2,4,8]:
    path = f'certificates/logs/condition-b{b}.jsonl'
    start,last = result(path)
    assert last['b'] == b and last['condition'] == 1 and last['completed'] == 2**28
    condition.append({'b':b,'completed':last['completed'],'collisions':0,'log':path})
vectors = {}
for backend in ['scalar','sse2','avx2','avx512']:
    v = read(f'certificates/logs/verify-{backend}.json')
    assert v['status'] == 'passed' and v['random_inputs'] == 10000
    vectors[backend] = v
m2 = read('certificates/bench/M2Pro/vectors.json')
assert len(set(m2.values())) == 1
vector_sha = next(iter(m2.values()))
for line in (root/'certificates/logs/vector-sha256.txt').read_text().splitlines():
    assert line.split()[0] == vector_sha
equiv = read('certificates/bench/M2Pro/final-header-equivalence.json')
assert equiv['identical'] and equiv['final_header_sha256'] == header
assert equiv['preprocessed_sha256']['timed'] == equiv['preprocessed_sha256']['final']
for name in ['ubsan','ubsan-gcc-final']:
    assert read(f'certificates/logs/{name}.json')['status'] == 'passed'
    assert (root/f'certificates/logs/{name}.txt').stat().st_size == 0
assert read('certificates/bench/M2Pro/verify-ubsan.json')['status'] == 'passed'
lean = (root/'certificates/lean/FixedLength.log').read_text()
assert lean.count('depends on axioms: [propext, Classical.choice, Quot.sound]') == 2
assert read('certificates/logs/style-compatibility.json')['identical_outputs'] == 40000
extra = read('certificates/logs/extra.json')
assert extra['status'] == 'passed' and extra['unequal_length_collisions'] == 0
assert all(extra[k] == 40000 for k in ['unequal_length_zero_message_pairs','one_leaf_live_readset_checks','independent_short_tail_formula_checks'])
summary = {
    'final_header_sha256':header,'validated_at':datetime.datetime.now(datetime.timezone.utc).isoformat(),
    'rank_certificates':[{'k':e['k'],'minimum_distance':e['minimum_distance'],'all_subsets':e['all_subsets_checked'],'critical_subsets':e['critical_subsets']} for e in rank['encoders']],
    'direct_encoder_flips':flips,'guard_page':{'cases':592,'exact_passes':592,'one_word_short_SIGSEGV':592},
    'witness':witness,'conditioning_high32_key6_zero':condition,
    'key_generator':'AES-128-CTR with getrandom seed per process, disjoint counter streams per worker, FIPS-197 KAT. Empirical pseudorandom key trials; the theorem separately assumes independent uniform key words.',
    'witness_execution':'Separate noinline API translation unit, no LTO, two full header calls and comparison of all three output words per trial. Every live key word is redrawn; unread holes remain zero.',
    'vector_sha256':vector_sha,'vector_bytes':4800000,'Xeon_vectors':vectors,'M2_vector_sha256':m2,
    'UBSan':['Xeon Clang 21','Xeon GCC 11 instrumentation linked with Clang UBSan runtime','M2 Apple Clang 17'],
    'style_compatibility':read('certificates/logs/style-compatibility.json'),
    'extra_checks':read('certificates/logs/extra.json'),
    'length_Lean_axioms':['propext','Classical.choice','Quot.sound'],
    'excluded':'All logs/pre-final-header data, both old b=8 chunk files, condition-b8-before-unsigned512.jsonl, and bench/Xeon8375C-before-unsigned512 are excluded from final counts/timings. Unreported interrupted work is excluded.'}
(root/'certificates/verification-summary.json').write_text(json.dumps(summary,indent=2)+'\n')
(root/'certificates/logs/witness-summary.json').write_text(json.dumps({'final_header_sha256':header,'results':witness},indent=2)+'\n')
print('Verified 2^36 completed keys and zero collisions at every width; all supporting certificates passed.')
