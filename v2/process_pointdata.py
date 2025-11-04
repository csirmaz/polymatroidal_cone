
# Process the pointdata output of convexhull_iter.py
# cat pointdata | python process_pointdata.py > processeddata


import sys
import os
import regex as re
import json

OUTPUT_UP_TO_ITER = 12
OUTPUT_LAST_ITER_ONLY = True

# First output line: Number of components
print(OUTPUT_UP_TO_ITER+1)

cmap = {'_':0, 'Y':2, 'm':1}

coord_to_status = {}
size = None

def print_matrix(matrix):
    for row in reversed(matrix):
        print(row)

def output(px,py,pz, desc, status, itr):
    global size
    
    if itr > OUTPUT_UP_TO_ITER: return
    if OUTPUT_LAST_ITER_ONLY and itr < OUTPUT_UP_TO_ITER: return
    
    diagonals = desc.split('|')
    if size is None:
        size = len(diagonals)
        assert size >= OUTPUT_UP_TO_ITER+1
    else:
        assert len(diagonals) == size
    
    matrix = []
    for j in range(size):
        matrix.append([0 for i in range(size)])
        
    for di, diag in enumerate(diagonals):
        for ci, c in enumerate(diag):
            coord_sum = size - 1 - di
            i = ci
            j = coord_sum - ci
            matrix[j][i] = cmap[c]
            
    #print(desc)
    #print_matrix(matrix)
    
    filled_per_row = [0 for i in range(size)]
    filled_per_col = [0 for i in range(size)]
    
    for j, row in enumerate(matrix):
        c = 0
        while c<size and row[c] > 0: c += 1
        filled_per_row[j] = c
        
    for j, row in enumerate(matrix):
        for i, v in enumerate(row):
            if v>0 and j+1>filled_per_col[i]: filled_per_col[i]=j+1
            
    #print(filled_per_row)
    #print(filled_per_col)
    
    for i in range(size):
        if i > itr:
            assert filled_per_col[i] == 0
            assert filled_per_row[i] == 0
    
    desc_short = "|".join(diagonals[(-OUTPUT_UP_TO_ITER-1):])
    
    print(json.dumps([status, filled_per_row[:OUTPUT_UP_TO_ITER+1], filled_per_col[:OUTPUT_UP_TO_ITER+1], [px, py, pz, desc_short, itr]]))
    
iter_done = set()

for line in sys.stdin:
    match = re.search(r'^--- iter=([\-0-9]+) done', line)
    if match:
        iter_done.add(int(match.group(1)))
        continue
    match = re.search(r'\(([0-9]+),([0-9]+),([0-9]+)\) <([^>]+)> status=([^\s]+) iter=([\-0-9]+)', line)
    if not match:
        print(f"Cannot parse {line}")
        exit(1)
    x = int(match.group(1))
    y = int(match.group(2))
    z = int(match.group(3))
    desc = match.group(4)
    status = match.group(5)
    itr = int(match.group(6))
    if status == 'additional_form':
        output(x,y,z, desc, coord_to_status[(x,y,z)], itr)
    elif status == 'not_vertex':
        assert (x,y,z) not in coord_to_status
        coord_to_status[(x,y,z)] = 0
        output(x,y,z, desc, 0, itr)
    elif status == 'vertex_and_previously':
        assert (x,y,z) not in coord_to_status
        coord_to_status[(x,y,z)] = 1
        output(x,y,z, desc, 1, itr)
    else:
        print("Wrong status")
        exit(1)

if OUTPUT_UP_TO_ITER+1 not in iter_done:
    msg = f"ERROR: iter {OUTPUT_UP_TO_ITER+1} is not complete, which is needed to see the final vertices\n"
    print(msg)
    assert False

