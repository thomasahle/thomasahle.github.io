from pathlib import Path
import re
# Preserve the parent's full proof audit, adding every declaration in this lane.
p = Path('FullAudit.lean')
s = p.read_text()
s = s.split('-- BRW lane additions')[0]
new = ['import ProvenHashes', 'set_option pp.universes false']
for module in ['BRW', 'EightLanes', 'ChartScores']:
    source = Path(f'ProvenHashes/{module}.lean').read_text()
    for name in re.findall(r'\b(?:theorem|lemma)\s+(\w+)', source):
        qualified = f'ProvenHashes.{module}.{name}'
        new += [f'#check @{qualified}', f'#print axioms {qualified}']
Path('BRWAudit.lean').write_text('\n'.join(new) + '\n')
p.write_text(s + '\n-- BRW lane additions\n' + '\n'.join(new[2:]) + '\n')
