
# cat pointdata | python process_pointdata.py


import sys
import os
import regex as re
import json

# #4/25 (14,17,14) <_____________|____________|___________|__________|_________|________|_______|______|_____|_YYY|mmm|mm|m> status=vertex_and_previously
# #-1/-1 (162,702,287) <____________Y|___________m|_________Ym|________mm|_______mm|_____Ymm|Y__Ymmm|m_mmmm|mmmmm|mmmm|mmm|mm|m> status=additional_form

cmap = {'_':0, 'Y':2, 'm':1}

coord_to_status = {}
size = None

def print_matrix(matrix):
    for row in reversed(matrix):
        print(row)

def output(px,py,pz, desc, status):
    global size
    
    diagonals = desc.split('|')
    if size is None:
        size = len(diagonals)
        print(size)
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
    
    print(json.dumps([status, filled_per_row, filled_per_col]))
    

for line in sys.stdin:
    match = re.search(r'\(([0-9]+),([0-9]+),([0-9]+)\) <([^>]+)> status=([^\s]+)', line)
    if not match:
        print(f"Cannot parse {line}")
        exit(1)
    x = int(match.group(1))
    y = int(match.group(2))
    z = int(match.group(3))
    desc = match.group(4)
    status = match.group(5)
    if status == 'additional_form':
        output(x,y,z, desc, coord_to_status[(x,y,z)])
    elif status == 'not_vertex':
        assert (x,y,z) not in coord_to_status
        coord_to_status[(x,y,z)] = 0
        output(x,y,z, desc, 0)
    elif status == 'vertex_and_previously':
        assert (x,y,z) not in coord_to_status
        coord_to_status[(x,y,z)] = 1
        output(x,y,z, desc, 1)
    else:
        print("Wrong status")
        exit(1)

    

