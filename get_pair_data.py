
# From the output of "make slicer_run_*" get the number of rays and pairs from the processed axioms

import sys
import re
import json

STATE = "filebegin"
AXIOM_SET = []
EXPERIENCES = []

for line in sys.stdin:
    if STATE=="filebegin":
        if re.search(r"SLICER_STARTING", line):
            m = re.search(r"VARS=([0-9]+)", line)
            VARS=int(m.group(1))
            
            m = re.search(r"AXIOMS=([0-9]+)", line)
            AXIOMS=int(m.group(1))
            
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

for e in EXPERIENCES:
    o = []
    o.append(f"Step={len(e['prev_axioms'])+1}")
    if e.get('too_many'):
        o.append("TooManyRayPairs")
    o.append(f"ResultRays={e.get('result_rays')}")
    o.append(f"RayPairs={e.get('ray_pairs')}")
    o.append(f"NewAxiom={e.get('new_axiom')}")
    o.append(f"""PrevAxioms={','.join([str(x) for x in e['prev_axioms']] + [f'+{e["new_axiom"]}'])}""")
    o.append(f"MissingAxioms={','.join([str(x) for x in range(AXIOMS) if x not in e['prev_axioms'] and x != e['new_axiom']])}")
    if e.get('total_time'):
        o.append(f"TotalTime={e.get('total_time')}")
    print(' '.join(o))
