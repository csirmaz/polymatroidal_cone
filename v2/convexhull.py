
# Expects the output of triads.c on STDIN
# Create convex hull
# Generate report on vertices and scad

import sys
import os
import regex as re
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header, Polyhedron

DO_PLOTS = True

def plot_2d(desc):
    """2D plot of the subset of points from the bitmap"""
    parts = desc.split('|')
    o = ''
    for j in range(len(parts)):
        o += '// = '
        for i in range(j+1):
            real_j = len(parts) - j - 1
            diagonal = real_j + i
            #c=f"{real_j}{i}[{diagonal}]"
            c = parts[len(parts) - diagonal - 1][i]
            if c=='_':
                o += '. '
            else:
                o += c+' '
        o += "\n"
    return o


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

points = []  # [[x,y,z], ...
descriptors = []  # ["desc", ...
desc_plots = []
nc_points = []  # indices of NC points
max_y = 0
max_z = 0

for line in sys.stdin:
    match = re.search(r'^Sum: ([0-9]+), ([0-9]+), ([0-9]+) \(([\-0-9]+)\) <([^>]+)>', line)
    if match:
        x = int(match.group(1))
        y = int(match.group(2))
        z = int(match.group(3))
        itr = int(match.group(4))
        desc = match.group(5)
        points.append([x,y,z])
        if max_y < y: max_y = y
        if max_z < z: max_z = z
        if check_necessary_condition(desc): nc_points.append(len(points)-1)
        descriptors.append(f"({itr:2.0f}) <{desc}>")
        if DO_PLOTS:
            desc_plots.append(plot_2d(desc))
        #small_spheres.append(Sphere(small_r).move([x,y,z]))
        #c = colors[itr]
        #big_spheres.append(Sphere(big_r).move([x,y,z]).color(*c))
print("// reading points done", flush=True)

# Extra points
assert max_y == max_z
print(f"// max = {max_y}")
max_v = max_y*2.
current_points = len(points)
for pi in range(current_points):
    p = points[pi]
    points.append([p[0], p[1], max_v-p[1]]); descriptors.append(f"from #{pi}")
    points.append([p[0], max_v-p[2], p[2]]); descriptors.append(f"from #{pi}")

points = np.array(points)
print("// generating points done", flush=True)
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
    coplanar[d[0]] = f"coplanar with f{d[1]}, its closest vertex is #{d[2]}"

def printpoint(pi, prefix='', plot=True):
    print(f"// {prefix} #{pi:4.0f} [{points[pi][0]:4.0f}, {points[pi][1]:4.0f}, {points[pi][2]:4.0f}] {descriptors[pi]} {'NC' if pi in nc_points else ''} {' '.join(point_to_facet.get(pi, []))} {coplanar.get(pi,'')}")
    if DO_PLOTS and plot and pi < len(desc_plots):
        if pi in nc_points:
            print(desc_plots[pi], end="")

print("// VERTICES OF THE CONVEX HULL")
seen = set()
for pi in chull.vertices:
    p = chull.points[pi]
    p_org = points[pi]
    assert p[0] == p_org[0] and p[1] == p_org[1] and p[2] == p_org[2]
    printpoint(pi, 'vertex')
    seen.add(pi)

print("// POINTS THAT ARE NOT VERTICES")
for pi in range(len(points)):
    if pi not in seen:
        if not descriptors[pi].startswith('from'):
            printpoint(pi, 'not vertex')
        

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
    # print(ohull.render_stl()); exit(0)
    print(Header("best").render())
    print(ohull.render())
    
    # Add points that are not vertices but NC (and perhaps coplanar)
    if False:
        for pi in range(len(points)):
            if pi not in seen:
                if pi in nc_points:
                    c = [0,.2,1]
                    if pi in coplanar: c=[1,0,0]
                    print(Sphere(.3).move(points[pi]).color(*c).render())
            
