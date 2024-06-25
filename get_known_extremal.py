
# Parse ray dumps from STDIN and collect known extremal rays

import re
import sys
import get_axioms

get_axioms.SET_N = 6
get_axioms.ADD_IDENTITY = False

axioms = get_axioms.get_axioms()
N_AXIOMS = len(axioms)
VARS = len(axioms[0])

non_rays = 0
num_inside = 0
num_outside = 0
ray_store = {}
re_ray = re.compile(r'^ray=[0-9]+ \[([\-0-9,]+)\] <([01]+)>')

for line in sys.stdin:
    
    m = re_ray.search(line)
    if m:
        coords = m.group(1)
        bitmask = m.group(2)
        
        coords = [int(x) for x in coords.split(',')]
        assert len(coords) == VARS
        assert len(bitmask) == N_AXIOMS
        ray_label = ','.join([str(x) for x in coords])

        if ray_label in ray_store: continue
        
        outside = False
        on_axiom = [0 for x in range(N_AXIOMS)]
        for i in range(N_AXIOMS):
            bitmask_says = (bitmask[i] == '1')
            dot = get_axioms.dot(coords, axioms[i])
            if bitmask_says: assert dot == 0
            if dot == 0:
                on_axiom[i] = 1
            if dot < 0:
                outside = True
                break
            
        ray_store[ray_label] = {
            'outside': outside,
            'coords': coords,
            'on_axiom': on_axiom
        }

        if outside:
            num_outside += 1
        else:
            num_inside += 1
    else:
        non_rays += 1

print(f"// Non-ray lines: {non_rays} Outside rays: {num_outside} Inside rays: {num_inside}")   
for v in ray_store.values():
    if not v['outside']:
        print("{" + (",".join([str(x) for x in v['coords']])) + "}, /* " + ("".join([str(x) for x in v['on_axiom']])) + " */")
    
