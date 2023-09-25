import argparse
import os
import pdb
import sys
import random
import timeit

import numpy as np
from sklearn.model_selection import StratifiedShuffleSplit

import opf_dataset as opf



def build_argparse():
    prog_desc = \
'''
Project a dataset in several dimensions by tSNE and trains the projections with SVM.
'''
    parser = argparse.ArgumentParser(description=prog_desc, formatter_class=argparse.RawTextHelpFormatter)

    parser.add_argument('opf_dataset', type=str,
                        help='OPF Dataset (*.zip)')
    parser.add_argument('perc_sampl', type=float,
                        help='Percentage for sampling the dataset')
    parser.add_argument('opf_dataset_new', type=str,
                        help='New OPF Dataset (*.zip)')
    return parser

def print_args(args):
    print('--------------------------------------------')
    print('- Input OPF Dataset: %s' % args.opf_dataset)
    print('- Percentage for sampling the dataset: %s' % args.perc_sampl)
    print('- New OPF Dataset: %s' % args.opf_dataset_new)
    print('--------------------------------------------\n')

def main():
    parser = build_argparse()
    args = parser.parse_args()

    if args.perc_sampl <= 0:
        print('Error: The sampling percentage must be greater than 0')
        sys.exit()

    print_args(args)

    print('- Reading the dataset')
    Z = opf.read_dataset(args.opf_dataset)
    X, Y = Z["feats"], Z["truelabel"]

    #stratified sampling
    print('- Sampling the dataset')
    sampler = StratifiedShuffleSplit(n_splits=1, test_size=(1.0-args.perc_sampl), random_state=0)
    for train_index, test_index in sampler.split(X, Y):
        Z['id'] = Z['id'][train_index]
        Z['feats'] = Z['feats'][train_index]
        Z['label'] = Z['label'][train_index]
        Z['truelabel'] = Z['truelabel'][train_index]
        Z['status'] = Z['status'][train_index]
        Z['weight'] = Z['weight'][train_index]
        Z['ref_data'] = Z['ref_data'][train_index]
    
    #save new dataset
    print('- Saving the new dataset')
    opf.save_dataset(args.opf_dataset_new, Z)

if __name__ == '__main__':
    main()


