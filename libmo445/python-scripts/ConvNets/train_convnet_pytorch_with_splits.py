#!/usr/bin/python

# author: Cesar Castelo
# date: Jul 10, 2019
# description: Runs the script train_convnet_pytorch_with_splits.py for several splits and then summarizes the results

from __future__ import print_function, division

import subprocess
import sys
import os
import json
import numpy as np

#===============#
# main function
#===============#
if __name__ == "__main__":
    # get the size of the terminal window
    terminal_rows, terminal_cols = subprocess.check_output(['stty', 'size']).decode().split()
    terminal_cols = int(terminal_cols)

    # verify input parameters
    if(len(sys.argv) != 13):
        print("#"*terminal_cols)
        print("usage: train_convnet_pytorch_with_splits.py <...>")
        print("[1] dataset_dir: Directory containing the datasets for each split")
        print("[2] subset_dirs: Directory names of the image subsets used to train the model (a two-elem list passed as string, e.g., ['train','eval'])")
        print("[3] n_splits: Number of splits")
        print("[4] model_name: Pre-trainned conv network to be used (the ones with * are the best for each category)")
        print("    'alexnet','vgg11','vgg13','vgg16','vgg19'(*),")
        print("    'resnet18','resnet34','resnet50','resnet101','resnet152'(*),")
        print("    'squeezenet1_0','squeezenet1_1'(*),")
        print("    'densenet121','densenet161'(*),'densenet169','densenet201'")
        print("[5] loss_func_name: Loss function, i.e., optimization criterion (the suggested one is marked with *):")
        print("    'L1Loss','MSELoss','CrossEntropyLoss'(*),'CTCLoss','NLLLoss','PoissonNLLLoss','KLDivLoss','BCELoss',")
        print("    'BCEWithLogitsLoss','MarginRankingLoss','HingeEmbeddingLoss','MultiLabelMarginLoss','SmoothL1Loss',")
        print("    'SoftMarginLoss','MultiLabelSoftMarginLoss','CosineEmbeddingLoss','MultiMarginLoss','TripletMarginLoss'")
        print("[6] optimizer_name: Optimization algorithm (the suggested one is marked with *):")
        print("    'Adadelta','Adagrad','Adam'(*),'SparseAdam','Adamax','ASGD','LBFGS','RMSprop','Rprop','SGD'")
        print("[7] train_mode: Training mode")
        print("    'rnd-weights': Load an arquitecture and initialize it with random weights")
        print("    'fine-tuning': Load pre-trained features and perform fine-tuning")
        print("    'fixed-feats': Load pre-trained features and freeze the feature layers")
        print("[8] batch_size: Batch size")
        print("[9] n_epochs: Number of epochs")
        print("[10] learn_rate: Learning rate")
        print("[11] torch_device: Device to execute the tests ('cpu','cuda:0','cuda:1',etc)")
        print("[12] output_suffix: Suffix to be added to the output files")
        print("#"*terminal_cols)
        sys.exit(-1)

    # read input parameters
    dataset_dir = sys.argv[1].rstrip('/')
    subset_dirs = eval(sys.argv[2])
    subset_dirs_str = sys.argv[2]
    n_splits = int(sys.argv[3])
    model_name = sys.argv[4]
    loss_func_name = sys.argv[5]
    optimizer_name = sys.argv[6]
    train_mode = sys.argv[7]
    batch_size = int(sys.argv[8])
    n_epochs = int(sys.argv[9])
    learn_rate = float(sys.argv[10])
    torch_device = sys.argv[11]
    output_suffix = sys.argv[12]

    # get the names for the image subsets
    if len(subset_dirs) != 2:
        sys.exit("The number of image subsets in the subset_dirs list must be 2")
    train_set_name = subset_dirs[0]
    eval_set_name = subset_dirs[1]

    # print input parameters
    print("#"*terminal_cols)
    print("INPUT PARAMETERS (train_convnet_pytorch_with_splits.py script):")
    print("#"*terminal_cols)
    print("dataset_dir: {}".format(dataset_dir))
    print("subset_dirs: {}".format(subset_dirs))
    print("n_splits: {}".format(n_splits))
    print("model_name: {}".format(model_name))
    print("loss_func_name: {}".format(loss_func_name))
    print("optimizer_name: {}".format(optimizer_name))
    print("train_mode: {}".format(train_mode))
    print("batch_size: {}".format(batch_size))
    print("n_epochs: {}".format(n_epochs))
    print("learn_rate: {}".format(learn_rate))
    print("torch_device: {}".format(torch_device))
    print("output_suffix: {}".format(output_suffix))

    # create the output folder
    output_dir_basename = "convnet_" + dataset_dir
    output_dirname = output_dir_basename + '_' + model_name + '_' + loss_func_name + '_' + optimizer_name + '_' + train_mode
    if not os.path.exists(output_dirname):
        os.makedirs(output_dirname)

    # execute the ConvNet for each split
    for split in range(1, int(n_splits)+1):
        split = str(split)
        print("\n"+"#"*terminal_cols)
        print("SPLIT: {} of {}".format(split, n_splits))
        print("#"*terminal_cols)

        # variables for each iteration
        dataset_dir_split = os.path.join(dataset_dir, "split_" + str(split).zfill(3))
        output_suffix_split = output_suffix + "_split_" + str(split).zfill(3)

        # execute script
        cmd_params = list() 
        cmd_params.append("python3")
        cmd_params.append(os.environ["NEWIFT_DIR"] + "/python-scripts/ConvNets/train_convnet_pytorch.py")
        cmd_params.append(dataset_dir_split)
        cmd_params.append(subset_dirs_str)
        cmd_params.append(model_name)
        cmd_params.append(loss_func_name)
        cmd_params.append(optimizer_name)
        cmd_params.append(train_mode)
        cmd_params.append(str(batch_size))
        cmd_params.append(str(n_epochs))
        cmd_params.append(str(learn_rate))
        cmd_params.append(torch_device)
        cmd_params.append(output_dir_basename)
        cmd_params.append(output_suffix_split)

        print("RUNNING {} ...".format(cmd_params[1]))
        print("#"*terminal_cols)
        return_code = subprocess.call(cmd_params)
        if return_code != 0:
            sys.exit(-1)

    # read the kappa, acc and train_time results from the json files of each iteration
    results_dict = dict()
    results_dict = dict()
    results_dict['kappa'] = list()
    results_dict['acc'] = list()
    results_dict['train_time'] = list()

    for split in range(1, int(n_splits)+1):
        output_suffix_split = output_suffix + "_split_" + str(split).zfill(3)
        results_filename = os.path.join(output_dirname, "results_" + output_suffix_split + ".json")
        fp = open(results_filename, 'r')
        results_json = json.load(fp)

        subset_results = results_json['train_results']
        results_dict['kappa'].append(subset_results['best_eval_kappa']*100.0)
        results_dict['acc'].append(subset_results['best_eval_acc']*100.0)
        results_dict['train_time'].append(subset_results['train_time'])

    # compute mean and stdev for the results
    final_results = dict()
    final_results['best_eval_kappa'] = "{:.2f} +- {:.2f}".format(np.mean(results_dict['kappa']), np.std(results_dict['kappa']))
    final_results['best_eval_acc'] = "{:.2f} +- {:.2f}".format(np.mean(results_dict['acc']), np.std(results_dict['acc']))
    final_results['train_time'] = "{:.2f} +- {:.2f}".format(np.mean(results_dict['train_time']), np.std(results_dict['train_time']))

    # print input parameters and results
    print("\n"+"#"*terminal_cols)
    print("FINAL RESULTS")
    print("#"*terminal_cols)
    print("dataset_dir: {}".format(dataset_dir))
    print("subset_dirs: {}".format(subset_dirs))
    print("n_splits: {}".format(n_splits))
    print("model_name: {}".format(model_name))
    print("loss_func_name: {}".format(loss_func_name))
    print("optimizer_name: {}".format(optimizer_name))
    print("train_mode: {}".format(train_mode))
    print("batch_size: {}".format(batch_size))
    print("n_epochs: {}".format(n_epochs))
    print("learn_rate: {}".format(learn_rate))
    print("output_suffix: {}".format(output_suffix))
    print("train_results:")
    for key in final_results:
        print("- {}: {}".format(key, final_results[key]))

    # save results
    results_json = dict()
    results_json['dataset_dir'] = dataset_dir
    results_json['subset_dirs'] = subset_dirs
    results_json['n_splits'] = n_splits
    results_json['model_name'] = model_name
    results_json['loss_func_name'] = loss_func_name
    results_json['optimizer_name'] = optimizer_name
    results_json['train_mode'] = train_mode
    results_json['batch_size'] = batch_size
    results_json['n_epochs'] = n_epochs
    results_json['learn_rate'] = learn_rate
    results_json['output_suffix'] = output_suffix
    results_json['train_results'] = final_results

    final_results_filename = os.path.join(output_dirname, "results_" + output_suffix + ".json")
    fp = open(final_results_filename, 'w')
    json.dump(results_json, fp, sort_keys=False, indent=4)