"""Recheck each explicitly marked lemma prefix with lake build on the Xeon.
The final source is restored even if a checkpoint fails. Mathlib is reused.
"""
from pathlib import Path
import os
import subprocess
import sys

for arg in sys.argv[1:]:
    path = Path(arg)
    source = path.read_text()
    prefix = ''
    try:
        for i, piece in enumerate(source.split('-- CHECKPOINT')[:-1], 1):
            prefix += piece
            path.write_text(prefix + '\nend ProvenHashes.UMASH\n')
            log = Path('logs') / (path.stem + '-checkpoint-' + str(i) + '.txt')
            with log.open('w') as out:
                result = subprocess.run(['bash', 'build-umash.sh',
                    '.'.join(path.with_suffix('').parts)], stdout=out, stderr=out)
            print(str(log), result.returncode, flush=True)
            if result.returncode:
                print(log.read_text(), flush=True)
                raise SystemExit(result.returncode)
    finally:
        path.write_text(source)
