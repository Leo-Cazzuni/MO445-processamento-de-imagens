#!/usr/bin/python

# author: Cesar Castelo
# date: Feb 1, 2019
# description: This program takes a directory following the PyTorch image directory convention and transforms it to the IFT filename convention.
# The PyTorch directory must have 'train' and 'eval' subdirectories inside it.

import os, sys
import shutil

def convert_files(dir_in, dir_out, subdir_name):
    # create the subdir
    subdir_out = os.path.join(dir_out, subdir_name)

    if not os.path.isdir(subdir_out):
        os.makedirs(subdir_out)

    # get number of classes
    classes = os.listdir(os.path.join(dir_in, subdir_name))
    classes.sort()
    n_class = len(classes)
    print('Number of classes:', n_class)

    # get the classes
    class_id = 1
    for class_name in classes:
        class_path = os.path.join(dir_in, subdir_name, class_name)
        print(class_path)

        # get the files for the current class
        files = os.listdir(class_path)
        # files.sort() # it is better not to sort the files!

        img_id = 1
        for file_name in files:
            file_path_in = os.path.join(class_path, file_name)
            new_file_name = class_id.__str__().zfill(6) + '_' + img_id.__str__().zfill(8) + '.png'
            file_path_out = os.path.join(subdir_out, new_file_name)
            shutil.copyfile(file_path_in, file_path_out)
            img_id += 1
        class_id += 1

# read parameters
if(len(sys.argv) != 3):
    print("usage: create_ift_dir_from_pytorch_dir.py <in_directory_path> <out_directory_path>")
    sys.exit(-1)
dir_in = sys.argv[1]
dir_out = sys.argv[2]

# training
print('Processing the training images ...')
convert_files(dir_in, dir_out, 'train')

print('Processing the validation images ...')
convert_files(dir_in, dir_out, 'eval')