#!/usr/bin/python

# author: Cesar Castelo
# date: Jul 8, 2019
# description: This program takes a directory containing a number of dataset splits following the IFT filename convention and creates a new 
# directory containing one folder for each split (this split directory follows the PyTorch image directory convention).
# 
# Example: If the input_dir contains the files:
# learn_001.csv, train_001.csv, test_001.csv
# learn_002.csv, train_002.csv, test_002.csv
# learn_003.csv, train_003.csv, test_003.csv
#
# given input_split_basenames="['learn','train','test']", n_splits=3 and output_split_basenames="['feat_learn','classif_learn','eval']"
# the script will create the following folder structure:
#
# ├── split_001
# │   ├── feat_learn
# │   ├── classif_learn
# │   └── eval
# ├── split_002
# │   ├── feat_learn
# │   ├── classif_learn
# │   └── eval
# ├── split_003
# │   ├── feat_learn
# │   ├── classif_learn
# │   └── eval
#
# Each of those folders is a image set folder following the PyTorch convention.

import subprocess
import os, sys
import shutil
import csv

# main function
if __name__ == "__main__":
    # get the size of the terminal window
    terminal_rows, terminal_cols = subprocess.check_output(['stty', 'size']).decode().split()
    terminal_cols = int(terminal_cols)

    # read parameters
    if(len(sys.argv) != 6):
        print("usage: create_pytorch_dir_from_ift_dir_with_splits.py <...>")
        print("[1] input_dir: Input directory containing the dataset splits in IFT format")
        print("[2] input_split_basenames: Basenames of the input splits (a list passed as string, e.g., ['learn','train','test'])")
        print("[3] n_splits: Number of splits")
        print("[4] output_dir: Output directory to create the image set in PyTorch format")
        print("[5] output_split_basenames: Basenames of the output splits (a list passed as string, e.g., ['feat_learn','classif_learn','eval'])")
        print()
        sys.exit(-1)

    input_dir = sys.argv[1].rstrip('/')
    input_split_basenames = eval(sys.argv[2])
    n_splits = int(sys.argv[3])
    output_dir = sys.argv[4].rstrip('/')
    output_split_basenames = eval(sys.argv[5])

    if len(input_split_basenames) != len(output_split_basenames):
        sys.exit("The number of input and output split names must be the same")

    for split in range(1, int(n_splits)+1):
        print("="*terminal_cols)
        print("Split {}/{}".format(split, n_splits))
        print("="*terminal_cols)
        # create the split folder
        split_str = "split_" + str(split).zfill(3)
        split_dir = os.path.join(output_dir, split_str)
        if not os.path.exists(split_dir):
            os.makedirs(split_dir)

        # create the subset folders
        for i in range(len(input_split_basenames)):
            input_fileset_split = input_split_basenames[i] + "_" + str(split).zfill(3) + ".csv"
            output_dir_split = os.path.join(split_str, output_split_basenames[i])

            print("--> Copying files from '{}' to '{}'".format(input_fileset_split, output_dir_split))

            # execute script
            cmd_params = list() 
            cmd_params.append("python3")
            cmd_params.append(os.environ["NEWIFT_DIR"] + "/python-scripts/ConvNets/create_pytorch_dir_from_ift_dir.py")
            cmd_params.append(os.path.join(input_dir, input_fileset_split))
            cmd_params.append(os.path.join(output_dir, output_dir_split))

            return_code = subprocess.call(cmd_params)
            if return_code != 0:
                sys.exit(-1)
        print()