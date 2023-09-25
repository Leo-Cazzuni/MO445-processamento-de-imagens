# This script load a model and predict images.

# Arguments

# 1 = CSV file ou directory containing images 
# 2 = Model
# 3 = file weights (H5 file).
# 4 = target layer
# 5 = kind of visuallization

# Author: Daniel Osaku 


import os
from utils import ReadDataset
import time
import sys
import numpy as np
from utils import util
import json
from keras.models import Model
from keras.optimizers import SGD
from sklearn.metrics import classification_report, cohen_kappa_score, confusion_matrix, accuracy_score
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
"Caffenet"
}


if __name__ == '__main__':
    inicio = time.time()
    if len(sys.argv)!=6:
       print("Usage: python <csv file or directory> <Model Name> <weights path> <target layer> <visualization>")
       print("Available models:")
       print(" -> Densenet121")
       print(" -> Densenet161")
       print(" -> Densenet169")
       print(" ----> Resnet50")
       print(" ---> Resnet101")
       print(" ---> Resnet152")
       print(" --> Squeezenet")
       print(" -------> Vgg16")
       print(" -------> Vgg19")
       print("Available type of visualization")
       print(" 1 - Activations per sample")
       print(" 2 - Weights per kernel")
       print(" 3 - Histogram per kernel")
       print(" 4 - Activations per kernel/image")
       print(" 5 - Activations per kernel/image (simplified)")
       print(" 6 - Selectivity per class/kernel")
    else:

       ## parameters
       src           = sys.argv[1]
       NN            = sys.argv[2]
       weights       = sys.argv[3]  
       target_layer  = sys.argv[4].split(",")
       Visualization = sys.argv[5].split(",")

       if NN not in models:
          print("The --model command line argument should be a key in the `models` dictionary")
          print("Available models:")
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
          print ("import "+NN)
          exec ("import "+NN)

          ### Loading setup
          arq = open("./setup.json", "r")
          setup = json.load(arq)
          arq.close()


          ## Loading model
          print("Loading model...")
          exec("model = "+NN+".load_model(input_shape = ("+str(setup['img_rows'])+","+str(setup['img_cols'])+","+str(setup['channel'])+"), num_classes = "+str(setup['num_classes'])+")")


          ## Loading model weights
          model.load_weights(weights)

          sgd = SGD(lr=setup["lr"], decay=setup["decay"], momentum=0.9, nesterov=True)
          model.compile(optimizer=sgd, loss='categorical_crossentropy', metrics=['accuracy'])

          print("Loading data...")
          X, Y = ReadDataset.load_tensorflow(setup['channel'], src, setup['num_classes'], setup['img_rows'])

          #Optional: Use it just when a pretrained model from imagenet is used
          #X = ReadDataset.preprocessing_imagenet(X)

          #Make predictions

          print("Make predictions...")
          predictions_valid = model.predict(X, batch_size=setup['batch_size'], verbose=2)

          Y_t = np.argmax(Y, axis=-1) # Convert one-hot to index
          Y_p = np.argmax(predictions_valid, axis=-1)


          print("Extracting iftDataset...")
          for i in target_layer:

                #get output in a specific layer name
                intermediate_layer_model = Model(inputs=model.input,
                                   outputs=model.get_layer(model.layers[int(i)].name).output)
             
                intermediate_output = intermediate_layer_model.predict(X, verbose=2)
                weights = model.layers[int(i)].get_weights()
                layer_weights  = model.layers[int(i)].get_weights()[0]
                
                if (len(weights)==2): 
                  bias_weights = model.layers[int(i)].get_weights()[1]
                else:
                  bias_weights = None

                

                if Visualization[0]=="all":

                      ## activations in the index i of the model
                      util.iftOutputCNNLayerToDataset(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftActivations.zip", src=src)

                      ## kernel weights
                      util.iftFilterToDataset(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftFilterProjection.zip", False, backend="tensorflow", filter_in=layer_weights, bias = bias_weights)  
                      ## histogram of activations
                      util.iftFilterToDataset(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftHistogramProjection.zip", False, backend="tensorflow", filter_in=None, bias = None) 

                      num_samples = 8
                      ## activations per image/per kernel 
                      util.iftActivationsPerFilter(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftActivationsPerFilter.zip", num_samples, False, backend="tensorflow")  

                      ## activations per kernel
                      util.iftActivationsPerFilter_simplified(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftSimplifiedActivationsPerKernel.zip", num_samples, False, backend="tensorflow")  

                       ## Selectivity                  
                      util.iftSelectivity(intermediate_output, Y_t, setup['num_classes'], NN+"_iftSelectivity.zip", backend = "tensorflow")
                else:
                  for j in Visualization:
                    if int(j)==1:
                       ## activations in the index i of the model
                       util.iftOutputCNNLayerToDataset(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftActivations.zip", src=src)
                    elif int(j)==2:
                       ## kernel weights
                       util.iftFilterToDataset(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftFilterProjection.zip", False, backend="tensorflow", filter_in=layer_weights, bias = bias_weights) 
                    elif int(j)==3:
                       ## histogram of activations
                       util.iftFilterToDataset(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftHistogramProjection.zip", False, backend="tensorflow", filter_in=None, bias = None)
 
                    elif int(j)==4:
                       num_samples = 8
                       ## activations per image/per kernel 
                       util.iftActivationsPerFilter(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftActivationsPerFilter.zip", num_samples, False, backend="tensorflow")  

                    elif int(j)==5:
                       ## activations per kernel
                       util.iftActivationsPerFilter_simplified(intermediate_output, Y_t, Y_p, setup['num_classes'], NN+"_iftSimplifiedActivationsPerKernel.zip", num_samples, False, backend="tensorflow") 
  
                    elif int(j)==6:  
                       ## Selectivity                  
                       util.iftSelectivity(intermediate_output, Y_t, setup['num_classes'], NN+"_iftSelectivity.zip", backend = "tensorflow")



          print("Finished")

 
          fim = time.time()
          total = fim-inicio
          print( "Execution time - ", fim-inicio," sec")

