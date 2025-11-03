
# Interpret model data saved by train_model.py

import json
import numpy as np
import scipy.special

MODELFILE = 'model_data'
DATAFILE = 'processeddata_13'

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
    return apply_layer(o, 3)

def tail(vec):  # After the concat, len(vec)==6
    o = apply_layer(vec, 4)
    o = apply_layer(o, 5)
    return apply_layer(o, 6)

def onehot(v, mx):
    return [1 if v==i else 0 for i in range(mx)]

#print(np.array(Data['layers'][5]['weights']))
#print(Data['layers'][5]['biases'])

print("TAIL: MAPPING THE CATEGORIZATION OF THE TWO INPUT VECTORS TO THE OVERALL PREDICTION")
for v1 in range(3):
    for v2 in range(3):
        inv = onehot(v1,3) + onehot(v2,3)
        out = tail(np.array(inv))
        out = "s0" if out[0] > out[1] else "s1"
        print(f"v1={v1} v2={v2} out={out}")


def run_data(filled_per_row, filled_per_col=None):
    if filled_per_col is None:
        filled_per_col = filled_per_row[13:26]
        filled_per_row = filled_per_row[0:13]
    
    row_enc = encode(np.array(filled_per_row))
    col_enc = encode(np.array(filled_per_col))
    row_cat = np.argmax(row_enc)
    col_cat = np.argmax(col_enc)
    print(f"per row: {filled_per_row} cat: {row_cat}")
    print(f"per col: {filled_per_col} cat: {col_cat}")
    
    row_enc = scipy.special.softmax(row_enc)
    col_enc = scipy.special.softmax(col_enc)

    out_smooth = tail(np.array(row_enc.tolist() + col_enc.tolist())) # ORDER IMPORTANT!
    out_forced = tail(np.array(onehot(row_cat,3) + onehot(col_cat,3))) # ORDER IMPORTANT!
    
    out_smooth = np.argmax(out_smooth)
    out_forced = np.argmax(out_forced)
    print(f"prediction: {out_smooth} (smooth), {out_forced} (forced)")

#run_data([12.0, 11.0, 10.0, 9.0, 6.0, 3.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0],[ 8.0, 7.0, 6.0, 5.0, 5.0, 5.0, 4.0, 4.0, 4.0, 3.0, 2.0, 1.0, 0.0])
run_data([12.0, 10.0, 9.0, 8.0, 6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0, 3.0, 2.0, 1.0, 1.0, 0.0])
exit(0)

# CHECK ON DATA        

for lix, line in enumerate(open(DATAFILE, 'r')):
    if lix == 0:
        assert int(line) == Size
        continue
    
    (status, filled_per_row, filled_per_col, p_data) = json.loads(line)
    print(f"{p_data} reality={status}")
    run_data(filled_per_row, filled_per_col)
    print("")
    
