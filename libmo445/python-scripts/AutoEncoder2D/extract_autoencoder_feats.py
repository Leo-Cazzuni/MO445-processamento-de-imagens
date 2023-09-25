import argparse
import os
import pdb
import sys

from keras.layers import Input
from keras.models import Model, load_model
import numpy as np

import util
import pyift as ift

def build_argparse():
    prog_desc = \
'''
Extract the feature vectors of an image or an image set using an AutoEncoder2D (
Encoder).

The output feature vector can be a CSV file, where the columns describes the features and the row i
contains the feature vector of the image i, or a OPF dataset (.zip).
'''
    parser = argparse.ArgumentParser(description=prog_desc, formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument('image_path', type=str,
                        help='Input Image.')
    parser.add_argument('autoencoder', type=str,
                        help='AutoEncoder2D (*.h5).')
    parser.add_argument('out_feature_vector_path', type=str,
                        help='Output CSV or OPF Dataset (*.zip) with the feature vectors.')

    return parser


def print_args(args):
    print('--------------------------------------------')
    print('- Input Image: %s' % args.image_path)
    print('- AutoEncoder2D: %s' % args.autoencoder)
    print('- Output Feature Vector File: %s' % args.out_feature_vector_path)
    print('--------------------------------------------\n')



def validate_args(args):
    if not args.autoencoder.endswith('.h5'):
        sys.exit('Invalid AutoEncoder2D extension: %s\nTry *.h5' %
                 args.autoencoder)

    if not args.out_feature_vector_path.endswith('.csv') and not args.out_feature_vector_path.endswith('.zip'):
        sys.exit('Invalid extension for the output feature vector file: %s\nTry .csv or .zip' %
                 args.out_feature_vector_path)

    parent_dir = os.path.dirname(args.out_feature_vector_path)
    if parent_dir and not os.path.exists(parent_dir):
        os.makedirs(parent_dir)


def get_encoder_model(autoencoder):
    n_encoding_layers = int(len(autoencoder.layers) / 2) # assuming that the
    # first half of layers is for encoding

    # assuming that the first layer is the Input Image Dimensions
    shape = autoencoder.layers[0].get_config()['batch_input_shape'] # returns something like: (None, 200, 200, 1)
    shape = shape[1:] # excluding the None value

    print('  - Setting up Encoder Layers')
    input_img = Input(shape=shape)
    encoded = autoencoder.layers[1](input_img)
    for i in range(2, n_encoding_layers):
        encoded = autoencoder.layers[i](encoded)
    encoder = Model(input_img, encoded)

    return encoder


def validate_image_autoencoder_shape(autoencoder, X):
    # assuming that the first layer is the Input Image Dimensions
    autoencoder_shape = autoencoder.layers[0].get_config()['batch_input_shape'] # returns something like: (None, 200, 200, 1)
    autoencoder_shape = autoencoder_shape[1:] # excluding the None value

    img_shape = X.shape[1:] # X.shape: (n_imgs, ysize, xsize, 1)

    if autoencoder_shape!= img_shape:
        sys.exit("\n\nERROR: AutoEncoder2D Input Layer and Images have different shapes (ysize, xsize, 1): " \
                 "{0} != {1}".format(autoencoder_shape, img_shape))


def main():
    parser = build_argparse()
    args = parser.parse_args()
    print_args(args)
    validate_args(args)

    print('- Loading Image Entry')
    img_paths = [args.image_path]
    X = util.read_image_list(img_paths)
    X = util.normalize_image_set(X)

    print('- Loading AutoEncoder2D')
    autoencoder = load_model(args.autoencoder)

    validate_image_autoencoder_shape(autoencoder, X)

    print('- Getting Encoder Model')
    encoder = get_encoder_model(autoencoder)

    print('- Extracting Features')
    n_imgs = X.shape[0]
    Xout = encoder.predict(X) # shape (n_imgs, ysize, xsize, 1)
    Xout = Xout.reshape((n_imgs, np.prod(Xout.shape[1:]))) # shape (n_imgs, ysize*xsize)

    print('- Saving Output Feature Vectors')
    util.save_feature_vectors(Xout, args.out_feature_vector_path, ref_data=img_paths)

    print('\n- Done...')



if __name__ == '__main__':
    main()
