#!/usr/bin/env python3
import json,math,pathlib
C=sum(8**i for i in range(1,9));data={'C8':C,'epsilon_bits':32,'h16_collision_bound_bits':96-math.log2(6804),'h7_collision_bound_bits':96-math.log2(972),'core_word_cap_bits':96,'style_word_cap_bits_formula':'64-log2(2-2^-64)','style_conservative_bits':63,'widths':[]}
for b in [1,2,4,8]:
 plateaus=[]
 for h in range(8):
  L=21*b*8**h;coef=(h+2)**2*(h+5)
  assert L>=coef
  if h:assert coef<8*((h+1)**2*(h+4))
  plateaus.append({'h':h,'first_L':L,'coefficient':coef,'score_bits':96+math.log2(L/coef)})
 data['widths'].append({'b':b,'raw24_exclusive_byte_limit':168*b*(C+1),'style_exclusive_byte_limit':144*b*(C+1),'raw24_key_prefix_words':217+215*b,'raw24_one_leaf_live_words':28+6*b,'coarse_score_plateaus':plateaus})
pathlib.Path('certificates/scores-layout.json').write_text(json.dumps(data,indent=2)+'\n')
print(json.dumps({k:v for k,v in data.items() if k!='widths'}))
