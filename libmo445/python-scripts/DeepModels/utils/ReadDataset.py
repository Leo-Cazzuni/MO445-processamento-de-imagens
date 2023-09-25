import os
import numpy as np
from PIL import Image
import PIL
import glob


def n_samples(path):
  num = 0
  ref_arquivo = open(path,"r")
  for linha in ref_arquivo:
    num+=1
  ref_arquivo.close()

  return num



def  load_binary_tensorflow(num_channels, dataset_path, nclass, img_rows):
  basewidth   = img_rows 
  n           = n_samples(dataset_path)
  index       = np.arange(nclass)
  X           = np.zeros((n,basewidth, basewidth, num_channels))
  Y           = np.zeros((n,1))
  if os.path.isfile(dataset_path): # Verify if is file

     ref_arquivo = open(dataset_path,"r")
     files = ref_arquivo.read().splitlines()
     ref_arquivo.close()
  else:
     files = glob.glob(dataset_path)

  i           = 0
  for linha in files:
    print (linha)
    base      = os.path.basename(linha)
    f         = os.path.splitext(base)[0]
    Label, ID = f.split("_")
    
    if int(Label)!=label:
      img  = Image.open(linha.rstrip())
      w, _ = img.size

      if basewidth!=w:
         wpercent = (basewidth / float(img.size[0]))
         hsize    = int((float(img.size[1]) * float(wpercent)))
         img      = img.resize((basewidth, hsize), PIL.Image.ANTIALIAS)

      im = np.array(img)
      img.close()
      if im.size < basewidth*basewidth*3:
       X[i, :, :, 0] = np.copy(im)
       X[i, :, :, 1] = np.copy(im)
       X[i, :, :, 2] = np.copy(im)
      else:
       X[i] = np.copy(im)

      del im 

      Y[i, 0] = index[int(Label)-1]

      i+=1

  # Switch RGB to BGR order 
  X = X[:,:,:,::-1].copy()

 
  return X, Y
 
def  load_tensorflow(num_channels, dataset_path, nclass, img_rows):
  basewidth   = img_rows
  
  index       = np.arange(nclass)

  if os.path.isfile(dataset_path): # Verify if is file

     _, file_extension = os.path.splitext(dataset_path)
     if file_extension not in [".png", ".jpg"]:
        ref_arquivo = open(dataset_path,"r")
        files = ref_arquivo.read().splitlines()
        ref_arquivo.close()
     else:
        files = []
        files.append(dataset_path)
  else:
     files = glob.glob(dataset_path+"*.png")

  n           = len(files)

  X           = np.zeros((n, basewidth, basewidth, num_channels))
  Y           = np.zeros((n, nclass))

  i           = 0
  for linha in files:
    
    base      = os.path.basename(linha)
    f         = os.path.splitext(base)[0]
    Label, ID = f.split("_")
    img       = Image.open(linha.rstrip())
    w, _      = img.size

    if basewidth!=w: ## Interpolation 
       wpercent = (basewidth / float(img.size[0]))
       hsize    = int((float(img.size[1]) * float(wpercent)))
       img      = img.resize((basewidth, hsize), PIL.Image.ANTIALIAS)
   
    im = np.array(img)
    img.close()
    if im.size < basewidth*basewidth*num_channels:
       X[i, :, :, 0] = np.copy(im)
       X[i, :, :, 1] = np.copy(im)
       X[i, :, :, 2] = np.copy(im)
    else:
       X[i] = np.copy(im)

    del im 

    Y[i, index[int(Label)-1]] = 1

    i+=1

  # Switch RGB to BGR order 
  X = X[:,:,:,::-1].copy()

 
  return X, Y


def  theano_format(X):
  num_samples, width, height, channels = X.shape
  X_out = np.zeros((num_samples, channels, width, height))
  X_out[:, 0, :, :] = X[:, :, :, 0]
  X_out[:, 1, :, :] = X[:, :, :, 1]
  X_out[:, 2, :, :] = X[:, :, :, 2]

  return X_out


def OpenSingleImage(src, img_rows, num_channels):

  basewidth=img_rows

  X = np.zeros((basewidth, basewidth, num_channels))


  img=Image.open(src)
  w, _ = img.size
  if basewidth!=w:
     wpercent = (basewidth / float(img.size[0]))
     hsize = int((float(img.size[1]) * float(wpercent)))
     img = img.resize((basewidth, hsize), PIL.Image.ANTIALIAS)


#====================================================
   
  im = np.array(img)
  img.close()
  if im.size < basewidth*basewidth*3:
     X[:, :, 0] = np.copy(im)
     X[:, :, 1] = np.copy(im)
     X[:, :, 2] = np.copy(im)
  else:
     X[:] = im[:]


  return X

def preprocessing_imagenet(X, backend="tensorflow"):
  
  if backend=="tensorflow":
    X[:, :, :, 0] -= 103.939
    X[:, :, :, 1] -= 116.779
    X[:, :, :, 2] -= 123.68
  else:
    X[:,0, :, :] -= 103.939
    X[:,1, :, :] -= 116.779
    X[:,2, :, :] -= 123.68

  return X


