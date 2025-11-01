
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

CREATE_SCAD = False

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

max_y = 0
max_z = 0
current_iteration = None

# THESE RESET WITH EACH ITERATION:

points = []  # [[x,y,z], ...
nc_points = {}  # {<point index>: <description str>, ...
extra_points = {}  # {<point index>: <org point index>
iteration_of_points = {}

def printpoint(pi, p, is_vertex):
    if pi in extra_points:
        status = 'extra'
    else:
        if is_vertex:
            status = 'vertex'
        else:
            if iteration_of_points[pi] < current_iteration:
                status = 'dropped_vertex'
            else:
                status = 'not_vertex'
    print(f"// #{pi:4.0f} [{p[0]:4.0f}, {p[1]:4.0f}, {p[2]:4.0f}] <{status}> {f'NC {nc_points[pi]}' if pi in nc_points else ''}")
    # {' '.join(point_to_facet.get(pi, []))} {coplanar.get(pi,'')}


def process_points():
    global max_y, max_z, current_iteration, points, nc_points, extra_points, iteration_of_points
    
    # Add extra points to create triangles
    assert max_y == max_z
    max_v = max_y*2.
    current_points = len(points)
    for pi in range(current_points):
        p = points[pi]
        points.append([p[0], p[1], max_v-p[1]]); extra_points[len(points)-1] = pi
        points.append([p[0], max_v-p[2], p[2]]); extra_points[len(points)-1] = pi
        
    points = np.array(points)
    
    chull = ConvexHull(points, qhull_options="Qc")
    # chull.points <n,3> coordinates
    # We have previously checked that chull.points === points (indices match)
    points = None
    
    # Create report
    
    print(f"// VERTICES OF THE CONVEX HULL iteration={current_iteration}")
    seen = set()
    for pi in chull.vertices:
        if not pi in extra_points:
            assert pi in nc_points  # Check that the necessary condition applies to all vertices
        p = chull.points[pi]
        printpoint(pi, p, True)
        seen.add(pi)

    print(f"// POINTS THAT ARE NOT VERTICES (but satisfy the necessary condition and/or were vertices before) iteration={current_iteration}")
    for pi in range(len(chull.points)):
        if pi not in seen:
            if pi in nc_points:
                printpoint(pi, p, False)
                
    print(f"// REPORT END iteration={current_iteration}", flush=True)
    
    # Initialize new input from the vertices of this hull to save space/time
    points = []
    nc_points_new = {}
    iteration_of_points_new = {}
    for pi in chull.vertices:
        if not pi in extra_points:
            p = chull.points[pi]
            pi_new = len(points)
            # print(f"//  Mapping #{pi} -> #{pi_new}")
            points.append(p)
            if pi in nc_points: nc_points_new[pi_new] = nc_points[pi]
            if pi in iteration_of_points: iteration_of_points_new[pi_new] = iteration_of_points[pi]
        
    nc_points = nc_points_new; del nc_points_new
    iteration_of_points = iteration_of_points_new; del iteration_of_points_new
    extra_points = {}
    

for line in sys.stdin:
    match = re.search(r'^Sum: ([0-9]+), ([0-9]+), ([0-9]+) \(([\-0-9]+)\) <([^>]+)>', line)
    if match:
        x = int(match.group(1))
        y = int(match.group(2))
        z = int(match.group(3))
        itr = int(match.group(4))
        desc = match.group(5)
        
        if current_iteration is None or current_iteration < itr:
            if current_iteration is not None and current_iteration >= 2:
                # Process the points so far
                process_points()
            current_iteration = itr

        pi_new = len(points)
        points.append([x,y,z])
        if max_y < y: max_y = y
        if max_z < z: max_z = z
        if check_necessary_condition(desc):
            # save the descriptor string but only if the necessary condition is met
            nc_points[pi_new] = f"({itr:2.0f}) <{desc}>"
            iteration_of_points[pi_new] = itr

process_points()


# chull.vertices <k> indices of points that are vertices

## Process facets
#point_to_facet = {} # {pix: [f.., f..
#for ix, d in enumerate(chull.simplices): # [[p1, p2, p3], ...
#    for pix in d:
#        if pix not in point_to_facet: point_to_facet[pix] = []
#        point_to_facet[pix].append(f"f{ix}")

## Process coplanar data
#coplanar = {}
#for d in chull.coplanar:
#    coplanar[d[0]] = f"coplanar with f{d[1]}, its closest vertex is #{d[2]}"


        
if CREATE_SCAD:
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
            
