import argparse
import os
import pdb
import sys

from keras.layers import Input
from keras.models import Model, load_model
import numpy as np

import pyift as ift
import util

def build_argparse():
    prog_desc = \
'''
Reconstruct a set of images by decoding their feature vectors with Decoder 
from a  trained autoencoder.

The input image path must be a CSV file or an OPF dataset.

The output reconstruct images will be saved into the output directory with the same filename of their
original images (before feature extraction).
'''
    parser = argparse.ArgumentParser(description=prog_desc, formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument('feature_vector_path', type=str,
                        help='Feature Vector (CSV or OPF Dataset *zip).')
    parser.add_argument('autoencoder', type=str,
                        help='AutoEncoder2D (*.h5).')
    parser.add_argument('output_dir', type=str,
                        help='Output Directory where the reconstructed images will be saved.')

    return parser


def print_args(args):
    print('--------------------------------------------')
    print('- Input Feature Vector: %s' % args.feature_vector_path)
    print('- AutoEncoder2D: %s' % args.autoencoder)
    print('- Output Directory: %s' % args.output_dir)
    print('--------------------------------------------\n')



def validate_args(args):
    if not args.feature_vector_path.endswith('.csv') and not args.feature_vector_path.endswith('.zip'):
        sys.exit('Invalid extension for the input feature vector file: %s\nTry .csv or .zip' %
                 args.feature_vector_path)

    if not args.autoencoder.endswith('.h5'):
        sys.exit('Invalid AutoEncoder2D extension: %s\nTry *.h5' %
                 args.autoencoder)

    if not os.path.exists(args.output_dir):
        os.makedirs(args.output_dir)


def get_decoder_input_shape(autoencoder):
    first_decoded_layer = int(len(autoencoder.layers) / 2) # assuming that the first half of layers is for encoding
    input_shape = autoencoder.layers[first_decoded_layer].get_input_shape_at(0) # returns something like: (None, 200, 200, 1)
    input_shape = input_shape[1:] # excluding the None value

    return input_shape

def get_decoder_model(autoencoder):
    n_total_layers = len(autoencoder.layers)
    first_decoding_layer = int(len(autoencoder.layers) / 2) # assuming that the
    # first half of layers is for encoding

    decoder_input_shape = get_decoder_input_shape(autoencoder)
    decoder_input = Input(shape=decoder_input_shape)

    decoded = autoencoder.layers[first_decoding_layer](decoder_input)
    for i in range(first_decoding_layer+1, n_total_layers):
        decoded = autoencoder.layers[i](decoded)
    decoder = Model(decoder_input, decoded)

    return decoder


def validate_feat_vector_autoencoder_shape(autoencoder, X):
    first_decoded_layer = int(len(autoencoder.layers) / 2) # assuming that the first half of layers is for encoding
    input_shape = autoencoder.layers[first_decoded_layer].get_input_shape_at(0) # returns something like: (None, 200, 200, 1)
    input_shape = input_shape[1:] # excluding the None value

    n_input_pixels = np.prod(input_shape)
    n_feats = X.shape[1] # X.shape: (n_imgs, n_feats)

    if n_input_pixels != n_feats:
        sys.exit("\n\nERROR: Decoder Input Layer and Images have different number of points/pixels/feats:\n" \
                "Decoder Input Layer shape: {0}\n" \
                "Decoder Input Layer number of pixels: {1}\n" \
                "Number of Feats: {2}".format(input_shape, n_input_pixels, n_feats))


def main():
    parser = build_argparse()
    args = parser.parse_args()
    print_args(args)
    validate_args(args)

    print('- Loading Feature Vector')
    X, ref_data, true_labels = util.read_feature_vectors(args.feature_vector_path)
    if X.ndim == 1:
        X = np.reshape(X, (1, X.shape[0]))

    print('- Loading AutoEncoder2D')
    autoencoder = load_model(args.autoencoder)

    validate_feat_vector_autoencoder_shape(autoencoder, X)

    decoder_input_shape = get_decoder_input_shape(autoencoder)
    n_imgs = X.shape[0]
    new_shape = tuple([n_imgs] + list(decoder_input_shape))
    X = X.reshape(new_shape)

    print('- Getting Decoder Model')
    decoder = get_decoder_model(autoencoder)

    print('- Decoding')
    Xout = decoder.predict(X) # shape (n_imgs, ysize, xsize, n_channels)
    img_npy = ift.ReadImageByExt(ref_data[0]).AsNumPy()
    max_range = util.normalization_value(img_npy) 

    Xout = Xout * max_range
    Xout = Xout.astype(np.int32)

    print('- Saving Reconstruct Images')
    for i in range(n_imgs):
        img_path = os.path.join(args.output_dir, os.path.basename(ref_data[i]))
        img = ift.CreateImageFromNumPy(Xout[i], False) # (img_npy, is3D)
        ift.WriteImageByExt(img, img_path)

    print('\n- Done...')



if __name__ == '__main__':
    main()
