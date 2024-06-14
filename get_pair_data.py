
# From the output of "make slicer_run_*" get the number of rays and pairs for the processed axioms.
# Usage: python get_pair_data.py N SET_N where 
# - N is -1 to show all data or only print data for step N (all Nth steps)
# - SET_N is 4|5|6

import sys
import re
import json

OUT_STEP = int(sys.argv[1])
SET_N = int(sys.argv[2])
STATE = "filebegin"
AXIOM_SET = []
EXPERIENCES = []

N_5_AXIOMS = [
"Axiom0 Group0 f({0}) + f({1}) >= f({0, 1})",
"Axiom1 Group1 f({0, 2}) + f({1, 2}) >= f({2}) + f({0, 1, 2})",
"Axiom2 Group1 f({0, 3}) + f({1, 3}) >= f({3}) + f({0, 1, 3})",
"Axiom3 Group2 f({0, 2, 3}) + f({1, 2, 3}) >= f({2, 3}) + f({0, 1, 2, 3, 4})",
"Axiom4 Group1 f({0, 4}) + f({1, 4}) >= f({4}) + f({0, 1, 4})",
"Axiom5 Group2 f({0, 2, 4}) + f({1, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})",
"Axiom6 Group2 f({0, 3, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})",
"Axiom7 Group3 f({0, 1, 2, 3, 4}) >= f({2, 3, 4})",
"Axiom8 Group0 f({0}) + f({2}) >= f({0, 2})",
"Axiom9 Group1 f({0, 1}) + f({1, 2}) >= f({1}) + f({0, 1, 2})",
"Axiom10 Group1 f({0, 3}) + f({2, 3}) >= f({3}) + f({0, 2, 3})",
"Axiom11 Group2 f({0, 1, 3}) + f({1, 2, 3}) >= f({1, 3}) + f({0, 1, 2, 3, 4})",
"Axiom12 Group1 f({0, 4}) + f({2, 4}) >= f({4}) + f({0, 2, 4})",
"Axiom13 Group2 f({0, 1, 4}) + f({1, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})",
"Axiom14 Group2 f({0, 3, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})",
"Axiom15 Group3 f({0, 1, 2, 3, 4}) >= f({1, 3, 4})",
"Axiom16 Group0 f({1}) + f({2}) >= f({1, 2})",
"Axiom17 Group1 f({0, 1}) + f({0, 2}) >= f({0}) + f({0, 1, 2})",
"Axiom18 Group1 f({1, 3}) + f({2, 3}) >= f({3}) + f({1, 2, 3})",
"Axiom19 Group2 f({0, 1, 3}) + f({0, 2, 3}) >= f({0, 3}) + f({0, 1, 2, 3, 4})",
"Axiom20 Group1 f({1, 4}) + f({2, 4}) >= f({4}) + f({1, 2, 4})",
"Axiom21 Group2 f({0, 1, 4}) + f({0, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})",
"Axiom22 Group2 f({1, 3, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})",
"Axiom23 Group3 f({0, 1, 2, 3, 4}) >= f({0, 3, 4})",
"Axiom24 Group0 f({0}) + f({3}) >= f({0, 3})",
"Axiom25 Group1 f({0, 1}) + f({1, 3}) >= f({1}) + f({0, 1, 3})",
"Axiom26 Group1 f({0, 2}) + f({2, 3}) >= f({2}) + f({0, 2, 3})",
"Axiom27 Group2 f({0, 1, 2}) + f({1, 2, 3}) >= f({1, 2}) + f({0, 1, 2, 3, 4})",
"Axiom28 Group1 f({0, 4}) + f({3, 4}) >= f({4}) + f({0, 3, 4})",
"Axiom29 Group2 f({0, 1, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})",
"Axiom30 Group2 f({0, 2, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})",
"Axiom31 Group3 f({0, 1, 2, 3, 4}) >= f({1, 2, 4})",
"Axiom32 Group0 f({1}) + f({3}) >= f({1, 3})",
"Axiom33 Group1 f({0, 1}) + f({0, 3}) >= f({0}) + f({0, 1, 3})",
"Axiom34 Group1 f({1, 2}) + f({2, 3}) >= f({2}) + f({1, 2, 3})",
"Axiom35 Group2 f({0, 1, 2}) + f({0, 2, 3}) >= f({0, 2}) + f({0, 1, 2, 3, 4})",
"Axiom36 Group1 f({1, 4}) + f({3, 4}) >= f({4}) + f({1, 3, 4})",
"Axiom37 Group2 f({0, 1, 4}) + f({0, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})",
"Axiom38 Group2 f({1, 2, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})",
"Axiom39 Group3 f({0, 1, 2, 3, 4}) >= f({0, 2, 4})",
"Axiom40 Group0 f({2}) + f({3}) >= f({2, 3})",
"Axiom41 Group1 f({0, 2}) + f({0, 3}) >= f({0}) + f({0, 2, 3})",
"Axiom42 Group1 f({1, 2}) + f({1, 3}) >= f({1}) + f({1, 2, 3})",
"Axiom43 Group2 f({0, 1, 2}) + f({0, 1, 3}) >= f({0, 1}) + f({0, 1, 2, 3, 4})",
"Axiom44 Group1 f({2, 4}) + f({3, 4}) >= f({4}) + f({2, 3, 4})",
"Axiom45 Group2 f({0, 2, 4}) + f({0, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})",
"Axiom46 Group2 f({1, 2, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})",
"Axiom47 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 4})",
"Axiom48 Group0 f({0}) + f({4}) >= f({0, 4})",
"Axiom49 Group1 f({0, 1}) + f({1, 4}) >= f({1}) + f({0, 1, 4})",
"Axiom50 Group1 f({0, 2}) + f({2, 4}) >= f({2}) + f({0, 2, 4})",
"Axiom51 Group2 f({0, 1, 2}) + f({1, 2, 4}) >= f({1, 2}) + f({0, 1, 2, 3, 4})",
"Axiom52 Group1 f({0, 3}) + f({3, 4}) >= f({3}) + f({0, 3, 4})",
"Axiom53 Group2 f({0, 1, 3}) + f({1, 3, 4}) >= f({1, 3}) + f({0, 1, 2, 3, 4})",
"Axiom54 Group2 f({0, 2, 3}) + f({2, 3, 4}) >= f({2, 3}) + f({0, 1, 2, 3, 4})",
"Axiom55 Group3 f({0, 1, 2, 3, 4}) >= f({1, 2, 3})",
"Axiom56 Group0 f({1}) + f({4}) >= f({1, 4})",
"Axiom57 Group1 f({0, 1}) + f({0, 4}) >= f({0}) + f({0, 1, 4})",
"Axiom58 Group1 f({1, 2}) + f({2, 4}) >= f({2}) + f({1, 2, 4})",
"Axiom59 Group2 f({0, 1, 2}) + f({0, 2, 4}) >= f({0, 2}) + f({0, 1, 2, 3, 4})",
"Axiom60 Group1 f({1, 3}) + f({3, 4}) >= f({3}) + f({1, 3, 4})",
"Axiom61 Group2 f({0, 1, 3}) + f({0, 3, 4}) >= f({0, 3}) + f({0, 1, 2, 3, 4})",
"Axiom62 Group2 f({1, 2, 3}) + f({2, 3, 4}) >= f({2, 3}) + f({0, 1, 2, 3, 4})",
"Axiom63 Group3 f({0, 1, 2, 3, 4}) >= f({0, 2, 3})",
"Axiom64 Group0 f({2}) + f({4}) >= f({2, 4})",
"Axiom65 Group1 f({0, 2}) + f({0, 4}) >= f({0}) + f({0, 2, 4})",
"Axiom66 Group1 f({1, 2}) + f({1, 4}) >= f({1}) + f({1, 2, 4})",
"Axiom67 Group2 f({0, 1, 2}) + f({0, 1, 4}) >= f({0, 1}) + f({0, 1, 2, 3, 4})",
"Axiom68 Group1 f({2, 3}) + f({3, 4}) >= f({3}) + f({2, 3, 4})",
"Axiom69 Group2 f({0, 2, 3}) + f({0, 3, 4}) >= f({0, 3}) + f({0, 1, 2, 3, 4})",
"Axiom70 Group2 f({1, 2, 3}) + f({1, 3, 4}) >= f({1, 3}) + f({0, 1, 2, 3, 4})",
"Axiom71 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 3})",
"Axiom72 Group0 f({3}) + f({4}) >= f({3, 4})",
"Axiom73 Group1 f({0, 3}) + f({0, 4}) >= f({0}) + f({0, 3, 4})",
"Axiom74 Group1 f({1, 3}) + f({1, 4}) >= f({1}) + f({1, 3, 4})",
"Axiom75 Group2 f({0, 1, 3}) + f({0, 1, 4}) >= f({0, 1}) + f({0, 1, 2, 3, 4})",
"Axiom76 Group1 f({2, 3}) + f({2, 4}) >= f({2}) + f({2, 3, 4})",
"Axiom77 Group2 f({0, 2, 3}) + f({0, 2, 4}) >= f({0, 2}) + f({0, 1, 2, 3, 4})",
"Axiom78 Group2 f({1, 2, 3}) + f({1, 2, 4}) >= f({1, 2}) + f({0, 1, 2, 3, 4})",
"Axiom79 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 2})"
]

for line in sys.stdin:
    # print(f"::{STATE}::{line}", end="")
    
    if STATE=="filebegin":
        if re.search(r"SLICER_STARTING", line):
            m = re.search(r"VARS=([0-9]+)", line)
            VARS=int(m.group(1))
            
            m = re.search(r"AXIOMS=([0-9]+)", line)
            AXIOMS=int(m.group(1))
            
            m = re.search(r"SET_N=([0-9]+)", line)
            SET_N=int(m.group(1))
            assert SET_N == 5
            
            AXIOM_SET = []
            STATE="filebegin2"
            continue

    if STATE=="filebegin2":
        m = re.search(r"(vary_axiom=|Last but one axiom: )(.*)$", line)
        if m:
            VARY_AXIOM = int(m.group(2))
            STATE="init"
    
    if STATE=="init":
        m = re.search(r"initial_axioms=(.*)$", line)
        if m:
            init_axioms = m.group(1).split(',')[:-1]
            assert len(init_axioms) == VARS
            for a in init_axioms:
                AXIOM_SET.append(int(a)) # collect for previous axioms
            STATE="initends"
            continue
        
        if re.search(r"NO_INITIAL_AXIOMS", line):
            EXPERIENCES.append({
                'vary_axiom': VARY_AXIOM,
                'no_initial': True
            })
            STATE="filebegin"
            
        
    if STATE=="initends":
        if re.search(r"ADDING MORE AXIOMS", line):
            STATE="axiombegins"
            continue
        
    if STATE=="axiombegins":
        m = re.search(r"will_apply_axiom=([0-9]+)", line)
        if m:
            NEW_AXIOM=int(m.group(1))
            m = re.search(r"will_process_pairs=([0-9]+)", line)
            RAY_PAIRS = int(m.group(1))
            EXPERIENCES.append({
                'prev_axioms': [x for x in AXIOM_SET],
                'new_axiom': NEW_AXIOM,
                'vary_axiom': VARY_AXIOM,
                'ray_pairs': RAY_PAIRS
            })
            STATE="axiomheader"
            continue
        
        m = re.search(r"total_elapsed_time=([0-9\.]+)", line)
        if m:
            EXPERIENCES[-1]['total_time'] = float(m.group(1))
            AXIOM_SET = []
            STATE='filebegin'
            continue
        
    if STATE=="axiomheader":
        if re.search(r"TOOMANYPAIRS", line):
            EXPERIENCES[-1]['too_many'] = 1
            AXIOM_SET = []
            STATE='filebegin'
            continue
        
        m = re.search(r"applying_axiom=([0-9]+)", line)
        if m:
            assert NEW_AXIOM == int(m.group(1))
            m = re.search(r"prev_total_rays=([0-9]+)", line)
            PREV_RAYS = int(m.group(1))
            EXPERIENCES[-1]['prev_rays'] = PREV_RAYS
            STATE='axiombody'
            
    if STATE=='axiombody':
        m = re.search(r"applied_axiom=([0-9]+)", line)
        if m:
            assert NEW_AXIOM == int(m.group(1))
            m = re.search(r"total_rays=([0-9]+)", line)
            EXPERIENCES[-1]['result_rays'] = int(m.group(1))
            AXIOM_SET.append(NEW_AXIOM)
            STATE='axiombegins'
            continue

def printw(v, n):
    # print v at width n
    x = str(v)
    while len(x) < n:
        x = " "+x
    return x

max_ray_pairs = {i:None for i in range(AXIOMS+1)}
max_prev_rays = {i:None for i in range(AXIOMS+1)}
pairs_to_axiom = {i:[] for i in range(AXIOMS+1)} # number of ray pairs to axiom
rays_to_axiom = {i:[] for i in range(AXIOMS+1)} # number of prev rays to axiom

for e in EXPERIENCES:
    if e.get('no_initial'):
        print(f"! VaryAxiom={e.get('vary_axiom')} NoInitialAxiomSet")
        continue
    
    step = len(e['prev_axioms'])+1
    o = []
    o.append(f"Step={step}")
    if e.get('too_many'):
        o.append("TooManyRayPairs")
    o.append(f"PrevRays={printw(e.get('prev_rays'), 6)}")
    if max_prev_rays[step] is None or e['prev_rays'] > max_prev_rays[step]:
        max_prev_rays[step] = e['prev_rays']
    o.append(f"ResultRays={printw(e.get('result_rays'), 6)}")
    o.append(f"RayPairs={printw(e.get('ray_pairs'), 10)}")
    if max_ray_pairs[step] is None or e['ray_pairs'] > max_ray_pairs[step]:
        max_ray_pairs[step] = e['ray_pairs']
    o.append(f"NewAxiom={e.get('new_axiom')}")
    o.append(f"""PrevAxioms={','.join([str(x) for x in e['prev_axioms']] + [f'+{e["new_axiom"]}'])}""")
    o.append(f"MissingAxioms={','.join([str(x) for x in range(AXIOMS) if x not in e['prev_axioms'] and x != e['new_axiom']])}")
    if e.get('total_time'):
        o.append(f"TotalTime={e.get('total_time')}")

    if OUT_STEP == -1 or OUT_STEP == step:
        print(' '.join(o))

    pairs_to_axiom[step].append({'ray_pairs': e['ray_pairs'], 'new_axiom': e['new_axiom']})
    rays_to_axiom[step].append({'prev_rays': e['prev_rays'], 'new_axiom': e['new_axiom']})

if OUT_STEP != -1:
    print()
    print("Ordered by number of ray pairs:")

    for step in range(AXIOMS+1):
        if OUT_STEP==-1 or OUT_STEP==step:
            pairs_to_axiom[step].sort(key=lambda x: x['ray_pairs'])
            for e in pairs_to_axiom[step]:
                o = []
                o.append(f"Step={step}")
                d = int(e['ray_pairs']*30/max_ray_pairs[step]+.5)
                o.append("#"*d + "."*(30-d))
                if e['ray_pairs'] == pairs_to_axiom[step][0]['ray_pairs']:
                    o.append("(min)")
                o.append(N_5_AXIOMS[e['new_axiom']])
                print(' '.join(o))

    print()
    print("Ordered by number of previous rays:")

    for step in range(AXIOMS+1):
        if OUT_STEP==-1 or OUT_STEP==step:
            rays_to_axiom[step].sort(key=lambda x: x['prev_rays'])
            for e in rays_to_axiom[step]:
                o = []
                o.append(f"Step={step}")
                d = int(e['prev_rays']*30/max_prev_rays[step]+.5)
                o.append("$"*d + "."*(30-d))
                if e['prev_rays'] == rays_to_axiom[step][0]['prev_rays']:
                    o.append("(min)")
                o.append(N_5_AXIOMS[e['new_axiom']])
                print(' '.join(o))

else: # For a single run, called with -1
    
    # Get initial axioms
    seen = set()
    for step in range(AXIOMS+1):
        if len(rays_to_axiom[step]):
            assert len(rays_to_axiom[step]) == 1
            seen.add(rays_to_axiom[step][0]['new_axiom'])

    print()
    print("Initial axioms, in no particular order:")
    for a in range(AXIOMS):
        if a not in seen:
            print(N_5_AXIOMS[a])
    
    # List axioms in order
    print()
    print("Axioms in order:")
    for step in range(AXIOMS+1):
        if len(rays_to_axiom[step]):
            assert len(rays_to_axiom[step]) == 1
            print(f"Step={step} {N_5_AXIOMS[rays_to_axiom[step][0]['new_axiom']]}")
    
