
# Expects the output of triads.c on STDIN

import sys
import os
import regex as re

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

from openscad_py import Sphere, Collection, Header, Point

small_spheres = []
big_spheres = []
zoom = 1.
small_r = .01
big_r = .1
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

points = {}

for line in sys.stdin:
    match = re.search(r'^Sum: ([0-9]+), ([0-9]+), ([0-9]+) \(([\-0-9]+)\)', line)
    if match:
        x = int(match.group(1)) * zoom
        y = int(match.group(2)) * zoom
        z = int(match.group(3)) * zoom
        itr = int(match.group(4))
        
        small_spheres.append(Sphere(small_r).move([x,y,z]))
        c = colors[itr]
        big_spheres.append(Sphere(big_r).move([x,y,z]).color(*c))
        
        if itr not in points:
            points[itr] = []
        points[itr].append(Point([x,y,z]))

# Get the farthest points from iter 3
points[3].sort(key=lambda p: p.length())

pa1 = points[3][-1] # 15,17,17
pa2 = points[3][-2] # 14,17,14
pa3 = points[3][-3] # 14,14,17
#print(pa1.render()) 
#print(pa2.render()) 
#print(pa3.render()) 

# Get the point that's around the 4th point of the approx square, 14,14,14
ref = Point([14,14,14])
points[3].sort(key=lambda p: p.sub(ref).length())
pa4 = points[3][0] # 13,14,14
#print(pa4.render())

# Get the point closest to ref from iter 4
points[4].sort(key=lambda p: p.sub(ref).length())
pb = points[4][0] # 13,14,15
#print(pb.render()) 

# pa1,pa2,pa3,pa4 are in one plane, as pa4 - pa3 + pa1 - pa2 = 0
# print(pa4.sub(pa3).add(pa1).sub(pa2).render()) # 0,0,0

# Project pb onto the pa4-pa3-pa2 plane
v1 = pa2.sub(pa4).norm()
v2 = pa3.sub(pa4).norm()
v3 = v1.cross(v2)
print(f"// v1={v1.render()} v2={v2.render()} v3={v3.render()}")
to_project = pb.sub(pa4)
v1coord = v1.dot(to_project)
v2coord = v2.dot(to_project)
v3coord = v3.dot(to_project)
print(f"// coords: {v1coord}, {v2coord}, {v3coord}") # coords: 0.0, 0.9486832980505138, -0.3
projection = v1.scale(v1coord).add(v2.scale(v2coord)).add(pa4)
print(f"// projection: {projection.render()}")
recreate = projection.add(v3.scale(v3coord))
print(f"// recreate pb: {recreate.render()}") # 13.03, 14.09, 14.99 - acceptable

marker = 0.5
markcol = [1,1,0]

hull = Collection(small_spheres).hull()
print(Header("best").render())
print(Collection(big_spheres).render())
print(Sphere(marker).move(pa1).color(*markcol).render())
print(Sphere(marker).move(pa2).color(*markcol).render())
print(Sphere(marker).move(pa3).color(*markcol).render())
print(Sphere(marker).move(pa4).color(*markcol).render())
print(Sphere(marker).move(pb).color(1,1,1).render())
print(Sphere(marker).move(projection).color(1,0,0,.6).render())
print(hull.color(0,1,0).render())
                
