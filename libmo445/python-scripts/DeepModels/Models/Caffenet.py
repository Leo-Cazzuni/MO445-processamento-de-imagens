from keras.models import Model
from keras.layers import Input
from keras.layers import Conv2D
from keras.layers import MaxPooling2D
from keras.layers import ZeroPadding2D
from keras.layers import Flatten
from keras.layers import Dense
from keras.layers import Dropout
from keras.layers import Activation
from keras.optimizers import SGD
from keras import backend as K
from keras.engine import Layer


class LRN2D(Layer):
    def __init__(self, alpha=1e-4, k=2, beta=0.75, n=5, **kwargs):
        if n % 2 == 0:
            raise NotImplementedError(
                "LRN2D only works with odd n. n provided: " + str(n))
        super(LRN2D, self).__init__(**kwargs)
        self.alpha = alpha
        self.k = k
        self.beta = beta
        self.n = n

    def get_output(self, train):
        X = self.get_input(train)
        b, ch, r, c = K.shape(X)
        half_n = self.n // 2
        input_sqr = K.square(X)

        extra_channels = K.zeros((b, ch + 2 * half_n, r, c))
        input_sqr = K.concatenate([extra_channels[:, :half_n, :, :],
                                   input_sqr,
                                   extra_channels[:, half_n + ch:, :, :]],
                                  axis=1)
        scale = self.k

        for i in range(self.n):
            scale += self.alpha * input_sqr[:, i:i + ch, :, :]
        scale = scale ** self.beta

        return X / scale

    def get_config(self):
        config = {"name": self.__class__.__name__,
                  "alpha": self.alpha,
                  "k": self.k,
                  "beta": self.beta,
                  "n": self.n}
        base_config = super(LRN2D, self).get_config()
        return dict(list(base_config.items()) + list(config.items()))





def load_model(input_shape=(227, 227, 3), num_classes=1000):
    inputs = Input(shape=input_shape)
    x = Conv2D(96, (11, 11), strides=(4, 4), activation='relu', name='conv1')(inputs)
    x = MaxPooling2D((3, 3), strides=(2, 2), name='pool1')(x)
    x = LRN2D(name='norm1')(x)
    x = ZeroPadding2D((2, 2))(x)
    x = Conv2D(256, (5, 5), activation='relu', name='conv2')(x)
    x = MaxPooling2D((3, 3), strides=(2, 2), name='pool2')(x)
    x = LRN2D(name='norm2')(x)
    x = ZeroPadding2D((1, 1))(x)
    x = Conv2D(384, (3, 3), activation='relu', name='conv3')(x)
    x = ZeroPadding2D((1, 1))(x)
    x = Conv2D(384, (3, 3), activation='relu', name='conv4')(x)
    x = ZeroPadding2D((1, 1))(x)
    x = Conv2D(256, (3, 3), activation='relu', name='conv5')(x)
    x = MaxPooling2D((3, 3), strides=(2, 2), name='pool5')(x)

    x = Flatten(name='flatten')(x)
    x = Dense(4096, activation='relu', name='fc6')(x)
    x = Dropout(0.5, name='drop6')(x)
    x = Dense(4096, activation='relu', name='fc7')(x)
    x = Dropout(0.5, name='drop7')(x)
    xf = Dense(1000, name='fc8')(x)
    xf = Activation('softmax', name='loss')(xf)

    model = Model(inputs, xf, name='caffenet')

    x_newfc = Dense(num_classes, name='fc_t')(x)
    x_newfc = Activation('softmax', name='loss_t')(x_newfc)

    #if weights_path is not None:
    #  model.load_weights(weights, by_name = True)

    model = Model(inputs=inputs, outputs=x_newfc)

    # Learning rate is changed to 0.001
    #sgd = SGD(lr=1e-3, decay=1e-6, momentum=0.9, nesterov=True)
    #model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])
  
    #model.summary()
    return model
