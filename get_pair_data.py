
# From the output of "make slicer_run_*" get the number of rays and pairs from the processed axioms

import sys
import re
import json

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
            STATE="init"
            continue
    
    if STATE=="init":
        m = re.search(r"initial_axioms=(.*)$", line)
        if m:
            init_axioms = m.group(1).split(',')[:-1]
            assert len(init_axioms) == VARS
            for a in init_axioms:
                AXIOM_SET.append(int(a))
            STATE="initends"
            continue
        
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
                'ray_pairs': RAY_PAIRS
            })
            STATE="axiomheader"
            continue
        
        m = re.search(r"total_elapsed_time=([0-9\.]+)", line)
        if m:
            EXPERIENCES[-1]['total_time'] = float(m.group(1))
            AXIOM_SET = []
            STATE='init'
            continue
        
    if STATE=="axiomheader":
        if re.search(r"TOOMANYPAIRS", line):
            EXPERIENCES[-1]['too_many'] = 1
            AXIOM_SET = []
            STATE='init'
            continue

        m = re.search(r"applied_axiom=([0-9]+)", line)
        if m:
            assert NEW_AXIOM == int(m.group(1))
            m = re.search(r"total_rays=([0-9]+)", line)
            EXPERIENCES[-1]['result_rays'] = int(m.group(1))
            AXIOM_SET.append(NEW_AXIOM)
            STATE='axiombegins'
            continue

def printw(v, n):
    x = str(v)
    while len(x) < n:
        x = " "+x
    return x

max_ray_pairs = {i:None for i in range(AXIOMS+1)}
pairs_to_axiom = {i:[] for i in range(AXIOMS+1)}

for e in EXPERIENCES:
    step = len(e['prev_axioms'])+1
    o = []
    o.append(f"Step={step}")
    if e.get('too_many'):
        o.append("TooManyRayPairs")
    o.append(f"ResultRays={printw(e.get('result_rays'), 6)}")
    o.append(f"RayPairs={printw(e.get('ray_pairs'), 10)}")
    if max_ray_pairs[step] is None or e['ray_pairs'] > max_ray_pairs[step]:
        max_ray_pairs[step] = e['ray_pairs']
    o.append(f"NewAxiom={e.get('new_axiom')}")
    o.append(f"""PrevAxioms={','.join([str(x) for x in e['prev_axioms']] + [f'+{e["new_axiom"]}'])}""")
    o.append(f"MissingAxioms={','.join([str(x) for x in range(AXIOMS) if x not in e['prev_axioms'] and x != e['new_axiom']])}")
    if e.get('total_time'):
        o.append(f"TotalTime={e.get('total_time')}")
    print(' '.join(o))
    pairs_to_axiom[step].append({'ray_pairs': e['ray_pairs'], 'new_axiom': e['new_axiom']})

print()

for step in range(AXIOMS+1):
    pairs_to_axiom[step].sort(key=lambda x: x['ray_pairs'])
    for e in pairs_to_axiom[step]:
        o = []
        o.append(f"Step={step}")
        d = int(e['ray_pairs']*30/max_ray_pairs[step]+.5)
        o.append("#"*d + "."*(30-d))
        o.append(N_5_AXIOMS[e['new_axiom']])
        print(' '.join(o))
