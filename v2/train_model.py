
# Use data from process_pointdata.py to try to train a model

import keras
import numpy as np
import json
import math
import random
import os

os.environ["CUDA_VISIBLE_DEVICES"] = "-1"

DATAFILE = 'processeddata_13'
OUTFILE = 'model_data'
SIZE = None
BATCH_SIZE = 64

TRAINING_DATA = [[], []]  # status=0, status=1
VALIDATION_DATA = [[], []]

for line in open(DATAFILE, 'r'):
    if SIZE is None:
        SIZE = int(line)
        continue
    
    (status, filled_per_row, filled_per_col) = json.loads(line)
    if random.randint(0,2) == 0:
        VALIDATION_DATA[status].append(filled_per_row + filled_per_col)
    else:
        TRAINING_DATA[status].append(filled_per_row + filled_per_col)

print(f"Training data: {len(TRAINING_DATA[0])}+{len(TRAINING_DATA[1])} Validation data: {len(VALIDATION_DATA[0])}+{len(VALIDATION_DATA[1])}")
    
def get_data_batch(data):
    inputs = []
    targets = []
    for i in range(BATCH_SIZE):
        target = random.randint(0, 1)
        targets.append(target)
        inputs.append(random.choice(data[target]))
    return np.array(inputs, dtype="float32"), np.array(targets)


def training_data(is_training: bool):
    while True:
        i, t = get_data_batch(TRAINING_DATA if is_training else VALIDATION_DATA)
        yield i, t

layers = []
input_tensor = keras.Input(shape=(SIZE*2,))
# Split into per_row and per_col
t1 = input_tensor[..., 0:SIZE]
t2 = input_tensor[..., SIZE:SIZE*2]
for i in range(3):
    layers.append(keras.layers.Dense(SIZE, activation="leaky_relu"))
    t1 = layers[-1](t1)
    t2 = layers[-1](t2)
layers.append(keras.layers.Dense(3))
t1 = layers[-1](t1)
t2 = layers[-1](t2)

t1 = keras.layers.Softmax(axis=-1)(t1)
t2 = keras.layers.Softmax(axis=-1)(t2)

t = keras.layers.Concatenate()([t1, t2])
for i in range(2):
    layers.append(keras.layers.Dense(3, activation="leaky_relu"))
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
    validation_data=training_data(is_training=False),
    steps_per_epoch=2000,
    validation_steps=500,
    epochs=19,
    # callbacks=[ScoringCallback(self)]            
)

print("SAMPLE PREDICTIONS")
x = get_data_batch(VALIDATION_DATA)
y = model.predict_on_batch(x[0])
for ix, row in enumerate(x[0]):
    print(f"target={x[1][ix]} pred={'s0' if y[ix][0] > y[ix][1] else 's1'}")


model_data = []
for layer in layers:
    d = layer.get_weights()
    model_data.append({
        'weights': d[0].tolist(),
        'biases': d[1].tolist()
    })
    
open(OUTFILE, "w").write(json.dumps({'size':SIZE, 'layers':model_data}))
print("model data saved")
