
# Generate candidates for vertices from vertices in the previous iteration
# using the "full-row" rule
# Generate reports on vertices, edges and scad

import sys
import os
import regex as re
from scipy.spatial import ConvexHull  # https://docs.scipy.org/doc/scipy/reference/generated/scipy.spatial.ConvexHull.html
import numpy as np
import time
import random
import json

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header, Polyhedron, Point, Cylinder

LOG_GENERATION = False


CREATE_SCAD = False # Whether to create an scad file for each iteration
CREATE_POINT_DATA = False
POINT_DATA_OUTFILE = 'pointdata_gen'
CREATE_EDGE_FACE_DATA = False
EDGE_FACE_DATA_OUTFILE = 'edge_face_data_gen'

CACHE_BINOM_TO = 200

if CREATE_POINT_DATA:
    point_data_file = open(POINT_DATA_OUTFILE, "w")
if CREATE_EDGE_FACE_DATA:
    edge_face_file = open(EDGE_FACE_DATA_OUTFILE, "w")


class PObjectClass:
    pass

class GlobalClass:
    pass


def assrt(test, msg):
    if not test:
        print(f"ERROR: {msg}")
        exit(1)


def pointtuple(p):
    return (int(p[0]), int(p[1]), int(p[2]))


BinomCache = [[None for i in range(CACHE_BINOM_TO)] for j in range(CACHE_BINOM_TO)]

def altbinom_impl(a, b):
    if a==0 or b==0: return 1
    return altbinom(a-1,b) + altbinom(a,b-1)

def altbinom(a, b):
    # altbinom(a,b) == choose(a,a+b)
    if a < CACHE_BINOM_TO and b < CACHE_BINOM_TO:
        r = BinomCache[a][b]
        if r is not None:
            return r
        r = altbinom_impl(a,b)
        BinomCache[a][b] = r
        return r
    return altbinom_impl(a,b)

assert altbinom(0,0) == 1
assert altbinom(10,0) == 1
assert altbinom(0,10) == 1
assert altbinom(1,1) == 2
assert altbinom(2,1) == 3


def fill2matrix(fill):
    """Given a column fill vector, produce a 0-1 matrix"""
    maxlength = len(fill)
    matrix = []
    for j in range(maxlength):
        matrix.append([0 for i in range(maxlength)])

    for i, f in enumerate(fill):
        for j in range(f):
            matrix[j][i] = 1
    return matrix

assert fill2matrix([2,0]) == [[1,0],[1,0]]


def fill2coords(fill):
    """Given a column fill vector, get the coordinates"""
    matrix = fill2matrix(fill)
    x = 0
    y = 0
    z = 0
    for j, row in enumerate(matrix):
        for i, v in enumerate(row):
            if v > 0:
                x += altbinom(i,j)
                y += i*altbinom(i,j)
                z += j*altbinom(i,j)
    return (x,y,z)

assert fill2coords([]) == (0,0,0)
assert fill2coords([2,0]) == (2,0,1)
assert fill2coords([2,0,0,0]) == (2,0,1)
assert fill2coords([4,2,0,0]) == (7,3,8)


def matrix2rowfill(matrix):
    """Given a matrix, return the per-row fill vector"""
    rowfill = []
    l = len(matrix)
    for j, row in enumerate(matrix):
        c = 0
        while c<l and row[c] > 0: c += 1
        rowfill.append(c)
    return rowfill

    
def invert_fill(fill):
    return matrix2rowfill(fill2matrix(fill))

assert invert_fill([]) == []
assert invert_fill([0,0,0]) == [0,0,0]
assert invert_fill([1]) == [1]
assert invert_fill([3,0,0]) == [1,1,1]
assert invert_fill([4,3,0,0]) == [2,2,2,1]
assert invert_fill([1,1,1]) == [3,0,0]
assert invert_fill([2,2,2,1]) == [4,3,0,0]


def check_nc(fill):
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


def is_beginner(fill):
    # whether the beginnings of diagonals are selected
    return (len(fill) == 0 or fill[0] == len(fill))


def is_ender(fill):
    # whether the ends of diagonals are selected
    return (len(fill) == 0 or fill[-1] == 1)


def generate_next_by_column(fill, length):
    """Given a fill vector, add a full column. Suitable for "beginner"s"""
    fill = [length] + fill  # clones
    if len(fill) < length:
        fill += [0] * (length-len(fill))
    return fill


def generate_next_by_row(fill, length):
    """Given a fill vector, add a full row. Suitable for "ender"s"""
    fill = list(fill)
    if len(fill) < length:
        fill += [0] * (length-len(fill))  # warning: modifies fill
    return [v+1 for v in fill]


def ungenerate(fill):
    """Given a fill vector, return the vector that generated it.
    Note the result may have superfluous 0s at the end"""
    if is_beginner(fill):
        return fill[1:]
    if is_ender(fill):
        return [v-1 for v in fill[:-1]]


def get_init_stats():
    return {
        'vertices': 0,
        'vertex->vertex': 0,
        'vertex->nonvertex': 0,
        'generated->vertex': 0,
        'generated->nonvertex': 0,
        'extra->vertex': 0,
    }


def get_init_pobj():
    """Get the initial collection of points"""
    globalv = GlobalClass
    globalv.PointInfo = {} # {<coords>: (iter, <fill_col>),...
    globalv.DuplicateSets = {} # {<coords>: [(iter, <fill_col>), ...
    
    Pobj = PObjectClass()
    Pobj.Global = globalv

    Pobj.Stats = get_init_stats()
    Pobj.Iteration = -1
    Pobj.RawPoints = [(0,0,0)]
    Pobj.max_y = 0
    Pobj.Global.PointInfo[(0,0,0)] = (-1, [])
    return Pobj
    

def get_next_pobj(PrevPobj):
    """Generate the next candidates for vertices from existing vertices by adding full rows/columns"""
    Pobj = PObjectClass()
    Pobj.Global = PrevPobj.Global
    Pobj.Stats = get_init_stats()
    Pobj.Iteration = PrevPobj.Iteration + 1
    Pobj.max_y = PrevPobj.max_y
    Pobj.RawPoints = list(PrevPobj.RetainPoints)  # clone for sanity
    if LOG_GENERATION:
        print(f"GENERATING POINTS for iteration={Pobj.Iteration}")

    for src_coords in PrevPobj.RetainPoints:  # loop through the vertices
        sources = [PrevPobj.Global.PointInfo[src_coords]]
        if src_coords in PrevPobj.Global.DuplicateSets:
            sources.extend(PrevPobj.Global.DuplicateSets[src_coords])
        for source in sources:
            src_itr, src_fill = source   # per-column fill
            assert src_itr + 1 == len(src_fill)

            beginner = is_beginner(src_fill)    # whether the beginnings of diagonals are selected
            ender = is_ender(src_fill)          # whether the ends of diagonals are selected
            
            new_fills = []
            
            if beginner:
                new_fills.append(generate_next_by_column(src_fill, Pobj.Iteration+1))
                
            if ender:
                new_fills.append(generate_next_by_row(src_fill, Pobj.Iteration+1))

            prev_coords = None
            for fill in new_fills:        
                assert check_nc(fill) # TODO Remove to speed up
            
                coords = fill2coords(fill)
                if prev_coords is None:
                    prev_coords = coords
                elif coords == prev_coords:
                    continue  # We allow a duplicate point from the same source
                if LOG_GENERATION:
                    print(f"iter={Pobj.Iteration} {src_fill} = {src_coords} generated {fill} = {coords}")
                
                if coords in Pobj.Global.PointInfo:
                    # We have seen this point before from a different S set
                    if coords not in Pobj.Global.DuplicateSets: Pobj.Global.DuplicateSets[coords] = []
                    Pobj.Global.DuplicateSets[coords].append((Pobj.Iteration, fill))
                    if LOG_GENERATION:
                        print(f"generated point {coords} is duplicate of {Pobj.Global.PointInfo[coords][1]}")
                else:
                    Pobj.RawPoints.append(coords)
                    Pobj.Global.PointInfo[coords] = (Pobj.Iteration, fill)
                
                if Pobj.max_y < coords[1]: Pobj.max_y = coords[1]

    if LOG_GENERATION:
        print(f"GENERATING POINTS ENDS for iteration={Pobj.Iteration}")
    return Pobj


def swap(*, array, minix, maxix, steps):
    for i in range(steps):
        a = random.randrange(minix, maxix)
        b = random.randrange(minix, maxix)
        if a != b:
            array[a], array[b] = array[b], array[a]

def reorder_points(*, Pobj, PrevPobj):
    """Reorder Pobj.RawPoints keeping RetainPoints first and ExtraPoints last"""
    swap(array=Pobj.RawPoints, minix=0, maxix=len(PrevPobj.RetainPoints), steps=1000)
    swap(array=Pobj.RawPoints, minix=len(PrevPobj.RetainPoints), maxix=Pobj.ExtraPointsFrom, steps=5000)
    swap(array=Pobj.RawPoints, minix=Pobj.ExtraPointsFrom, maxix=len(Pobj.RawPoints), steps=5000)


def process_points(*, Pobj, PrevPobj):
    
    if PrevPobj:
        assrt(PrevPobj.Iteration + 1 == Pobj.Iteration, "PrePobj iteration")
        print(f"process_points: starting with {len(Pobj.RawPoints)} points of which {len(PrevPobj.RetainPoints)} is retained, iter={Pobj.Iteration}")
    
    if Pobj.Iteration < 2:
        Pobj.RetainPoints = Pobj.RawPoints
        return
    
    # Add extra points to create triangles
    max_v = Pobj.max_y * 2.
    
    Pobj.ExtraPointsFrom = len(Pobj.RawPoints)
    
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
    
    print(f"process_points: {len(Pobj.RawPoints)} after adding the extra points")
    
    for tryix in range(3):
        try:
            Pobj.Hull = ConvexHull(np.array(Pobj.RawPoints, dtype=np.float64), qhull_options="Qc")
        except Exception as e:
            print(f"ConvexHull error: {e}")
            reorder_points(Pobj=Pobj, PrevPobj=PrevPobj)
            continue
        break
    # Hull.points shape=<n,3> coordinates of all points
    # Hull.vertices shape=<k> indices of points that are vertices
    # Hull.simplices  triads of indices of points that form faces (triangulated)
    # We have previously checked that Hull.points === input points (indices match)
    del Pobj.RawPoints
    
    ## Process faces from the previous iteration
    ## We have already reindexed the points to match this iteration
    # if PrevPobj and hasattr(PrevPobj, 'Simplices'):
    #     print(f"// EDGES OF THE CONVEX HULL iteration={PrevPobj.Iteration}")
    #     process_faces(Pobj, PrevPobj) # TODO enable later
    #     print(f"// EDGES REPORT END iteration={PrevPobj.Iteration}")
    
    # Create report
    
    def assess_point(*, pi, p, Pobj, PrevPobj, currently_vertex):
        was_vertex = False
        # IMPORTANT Pobj.RawPoints starts with PrevPobj.RetainPoints
        if PrevPobj and PrevPobj.Iteration >= 2 and pi < len(PrevPobj.RetainPoints):
            was_vertex = True

        is_extra = (pi >= Pobj.ExtraPointsFrom)

        # currently_vertex, is_extra, was_vertex

        if currently_vertex:
            Pobj.Stats['vertices'] += 1
        
        if was_vertex:
            status = 'vertex'
        elif is_extra:
            status = 'extra'
        else:
            status = 'generated'
        status += '->'
        if currently_vertex:
            status += 'vertex'
        else:
            status += 'nonvertex'
            
        if status not in ['extra->nonvertex']:
            Pobj.Stats[status] += 1
            print_point(pi=pi, p=p, Pobj=Pobj, status=status)

    print(f"// VERTICES OF THE CONVEX HULL iteration={Pobj.Iteration}")
    seen_pk = set()
    for pi in Pobj.Hull.vertices:
        p = pointtuple(Pobj.Hull.points[pi])
        assess_point(pi=pi, p=p, Pobj=Pobj, PrevPobj=PrevPobj, currently_vertex=True)
        assrt(p not in seen_pk, "point listed as vertex multiple times")
        seen_pk.add(p)

    print(f"// POINTS THAT ARE NOT VERTICES iteration={Pobj.Iteration}")
    for pi in range(len(Pobj.Hull.points)):
        p = pointtuple(Pobj.Hull.points[pi])
        if p not in seen_pk:
            assess_point(pi=pi, p=p, Pobj=Pobj, PrevPobj=PrevPobj, currently_vertex=False)
                
    print(f"// VERTEX REPORT END for iteration={Pobj.Iteration}", flush=True)
    
    # Initialize new input from the vertices of this hull
    Pobj.RetainPoints = []
    old2new_index = {}
    for pi in Pobj.Hull.vertices:
        if pi < Pobj.ExtraPointsFrom:  # not extra point
            old2new_index[pi] = len(Pobj.RetainPoints)
            p = pointtuple(Pobj.Hull.points[pi])
            Pobj.RetainPoints.append(p)
        
    ## Store faces for processing later
    ## We do it in the next step as that's when we know which vertices stay
    ## Reindex data to index into RetainPoints
    # Pobj.Simplices = []
    # for face in Pobj.Hull.simplices:
    #     face = [int(v) for v in face]
    #     if max(face) < Pobj.ExtraPointsFrom:
    #         Pobj.Simplices.append([
    #             old2new_index[face[0]],
    #             old2new_index[face[1]],
    #             old2new_index[face[2]]
    #         ])

    if CREATE_SCAD:
        Pobj.ScadOfHull = get_scad_hull(Pobj.Hull)

    del Pobj.Hull


def print_point(*, pi, p, Pobj, status):
    if hasattr(Pobj, 'Hull'):
        p2 = pointtuple(Pobj.Hull.points[pi])
    elif hasattr(Pobj, 'RetainPoints'):
        p2 = Pobj.RetainPoints[pi]
    assrt(p == p2, 'printpoint pi ~ p')
    
    if pi >= Pobj.ExtraPointsFrom:
        info = [('E', '')]
    else:
        info = [Pobj.Global.PointInfo[p]]
        if p in Pobj.Global.DuplicateSets:
            info.extend(Pobj.Global.DuplicateSets[p])
    for i, infod in enumerate(info):
        #if status in  ['vertex->notvertex', 'generated->notvertex']:
        #    # Find the generator
        #    parent_coords = fill2coords(ungenerate(infod[1]))
        #    ancestry = f" generated by {parent_coords}, first appeared in iter={Pobj.Global.PointInfo[parent_coords][0]}"
        print(f"iter={Pobj.Iteration} {p} <{infod[1]}> iter={infod[0]} <{status}{'(duplicate)' if i>0 else ''}>")



def main():
    Pobj = get_init_pobj()
    PrevPobj = None
    prev_time = time.time()
    for x in range(200):
        process_points(Pobj=Pobj, PrevPobj=PrevPobj)
        now_time = time.time()
        print(f"ITERATION END for iteration={Pobj.Iteration} time={now_time-prev_time:.0f}s {json.dumps(Pobj.Stats)}", flush=True)
        prev_time = now_time
        PrevPobj = Pobj
        Pobj = get_next_pobj(PrevPobj)


main()    
exit(0)












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

    
    






def process_faces(Pobj, PrevPobj):
    # Find real edges
    
    assrt(PrevPobj.Iteration + 1 == Pobj.Iteration, "PrevPobj iteration #2")
    
    PrevPobj.Point2Face = {}  # {<point index>: set(<face index>, ...), ...
    for fi, face in enumerate(PrevPobj.Simplices):  # This already excludes simplices with an "extra" point vertex
        for pi in face:
            if pi not in PrevPobj.Point2Face: PrevPobj.Point2Face[pi] = set()
            PrevPobj.Point2Face[pi].add(fi)
            
    def pointstr(p):
        return f"{p[0]:.0f},{p[1]:.0f},{p[2]:.0f}"
    
    def pointstr2(p):
        return f"{p.c[0]:.0f},{p.c[1]:.0f},{p.c[2]:.0f}"
    
    xvec = Point([1,0,0])
    
    PrevPobj.FaceNormals = []  # [Point(), ...
    for fi, face in enumerate(PrevPobj.Simplices):
        p1 = Point(PrevPobj.RetainPoints[face[0]])
        p2 = Point(PrevPobj.RetainPoints[face[1]])
        p3 = Point(PrevPobj.RetainPoints[face[2]])
        norm = (p2-p1).cross(p3-p2).norm()
        PrevPobj.FaceNormals.append(norm)
        if CREATE_EDGE_FACE_DATA:
            edge_face_file.write(f"TRIANGLE f{fi} ({pointstr2(p1)}) ({pointstr2(p2)}) ({pointstr2(p3)}) norm={norm.render()} iter={PrevPobj.Iteration}\n")

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
            if CREATE_EDGE_FACE_DATA:
                edge_face_file.write(f"MERGE f{f1} f{f2}\n")
            return 'non_edge'
        diff = PrevPobj.FaceNormals[f1] + PrevPobj.FaceNormals[f2]
        if abs(diff.c[0]) < limit and abs(diff.c[1]) < limit and abs(diff.c[2]) < limit:
            if CREATE_EDGE_FACE_DATA:
                edge_face_file.write(f"MERGE f{f1} f{f2}\n")
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
            print(f"// EDGE({pointstr(p1)})-({pointstr(p2)}): {status} (iteration={PrevPobj.Iteration})")
            print_point(pi=pointpair[0], p=p1, Pobj=PrevPobj, status=('vertex_and_previously' if is_vertex1 else 'will_drop'))
            print_point(pi=pointpair[1], p=p2, Pobj=PrevPobj, status=('vertex_and_previously' if is_vertex2 else 'will_drop'))
            print("")
            
            if CREATE_EDGE_FACE_DATA:
                if status == "real_edge" and is_vertex1 and is_vertex2:
                    edge_face_file.write(f"REALEDGE ({pointstr(p1)}) ({pointstr(p2)}) iter={PrevPobj.Iteration}\n")
            
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
    if CREATE_EDGE_FACE_DATA:
        edge_face_file.write(f"--- iter={PrevPobj.Iteration} done\n")
        edge_face_file.flush()
    
    print(f"// skipped edges: {skipped_edges}", flush=True)
    print("")
            

    

read_raw_data()

if CREATE_POINT_DATA:
    point_data_file.close()
if CREATE_EDGE_FACE_DATA:
    edge_face_file.close()


## Process coplanar data
#coplanar = {}
#for d in chull.coplanar:
#    coplanar[d[0]] = f"coplanar with f{d[1]}, its closest vertex is #{d[2]}"
