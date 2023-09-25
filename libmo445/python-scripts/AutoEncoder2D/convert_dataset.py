import os
import pdb
import sys

import numpy as np

import util




def main():
    if len(sys.argv) != 3:
        sys.exit("convert_dataset.py <input_dataset.[csv,zip]> <output_dataset.[csv,zip]>")

    input_dataset_path = sys.argv[1]
    output_dataset_path = sys.argv[2]

    X, ref_data, true_labels = util.read_feature_vectors(input_dataset_path)
    util.save_feature_vectors(X, output_dataset_path, ref_data=ref_data)

    print('\n- Done...')




if __name__ == "__main__":
    main()
