
# Keep track of the unique rays found from the output of `make int_run`

import sys
import time

rays = {}

latest = time.time()
for line in sys.stdin:
    if line[0] == " ": continue
    tries = line.split('Chosen')
    if len(tries) == 1: # Not a useful line
        continue
    tries = tries[0][7:-1]  # first element and remove characters (for "Tries: " and " ")
    ray = line.split('[')[1][0:-2] # second element and remove characters (for "]\n")
    
    if ray not in rays: 
        rays[ray] = 1
    else:
        rays[ray] += 1

    if time.time() - latest >= 3:
        latest = time.time()
        print(f"Tries: {tries} Unique rays: {len(rays)}")
        print(f"Distribution: {sorted(rays.values())}")
        