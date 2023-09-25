import argparse
import os
import pdb
import sys

from keras.layers import Input, Conv2D, MaxPooling2D, UpSampling2D
from keras.models import Model
import numpy as np

import util

def build_argparse():
    prog_desc = \
'''
Train an AutoEncoder2D from set of source and target 2D images.
All images must have the same resolution and orientation.

The source and target 2D images can be read from CSV files or directories.
The number of source images must be the same number of target images.
The list of images is read is sorted in alphabetic order, so that the source image [i] will reconstruct the
target image [j].

The CSV file must have one image per row.

The filenames's pattern follows the libIFT convention:
Eg: 000020_00000003.png

where 000020 is the class and 00000003 is the image id.
'''
    parser = argparse.ArgumentParser(description=prog_desc, formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument('source_image_entry', type=str,
                        help='Directory or CSV file with pathnames from the SOURCE images.')
    parser.add_argument('target_image_entry', type=str,
                        help='Directory or CSV file with pathnames from the TARGET images.')
    parser.add_argument('out_autoencoder', type=str,
                        help='Output AutoEncoder2D file *.h5.')
    parser.add_argument('-e', '--epochs', type=int, default=500,
                        help='Number of Epochs used for autoencoder training. Default 500')
    parser.add_argument('-b', '--batch-size', type=int, default=1,
                        help='Batch size used for autoencoder training. '
                             'Default 1')
    return parser


def print_args(args):
    print('--------------------------------------------')
    print('- SOURCE image entry: %s' % args.source_image_entry)
    print('- TARGET image entry: %s' % args.target_image_entry)
    print('- Ouput AutoEncoder2D: %s' % args.out_autoencoder)
    print('--------------------------------------------')
    print('- Epochs: %d' % args.epochs)
    print('- Batch Size: %d' % args.batch_size)
    print('--------------------------------------------\n')


def validate_args(args):
    if not os.path.isdir(args.source_image_entry) and not args.source_image_entry.endswith('.csv'):
        sys.exit('Invalid SOURCE image entry: %s\nTry a CSV or a Directory' % args.source_image_entry)

    if not os.path.isdir(args.target_image_entry) and not args.target_image_entry.endswith('.csv'):
        sys.exit('Invalid TARGET image entry: %s\nTry a CSV or a Directory' % args.target_image_entry)

    if not args.out_autoencoder.endswith('.h5'):
        sys.exit('Invalid AutoEncoder2D extension: %s\nTry *.h5' %
                 args.out_autoencoder)

    parent_dir = os.path.dirname(args.out_autoencoder)
    if parent_dir and not os.path.exists(parent_dir):
        os.makedirs(parent_dir)


def train_autoencoder(Xsource, Xtarget, epochs=500, batch_size=1):
    # source: https://blog.keras.io/building-autoencoders-in-keras.html
    # Xsource.shape = (n_imgs, ysize, xsize, n_channels) dataset of 2D gray/color images
    # Xsource[0].shape = shape = (ysize, xsize, n_channels) 2D gray/color images
    shape = Xsource[0].shape
    n_channels = shape[-1]

    # for the gray channel
    input_img = Input(shape=shape) # (ysize, xsize, n_channels)

    print('  - Setting up AutoEncoder2D Layers')
    # To train an autoencoder, we must setup its architecture in the same
    # (function) scope, otherwise, it does not work.
    x = Conv2D(16, (3, 3), activation='relu', padding='same')(input_img)
    x = MaxPooling2D((2, 2), padding='same')(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = MaxPooling2D((2, 2), padding='same')(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    # "encoded" is the encoded representation of the input
    encoded = MaxPooling2D((2, 2), padding='same')(x)

    x = Conv2D(8, (3, 3), activation='relu', padding='same')(encoded)
    x = UpSampling2D((2, 2))(x)
    x = Conv2D(8, (3, 3), activation='relu', padding='same')(x)
    x = UpSampling2D((2, 2))(x)
    x = Conv2D(16, (3, 3), activation='relu', padding='same')(x)
    x = UpSampling2D((2, 2))(x)
    # "decoded" is the lossy reconstruction of the input
    decoded = Conv2D(n_channels, (3, 3), activation='sigmoid', padding='same')(x)

    autoencoder = Model(input_img, decoded)
    autoencoder.compile(optimizer='rmsprop', loss='binary_crossentropy')

    print('  - Fitting AutoEncoder2D')
    autoencoder.fit(Xsource, Xtarget,
                    epochs=epochs,
                    batch_size=batch_size,
                    shuffle=True,
                    verbose=1,
                    validation_data=(None))

    return autoencoder


def main():
    parser = build_argparse()
    args = parser.parse_args()
    print_args(args)
    validate_args(args)

    print('- Loading Image Sets')
    source_img_paths = util.read_image_entry(args.source_image_entry)
    target_img_paths = util.read_image_entry(args.target_image_entry)

    if len(source_img_paths) != len(target_img_paths):
        sys.exit("ERROR: Number of SOURCE images is != Number of TARGET images: %d != %d" % (len(source_img_paths), len(target_img_paths)))

    Xsource = util.read_image_list(source_img_paths)
    Xsource = util.normalize_image_set(Xsource)

    Xtarget = util.read_image_list(target_img_paths)
    Xtarget = util.normalize_image_set(Xtarget)

    autoencoder = train_autoencoder(Xsource, Xtarget, epochs=args.epochs, batch_size=args.batch_size)

    print('- Saving AutoEncoder2D')
    autoencoder.save(args.out_autoencoder)

    print('Done...')



if __name__ == '__main__':
    main()


