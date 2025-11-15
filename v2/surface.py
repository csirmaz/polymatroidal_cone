import sys
import os

sys.path.append(os.path.dirname(__file__) + "/../../openscad-py")  # https://codeberg.org/csirmaz/openscad-py

import math
from openscad_py import Intersection, Union, Header, Polyhedron, Point, Cube

def uu(x):
    return (x+0.5)*math.log(x)

data = []
for j in range(100):
    row = []
    for i in range(100):
        x = i+1
        y = j+1
        z = uu(x+y)-uu(x)-uu(y)
        row.append(z)
    data.append(row)
    

surface = Polyhedron.from_heightmap(data, convexity=10).move([1,1,0])


step_height = 2
layer = Cube([105,105,step_height]) # size of cube

layers1 = []
for i in range(int(55/step_height)):
    layers1.append(layer.move([0,0,i*2*step_height]))
layers1 = Union(layers1)

layers2 = []
for i in range(int(55/step_height)):
    layers2.append(layer.move([0,0,(i*2+1)*step_height]))
layers2 = Union(layers2)

surface1 = Intersection([surface, layers1])
surface2 = Intersection([surface, layers2])

print(surface1.color(1,.3,.3).render())
print(surface2.color(.3,.3,1).render())

        
