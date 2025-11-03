
# Interpret model data saved by train_model.py

import json
import numpy as np

MODELFILE = 'model_data'

Data = json.loads(open(MODELFILE,'r').readline())
Size = Data['size']


print(f"size={Size}")
Layersizes = []
for layer in Data['layers']:
    weights_h = len(layer['weights'])
    weights_w = len(layer['weights'][0])
    biases_l = len(layer['biases'])
    assert biases_l == weights_w
    s = {
        'in': weights_h,
        'out': weights_w
    }
    Layersizes.append(s)
    print(f"Layer weights h(in)={weights_h} w(out)={weights_w} biases {biases_l}")


def apply_layer(vec, n):
    o = np.dot(vec, np.array(Data['layers'][n]['weights']))
    o += np.array(Data['layers'][n]['biases'])
    return o

def encode(vec):  # The encoder
    o = apply_layer(vec, 0)
    o = apply_layer(o, 1)
    o = apply_layer(o, 2)
    o = apply_layer(o, 3)

def tail(vec):  # After the concat, len(vec)==6
    o = apply_layer(vec, 4)
    o = apply_layer(o, 5)
    return apply_layer(o, 6)

#print(np.array(Data['layers'][5]['weights']))
#print(Data['layers'][5]['biases'])

print("TAIL: MAPPING THE CATEGORIZATION OF THE TWO INPUT VECTORS TO THE OVERALL PREDICTION")
for v1 in range(3):
    for v2 in range(3):
        inv = [
            1 if v1==0 else 0,
            1 if v1==1 else 0,
            1 if v1==2 else 0,
            1 if v2==0 else 0,
            1 if v2==1 else 0,
            1 if v2==2 else 0
        ]
        out = tail(np.array(inv))
        out = "s0" if out[0] > out[1] else "s1"
        print(f"v1={v1} v2={v2} out={out}")
        
