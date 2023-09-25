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
from utils import util
import json
from keras.callbacks import ModelCheckpoint, ReduceLROnPlateau
from sklearn.metrics import classification_report, cohen_kappa_score, confusion_matrix, accuracy_score
from keras.models import Model
from keras.optimizers import SGD
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
    if len(sys.argv)!=2:
       print(len(sys.argv))
       print("Usage: python <Model Name>")
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
       NN          = sys.argv[1]

       if NN not in models:
          print("The --model command line argument should be a key in the `models` dictionary")
          print("Available models:")
          print(" ----> Caffenet")
          print(" -> Densenet121")
          print(" -> Densenet161")
          print(" -> Densenet169")
          print(" ----> Resnet50")
          print(" ---> Resnet101")
          print(" ---> Resnet152")
          print(" --> Squeezenet")
          print(" -------> Vgg16")
          print(" -------> Vgg19")
       else:

          ### Loading setup
          arq = open("./setup.json", "r")
          setup = json.load(arq)
          arq.close()

          exec ("import "+NN)


          exec("model = "+NN+".load_model(input_shape = ("+str(setup['img_rows'])+","+str(setup['img_cols'])+","+str(setup['channel'])+"))")
    
          model.summary()
          ## show layers
          for i in range(len(model.layers)):
             if len(model.layers[i].get_weights())> 0:
               weights = model.layers[i].get_weights()
               if len(weights)==2:
                  print (str(i), "layer name:", model.layers[i].name, "weights shape = ", weights[0].shape, "bias shape = ", weights[1].shape)
               else:
                  print (str(i), "layer name:",model.layers[i].name, "weights shape = ", weights[0].shape)
             else:
               print(str(i), "layer name:",model.layers[i].name)


          fim = time.time()
          total = fim-inicio
          print( "Execution time - ", fim-inicio," sec")

