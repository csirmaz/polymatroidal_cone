
# Expects the output of triads.c on STDIN

import sys
import os
import regex as re
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header

small_spheres = []
big_spheres = []
zoom = 1.
small_r = .01
big_r = .4
colors = {
    -1: [0,0,0],
    0: [0,0,1],
    1: [1,0,0],
    2: [0,1,0],
    3: [0,.8,.8],
    4: [.8,0,.8],
    5: [.6,.6,0],
    6: [.5,0,0],
}

def check_necessary_condition(desc):
    """Based on the string descriptor, check the necessary condition of being a vertex"""
    starters = 0
    enders = 0
    parts = 0
    for part in desc.split('|'):
        if re.match(r'^[Ym]*_*$', part): starters += 1
        if re.match(r'^_*[Ym]*$', part): enders += 1
        parts += 1
    return ((starters == parts) or (enders == parts))

points = []
descriptors = []

for line in sys.stdin:
    match = re.search(r'^Sum: ([0-9]+), ([0-9]+), ([0-9]+) \(([\-0-9]+)\) <([^>]+)>', line)
    if match:
        x = int(match.group(1)) * zoom
        y = int(match.group(2)) * zoom
        z = int(match.group(3)) * zoom
        itr = int(match.group(4))
        desc = match.group(5)
        points.append([x,y,z])        
        descriptors.append(f"({itr:2.0f}) <{desc}> {'NC' if check_necessary_condition(desc) else ''}")
        #small_spheres.append(Sphere(small_r).move([x,y,z]))
        #c = colors[itr]
        #big_spheres.append(Sphere(big_r).move([x,y,z]).color(*c))

points = np.array(points)
chull = ConvexHull(points)
# chull.points <n,3> coordinates
# chull.vertices <k> indices of points that are vertices

def printpoint(pi):
    print(f"[{points[pi][0]:4.0f}, {points[pi][1]:4.0f}, {points[pi][2]:4.0f}] {descriptors[pi]}")

print("VERTICES OF THE CONVEX HULL")
seen = set()
for pi in chull.vertices:
    p = chull.points[pi]
    p_org = points[pi]
    assert p[0] == p_org[0] and p[1] == p_org[1] and p[2] == p_org[2]
    printpoint(pi)
    seen.add(pi)

print("POINTS THAT ARE NOT VERTICES")
for pi in range(len(points)):
    if pi not in seen:
        printpoint(pi)
        


## OpenSCAD output
#hull = Collection(small_spheres).hull()
#print(Header("best").render())
#print(hull.render())
#print(Collection(big_spheres).render())
                
