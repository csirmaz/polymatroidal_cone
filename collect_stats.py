
# Keep track of the unique rays found from the output of `make int_run`

import sys
import time
import re

rays = {}
finds = 0

re_line = re.compile(r'Tries: ([0-9]+) Chosen: [0-9,]+ Ray: \[([0-9,]+)\] ZeroAxioms: ([0-9]+)')

latest = time.time()
for line in sys.stdin:
    m = re_line.match(line)
    if not m:
        # print(line)
        continue
    
    tries = m.group(1)
    ray = m.group(2)
    zero_axioms = m.group(3)

    if ray not in rays: 
        rays[ray] = {'count':1, 'zero_axioms':zero_axioms}
    else:
        rays[ray]['count'] += 1
        if rays[ray]['zero_axioms'] != zero_axioms:
            raise ValueError("zero axioms mismatch")
    finds += 1

    if time.time() - latest >= 60:
        latest = time.time()
        print(f"Tries: {tries} Unique rays: {len(rays)} All finds: {finds} ({finds/int(tries)*100}%)")
        print(f"Distribution: {sorted([x['count'] for x in rays.values()])}")
        for ray, data in rays.items():
            print(f"  {ray} Count={data['count']} ZeroAxioms={data['zero_axioms']}")
        print("", flush=True)        