import csv
import math
import os
import pdb
import sys

import numpy as np

import pyift as ift

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


