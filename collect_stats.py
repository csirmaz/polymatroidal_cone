
# Keep track of the unique rays found from the output of `make int_run`

import sys
import time
import regex as re
import json

rays = {}
finds = 0

re_line = re.compile(r'Tries: ([0-9]+) Chosen: [0-9,]+ Ray: \[([0-9,]+)\]')

latest = time.time()
for line in sys.stdin:
    if line[0] == " ": continue
    
    m = re_line.match(line)
    if not m: continue
    
    tries = m.group(1)
    ray = m.group(2)

    if ray not in rays: 
        rays[ray] = 1
    else:
        rays[ray] += 1
    finds += 1

    if time.time() - latest >= 60:
        latest = time.time()
        print(f"Tries: {tries} Unique rays: {len(rays)} All finds: {finds} ({finds/int(tries)*100}%)")
        print(f"Distribution: {sorted(rays.values())}")
        print(json.dumps(rays, indent=4))
        