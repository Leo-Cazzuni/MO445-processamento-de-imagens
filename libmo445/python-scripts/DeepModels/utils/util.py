import csv
import math
import os
import pdb
import sys
import random
import numpy as np
import glob
import pyift.pyift as ift

def read_image_entry(img_entry):
    img_list = []
    if os.path.isdir(img_entry):
        img_list = [os.path.join(img_entry, img_path) for img_path in
                    sorted(os.listdir(img_entry))]
    elif img_entry.endswith(".csv"):
        with open(img_entry) as f:
            img_list = f.read().splitlines()

    return img_list


def read_image_list(img_list):
    X = []

    for img_path in img_list:
        img = ift.ReadImageByExt(img_path).AsNumPy()
        X.append(img)

    X = np.array(X)
    if len(X.shape) == 3: # gray images (n_imgs, ysize, xsize)
        new_shape = tuple(list(X.shape) + [1])
        X = np.reshape(X, new_shape)

    return X

def normalization_value(img):
    # Given that the image formats are only 8, 12, 16, 32, and 64 bits, we must impose this constraint here.
    n_bits = math.floor(math.log2(img.max()))
    
    if n_bits > 1 and n_bits < 8:
        n_bits = 8
    elif n_bits > 8 and n_bits < 12:
        n_bits = 12
    elif n_bits > 12 and n_bits < 16:
        n_bits = 16
    elif n_bits > 16 and n_bits < 32:
        n_bits = 32
    elif n_bits > 32 and n_bits < 64:
        n_bits = 64
    elif n_bits > 64:
        sys.exit("Error: Number of Bits %d not supported. Try <= 64" % n_bits)

    return math.pow(2, n_bits) - 1


def normalize_image_set(X):
    # convert the int images to float and normalize them to [0, 1]
    norm_val = normalization_value(X[0])

    return X.astype('float32') / norm_val


def get_true_labels_from_paths(img_paths):
    true_labels = []
    for img in img_paths:
        img_basename = os.path.basename(img) # eg: 000010_000001.png
        truelabel = int(img_basename.split('_')[0])
        true_labels.append(truelabel)

    return np.array(true_labels, dtype=np.int32)


def read_feature_vectors_from_csv(dataset_path, delimiter=','):
    X = []
    ref_data = []
    true_labels = []
    with open(dataset_path, 'r') as csvfile:
        spamreader = csv.reader(csvfile, delimiter=delimiter)
        for row in spamreader:
            ref_data.append(row[0])
            true_labels.append(row[1])
            X.append(row[2:])
    X = np.array(X)
    X = X.astype(np.float32)
    true_labels = np.array(true_labels,dtype=np.int32)

    return X, ref_data, true_labels


def read_feature_vectors_from_opf_dataset(dataset_path):
    Z = ift.ReadDataSet(dataset_path)
    X = Z.GetData()
    ref_data = Z.GetRefData()
    true_labels = Z.GetTrueLabels()

    return X, ref_data, true_labels

def read_feature_vectors(dataset_path, delimiter=','):
    if dataset_path.endswith(".csv"):
        return read_feature_vectors_from_csv(dataset_path, delimiter=delimiter)
    elif dataset_path.endswith(".zip"):
        return read_feature_vectors_from_opf_dataset(dataset_path)
    else:
        return None

def save_feature_vectors_as_csv(X, out_path, ref_data, delimiter=','):
    Xsave = X

    true_labels = get_true_labels_from_paths(ref_data)
    true_labels = true_labels.reshape((len(true_labels), 1))
    Xsave = np.array(ref_data)
    Xsave = Xsave.reshape((len(ref_data), 1))
    Xsave = np.concatenate((Xsave, true_labels), axis=1)
    Xsave = np.concatenate((Xsave, X), axis=1)

    Xlist = Xsave.tolist()
    with open(out_path, 'w') as f:
        wr = csv.writer(f)
        wr.writerows(Xlist)

def save_feature_vectors_as_opf_dataset(X, out_path, ref_data):
    Xf = X.astype(np.float32)
    true_labels = get_true_labels_from_paths(ref_data)
    Z = ift.CreateDataSetFromNumPy(Xf, true_labels)
    Z.SetRefData(ref_data)
    ift.WriteDataSet(Z, out_path)

def save_feature_vectors(X, out_path, ref_data, delimiter=','):
    if out_path.endswith('.csv'):
        save_feature_vectors_as_csv(X, out_path, ref_data=ref_data, delimiter=delimiter)
    elif out_path.endswith('.zip'):
        save_feature_vectors_as_opf_dataset(X, out_path, ref_data=ref_data)


def Write_sup_dataset(X, Y_valid, Y_pred, nclasses, out_path, src=False, delimiter=',', groups = None):

    if out_path.endswith('.csv'):
         save_feature_vectors_as_csv(X, out_path, ref_data=src, delimiter=delimiter)
    elif out_path.endswith('.zip'):
         Xf = X.astype(np.float32)
         idx = np.arange(Y_valid.shape[0])
         Z = ift.CreateDataSetFromNumPy(Xf, np.array(Y_valid, dtype=np.int32))
         if src:    
            Z.SetRefData(src)
         H = np.array(Y_pred, dtype=np.int32)

         Z.SetLabels(np.array(Y_pred, dtype=np.int32)) 
         Z.SetId(np.array(idx, dtype=np.int32))
         Z.SetNClasses(int(nclasses))

         if groups is not None:
            Z.SetGroups(np.array(groups, dtype=np.int32))
         ift.AddStatus(Z, ift.IFT_SUPERVISED)
         ift.WriteDataSet(Z, out_path)
         
 



def iftActivationsPerFilter(X, Y_valid, Y_pred, nclasses, fileout, num_samples = 10, src=False, backend="tensorflow"):

   lista = []
   X_1 = np.zeros((num_samples * nclasses, X.shape[1], X.shape[2], X.shape[3]))
   Y = np.zeros(num_samples*nclasses)
   Y_predicted = np.zeros(num_samples*nclasses)
   for i in range(nclasses):
       lista.append([])


   for i in range(X.shape[0]):
       lista[Y_valid[i]].append(i) ## building a list of samples in each class
   k = 0
   ## choosing samples of each class
   for i in range(nclasses):
      for j in range(num_samples):
          item = lista[i].pop(random.randrange(len(lista[i])))
          X_1[k] = X[item].copy() 
          Y[k] = Y_valid[item].copy()

          Y_predicted[k] = Y_pred[item].copy()

          k+=1
   
   if len(X.shape)!=4: 
      print("Error: Invalid shape for X")
      exit()
   else:
      if backend=="tensorflow":

            X_new   = np.zeros((X_1.shape[0]*X_1.shape[3], X_1.shape[1]*X_1.shape[2]))
            groups  = np.zeros(X_1.shape[0]*X_1.shape[3])
            y_true  = np.zeros(X_1.shape[0]*X_1.shape[3])
            y_label = np.zeros(X_1.shape[0]*X_1.shape[3])
            m = 0
            for i in range(X_1.shape[0]):
                for j in range(X_1.shape[3]):
                   X_new[m] = X_1[i, :,:, j].reshape(-1)  ## change 2D shape to flat shape 
                   groups[m] = j+1
                   y_true[m] = Y[i]
                   y_label[m] = Y_predicted[i] 
                   m+=1


      else:   ## backend theano

            X_new   = np.zeros((X_1.shape[0]*X_1.shape[1], X_1.shape[2]*X_1.shape[3]))
            groups  = np.zeros(X_1.shape[0]*X_1.shape[1])
            y_true  = np.zeros(X_1.shape[0]*X_1.shape[1])
            y_label = np.zeros(X_1.shape[0]*X_1.shape[1])
            m = 0
            for i in range(X_1.shape[0]):
                for j in range(X_1.shape[1]):
                   X_new[m] = X_1[i, j, :,:].reshape(-1)  ## change 2D shape to flat shape 
                   groups[m] = j+1
                   y_true[m] = Y[i]
                   y_label[m] = Y_predicted[i] 
                   m+=1

      y_true+=1
      y_label+=1
      
      Write_sup_dataset(X_new, y_true, y_label, nclasses, fileout, src, groups=groups) 



def iftSelectivity(X, Y_valid, nclasses, fileout, backend = "tensorflow"):

   selectivity = np.zeros((nclasses, X.shape[1], X.shape[2], X.shape[3]))

   for j in range(nclasses):
      histogram = X[Y_valid==j].copy()
      NAP = np.count_nonzero(histogram>0, axis = 0)
      NAN = np.count_nonzero(histogram<0, axis = 0)
      
      
      Skcp = (NAP - NAN)/ histogram.shape[0]
      selectivity[j] = Skcp.copy()    #selectivity   ativacoes medias em cada classe e em cada posicao 

   if backend=="tensorflow":
       
      Slabel = np.argmax(np.max(np.max(selectivity, axis=1), axis=1), axis=0) + 1 ##shape(num_kernels)

      X_new = np.zeros((selectivity.shape[0]*selectivity.shape[3], selectivity.shape[1]*selectivity.shape[2]))
      y_true = np.zeros(selectivity.shape[0]*selectivity.shape[3])
      groups = np.zeros(selectivity.shape[0]*selectivity.shape[3])   
      idx = 0
      for k in range(selectivity.shape[0]):
          for l in range(selectivity.shape[3]):
              sample = selectivity[k, :, :, l].reshape(-1)
              X_new[idx] = sample.copy()
              y_true[idx] = k+1 
              groups[idx] = l+1
              idx+=1
              
   else: # backend theano
      
      Slabel = np.argmax(np.max(np.max(selectivity, axis=2), axis=2), axis=0) + 1 ##shape(num_kernels)

      X_new= np.zeros((selectivity.shape[0]*selectivity.shape[1], selectivity.shape[2]*selectivity.shape[3]))
      y_true = np.zeros(selectivity.shape[0]*selectivity.shape[1])
      groups = np.zeros(selectivity.shape[0]*selectivity.shape[1])   
      idx = 0
      for k in range(selectivity.shape[0]):
          for l in range(selectivity.shape[1]):
              sample = selectivity[k, l, :, :].reshape(-1)
              X_new[idx] = sample.copy()
              y_true[idx] = k+1 
              groups[idx] = l+1
              idx+=1

   Write_sup_dataset(X_new, y_true, y_true, nclasses, fileout, False, groups=groups) 



def iftActivationsPerFilter_simplified(X, Y_valid, Y_pred, nclasses, fileout, num_samples = 10, src=False, backend="tensorflow"):
   #nclasses = int(np.max(Y_valid))+1
   lista = []
   X_1 = np.zeros((num_samples * nclasses, X.shape[1], X.shape[2], X.shape[3]))
   Y = np.zeros(num_samples*nclasses)
   Y_predicted = np.zeros(num_samples*nclasses)
   for i in range(nclasses):
       lista.append([])


   for i in range(X.shape[0]):
       lista[Y_valid[i]].append(i) ## building a list of samples in each class
   k = 0
   ## choosing samples of each class
   for i in range(nclasses):
      for j in range(num_samples):
          item = lista[i].pop(random.randrange(len(lista[i])))
          X_1[k] = X[item].copy() 
          Y[k] = Y_valid[item].copy()

          Y_predicted[k] = Y_pred[item].copy()

          k+=1
   
   if len(X.shape)!=4: 
      print("Error: Invalid shape for X")
      exit()
   else:
      if backend=="tensorflow":


            # Performing activation histogram
            Sum = np.zeros((X.shape[3], nclasses))
            label = np.zeros(X.shape[3])
            for sample in range(X.shape[0]):
              for f in range(X.shape[3]):
                  for g in range(X.shape[1]):
                      for h in range(X.shape[2]):
                          if (X[sample, g, h, f]>0):
                             Sum[f,int(Y_valid[sample])]+=X[sample, g, h, f]

            for i in range(X.shape[3]):
              label[i] = int(np.argmax(Sum[i], axis=-1))+1


            X_new   = np.zeros((X_1.shape[3], X_1.shape[0]*X_1.shape[1]*X_1.shape[2]))
            groups  = np.zeros(X_1.shape[3])

            for i in range(X_1.shape[3]):
                   x = X_1[:, :,:, i].reshape(-1)
                   #print(x.shape)
                   X_new[i, :] = x  ## change 2D shape to flat shape 
                   #m+=x.shape[0]
                   groups[i] = i+1
 



      else:   ## backend theano


            # Performing activation histogram
            Sum = np.zeros((X.shape[1], nclasses))
            label = np.zeros(X.shape[1])
            for sample in range(X.shape[0]):
              for f in range(X.shape[1]):
                  for g in range(X.shape[2]):
                      for h in range(X.shape[3]):
                          if (X[sample, f, g, h]>0):
                             Sum[f,int(Y_valid[sample])]+=X[sample, f, g, h]

            for i in range(X.shape[1]):
              label[i] = int(np.argmax(Sum[i], axis=-1))+1


            X_new   = np.zeros((X_1.shape[1], X_1.shape[0]*X_1.shape[3]*X_1.shape[2]))
            groups  = np.zeros(X_1.shape[1])

            for i in range(X_1.shape[1]):
                   x = X_1[:, i, :,:].reshape(-1)
                   #print(x.shape)
                   X_new[i, :] = x  ## change 2D shape to flat shape 
                   #m+=x.shape[0]
                   groups[i] = i+1
 


      Write_sup_dataset(X_new, label, label, nclasses, fileout, src, groups = groups) 


def iftOutputCNNLayerToDataset(X, Y_valid, Y_pred, nclasses, fileout, src=False):

   ## output layer can have shape with 2 [0,0] or 4 dimensions [0,0,0,0]

         if src:
            if os.path.isfile(src): # Verify if is file

                ref_arquivo = open(src,"r")
                files = ref_arquivo.read().splitlines()
                ref_arquivo.close()             

            else:
                files = glob.glob(src)
            ref_data = []
            for b in files:
                aux = []
                aux.append(b)
                ref_data.append(aux)
         y_true = Y_valid.copy()
         y_label = Y_pred.copy()
         y_true+=1
         y_label+=1

         if len(X.shape)==2:
            Write_sup_dataset(X, y_true, y_label, nclasses, fileout, ref_data)
         else:
            X_1 = X.reshape(X.shape[0], X.shape[1]*X.shape[2]*X.shape[3])
            Write_sup_dataset(X_1, y_true, y_label, nclasses, fileout, ref_data) 


def iftFilterToDataset(X, Y_valid, Y_pred, nclasses, fileout, src, backend="tensorflow", filter_in=None, bias=None):
    
    #num_classes = int(np.max(Y_valid))+1
    #print(num_classes)
    if backend=="tensorflow":
       if (len(X.shape)!=4): ## shape [0,0,0]
          print("Error: Invalid shape for X variable")
          exit()
       else:
          # Performing activation histogram
          Sum = np.zeros((X.shape[3], nclasses))
          Hist = np.zeros((X.shape[3], nclasses))
          for sample in range(X.shape[0]):
              for f in range(X.shape[3]):
                  flag=False
                  for g in range(X.shape[1]):
                      for h in range(X.shape[2]):
                          if (X[sample, g, h, f]>0):
                             Sum[f,int(Y_valid[sample])]+=X[sample, g, h, f]
                             flag = True
                  if flag:
                     Hist[f, int(Y_valid[sample])]+=1
          idx =  X.shape[3]         		      
    else:  ## if backend==theano
       if (len(X.shape)!=4):
          print("Error: Invalid shape for X variable")
          exit()
       else:
          # Performing activation histogram
          Sum = np.zeros((X.shape[1], nclasses))
          Hist = np.zeros((X.shape[1], nclasses))
          
          for sample in range(X.shape[0]):
              for f in range(X.shape[1]):
                  flag = False
                  for g in range(X.shape[2]):
                      for h in range(X.shape[3]):
                          if (X[sample,f, g,h]>0):
                             Sum[f,Y_valid[sample]]+=1
                             flag = True
                  if flag:
                     Hist[f,Y_valid[sample]]+=1
          idx =  X.shape[1]              


    for j in range(idx):
        for k in range(nclasses):
            if Hist[j, k]>0:
               Sum[j, k]/=Hist[j, k]

    if filter_in is not None:  ## If filter_in is not None, we are interested in extract the filter weights as features, which each sample is a filter


       X_1 = filter_in.reshape(filter_in.shape[3], filter_in.shape[0]*filter_in.shape[1]*filter_in.shape[2]) 
       if bias is not None:
          x_out = np.zeros((X_1.shape[0], X_1.shape[1]+1))
          x_out[:,0] = bias.copy()
          x_out[:, 1:] = X_1.copy()
       else:
          x_out = X_1.copy()
       del X_1
       
       #print (filter_in.shape)
       label = np.zeros(filter_in.shape[3])       
       for i in range(filter_in.shape[3]): 
           label[i] = int(np.argmax(Sum[i], axis=-1))+1 
       groups = np.arange(filter_in.shape[3])+1
       Write_sup_dataset(x_out, label, label, nclasses, fileout, src, groups=groups)
                                       
    else:  ## If filter_in is None, we are interested in extract the histogram as features, which each samples is a filter
         
         label = np.zeros(idx)
         groups = np.arange(idx)+1
         for i in range(idx):
             label[i] = int(np.argmax(Sum[i], axis=-1))+1  
         Write_sup_dataset(Sum, label, label, nclasses, fileout, src, groups=groups)


