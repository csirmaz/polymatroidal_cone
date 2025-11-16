import sys
import os
import numpy as np
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import math
import json

LIMIT_COORDINATES = 2*1000*1000  # limit generated points to have smaller coordinates than this, or None
DATA_FILE = f'store/approximation.json'  # data needed for scad


def func_u(x):
    return (x+0.5)*math.log(x)

def func_f(u,v):
    return func_u(u+v) - func_u(u) - func_u(v)

def func_point(u,v):
    return (
        math.log(u) + func_f(u,v), # x
        math.log(v) + func_f(u,v), # y
        func_f(u,v) # z
    )

points = [
    (0,0,0),
    (0,LIMIT_COORDINATES,LIMIT_COORDINATES)
]
for u in range(0, 2000):
    for v in range(u, 2000):
        p = func_point( 1+2**(u/10.), 1+2**(v/10.) )
        if p[2] > LIMIT_COORDINATES: break
        if p[0] <= LIMIT_COORDINATES and p[1] <= LIMIT_COORDINATES and p[2] <= LIMIT_COORDINATES:
            points.append(p)
            if u != v:
                points.append((p[1], p[0], p[2]))
            # Extra points
            x, y = p[0], p[1]
            if x > y: y, x = x, y
            m = LIMIT_COORDINATES
            points.append((x, m, p[2]))
            points.append((m, x, p[2]))
    if p[2] > LIMIT_COORDINATES: break

chull = ConvexHull(np.array(points, dtype=np.float64), qhull_options="Qc")            

point_reindex = {} # {old_index: new_index (only vertices)
for pinew, piold in enumerate(chull.vertices):
    point_reindex[piold] = pinew
new_faces = [
    [point_reindex[pi] for pi in face]
    for face in chull.simplices
]
ohull = {'points': [chull.points[pi].tolist() for pi in chull.vertices], 'faces': new_faces}

with open(DATA_FILE, 'w') as f:
    f.write(json.dumps(ohull))

