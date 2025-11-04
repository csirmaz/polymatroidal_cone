

import sys
import os
import regex as re
import json
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np

UP_TO_ITER = 14
POINT_FILE = 'pointdata'

cmap = {'_':0, 'Y':2, 'm':1}

diagonal_num = None


def print_matrix(matrix):
    for row in reversed(matrix):
        print(row)


def check_nv(fill):
    """Check the necessary condition based on a fill vector"""
    failed = False
    for i, v in enumerate(fill):
        if i>0 and v >= fill[i-1] and fill[i-1] > 0:
            failed = True
            break
    if not failed:
        return True
    fill = invert_fill(fill)
    failed = False
    for i, v in enumerate(fill):
        if i>0 and v >= fill[i-1] and fill[i-1] > 0:
            failed = True
            break
    if not failed:
        return True
    return False


def invert_fill(fill):
    """Given a row or column fill vector, return the other fill vector"""
    
    maxlength = len(fill)
    matrix = []
    for j in range(maxlength):
        matrix.append([0 for i in range(maxlength)])

    for j, f in enumerate(fill):
        for i in range(f):
            matrix[j][i] = 1
            
    inverse = [0 for i in range(maxlength)]
            
    for j, row in enumerate(matrix):
        for i, v in enumerate(row):
            if v>0 and j+1>inverse[i]: inverse[i]=j+1

    return inverse

assert invert_fill([0,0,0]) == [0,0,0]
assert invert_fill([1]) == [1]
assert invert_fill([3,0,0]) == [1,1,1]
assert invert_fill([4,3,0,0]) == [2,2,2,1]
assert invert_fill([1,1,1]) == [3,0,0]
    

def desc_to_fills(desc, itr):
    """Given a "_/Y" diagonal description, return the fill rate per column and row"""
    # itr is the iteration where the point appeared
    global diagonal_num

    diagonals = desc.split('|')
    if diagonal_num is None:
        diagonal_num = len(diagonals)
    else:
        assert len(diagonals) == diagonal_num
    
    matrix = []
    for j in range(diagonal_num):
        matrix.append([0 for i in range(diagonal_num)])
        
    for di, diag in enumerate(diagonals):
        for ci, c in enumerate(diag):
            coord_sum = diagonal_num - 1 - di
            i = ci
            j = coord_sum - ci
            matrix[j][i] = cmap[c]
            
    #print(desc)
    #print_matrix(matrix)
    
    filled_per_row = [0 for i in range(diagonal_num)]
    filled_per_col = [0 for i in range(diagonal_num)]
    
    for j, row in enumerate(matrix):
        c = 0
        while c<diagonal_num and row[c] > 0: c += 1
        filled_per_row[j] = c
        
    for j, row in enumerate(matrix):
        for i, v in enumerate(row):
            if v>0 and j+1>filled_per_col[i]: filled_per_col[i]=j+1
            
    #print(filled_per_row)
    #print(filled_per_col)
    
    for i in range(diagonal_num):
        if i > itr:
            assert filled_per_col[i] == 0
            assert filled_per_row[i] == 0
    
    desc_short = "|".join(diagonals[(-UP_TO_ITER-1):])
    return (tuple(filled_per_row[:UP_TO_ITER+1]), tuple(filled_per_col[:UP_TO_ITER+1]), desc_short)

Points = {}        # { (x,y,z):   [[*], [(iter, rowfill, colfill),... ]]...
                    # [*]: 0=not vertex  1=vertex  2=vertex&generated
RowFill2Data = {}  # { <rowfill>: (iter, (x,y,z)), ...
ColFill2Data = {}
Stats = {
    'org_not_vertex': 0,
    'not_nc': 0,
    'not_listed': 0,
    'not_vertex': 0
}


def read_point_data():
    
    iter_done = set()

    for line in open(POINT_FILE, 'r'):
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
        pkey = (x,y,z)
        desc = match.group(4)
        status = match.group(5)
        itr = int(match.group(6))
        if itr > UP_TO_ITER:
            continue
        fill_per_row, fill_per_col, desc_short = desc_to_fills(desc, itr)
        
        #assert check_nv(fill_per_col)
        #assert check_nv(fill_per_row)
        
        print(f"Point: {pkey} iter={itr} rows={fill_per_row} cols={fill_per_col} desc={desc_short} <{status}>")
        
        if status == 'additional_form':
            Points[pkey][1].append((itr, fill_per_row, fill_per_col, desc_short))
            RowFill2Data[fill_per_row] = (itr, pkey)
            ColFill2Data[fill_per_col] = (itr, pkey)
        elif status == 'not_vertex':
            assert pkey not in Points
            Points[pkey] = [0, [(itr, fill_per_row, fill_per_col, desc_short)]]
            RowFill2Data[fill_per_row] = (itr, pkey)
            ColFill2Data[fill_per_col] = (itr, pkey)
            Stats['org_not_vertex'] += 1
        elif status == 'vertex_and_previously':
            assert pkey not in Points
            Points[pkey] = [1, [(itr, fill_per_row, fill_per_col, desc_short)]]
            RowFill2Data[fill_per_row] = (itr, pkey)
            ColFill2Data[fill_per_col] = (itr, pkey)
        else:
            print("Wrong status")
            exit(1)

    if UP_TO_ITER+1 not in iter_done:
        msg = f"ERROR: iter {UP_TO_ITER+1} is not complete, which is needed to see the final vertices\n"
        print(msg)
        exit(1)


def find_recursion():
    for target_itr in range(UP_TO_ITER+1):
        for coords, pdata in Points.items():
            is_vertex, sources = pdata
            if is_vertex == 0: continue
            for itr, fill_per_row, fill_per_col, desc in sources:
                # print(f"{fill_per_row} :: {fill_per_col} :: {desc}")
                # These come from the necessary condition
                ender = (fill_per_row[0] == itr+1)
                beginner = (fill_per_col[0] == itr+1)
                assert beginner or ender
                
                if fill_per_col[-1] > 0 or fill_per_row[-1] > 0:
                    # Last iteration processed, cannot generate points
                    continue
                
                if itr >= target_itr:
                    continue
                
                if ender:
                    candidate_row = tuple([target_itr+1] + list(fill_per_row)[:-1])                
                    if candidate_row not in RowFill2Data:
                        if check_nv(candidate_row):
                            print(f"L From {coords} and row fill [{fill_per_row}] generated [{candidate_row}] but it is not a listed point (but NC)")
                            Stats['not_listed'] += 1
                        else:
                            print(f"C From {coords} and row fill [{fill_per_row}] generated [{candidate_row}] which is not NC")
                            Stats['not_nc'] += 1
                    else:
                        c_row_coords = RowFill2Data[candidate_row][1]
                        if Points[c_row_coords][0] == 0:
                            print(f"N From {coords} and row fill [{fill_per_row}] generated [{candidate_row}] = {c_row_coords} but it is not a vertex")
                            Stats['not_vertex'] += 1
                        else:
                            Points[c_row_coords][0] = 2
                            print(f"v From {coords} and row fill [{fill_per_row}] generated [{candidate_row}] = {c_row_coords} which is a vertex")

                if beginner:
                    candidate_col = tuple([target_itr+1] + list(fill_per_col)[:-1])
                    if candidate_col not in ColFill2Data:
                        if check_nv(candidate_col):
                            print(f"L From {coords} and col fill [{fill_per_col}] generated [{candidate_col}] but it is not a listed point (but NC)")
                            Stats['not_listed'] += 1
                        else:
                            print(f"C From {coords} and col fill [{fill_per_col}] generated [{candidate_col}] which is not NC")
                            Stats['not_nc'] += 1
                    else:
                        c_col_coords = ColFill2Data[candidate_col][1]
                        if Points[c_col_coords][0] == 0:
                            print(f"N From {coords} and col fill [{fill_per_col}] generated [{candidate_col}] = {c_col_coords} but it is not a vertex")
                            Stats['not_vertex'] += 1
                        else:
                            Points[c_col_coords][0] = 2
                            print(f"v From {coords} and col fill [{fill_per_col}] generated [{candidate_col}] = {c_col_coords} which is a vertex")

    print("")
    print(f"Using points from iteration {UP_TO_ITER-1} and below as a basis")
    print(f"{Stats['not_nc']} points were generated that do not satisfy the necessary condition")
    print(f"{Stats['not_listed']} points were generated that satisfy the necessary condition but were not listed (shouldn't happen)")
    print(f"{Stats['not_vertex']} points were generated that were not vertices\n (compare: {Stats['org_not_vertex']} points satisfy the necessary condition that are not vertices)")

    for coords, pdata in Points.items():
        is_vertex, sources = pdata
        if is_vertex == 1:
            print(f"Point {coords} is a vertex but has not been generated")
    print(f"All other vertices up to iteration {UP_TO_ITER} (that do not disappear at least until iteration {UP_TO_ITER+1}) were generated")

read_point_data()
find_recursion()
