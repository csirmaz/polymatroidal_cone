
# Use data from process_pointdata.py to try to train a model

import keras
import numpy as np
import json
import math
import random
import os

os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

DATAFILE = 'processeddata_12'
OUTFILE = 'model_data'
SIZE = None
BATCH_SIZE = 64
USE_VALIDATION_DATA = False
validation_set_one_of = 10
steps_per_epoch = 2000
num_epochs = 35

all_samples = 0

# TODO - sample equally from all iterations?

TRAINING_DATA = [[], []]  # status=0, status=1
VALIDATION_DATA = []

for line in open(DATAFILE, 'r'):
    if SIZE is None:
        SIZE = int(line)
        continue
    
    (status, filled_per_row, filled_per_col, _) = json.loads(line)
    if USE_VALIDATION_DATA and random.randint(0, validation_set_one_of) == 0:   # train/validation split
        VALIDATION_DATA.append((filled_per_row + filled_per_col, status))
    else:
        TRAINING_DATA[status].append(filled_per_row + filled_per_col)
    all_samples += 1

print(f"*** Training data: {len(TRAINING_DATA[0])}+{len(TRAINING_DATA[1])} Validation data: {len(VALIDATION_DATA)}")
print(f"*** Samples: {all_samples}")


def training_data(is_training: bool):
    
    if is_training:
        while True:
            inputs = []
            targets = []
            for i in range(BATCH_SIZE):
                target = 1 if random.random() < .5 else 0  # target split
                targets.append(target)
                inputs.append(random.choice(TRAINING_DATA[target]))
            yield np.array(inputs, dtype="float32"), np.array(targets)
            
    else:
        dix = 0
        while True:
            inputs = []
            targets = []
            for i in range(BATCH_SIZE):
                inp, target = VALIDATION_DATA[dix]
                targets.append(target)
                inputs.append(inp)
                dix += 1
                if dix >= len(VALIDATION_DATA):
                    # print(f"VALIDATION_DATA folded from {dix}")
                    dix = 0
            yield np.array(inputs, dtype="float32"), np.array(targets)


layers = []
input_tensor = keras.Input(shape=(SIZE*2,))
# Split into per_row and per_col
t1 = input_tensor[..., 0:SIZE]
t2 = input_tensor[..., SIZE:SIZE*2]
for i in range(1):
    layers.append(keras.layers.Dense(SIZE*2, activation="sigmoid"))
    t1 = layers[-1](t1)
    t2 = layers[-1](t2)
layers.append(keras.layers.Dense(4))
t1 = layers[-1](t1)
t2 = layers[-1](t2)

t1 = keras.layers.Softmax(axis=-1)(t1)
t2 = keras.layers.Softmax(axis=-1)(t2)

t = keras.layers.Concatenate()([t1, t2])
for i in range(4):
    layers.append(keras.layers.Dense(8, activation="sigmoid"))
    t = layers[-1](t)
layers.append(keras.layers.Dense(2))
t = layers[-1](t)
t = keras.layers.Softmax(axis=-1)(t)

model = keras.Model(inputs=input_tensor, outputs=t)
model.compile(
    loss=keras.losses.SparseCategoricalCrossentropy(from_logits=False),
    optimizer=keras.optimizers.Adam(),
    metrics=["accuracy"]
)

model.fit(
    x=training_data(is_training=True),
    validation_data=training_data(is_training=False) if USE_VALIDATION_DATA else None,
    steps_per_epoch=steps_per_epoch,
    validation_steps=500,
    epochs=num_epochs,
    # callbacks=[ScoringCallback(self)]            
)

#print("SAMPLE PREDICTIONS")
#x = get_data_batch(VALIDATION_DATA)
#y = model.predict_on_batch(x[0])
#for ix, row in enumerate(x[0]):
#    print(f"row={row.tolist()} target={x[1][ix]} pred={'s0' if y[ix][0] > y[ix][1] else 's1'}")


model_data = []
for layer in layers:
    d = layer.get_weights()
    model_data.append({
        'weights': d[0].tolist(),
        'biases': d[1].tolist()
    })
    
open(OUTFILE, "w").write(json.dumps({'size':SIZE, 'layers':model_data}))
print("model data saved")
