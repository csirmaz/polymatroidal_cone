
# Interpret model data saved by train_model.py

import json

MODELFILE = 'model_data'

Data = json.loads(open(MODELFILE,'r').readline())
Size = Data['size']

# input = SIZE*2 = filled_per_row + filled_per_col
# Dense(SIZE*2)
# Dense(SIZE)
# Dense(2)

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

# Label the inputs to the last layer first
layer_no = 2
labels = []
for j in range(Layersizes[layer_no]['in']):
    weightsrow = Data['layers'][layer_no]['weights'][j]
    labels.append('to1' if weightsrow[1] > weightsrow[0] else 'to0')
    
print(labels)
