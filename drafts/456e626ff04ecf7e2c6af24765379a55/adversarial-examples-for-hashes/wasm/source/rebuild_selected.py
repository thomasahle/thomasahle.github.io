#!/usr/bin/env python3
"""Rebuild the selected-witness modules from the published adjacent sources."""
from pathlib import Path
import subprocess, json, hashlib
root = Path(__file__).resolve().parents[1]
manifest = json.loads((root / 'manifest.json').read_text())
exports = ['init','validate','pair_count','pair_bytes','pair_len','hash_pair','hash_key','run_batch','sample_seed','rng_init','scratch','example_key','seed_bits','key_words','has_class']
for module in ['xxh3-64', 't1ha2-64', 'rust-ahash']:
    wrapper = root / 'source/wrappers' / (module + '.c')
    subprocess.run(['emcc', str(wrapper), '-O2', '-std=c11', '--no-entry', '-sMODULARIZE=1', '-sEXPORT_ES6=1', '-sENVIRONMENT=worker,node', '-sFILESYSTEM=0', '-sASSERTIONS=0', '-sALLOW_MEMORY_GROWTH=0', '-sINITIAL_MEMORY=16777216', '-sSTACK_SIZE=262144', '-sEXPORTED_FUNCTIONS='+json.dumps(['_'+n for n in exports]), '-sEXPORTED_RUNTIME_METHODS=["UTF8ToString","HEAPU8"]', '-o', str(root / (module+'.js'))], check=True)
    info = manifest['modules'][module]
    info['bytes'] = (root / (module+'.wasm')).stat().st_size
    info['sha256'] = hashlib.sha256((root.parent / info['source']).read_bytes()).hexdigest()
(root / 'manifest.json').write_text(json.dumps(manifest, indent=2, ensure_ascii=False)+'\n')
