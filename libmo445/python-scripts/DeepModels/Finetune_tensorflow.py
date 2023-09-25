# This script load a model and finetune it.

# Arguments

# 1 = CSV file ou directory containing images 
# 2 = Model
# 3 = filename to save the weights (*.h5)
# 4 = file weights if pretrained (Optional).
# Author: Daniel Osaku 

import os
from utils import ReadDataset
import time
import sys
import numpy as np
import json
import keras.backend as K
from keras.callbacks import ModelCheckpoint, ReduceLROnPlateau, Callback
from sklearn.metrics import classification_report, cohen_kappa_score, confusion_matrix, accuracy_score
from keras.models import Model
from keras.optimizers import SGD


class PrintLearningRate(Callback):
    def on_epoch_end(self, epoch, logs=None):
        lr = self.model.optimizer.lr
        decay = self.model.optimizer.decay
        iterations = self.model.optimizer.iterations
        lr_with_decay = lr / (1. + decay * K.cast(iterations, K.dtype(decay)))
        print(K.eval(lr_with_decay))




sys.path.insert(0, "./Models/")

models = {
"Vgg16",
"Vgg19",
"Resnet50",
"Resnet101",
"Resnet152",
"Squeezenet",
"Densenet121",
"Densenet161",
"Densenet169",
"Caffenet",
"InceptionV3"
}

if __name__ == '__main__':
    inicio = time.time()
    if len(sys.argv)!=5 and len(sys.argv)!=4 :
       print(len(sys.argv))
       print("Usage: python <csv file or directory> <Model Name> <output weights> <weights path (optional)> ")
       print("Available models:")
       print(" ----> Caffenet")
       print(" -> Densenet121")
       print(" -> Densenet161")
       print(" -> Densenet169")
       print(" -> InceptionV3")
       print(" ----> Resnet50")
       print(" ---> Resnet101")
       print(" ---> Resnet152")
       print(" --> Squeezenet")
       print(" -------> Vgg16")
       print(" -------> Vgg19")
    else:
       src         = sys.argv[1]
       NN          = sys.argv[2]
       if len(sys.argv)==5: 
          weights     = sys.argv[4]
          output_weights = sys.argv[3]
       else:
          weights = None 
          output_weights = sys.argv[3]

       ### Loading setup
       arq = open("./setup.json", "r")
       setup = json.load(arq)
       arq.close()

       if NN not in models:
          print("The --model command line argument should be a key in the `models` dictionary")
          print("Available models:")
          print(" ----> Caffenet")
          print(" -> Densenet121")
          print(" -> Densenet161")
          print(" -> Densenet169")
          print(" -> InceptionV3")
          print(" ----> Resnet50")
          print(" ---> Resnet101")
          print(" ---> Resnet152")
          print(" --> Squeezenet")
          print(" -------> Vgg16")
          print(" -------> Vgg19")
       else:
          exec ("import "+NN)


          exec("model = "+NN+".load_model(input_shape = ("+str(setup['img_rows'])+","+str(setup['img_cols'])+","+str(setup['channel'])+"), num_classes = "+str(setup['num_classes'])+")")
    

          ## show convolutional layer index
          #for i in range(len(model.layers)):
          #   if len(model.layers[i].get_weights())> 0 and "bn" not in model.layers[i].name:
          #      print (str(i))
          if weights is not None:
             model.load_weights(weights, by_name=True)


          sgd = SGD(lr=setup["lr"], decay=setup["decay"], momentum=0.9, nesterov=True)
          model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])


          X_train, Y_train = ReadDataset.load_tensorflow(setup['channel'], src, setup['num_classes'], setup['img_rows'])

          #X = ReadDataset.preprocessing_imagenet(X)
          ## save best architecture
          mcp = ModelCheckpoint(output_weights, monitor="loss", mode="min",
                             save_best_only=setup['save_weights'], save_weights_only=False)

          ## print learning rate in each epoch
          lr = PrintLearningRate()

          model.fit(X_train, Y_train,
                   batch_size=setup['batch_size'],
                   epochs=setup['nb_epoch'],
                   #shuffle=True,
                   verbose=2,
                   callbacks=[mcp, lr]
                   )
          print("Finetune finished!")

          

          predictions_train = model.predict(X_train, batch_size=setup['batch_size'], verbose=2) 

          Y_t = np.argmax(Y_train, axis=-1) # Convert one-hot to index

          Y_p = np.argmax(predictions_train, axis=-1)

          print("Accuracy(%)    = ", accuracy_score(Y_t, Y_p)*100)
          print("Cohen Kappa(%) = ", cohen_kappa_score(Y_t, Y_p)*100)
          print(classification_report(Y_t, Y_p))
          print("*****  Confusion Matrix  *****")
          print(confusion_matrix(Y_t, Y_p))



          fim = time.time()
          total = fim-inicio
          print( "Execution time - ", fim-inicio," sec")

