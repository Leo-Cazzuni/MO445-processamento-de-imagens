#!/usr/bin/python

# author: Cesar Castelo
# date: Jul 10, 2019
# description: This program takes two directories following the PyTorch image directory convention and joins them into one new directory

import subprocess
import os, sys
import shutil

def copy_files_from_class_dir(input_dir, out_dir, class_name):
    class_path_in = os.path.join(input_dir, class_name)
    class_path_out = os.path.join(out_dir, class_name)
    if not os.path.isdir(class_path_out):
        os.makedirs(class_path_out)

    # get the files for the current class
    files = os.listdir(class_path_in)

    img_id = 1
    for file_name in files:
        file_path_in = os.path.join(class_path_in, file_name)
        file_path_out = os.path.join(class_path_out, file_name)
        shutil.copyfile(file_path_in, file_path_out)
        img_id += 1

#===============#
# main function
#===============#
if __name__ == "__main__":
    # get the size of the terminal window
    terminal_rows, terminal_cols = subprocess.check_output(['stty', 'size']).decode().split()
    terminal_cols = int(terminal_cols)

    # verify input parameters
    if(len(sys.argv) != 4):
        print("usage: join_pytorch_dirs.py <input_dir_1> <input_dir_2> <out_dir>")
        sys.exit(-1)
    
    # read input parameters
    input_dir_1 = sys.argv[1].rstrip('/')
    input_dir_2 = sys.argv[2].rstrip('/')
    out_dir = sys.argv[3].rstrip('/')

    # create the output dir
    if not os.path.isdir(out_dir):
        os.makedirs(out_dir)

    # get number of classes
    classes = os.listdir(input_dir_1)
    classes.sort()
    n_class = len(classes)
    print('Number of classes:', n_class)

    # get the classes
    class_id = 1
    for class_name in classes:
        print('Joining files from class {} ...'.format(class_id))

        copy_files_from_class_dir(input_dir_1, out_dir, str(class_id))
        copy_files_from_class_dir(input_dir_2, out_dir, str(class_id))

        class_id += 1
    print('Done')