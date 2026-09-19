#!/usr/bin/env python3
"""Inventory the public exact-certificate package (SHA-256)."""
from pathlib import Path
import hashlib
import json
root = Path(__file__).resolve().parent
files = {p.name: hashlib.sha256(p.read_bytes()).hexdigest()
         for p in sorted(root.iterdir()) if p.is_file() and p.name != 'manifest.json'}
(root / 'manifest.json').write_text(json.dumps({'files': files}, indent=2) + '\n')
print(f"Recorded {len(files)} certificate files.")
