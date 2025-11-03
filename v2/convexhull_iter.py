
# Expects the output of triads.c on STDIN (for the maximum iteration)
# Create a convex hull for each iteration (identified by the points returned)
# Generate reports on vertices, edges and scad

import sys
import os
import regex as re
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header, Polyhedron, Point, Cylinder

CREATE_SCAD = False # Whether to create an scad file for each iteration
CREATE_POINT_DATA = True


if CREATE_POINT_DATA:
    point_data_file = open("pointdata", "w")


class PObjectClass:
    pass

class GlobalClass:
    pass

def assrt(test, msg):
    if not test:
        print(f"ERROR: {msg}")
        exit(1)

def get_pkey(p) -> str:
    return f"{p[0]:.0f},{p[1]:.0f},{p[2]:.0f}"

def check_necessary_condition(desc: str) -> bool:
    """Based on the string descriptor, check the necessary condition of being a vertex"""
    starters = 0
    enders = 0
    parts = 0
    for part in desc.split('|'):
        if re.match(r'^[Ym]*_*$', part): starters += 1
        if re.match(r'^_*[Ym]*$', part): enders += 1
        parts += 1
    return ((starters == parts) or (enders == parts))


# Read output of triad.c
def read_raw_data():
    current_iteration = None
    
    globalv = GlobalClass()
    globalv.max_y = 0
    globalv.max_z = 0
    globalv.all_points_in_outdata = {}
    globalv.nc_points_info = {}  # {<point_key>: [(<desc string>, <iteration>) ...   Only contains points satisfying necessary condition
    
    Pobj = None
    PrevPobj = None
    
    for line in sys.stdin:
        match = re.search(r'^Sum: ([0-9]+), ([0-9]+), ([0-9]+) \(([\-0-9]+)\) <([^>]+)>', line)
        if match:
            x = int(match.group(1))
            y = int(match.group(2))
            z = int(match.group(3))
            itr = int(match.group(4))
            desc = match.group(5)
            
            if current_iteration is None or current_iteration < itr:
                if current_iteration is not None:
                    # Process the points so far
                    process_points(Pobj, PrevPobj)
                    if CREATE_POINT_DATA: output_point_data_end_iter(Pobj)
                    PrevPobj = Pobj
                Pobj = PObjectClass()
                Pobj.Iteration = itr
                Pobj.Global = globalv  # object that keeps changing
                # We MUST start with the retained points (vertices from the prev iteration) so we can check which vertex remains by index
                # Cloning for sanity
                Pobj.RawPoints = list(PrevPobj.RetainPoints) if PrevPobj else []
                current_iteration = itr

            # Points CAN be repeated from different sets
            
            # We can prove that vertices must satisfy the necessary condition (NC)
            if not check_necessary_condition(desc):
                continue

            if globalv.max_y < y: globalv.max_y = y
            if globalv.max_z < z: globalv.max_z = z
            p = [x,y,z]
            pkey = get_pkey(p)

            # Save the descriptor string
            if pkey in globalv.nc_points_info:
                globalv.nc_points_info[pkey].append((desc, itr))
                if CREATE_POINT_DATA and pkey in globalv.all_points_in_outdata:
                    output_point_data_msg_add(pkey=pkey, desc=desc, Pobj=Pobj)                    
                # Skip repeated points - important as we can't know which form of a duplicate qhull would keep
                continue
                
            globalv.nc_points_info[pkey] = [(desc, itr)]
            
            Pobj.RawPoints.append(p)

    # Process last iteration
    process_points(Pobj, PrevPobj)


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
    return ohull.render()


def process_points(Pobj, PrevPobj):
    
    if PrevPobj:
        assrt(PrevPobj.Iteration + 1 == Pobj.Iteration, "PrePobj iteration")
    
    print(f"// process_points: starting with {len(Pobj.RawPoints)} points (retained+loaded), iter={Pobj.Iteration}")
    
    if Pobj.Iteration < 2:
        Pobj.RetainPoints = Pobj.RawPoints
        return
    
    # Add extra points to create triangles
    assrt(Pobj.Global.max_y == Pobj.Global.max_z, "max y != max z")
    max_v = Pobj.Global.max_y * 2.
    
    Pobj.ExtraPointsFrom = len(Pobj.RawPoints)
    
    if True: # OPTIMIZED EXTRA POINTS
        # We know for every (x,y,z) point we also have (x,z,y)
        # (x,y,z) -> (x, y, m-y); (x, m-z, z)
        # (x,z,y) -> (x, z, m-z); (x, m-y, y)
        seen_yz = set()
        for pi in range(Pobj.ExtraPointsFrom):
            ep = Pobj.RawPoints[pi]
            ex = ep[0]
            ey = ep[1]
            ez = ep[2]
            if ey > ez: (ez, ey) = (ey, ez)
            key = (ey, ez)
            if key in seen_yz: continue
            seen_yz.add(key)
            Pobj.RawPoints.append([ex, ey, max_v-ey])
            Pobj.RawPoints.append([ex, max_v-ey, ey])
        del seen_yz
    else: # ORG LOGIC
        for pi in range(Pobj.ExtraPointsFrom):
            p = Pobj.RawPoints[pi]
            Pobj.RawPoints.append([p[0], p[1], max_v-p[1]])
            Pobj.RawPoints.append([p[0], max_v-p[2], p[2]])
    
    Pobj.Hull = ConvexHull(np.array(Pobj.RawPoints), qhull_options="Qc")
    # Hull.points shape=<n,3> coordinates of all points
    # Hull.vertices shape=<k> indices of points that are vertices
    # Hull.simplices  triads of indices of points that form faces (triangulated)
    # We have previously checked that Hull.points === input points (indices match)
    del Pobj.RawPoints
    
    # Process faces from the previous iteration
    # We have already reindexed the points to match this iteration
    if PrevPobj and hasattr(PrevPobj, 'Simplices'):
        process_faces(Pobj, PrevPobj)
    
    # Create report
    
    def assess_point(*, pi, p, pkey, Pobj, PrevPobj, currently_vertex):
        was_vertex = False
        # IMPORTANT Pobj.RawPoints starts with PrevPobj.RetainPoints
        if PrevPobj and pi < len(PrevPobj.RetainPoints):
            was_vertex = True

        is_extra = (pi >= Pobj.ExtraPointsFrom)

        if currently_vertex:
            if was_vertex:
                status = 'vertex_and_previously'  # a final vertex?
            else:
                status = 'new_vertex'
        else:
            if was_vertex:
                status = 'dropped_vertex'
                # Check that we are only dropping vertices from the previous iteration
                pkey = get_pkey(p)
                assrt(Pobj.Iteration == Pobj.Global.nc_points_info[pkey][0][1] + 1, 'only dropping vertex from prev iter')
            else:
                status = 'not_vertex'
        if is_extra:
            status += '(extra)'
        
        print_point(pi=pi, p=p, Pobj=Pobj, status=status)
        if CREATE_POINT_DATA and (not is_extra) and (status in ['vertex_and_previously', 'dropped_vertex', 'not_vertex']):
            output_point_data(pi=pi, p=p, Pobj=Pobj, status=(status if status == 'vertex_and_previously' else 'not_vertex'))
    
    print(f"// VERTICES OF THE CONVEX HULL iteration={Pobj.Iteration}")
    seen_pk = set()
    for pi in Pobj.Hull.vertices:
        p = Pobj.Hull.points[pi]
        pkey = get_pkey(p)
        if pi < Pobj.ExtraPointsFrom:  # not an extra point
            # Check that the necessary condition (NC) applies to all vertices
            assrt(pkey in Pobj.Global.nc_points_info, "vertex not NC")
        assess_point(pi=pi, p=p, pkey=pkey, Pobj=Pobj, PrevPobj=PrevPobj, currently_vertex=True)
        assrt(pkey not in seen_pk, "point listed as vertex multiple times")
        seen_pk.add(pkey)

    print(f"// POINTS THAT ARE NOT VERTICES (but satisfy the necessary condition and/or were vertices before) iteration={Pobj.Iteration}")
    for pi in range(len(Pobj.Hull.points)):
        p = Pobj.Hull.points[pi]
        pkey = get_pkey(p)
        if pkey not in seen_pk:
            if pkey in Pobj.Global.nc_points_info:
                assess_point(pi=pi, p=p, pkey=pkey, Pobj=Pobj, PrevPobj=PrevPobj, currently_vertex=False)
                
    print(f"// REPORT END for iteration={Pobj.Iteration}", flush=True)
    
    # Initialize new input from the vertices of this hull to save space/time
    Pobj.RetainPoints = []
    old2new_index = {}
    for pi in Pobj.Hull.vertices:
        if pi < Pobj.ExtraPointsFrom:  # not extra point
            old2new_index[pi] = len(Pobj.RetainPoints)
            p = Pobj.Hull.points[pi]
            Pobj.RetainPoints.append(p)
        
    # Store faces for processing later
    # We do it in the next step as that's when we know which vertices stay
    # Reindex data to index into RetainPoints
    Pobj.Simplices = []
    for face in Pobj.Hull.simplices:
        face = [int(v) for v in face]
        if max(face) < Pobj.ExtraPointsFrom:
            Pobj.Simplices.append([
                old2new_index[face[0]],
                old2new_index[face[1]],
                old2new_index[face[2]]
            ])

    if CREATE_SCAD:
        Pobj.ScadOfHull = get_scad_hull(Pobj.Hull)

    del Pobj.Hull


def output_point_data_end_iter(Pobj):
    point_data_file.write(f"--- iter={Pobj.Iteration} done\n")
    point_data_file.flush()

def output_point_data_msg_add(*, pkey, desc, Pobj):
        point_data_file.write(f"#-1/-1 ({pkey}) <{desc}> status=additional_form iter={Pobj.Iteration}\n")

def output_point_data_msg(*, pi, pkey, Pobj, status):
    for i, info in enumerate(Pobj.Global.nc_points_info[pkey]):
        point_data_file.write(f"#{Pobj.Iteration}/{pi} ({pkey}) <{info[0]}> status={status}{'(additional_form)' if i>0 else ''} iter={Pobj.Iteration}\n")

def output_point_data(*, pi, p, Pobj, status):
    # Write point data to a file
    if not CREATE_POINT_DATA: return
    pkey = get_pkey(p)
    seen = Pobj.Global.all_points_in_outdata
    if pkey in seen:
        if seen[pkey] != status:
            point_data_file.write(f"ERROR: STATUS OF POINT CHANGED FROM {seen[pkey]} TO:\n")
            output_point_data_msg(pi=pi, pkey=pkey, Pobj=Pobj, status=status)
            assert False
        return
    seen[pkey] = status
    output_point_data_msg(pi=pi, pkey=pkey, Pobj=Pobj, status=status)


def print_point(*, pi, p, Pobj, status):
    pkey = get_pkey(p)
    
    if hasattr(Pobj, 'Hull'):
        pkey2 = get_pkey(Pobj.Hull.points[pi])
    elif hasattr(Pobj, 'RetainPoints'):
        pkey2 = get_pkey(Pobj.RetainPoints[pi])
    assrt(pkey == pkey2, 'printpoint pi ~ p')
    
    if pi >= Pobj.ExtraPointsFrom:
        info = [('', 'E')]
    else:
        info = Pobj.Global.nc_points_info[pkey]
    for i, di in enumerate(info):
        point_iter = di[1]
        point_desc = di[0]
        print(f"// #{Pobj.Iteration}/{pi:4.0f} [{p[0]:4.0f}, {p[1]:4.0f}, {p[2]:4.0f}] <{point_desc}> iter={point_iter} <{status}{'(additional_form)' if i>0 else ''}>")


def process_faces(Pobj, PrevPobj):
    # Find real edges
    
    assrt(PrevPobj.Iteration + 1 == Pobj.Iteration, "PrevPobj iteration #2")
    
    PrevPobj.Point2Face = {}  # {<point index>: set(<face index>, ...), ...
    for fi, face in enumerate(PrevPobj.Simplices):
        for pi in face:
            if pi not in PrevPobj.Point2Face: PrevPobj.Point2Face[pi] = set()
            PrevPobj.Point2Face[pi].add(fi)
    
    PrevPobj.FaceNormals = []  # [Point(), ...
    for face in PrevPobj.Simplices:
        p1 = Point(PrevPobj.RetainPoints[face[0]])
        p2 = Point(PrevPobj.RetainPoints[face[1]])
        p3 = Point(PrevPobj.RetainPoints[face[2]])
        norm = (p2-p1).cross(p3-p2).norm()
        PrevPobj.FaceNormals.append(norm)

    seen_edges = set()  # Set of string keys "<point index>:<point index>"
    
    def check_edge(pi1, pi2):
        key1 = f"{pi1}:{pi2}"
        if key1 in seen_edges: return None
    
        seen_edges.add(key1)
        seen_edges.add(f"{pi2}:{pi1}")
        
        r = PrevPobj.Point2Face[pi1].intersection(PrevPobj.Point2Face[pi2])
        assert len(r) > 0
        if len(r) == 1: return 'edge_of_area'
        assert len(r) == 2
        f1 = r.pop()
        f2 = r.pop()
        
        limit = 1e-7
        diff = PrevPobj.FaceNormals[f1] - PrevPobj.FaceNormals[f2]
        if abs(diff.c[0]) < limit and abs(diff.c[1]) < limit and abs(diff.c[2]) < limit:
            return 'non_edge'
        diff = PrevPobj.FaceNormals[f1] + PrevPobj.FaceNormals[f2]
        if abs(diff.c[0]) < limit and abs(diff.c[1]) < limit and abs(diff.c[2]) < limit:
            return 'non_edge'
        
        return 'real_edge'
    
    if CREATE_SCAD:
        scad_file = open(f"hull{PrevPobj.Iteration}.scad", 'w')
        scad_file.write(Header("best").render())
        scad_file.write(PrevPobj.ScadOfHull)

    skipped_edges = 0
    seen_points = set()  # set of <point indices>, used to add spheres to scad
    for fi, face in enumerate(PrevPobj.Simplices):
        # Check if still vertex in the current (next) iteration
        # Note that we start Pobj.RawPoints with PrevPobj.RetainPoints so we can look things up by index
        vertex_stays = [(pi in Pobj.Hull.vertices) for pi in face]
        
        for pointpair in [
            [face[0], face[1], 0, 1],
            [face[1], face[2], 1, 2],
            [face[2], face[0], 2, 0]
        ]:
            status = check_edge(pointpair[0], pointpair[1])
            if status is None or status == 'non_edge':
                skipped_edges += 1
                continue
            # Check against the current (next) iteration
            is_vertex1 = vertex_stays[pointpair[2]]
            is_vertex2 = vertex_stays[pointpair[3]]
            if not is_vertex1: status += " (p1 will drop)"
            if not is_vertex2: status += " (p2 will drop)"
            p1 = PrevPobj.RetainPoints[pointpair[0]]
            p2 = PrevPobj.RetainPoints[pointpair[1]]
            print(f"// EDGE({p1[0]:.0f},{p1[1]:.0f},{p1[2]:.0f})-({p2[0]:.0f},{p2[1]:.0f},{p2[2]:.0f}): {status} (iteration={PrevPobj.Iteration})")
            print_point(pi=pointpair[0], p=p1, Pobj=PrevPobj, status=('vertex_and_previously' if is_vertex1 else 'will_drop'))
            print_point(pi=pointpair[1], p=p2, Pobj=PrevPobj, status=('vertex_and_previously' if is_vertex1 else 'will_drop'))
            print("")
            
            if CREATE_SCAD:
                c = [.2, .2, 1.]
                if status == 'edge_of_area': c = [.2, 1., 1.]
                scad_file.write(Cylinder.from_ends(.1, p1, p2).color(*c).render())
            
        if CREATE_SCAD:
            for ii, pi in enumerate(face):
                if pi in seen_points: continue
                seen_points.add(pi)
                c = [.2, .2, 1.]
                if not vertex_stays[ii]: # vertex is from prev iteration and is dropped in this iteration
                    c = [1., .2, .2]
                scad_file.write(Sphere(.4).move(PrevPobj.RetainPoints[pi]).color(*c).render())
                
    if CREATE_SCAD:
        scad_file.close()
    
    print(f"// skipped edges: {skipped_edges}", flush=True)
    print("")
            

    

read_raw_data()

if CREATE_POINT_DATA:
    point_data_file.close()


## Process coplanar data
#coplanar = {}
#for d in chull.coplanar:
#    coplanar[d[0]] = f"coplanar with f{d[1]}, its closest vertex is #{d[2]}"
