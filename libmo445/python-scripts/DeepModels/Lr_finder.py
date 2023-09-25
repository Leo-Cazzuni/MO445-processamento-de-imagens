# USAGE
# python lr_finder.py <csv file or directory> <Model Name>

# set the matplotlib backend so figures can be saved in the background
import matplotlib
matplotlib.use("Agg")

# import the necessary packages
from pyimagesearch.learningratefinder import LearningRateFinder
from pyimagesearch import config
from keras.preprocessing.image import ImageDataGenerator
from keras.optimizers import SGD
import matplotlib.pyplot as plt
import numpy as np
import sys
import keras.backend as K
from utils import ReadDataset
import json
from keras.models import Model
from keras.optimizers import SGD
sys.path.insert(0, "./Models/")
import time

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
    if len(sys.argv)!=3:
       print("Usage: python lr_finder <csv file or directory> <Model Name>")
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

       ## parameters
       src         = sys.argv[1]
       NN          = sys.argv[2]

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
          exec("model = "+NN+".load_model(input_shape = ("+str(setup['img_rows'])+","+str(setup['img_cols'])+","+str(setup['channel'])+"), num_classes = "+str(setup['num_classes'])+")")

          ## Loading data
          X, Y = ReadDataset.load_tensorflow(setup['channel'], src, setup['num_classes'], setup['img_rows'])


          # construct the image generator for data augmentation
          aug = ImageDataGenerator(width_shift_range=0.1,
	        height_shift_range=0.1, horizontal_flip=True,
	        fill_mode="nearest")

          # initialize the optimizer and model
          print("[INFO] compiling model...")
          opt = SGD(lr=config.MIN_LR, momentum=0.9)
          model.compile(loss="categorical_crossentropy", optimizer=opt,
	                metrics=["accuracy"])

	  # initialize the learning rate finder and then train with learning
	  # rates ranging from 1e-10 to 1e+1
          print("[INFO] finding learning rate...")
          lrf = LearningRateFinder(model)
          lrf.find(
		   aug.flow(X, Y, batch_size=config.BATCH_SIZE),
		   1e-10, 1e+1,
		   stepsPerEpoch=np.ceil((len(X) / float(config.BATCH_SIZE))),
		   batchSize=config.BATCH_SIZE)

	  # plot the loss for the various learning rates and save the
	  # resulting plot to disk
          lrf.plot_loss()
          plt.savefig(config.LRFIND_PLOT_PATH)

	  # gracefully exit the script so we can adjust our learning rates
	  # in the config and then train the network for our full set of
	  # epochs
          print("[INFO] learning rate finder complete")
          print("[INFO] examine plot and adjust learning rates before training")
          fim = time.time()
          total = fim-inicio
          print( "Execution time - ", fim-inicio," sec")

