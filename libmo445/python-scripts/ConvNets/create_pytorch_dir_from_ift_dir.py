#!/usr/bin/python

# author: Cesar Castelo
# date: May 9, 2019
# description: This program takes an image directory/CSV file following the IFT filename convention and creates a new one following the PyTorch image directory convention
#
# Example: If we use the following folder as input (a dataset with 3 classes containing 3 images per class):
# input_fileset/
# ├── 000001_00000001.png
# ├── 000001_00000002.png
# ├── 000001_00000003.png
# ├── 000002_00000001.png
# ├── 000002_00000002.png
# ├── 000002_00000003.png
# ├── 000003_00000001.png
# ├── 000003_00000002.png
# └── 000003_00000003.png
#
# output_dir/
# ├── 1
# │   ├── 000001_00000001.png
# │   ├── 000001_00000002.png
# │   └── 000001_00000003.png
# ├── 2
# │   ├── 000002_00000001.png
# │   ├── 000002_00000002.png
# │   └── 000002_00000003.png
# └── 3
#     ├── 000003_00000001.png
#     ├── 000003_00000002.png
#     └── 000003_00000003.png

import os, sys
import shutil
import csv

# main function
if __name__ == "__main__":
    # read parameters
    if(len(sys.argv) != 3):
        print("usage: create_pytorch_dir_from_ift_dir.py <...>")
        print("[1] input_fileset: Input directory/CSV file containing the image set in IFT format")
        print("[2] output_dir: Output directory to create the image set in PyTorch format")
        print()
        sys.exit(-1)

    input_fileset = sys.argv[1].rstrip('/')
    output_dir = sys.argv[2].rstrip('/')

    # read the files in the directory/csv and sort them by filename
    if os.path.isdir(input_fileset):
        files = os.listdir(input_fileset)
        files.sort()
        input_dir = input_fileset
    elif os.path.isfile(input_fileset):
        files = list()
        csv_file = open(input_fileset, 'r')
        csv_reader = csv.reader(csv_file, delimiter=',', quotechar='|')
        for row in csv_reader:
            files.append(row[0])
            input_dir, _ = os.path.split(row[0])
        files.sort()

    current_class = -1
    i = 0
    for file in files:
        # if the input is a CSV file, we get the filename from each row
        if os.path.isfile(input_fileset):
            _, file = os.path.split(file)
        file_with_path = os.path.join(input_dir, file)
        print("Processing file {} of {} ...\r".format(i+1, len(files)), end='')
        
        # split the filename
        token = file.split('_')
        class_id = int(token[0])

        # if the file is from a new class, create a new folder
        if(class_id != current_class):
            current_class_id = class_id
            class_dir = os.path.join(output_dir, str(class_id))
            if not os.path.isdir(class_dir):
                os.makedirs(class_dir)

        # copy the file to the current class folder
        _, filename = os.path.split(file)
        new_file_path = os.path.join(class_dir, filename)
        shutil.copyfile(file_with_path, new_file_path)
        i += 1
    print('\nDone.')