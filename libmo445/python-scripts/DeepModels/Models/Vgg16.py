from keras.applications import vgg16
from keras.optimizers import SGD
from keras.layers import Input, Flatten, Dense, Dropout
from keras.models import Model
from keras.callbacks import ModelCheckpoint, ReduceLROnPlateau

def load_model(input_shape, num_classes = 1000):

  img_input = Input(shape=input_shape)

  model = vgg16.VGG16(include_top=False, weights=None, input_tensor=img_input, pooling=None, classes=num_classes)

  x = model.output

  x = Flatten(name='flatten')(x)
  x = Dense(4096, activation='relu', name='fc1_a')(x)
  x = Dropout(0.5)(x)
  x = Dense(4096, activation='relu', name='fc2_a')(x)
  x = Dropout(0.5)(x)
  x = Dense(num_classes, activation='softmax', name='predictionsa')(x)
  model = Model(img_input, x)

  
  #if weights_path is not None:
  #   model.load_weights(weights_path, by_name=True)


  #sgd = SGD(lr=1e-3, decay=1e-6, momentum=0.9, nesterov=True)
  #model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])

  return model
