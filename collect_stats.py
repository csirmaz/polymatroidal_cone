
# Keep track of the unique rays from rays received on STDIN in a stream
# TODO: Read rays from previous runs stored in files
# TODO: Apply rotations to the rays found (see GA.permutations(), GA.expression_apply_permutation())
# TODO: check rays against axioms (see GA.get_axioms())

import sys
import time
import re

import get_axioms as GA

rays = {}
finds = 0

# Regex to process STDIN stream
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

    if time.time() - latest >= 600:
        latest = time.time()
        print(f"Tries: {tries} Unique rays: {len(rays)} All finds: {finds} ({finds/int(tries)*100}%)")
        print(f"Distribution: {sorted([x['count'] for x in rays.values()])}")
        for ray, data in rays.items():
            print(f"  {ray} Count={data['count']} ZeroAxioms={data['zero_axioms']}")
        print("", flush=True)        
