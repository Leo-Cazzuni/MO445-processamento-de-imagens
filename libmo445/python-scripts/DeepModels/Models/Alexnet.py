from keras.models import Sequential, Model
from keras.layers import Flatten, Dense, Dropout, Reshape, Permute, Activation, \
    Input, merge
from keras.layers.convolutional import Convolution2D, MaxPooling2D, ZeroPadding2D

import numpy as np

from custom_layers.alexnet_customlayers import convolution2Dgroup, crosschannelnormalization, \
    splittensor, Softmax4D

def load_model(input_shape=(227, 227, 3), num_classes = 1000):

    inputs = Input(shape=input_shape)

    conv_1 = Convolution2D(96, (11, 11),strides=(4,4),activation='relu',
                           name='conv_1')(inputs)

    conv_2 = MaxPooling2D((3, 3), strides=(2,2))(conv_1)
    conv_2 = crosschannelnormalization(name="convpool_1")(conv_2)
    conv_2 = ZeroPadding2D((2,2))(conv_2)
    conv_2 = merge([
        Convolution2D(128,(5,5),activation="relu",name='conv_2_'+str(i+1))(
            splittensor(ratio_split=2,id_split=i)(conv_2)
        ) for i in range(2)], mode='concat',concat_axis=1,name="conv_2")

    conv_3 = MaxPooling2D((3, 3), strides=(2, 2))(conv_2)
    conv_3 = crosschannelnormalization()(conv_3)
    conv_3 = ZeroPadding2D((1,1))(conv_3)
    conv_3 = Convolution2D(384,(3,3),activation='relu',name='conv_3')(conv_3)

    conv_4 = ZeroPadding2D((1,1))(conv_3)
    conv_4 = merge([
        Convolution2D(192,(3,3),activation="relu",name='conv_4_'+str(i+1))(
            splittensor(ratio_split=2,id_split=i)(conv_4)
        ) for i in range(2)], mode='concat',concat_axis=1,name="conv_4")

    conv_5 = ZeroPadding2D((1,1))(conv_4)
    conv_5 = merge([
        Convolution2D(128,(3,3),activation="relu",name='conv_5_'+str(i+1))(
            splittensor(ratio_split=2,id_split=i)(conv_5)
        ) for i in range(2)], mode='concat',concat_axis=1,name="conv_5")

    dense_1 = MaxPooling2D((3, 3), strides=(2,2),name="convpool_5")(conv_5)
    dense_1 = Flatten(name="flatten")(dense_1)
    dense_1 = Dense(4096, activation='relu',name='dense_1')(dense_1)
    dense_2 = Dropout(0.5)(dense_1)
    dense_2 = Dense(4096, activation='relu',name='dense_2')(dense_2)
    dense_3 = Dropout(0.5)(dense_2)   
    dense_3 = Dense(num_classes, name='dense_3a')(dense_3)
    prediction = Activation("softmax",name="softmaxa")(dense_3)


    model = Model(inputs=inputs, outputs=prediction, name='alexnet')

    #if weights_path:
    #    model.load_weights(weights_path, by_name=True)

    # Learning rate is changed to 0.0001
    #sgd = SGD(lr=1e-5, decay=0.0, momentum=0.9, nesterov=True)
    #sgd = Adam(lr=1e-4, beta_1=0.9, beta_2=0.999, epsilon=1e-08, decay=0.0)
    #sgd = Adagrad(lr=1e-4, epsilon=1e-08, decay=1e-6)
    #model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])


    return model


   
