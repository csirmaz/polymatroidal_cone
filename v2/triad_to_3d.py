
# Expects the output of triads.c on STDIN

import sys
import os
import regex as re

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

hull = Collection(small_spheres).hull()

print(Header("best").render())
print(hull.render())
print(Collection(big_spheres).render())
                
