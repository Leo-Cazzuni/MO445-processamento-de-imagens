#import PIL
import os
import sys
import csv
#from PIL import Image
#import glob
#import cv2
import numpy as np
#from scipy.misc import imsave
import scipy.misc
import random
#import copy

def Create_Random_Sampling_File(src, prefix, num_fold, percent):

  count = 0
  samples_list = []
  #for i in range(n_class):
    #samples_list.append([]) 

  ref_arquivo = open(src,"r")
  for linha in ref_arquivo:
    count+=1
    samples_list.append(linha)
  ref_arquivo.close()
  
  per_class = int(count*percent)
  path = "random" 
  folder = prefix+"_fold" 
  for i in range(num_fold):
       full_dataset =list(samples_list)

       fileout = folder+"/train"+str(i)+"_"+path+".csv"
       if not os.path.exists(folder):
          os.makedirs(folder)
       f = open(fileout, 'w')
       for k in range(per_class):
           item = full_dataset.pop(random.randrange(len(full_dataset))) 
           f.write(item)  
       f.close()
       fileout = folder+"/test"+str(i)+"_"+path+".csv"
       if not os.path.exists(folder):
          os.makedirs(folder)  
       f = open(fileout, 'w')      
       for k in range(len(full_dataset)):
           item = full_dataset.pop()  
           f.write(item)

       f.close()      

  return 1



def Create_Sampling_File(src, prefix, num_fold, percent_minor_class=None, percent_per_class=None, num_images=None):
  n_class = 0
  ref_arquivo = open(src,"r")
  for linha in ref_arquivo:
    base=os.path.basename(linha)
    f = os.path.splitext(base)[0]
    Label, ID = f.split("_")   
    n_class = max(n_class, int(Label))  
  hist_label = np.zeros(n_class)
  per_class = {}
  per_minor_class = {}
  samples_list = []
  ref_arquivo.seek(0)
  # Create one list for each class
  for i in range(n_class):
    samples_list.append([])   
  i = 0
  for linha in ref_arquivo:
    base=os.path.basename(linha)
    f = os.path.splitext(base)[0]
    #path = src+f+".png"
    Label, ID = f.split("_")   
    hist_label[int(Label)-1]+=1 # count the number of samples of each class
    samples_list[int(Label)-1].append(linha) # Add one csv file row into the respective class list
  ref_arquivo.close()
  minor = np.min(hist_label) # get the minor class samples number
  folder = prefix+"_fold" 
  if percent_minor_class:
     path = "percent_per_minor_class"
     for i in range(n_class):
         per_class[i] = int(minor*percent_minor_class) # It used when performing sampling by percent of minor class
  else:
     if percent_per_class:
        path = "percent_per_class"
        minor_class = int(minor*percent_per_class) # It used when performing stratified sampling
        for i in range(n_class):
           per_class[i] = int(hist_label[i]*percent_per_class)    
     else:
        if minor < num_images:
           num_images = int(0.9*minor)
        path = "number_of_images"
        for i in range(n_class):
          per_class[i] = num_images 
          #print per_class[i]
  if not os.path.exists(folder): 
      os.makedirs(folder) 
  for i in range(num_fold):
       full_dataset =list(samples_list) 
       for j in range(n_class):
          full_dataset[j] = list(samples_list[j]) # copy rows list of each class

       fileout = folder+"/train"+str(i)+"_"+path+".csv" # csv filename with header

       f = open(fileout, 'w')
       for j in range(n_class):
           cont = 0;
           for k in range(per_class[j]):
             item = full_dataset[j].pop(random.randrange(len(full_dataset[j]))) 
             f.write(item)  
             cont = cont + 1
       f.close()

       fileout = folder+"/test"+str(i)+"_"+path+".csv" # csv filename with header 
       if not os.path.exists(folder):
          os.makedirs(folder)  
       f = open(fileout, 'w')      
       for j in range(n_class):
           for k in range(len(full_dataset[j])):
               item = full_dataset[j].pop()  
               f.write(item)
       f.close()      

  return 1
 

if __name__ == "__main__":

  if len(sys.argv) != 6:
    print("python "+sys.argv[0]+" <input csv file> <output folder> <number of splits> <type of sampling> <percentage of training samples/number of images per class>")
    print("[1]   csv file containing all images of the dataset (as created by CreateCSVfileForDataset.py).")
    print("[2]   output folder to save the csv files of the training and testing splits.") 
    print("[3]   number of splits.")
    print("[4]   sampling approach:") 
    print("1 = sampling based on a percentage of images in the smallest class.") 
    print("2 = sampling based on a percentage of images per class (the stratified approach).") 
    print("3 = sampling based on the number of images per class (constrained to 0.9*size of the smallest class).")
    print("4 = random sampling, independently of the class information and according to a given percentage of training samples.")
    print("[5]   percentage of training samples in [0,1] or the number of images per class in the case of approach 3.") 
  else:
    src = sys.argv[1]
    prefix = sys.argv[2]
    folds = int(sys.argv[3])
    if int(sys.argv[4])==1:
      y = Create_Sampling_File(src, prefix, folds, percent_minor_class=float(sys.argv[5]))
    else:
      if int(sys.argv[4])==2:
        y = Create_Sampling_File(src, prefix, folds, percent_per_class=float(sys.argv[5]))
      else:
        if int(sys.argv[4])==3:
          y = Create_Sampling_File(src, prefix, folds, num_images=int(sys.argv[5]))
        else:
          y = Create_Random_Sampling_File(src, prefix, folds, float(sys.argv[5]))

    if y==1:
      print ("The train and test files have been successfully created")
      

