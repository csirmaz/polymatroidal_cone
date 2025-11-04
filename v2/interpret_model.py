
# Interpret model data saved by train_model.py

import json
import numpy as np
import scipy.special

MODELFILE = 'model_data'
DATAFILE = 'processeddata_12'
encoder_categories = 4
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


def apply_layer(vec, n, sigmoid=False):
    o = np.dot(vec, np.array(Data['layers'][n]['weights']))
    o += np.array(Data['layers'][n]['biases'])
    if sigmoid:
        o = scipy.special.expit(o)
    return o

def encode(vec):  # The encoder
    o = apply_layer(vec, 0, sigmoid=True)
    o = apply_layer(o, 1)
    o = scipy.special.softmax(o)
    return o

def tail(vec):  # After the concat, len(vec)==6
    o = apply_layer(vec, 2, sigmoid=True)
    o = apply_layer(o, 3, sigmoid=True)
    o = apply_layer(o, 4, sigmoid=True)
    o = apply_layer(o, 5, sigmoid=True)
    o = apply_layer(o, 6)
    o = scipy.special.softmax(o)
    return o

def onehot(v):
    return [1 if v==i else 0 for i in range(encoder_categories)]

#print(np.array(Data['layers'][5]['weights']))
#print(Data['layers'][5]['biases'])


def run_data(filled_per_row, filled_per_col=None):
    if filled_per_col is None:
        filled_per_col = filled_per_row[13:26]
        filled_per_row = filled_per_row[0:13]
    
    row_enc = encode(np.array(filled_per_row))
    col_enc = encode(np.array(filled_per_col))
    row_cat = np.argmax(row_enc)
    col_cat = np.argmax(col_enc)
    if print_details:
        print(f"per row: {filled_per_row} category: {row_cat}")
        print(f"per col: {filled_per_col} category: {col_cat}")

    out_smooth = tail(np.array(row_enc.tolist() + col_enc.tolist())) # ORDER IMPORTANT!
    out_forced = tail(np.array(onehot(row_cat) + onehot(col_cat))) # ORDER IMPORTANT!

    out_smooth = np.argmax(out_smooth)
    out_forced = np.argmax(out_forced)
    if print_details:
        print(f"prediction: {out_smooth} (smooth), {out_forced} (forced)")
    return (out_smooth, out_forced, row_cat, col_cat)

#run_data([12.0, 11.0, 10.0, 9.0, 6.0, 3.0, 2.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0],[ 8.0, 7.0, 6.0, 5.0, 5.0, 5.0, 4.0, 4.0, 4.0, 3.0, 2.0, 1.0, 0.0])
#run_data([12.0, 10.0, 9.0, 8.0, 6.0, 1.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 6.0, 5.0, 5.0, 5.0, 5.0, 5.0, 4.0, 4.0, 3.0, 2.0, 1.0, 1.0, 0.0])
#exit(0)

# CHECK ON DATA        

all_items = 0
smooth_match = 0
forced_match = 0

by_category = {}
for j in range(encoder_categories):
    by_category[j] = {}
    for i in range(encoder_categories):
        by_category[j][i] = {
            'points': 0,
            'ideal_pred': '?',
            'smooth_match': 0,
            'forced_match': 0,
            's0_points': 0,
            's1_points': 0
        }

print("TAIL: MAPPING THE CATEGORIZATION OF THE TWO INPUT VECTORS TO THE OVERALL PREDICTION")
for v1 in range(encoder_categories):
    for v2 in range(encoder_categories):
        inv = onehot(v1) + onehot(v2)
        out = tail(np.array(inv))
        out = "s0" if out[0] > out[1] else "s1"
        print(f"{out}  ", end="")
        by_category[v1][v2]['ideal_pred'] = out
    print("")


for lix, line in enumerate(open(DATAFILE, 'r')):
    if lix == 0:
        assert int(line) == Size
        continue
    
    (status, filled_per_row, filled_per_col, p_data) = json.loads(line)
    if print_details:
        print(f"{p_data} reality={status} (0=not vertex, 1=vertex)")
    (out_smooth, out_forced, row_cat, col_cat) = run_data(filled_per_row, filled_per_col)
    if print_details:
        print("")

    all_items += 1
    by_category[row_cat][col_cat]['points'] += 1
    if out_smooth == status:
        smooth_match += 1
        by_category[row_cat][col_cat]['smooth_match'] += 1
    if out_forced == status:
        forced_match += 1
        by_category[row_cat][col_cat]['forced_match'] += 1
    if status == 1:
        by_category[row_cat][col_cat]['s1_points'] += 1
    else:
        by_category[row_cat][col_cat]['s0_points'] += 1
        
    
    
print(f"smooth match={smooth_match/all_items}  forced match={forced_match/all_items}")

for j in range(encoder_categories):
    print(f"rowCat: {j} ", end="")
    for i in range(encoder_categories):
        print(f"points:{by_category[j][i]['points']:5.0f}     ", end="")
    print("")
    print(f"          ", end="")
    for i in range(encoder_categories):
        print(f"s0/s1:{by_category[j][i]['s0_points']:4.0f}/{by_category[j][i]['s1_points']:4.0f}  ", end="")
    print("")
    print(f"          ", end="")
    for i in range(encoder_categories):
        print(f"idealPred:{by_category[j][i]['ideal_pred']}     ", end="")
    print("")
    print(f"          ", end="")
    for i in range(encoder_categories):
        if by_category[j][i]['points'] == 0:
            print(f"smooth:---       ", end="")
        elif by_category[j][i]['smooth_match'] == by_category[j][i]['points']:
            print(f"smooth:ALL       ", end="")
        else:
            print(f"smooth:{by_category[j][i]['smooth_match']/by_category[j][i]['points']*100.:6.2f}%   ", end="")
    print("")
    print(f"          ", end="")
    for i in range(encoder_categories):
        if by_category[j][i]['points'] == 0:
            print(f"forced:---       ", end="")
        elif by_category[j][i]['forced_match'] == by_category[j][i]['points']:
            print(f"forced:ALL       ", end="")
        else:
            print(f"forced:{by_category[j][i]['forced_match']/by_category[j][i]['points']*100.:6.2f}%   ", end="")
    print("")
    print("")
