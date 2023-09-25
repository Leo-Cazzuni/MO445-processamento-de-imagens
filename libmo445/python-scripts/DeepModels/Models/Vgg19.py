from keras.applications import vgg19
from keras.layers import Input, Flatten, Dense, Dropout
from keras.models import Model

def load_model(input_shape, num_classes):

  img_input = Input(shape=input_shape)

  model = vgg19.VGG19(include_top=False, weights=None, input_tensor=img_input, pooling=None, classes=num_classes)

  x = model.output

  x = Flatten(name='flatten')(x)
  x = Dense(4096, activation='relu', name='fc1_a')(x)
  x = Dropout(0.5)(x)
  x = Dense(4096, activation='relu', name='fc2_a')(x)
  x = Dropout(0.5)(x)
  x = Dense(num_classes, activation='softmax', name='predictionsa')(x)
  model = Model(img_input, x)
  #model.add(Dense(num_classes, activation='softmax_final'))

  #if weights_path is not None:  
  #   model.load_weights(weights_path, by_name=True)

    # Learning rate is changed to 0.001
  #sgd = SGD(lr=1e-3, decay=1e-6, momentum=0.9, nesterov=True)
  #model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])

  return model
