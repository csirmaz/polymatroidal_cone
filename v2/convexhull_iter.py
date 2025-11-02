
# Expects the output of triads.c on STDIN
# Create a convex hull for each iteration
# Generate reports on vertices and scad

import sys
import os
import regex as re
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header, Polyhedron, Point, Cylinder

CREATE_SCAD = True

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
iteration_of_points = {}  # {<point index>: <iteration number>

faces_of_prev_iter = None
scad_hull_of_prev_iter = None

def get_scad_hull(chull):
    # Convert the Qhull hull into openscad
    point_reindex = {} # {old_index: new_index (only vertices)
    for pinew, piold in enumerate(chull.vertices):
        point_reindex[piold] = pinew
    new_faces = [
        [point_reindex[pi] for pi in face]
        for face in chull.simplices
    ]
    ohull = Polyhedron(points=[chull.points[pi] for pi in chull.vertices], faces=new_faces)
    # print(ohull.render_stl()); exit(0)
    return ohull.render()


def printpoint(pi, p, is_vertex):
    if pi in extra_points:
        status = 'extra'
    else:
        if is_vertex:
            status = 'vertex'
        else:
            if iteration_of_points[pi] < current_iteration:
                # Check that we are only dropping vertices from the previous iteration
                assert current_iteration == iteration_of_points[pi] + 1
                status = 'dropped_vertex'
            else:
                status = 'not_vertex'
    print(f"// #{pi:4.0f} [{p[0]:4.0f}, {p[1]:4.0f}, {p[2]:4.0f}] {f'NC {nc_points[pi]}' if pi in nc_points else ''} <{status}>")
    # {' '.join(point_to_facet.get(pi, []))} {coplanar.get(pi,'')}


def process_faces(chull):
    # Merge faces and find real edges
    # chull - hull of the current iteration
    #    This tells us which vertex is still a vertex
    #    Contains all points (even the dropped vertices)
    # faces_of_prev_iter - faces from the previous iteration with the new point indices
    #    Excludes faces that contained "extra" points
    
    if faces_of_prev_iter is None:
        return
    
    point_to_face = {}  # {<point index>: set(<face index>, ...), ...
    for fi, face in enumerate(faces_of_prev_iter):
        for pi in face:
            if pi not in point_to_face: point_to_face[pi] = set()
            point_to_face[pi].add(fi)
    
    face_normals = []  # [Point(), ...
    for face in faces_of_prev_iter:
        p1 = Point(chull.points[face[0]])
        p2 = Point(chull.points[face[1]])
        p3 = Point(chull.points[face[2]])
        norm = (p2-p1).cross(p3-p2).norm()
        face_normals.append(norm)

    seen_edges = set()
    
    def check_edge(pi1, pi2):
        key1 = f"{pi1}:{pi2}"
        if key1 in seen_edges: return None
    
        seen_edges.add(key1)
        seen_edges.add(f"{pi2}:{pi1}")
        
        r = point_to_face[pi1].intersection(point_to_face[pi2])
        assert len(r) > 0
        if len(r) == 1: return 'edge_of_area'
        assert len(r) == 2
        f1 = r.pop()
        f2 = r.pop()
        
        limit = 1e-7
        diff = face_normals[f1] - face_normals[f2]
        if abs(diff.c[0]) < limit and abs(diff.c[1]) < limit and abs(diff.c[2]) < limit:
            return 'non_edge'
        diff = face_normals[f1] + face_normals[f2]
        if abs(diff.c[0]) < limit and abs(diff.c[1]) < limit and abs(diff.c[2]) < limit:
            return 'non_edge'
        
        return 'real_edge'
    
    if CREATE_SCAD and scad_hull_of_prev_iter:
        scad_file = open(f"hull{current_iteration-1}.scad", 'w')
        scad_file.write(Header("best").render())
        scad_file.write(scad_hull_of_prev_iter)

    skipped_edges = 0
    seen_points = set()
    for fi, face in enumerate(faces_of_prev_iter):
        for pointpair in [
            [face[0], face[1]],
            [face[1], face[2]],
            [face[2], face[0]]
        ]:
            status = check_edge(*pointpair)
            if status is None or status == 'non_edge':
                skipped_edges += 1
                continue
            is_vertex1 = pointpair[0] in chull.vertices
            is_vertex2 = pointpair[1] in chull.vertices
            if not is_vertex1: status += " (p1 will drop)"
            if not is_vertex2: status += " (p2 will drop)"
            p1 = chull.points[pointpair[0]]
            p2 = chull.points[pointpair[1]]
            print(f"// EDGE({p1[0]:.0f},{p1[1]:.0f},{p1[2]:.0f})-({p2[0]:.0f},{p2[1]:.0f},{p2[2]:.0f}): {status} (iteration={current_iteration-1})")
            printpoint(pointpair[0], p1, is_vertex1)
            printpoint(pointpair[1], p2, is_vertex2)
            print("")
            
            if CREATE_SCAD and scad_hull_of_prev_iter:
                c = [.2, .2, 1.]
                if status == 'edge_of_area': c = [.2, 1., 1.]
                scad_file.write(Cylinder.from_ends(.1, p1, p2).color(*c).render())
            
        if CREATE_SCAD and scad_hull_of_prev_iter:
            for pi in face:
                if pi in seen_points: continue
                seen_points.add(pi)
                c = [.2, .2, 1.]
                if pi not in chull.vertices: # will drop
                    c = [1., .2, .2]
                scad_file.write(Sphere(.4).move(chull.points[pi]).color(*c).render())
    
    if CREATE_SCAD and scad_hull_of_prev_iter:
        scad_file.close()
    
    print(f"// skipped edges: {skipped_edges}")
    print("")
            

def process_points():
    global max_y, max_z, current_iteration, points, nc_points, extra_points, iteration_of_points, faces_of_prev_iter, scad_hull_of_prev_iter
    
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
    # chull.points shape=<n,3> coordinates of all points
    # chull.vertices shape=<k> indices of points that are vertices
    # chull.simplices  triads of indices of points that form faces (triangulated)
    # We have previously checked that chull.points === points (indices match)
    points = None
    
    # Process faces from the previous iteration
    # We have already reindexed the points to match this iteration
    process_faces(chull)
    
    # Create report
    
    print(f"// VERTICES OF THE CONVEX HULL iteration={current_iteration}")
    seen = set()
    for pi in chull.vertices:
        if pi in extra_points:
            # Check that we have added this extra point (which is a vertex)
            # from a point which is also a vertex
            assert extra_points[pi] in chull.vertices
        else:
            # Check that the necessary condition applies to all vertices
            assert pi in nc_points  
        p = chull.points[pi]
        printpoint(pi, p, True)
        seen.add(pi)

    print(f"// POINTS THAT ARE NOT VERTICES (but satisfy the necessary condition and/or were vertices before) iteration={current_iteration}")
    for pi in range(len(chull.points)):
        if pi not in seen:
            if pi in nc_points:
                printpoint(pi, p, False)
                
    print(f"// REPORT END iteration={current_iteration}", flush=True)
    
    print(f"// ! Reindexing points...")
    
    # Initialize new input from the vertices of this hull to save space/time
    points = []  # global
    nc_points_new = {}  # local
    iteration_of_points_new = {}  # local
    point_reindex = {}  # local, {<old point index>: <new point index>, ...
    for pi in chull.vertices:
        if pi not in extra_points:
            p = chull.points[pi]
            pi_new = len(points)
            # print(f"//  Mapping #{pi} -> #{pi_new}")
            point_reindex[pi] = pi_new
            points.append(p)
            if pi in nc_points: nc_points_new[pi_new] = nc_points[pi]
            if pi in iteration_of_points: iteration_of_points_new[pi_new] = iteration_of_points[pi]
        
    # Store faces for processing later
    # We do it in the next step as that's when we know which vertices stay
    faces_of_prev_iter = []
    for face in chull.simplices:
        face = [int(v) for v in face]
        if face[0] not in extra_points and face[1] not in extra_points and face[2] not in extra_points:
            faces_of_prev_iter.append([
                point_reindex[face[0]],
                point_reindex[face[1]],
                point_reindex[face[2]]
            ])

    if CREATE_SCAD:
        scad_hull_of_prev_iter = get_scad_hull(chull)

    # Set global data structures
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
            nc_points[pi_new] = f"<{desc}> (iter={itr})"
            iteration_of_points[pi_new] = itr

process_points()


## Process coplanar data
#coplanar = {}
#for d in chull.coplanar:
#    coplanar[d[0]] = f"coplanar with f{d[1]}, its closest vertex is #{d[2]}"
