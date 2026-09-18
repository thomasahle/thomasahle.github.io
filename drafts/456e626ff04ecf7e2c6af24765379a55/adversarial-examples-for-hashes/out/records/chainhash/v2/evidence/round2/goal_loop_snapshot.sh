#!/bin/bash
# Goal loop: keep re-invoking the lane until the Xeon bulk figure for chainhash-x86 reaches the target or 8 rounds pass.
J="$(cd "$(dirname "$0")" && pwd)"; TARGET=20.0; MAXROUNDS=8
metric() { python3 - "$J/speeds_chainhash_x86.json" <<'PY'
import json,sys
try:
    d=json.load(open(sys.argv[1]))
except Exception: print("none"); sys.exit()
best=0.0
def walk(o,path=()):
    global best
    if isinstance(o,dict):
        if 'bulk_bytes_per_cycle' in o and any('x86' in p.lower() or 'chainhash' in p.lower() for p in path) and any('xeon' in p.lower() for p in path):
            try: best=max(best,float(o['bulk_bytes_per_cycle'] or 0))
            except Exception: pass
        for k,v in o.items(): walk(v,path+(str(k),))
walk(d); print(best)
PY
}
for r in $(seq 1 $MAXROUNDS); do
  while ps aux | grep '[c]odex exec' | grep -v 'node ' | grep -q "codex/chainhash-x86 "; do sleep 120; done
  m=$(metric); echo "round $r: best Xeon bulk for chainhash-x86 = $m (target $TARGET) $(date -u +%FT%TZ)" >> $J/goal_loop.log
  awk "BEGIN{exit !($m >= $TARGET)}" && { echo "GOAL MET" >> $J/goal_loop.log; exit 0; }
  mv $J/codex.log $J/codex.round$r.log 2>/dev/null
  { echo "# GOAL LOOP, round $((r+1)): the goal is NOT yet met. Current best Xeon bulk for chainhash-x86 in ./speeds_chainhash_x86.json: $m B/cycle; the goal is >= $TARGET B/cycle under the SMHasher3 bulk protocol with a provable design (see the original task below). Read your REPORT.md, SPEC.md and state files, keep what works, and try the next most promising idea (from your phase-1 table, ./DESIGN_PANEL.md if present, or new ones: wider loads, fewer instructions per product, better port balance, interleaving two blocks, removing dependency chains, a different level-1 candidate). Re-verify byte-identity and re-time after each change. If you can PROVE a hardware ceiling below the goal for every provable design you can think of, write CEILING.md with the argument and the measurements and stop."; echo; cat $J/PROMPT.md; } > $J/PROMPT.round$((r+1)).md
  (cd $J && codex exec --skip-git-repo-check --sandbox danger-full-access --cd "$J" -c model_reasoning_effort='"max"' -o "$J/last_message.txt" - < $J/PROMPT.round$((r+1)).md > codex.log 2>&1)
  [ -f $J/CEILING.md ] && { echo "CEILING claimed; stopping" >> $J/goal_loop.log; exit 0; }
done
echo "max rounds reached" >> $J/goal_loop.log
