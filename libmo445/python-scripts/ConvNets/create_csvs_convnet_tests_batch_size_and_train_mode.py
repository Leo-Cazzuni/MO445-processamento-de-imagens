import argparse
import numpy as np
import subprocess
import sys
import os
import json
import csv

#===============#
# main function
#===============#
if __name__ == "__main__":
    # get the size of the terminal window
    terminal_rows, terminal_cols = subprocess.check_output(['stty', 'size']).decode().split()
    terminal_cols = int(terminal_cols)

    # verify input parameters
    parser = argparse.ArgumentParser('Creates CSV files containing results of tests with different batch sizes and train modes (one file for each chosen metric)')
    required_named = parser.add_argument_group('required named arguments')
    required_named.add_argument('-i', '--input_folder', type=str, help='Folder containing the folders with the tests', required=True)
    required_named.add_argument('-d', '--dataset_basename', type=str, help='Basename of the dataset that was used for the tests', required=True)
    required_named.add_argument('-t', '--train_modes', type=str, help='The train modes used for the tests (a list passed as text)', required=True)
    required_named.add_argument('-m', '--metrics', type=str, help='The accuracy metrics used for the tests (a list passed as text)', required=True)
    required_named.add_argument('-b', '--batch_sizes', type=str, help='The batch sizes used for the tests (a list passed as text)', required=True)
    parser.add_argument("-a", "--model_name", type=str, default="alexnet", help="Convnet model that was used for the tests")
    parser.add_argument("-l", "--loss_func_name", type=str, default="CrossEntropyLoss", help="Loss function that was used for the tests")
    parser.add_argument("-p", "--optimizer_name", type=str, default="SGD", help="Optimizer that was used for the tests")
    parser.add_argument("-e", "--n_epochs", type=int, default=100, help="Number of epochs that was used for the tests")
    parser.add_argument("-r", "--learn_rate", type=float, default=0.001, help="Learn rate that was used for the tests")
    parser.add_argument("-n", "--train_phase", type=str, default='train', choices=['train','feat_learn','classif_learn'], help="Train phase to get the results from")
    args = parser.parse_args()

    # read input parameters
    input_folder = args.input_folder.rstrip('/')
    dataset_basename = args.dataset_basename.rstrip('/')
    train_modes = eval(args.train_modes)
    metrics = eval(args.metrics)
    batch_sizes = eval(args.batch_sizes)
    model_name = args.model_name
    loss_func_name = args.loss_func_name
    optimizer_name = args.optimizer_name
    n_epochs = args.n_epochs
    learn_rate = args.learn_rate
    train_phase = args.train_phase

    # lists for the results    
    results_dict = dict()
    for metric in metrics:
        results_dict[metric] = dict()
        results_dict[metric]['batch_size'] = train_modes
        for batch_size in batch_sizes:
            results_dict[metric][batch_size] = list()

    # read the folders with tests
    for train_mode in train_modes:
        folder_name = os.path.join(input_folder, '{}_{}_{}_{}_{}'.format(dataset_basename,model_name,loss_func_name,optimizer_name,train_mode))
        for batch_size in batch_sizes:
            file_name = os.path.join(folder_name, 'results_batch{}_nepochs{}_learnrate{}.json'.format(batch_size,n_epochs,learn_rate))
            fp = open(file_name, 'r')
            results_json = json.load(fp)
            for metric in metrics:
                results_dict[metric][batch_size].append(results_json[train_phase + '_results'][metric])

    # create the CSV files
    for metric in metrics:
        aux = results_dict[metric]
        csv_data = [[k]+aux[k] for k in list(aux)]
        csv_filename = os.path.join(input_folder, '{}_{}_{}_{}_{}.csv'.format(dataset_basename,model_name,loss_func_name,optimizer_name,metric))
        with open(csv_filename, 'w') as csv_file:
            writer = csv.writer(csv_file, delimiter=';')
            writer.writerows(csv_data)
        csv_file.close()
        print("File '{}' created ...".format(csv_filename))
