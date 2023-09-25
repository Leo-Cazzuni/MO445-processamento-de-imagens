from keras.applications import inception_v3
from keras.layers import Input, Flatten, Dense, GlobalAveragePooling2D 
from keras.models import Model
import numpy as np

def load_model(input_shape, num_classes):

  img_input = Input(shape=input_shape)

  model = inception_v3.InceptionV3(include_top=False, weights=None, input_tensor=img_input, pooling=None, classes=1000)
  #model.summary()
  x = model.output

  # Fully Connected Softmax Layer
  x_fc = GlobalAveragePooling2D(name='global_average_pooling_1')(x)
  x_fc = Dense(num_classes, activation='softmax', name='predictions')(x_fc)

  model = Model(img_input, x_fc, name='InceptionV3')
  #if weights_path is not None:
  #   model.load_weights(weights_path, by_name=True)

    # Truncate and replace softmax layer for transfer learning
  #model.layers.pop()
  #model.outputs = [model.layers[-1].output]
  #model.layers[-1].outbound_nodes = []
  #model.add(Dense(num_classes, activation='softmax'))

    # Uncomment below to set the first 10 layers to non-trainable (weights will not be updated)
    #for layer in model.layers[:10]:
    #    layer.trainable = False

    # Learning rate is changed to 0.001
  #sgd = SGD(lr=1e-3, decay=1e-6, momentum=0.9, nesterov=True)
  #model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])

  return model
