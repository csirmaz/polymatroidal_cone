
# Interpret model data saved by train_model.py

import json
import numpy as np
import scipy.special

MODELFILE = 'model_data'
DATAFILE = 'processeddata_12'
print_details = False

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

def print_layer(n):
    print(np.array(Data['layers'][n]['weights']))
    print(np.array(Data['layers'][n]['biases']))
    
def apply_layer(vec, n, sigmoid=False):
    o = np.dot(vec, np.array(Data['layers'][n]['weights']))
    o += np.array(Data['layers'][n]['biases'])
    if sigmoid:
        o = scipy.special.expit(o)
    return o

def encode(vec):  # The encoder
    o = apply_layer(vec, 0, sigmoid=True)
    o = apply_layer(o, 1, sigmoid=True)
    return o

def tail(vec):  # After the concat, len(vec)==6
    o = apply_layer(vec, 2, sigmoid=True)
    # o = scipy.special.softmax(o)
    return o

def onehot(v):
    return [1 if v==i else 0 for i in range(encoder_categories)]

def binary(v):
    return [(v & (1<<i)) for i in range(encoder_bits)]

def binary_read(v):
    o = 0
    b = 1
    for i in reversed(v):
        if i >= .5:
            o += b
        b = b << 1
    return o

assert binary_read([0]) == 0
assert binary_read([0,1,0]) == 2
assert binary_read([.4,1,.6]) == 3

#print(np.array(Data['layers'][5]['weights']))
#print(Data['layers'][5]['biases'])


def run_data(filled_per_row, filled_per_col=None):
    if filled_per_col is None:
        filled_per_col = filled_per_row[13:26]
        filled_per_row = filled_per_row[0:13]
    
    row_enc = encode(np.array(filled_per_row))
    col_enc = encode(np.array(filled_per_col))
    #row_cat = binary_read(row_enc) # np.argmax(row_enc)
    #col_cat = binary_read(col_enc) # np.argmax(col_enc)
    #if print_details:
    #    print(f"per row: {filled_per_row} category: {row_cat}")
    #    print(f"per col: {filled_per_col} category: {col_cat}")

    out_smooth = tail(np.array(row_enc.tolist() + col_enc.tolist())) # ORDER IMPORTANT!
    #out_forced = tail(np.array(binary(row_cat) + binary(col_cat))) # ORDER IMPORTANT!

    out_smooth = 0 if out_smooth < .5 else 1 # np.argmax(out_smooth)
    #out_forced = np.argmax(out_forced)
    if print_details:
        print(f"prediction: {out_smooth} (smooth)")
    return out_smooth

#run_data([12.0, 11.0, 10.0, 9.0, 6.0, 3.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0],[ 8.0, 7.0, 6.0, 5.0, 5.0, 5.0, 4.0, 4.0, 4.0, 3.0, 2.0, 1.0, 0.0])
#run_data([12.0, 10.0, 9.0, 8.0, 6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0, 3.0, 2.0, 1.0, 1.0, 0.0])
#exit(0)

# CHECK ON DATA        

all_items = 0
smooth_match = 0

for lix, line in enumerate(open(DATAFILE, 'r')):
    if lix == 0:
        assert int(line) == Size
        continue
    
    (status, filled_per_row, filled_per_col, p_data) = json.loads(line)
    if print_details:
        print(f"{p_data} reality={status} (0=not vertex, 1=vertex)")
    out_smooth = run_data(filled_per_row, filled_per_col)
    if print_details:
        print("")

    all_items += 1
    if out_smooth == status:
        smooth_match += 1
    
print(f"smooth match={smooth_match/all_items}")

print_layer(2)
