
# Expects the output of triads.c on STDIN

import sys
import os
import regex as re
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header, Polyhedron

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
nc_points = [] # indices of NC points

for line in sys.stdin:
    match = re.search(r'^Sum: ([0-9]+), ([0-9]+), ([0-9]+) \(([\-0-9]+)\) <([^>]+)>', line)
    if match:
        x = int(match.group(1)) * zoom
        y = int(match.group(2)) * zoom
        z = int(match.group(3)) * zoom
        itr = int(match.group(4))
        desc = match.group(5)
        points.append([x,y,z])
        is_nc = check_necessary_condition(desc)
        if is_nc: nc_points.append(len(points)-1)
        descriptors.append(f"({itr:2.0f}) <{desc}>")
        #small_spheres.append(Sphere(small_r).move([x,y,z]))
        #c = colors[itr]
        #big_spheres.append(Sphere(big_r).move([x,y,z]).color(*c))

points = np.array(points)
chull = ConvexHull(points, qhull_options="Qc")
# chull.points <n,3> coordinates
# chull.vertices <k> indices of points that are vertices

# Process facets
point_to_facet = {} # {pix: [f.., f..
for ix, d in enumerate(chull.simplices): # [[p1, p2, p3], ...
    for pix in d:
        if pix not in point_to_facet: point_to_facet[pix] = []
        point_to_facet[pix].append(f"f{ix}")

# Process coplanar data
coplanar = {}
for d in chull.coplanar:
    coplanar[d[0]] = f"cp(f{d[1]} #{d[2]})"

def printpoint(pi):
    print(f"// #{pi:4.0f} [{points[pi][0]:4.0f}, {points[pi][1]:4.0f}, {points[pi][2]:4.0f}] {descriptors[pi]} {'NC' if pi in nc_points else ''} {' '.join(point_to_facet.get(pi, []))} {coplanar.get(pi,'')}")

print("// VERTICES OF THE CONVEX HULL")
seen = set()
for pi in chull.vertices:
    p = chull.points[pi]
    p_org = points[pi]
    assert p[0] == p_org[0] and p[1] == p_org[1] and p[2] == p_org[2]
    printpoint(pi)
    seen.add(pi)

print("// POINTS THAT ARE NOT VERTICES")
for pi in range(len(points)):
    if pi not in seen:
        printpoint(pi)
        

## OpenSCAD output
#hull = Collection(small_spheres).hull()
#print(Header("best").render())
#print(hull.render())
#print(Collection(big_spheres).render())

if False:
    # Convert the Qhull hull into openscad
    point_reindex = {} # {old_index: new_index (only vertices)
    for pinew, piold in enumerate(chull.vertices):
        point_reindex[piold] = pinew
    new_faces = [
        [point_reindex[pi] for pi in face]
        for face in chull.simplices
    ]
    ohull = Polyhedron(points=[points[pi] for pi in chull.vertices], faces=new_faces)
    print(Header("best").render())
    print(ohull.render())
    
    # Add points that are not vertices but NC (and perhaps coplanar)
    for pi in range(len(points)):
        if pi not in seen:
            if pi in nc_points:
                c = [0,.2,1]
                if pi in coplanar: c=[1,0,0]
                print(Sphere(big_r).move(points[pi]).color(*c).render())
            
