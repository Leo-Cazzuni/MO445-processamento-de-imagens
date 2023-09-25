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
    parser = argparse.ArgumentParser('Perform convnet tests with different batch sizes and train modes')
    required_named = parser.add_argument_group('required named arguments')
    required_named.add_argument('-i', '--input_imgs', type=str, help='Directory containing the train and eval image sets (following the PyTorch convention)', required=True)
    required_named.add_argument('-s', '--subset_dirs', type=str, help="Directory names of the image subsets used to train the model (a two-elem or three-elem list passed as string, e.g., ['train','eval'])", required=True)
    required_named.add_argument('-n', '--n_splits', type=int, help="Number of splits", required=True)
    required_named.add_argument('-t', '--train_modes', type=str, help='The train modes for the tests (a list passed as text)', required=True)
    required_named.add_argument('-b', '--batch_sizes', type=str, help='The batch sizes for the tests (a list passed as text)', required=True)
    parser.add_argument('--model_name', type=str, default='alexnet', choices=['alexnet','vgg11','vgg13','vgg16','vgg19','resnet18',
                        'resnet34','resnet50','resnet101','resnet152','squeezenet1_0','squeezenet1_1','densenet121','densenet161','densenet169',
                        'densenet201'], help='Convnet model to be used')
    parser.add_argument('--loss_func_name', type=str, default='CrossEntropyLoss', choices=['L1Loss','MSELoss','CrossEntropyLoss','CTCLoss',
                        'NLLLoss','PoissonNLLLoss','KLDivLoss','BCELoss','BCEWithLogitsLoss','MarginRankingLoss','HingeEmbeddingLoss',
                        'MultiLabelMarginLoss','SmoothL1Loss','SoftMarginLoss','MultiLabelSoftMarginLoss','CosineEmbeddingLoss','MultiMarginLoss',
                        'TripletMarginLoss'], help='Loss function, i.e., optimization criterion')
    parser.add_argument('--optimizer_name', type=str, default='SGD', choices=['Adadelta','Adagrad','Adam','SparseAdam','Adamax','ASGD',
                        'LBFGS','RMSprop','Rprop','SGD'], help='Optimization algorithm')
    parser.add_argument("-e", "--n_epochs", type=int, default=100, help="Number of epochs that was used for the tests")
    parser.add_argument("-r", "--learn_rate", type=float, default=0.001, help="Learn rate that was used for the tests")
    parser.add_argument("-d", "--torch_device", type=str, default='cpu', help="Device to execute the tests")
    parser.add_argument("-x", "--convnet_script", type=str, default='/python-scripts/ConvNets/train_convnet_pytorch_with_splits.py',
                        help="Script to train the convnet")
    args = parser.parse_args()

    # read input parameters
    input_imgs = args.input_imgs.rstrip('/')
    subset_dirs = args.subset_dirs
    n_splits = args.n_splits
    train_modes = eval(args.train_modes)
    batch_sizes = eval(args.batch_sizes)
    model_name = args.model_name
    loss_func_name = args.loss_func_name
    optimizer_name = args.optimizer_name
    n_epochs = args.n_epochs
    learn_rate = args.learn_rate
    torch_device = args.torch_device
    convnet_script = args.convnet_script

    # execute the script to train the convnet
    for train_mode in train_modes:
        for batch_size in batch_sizes:
            output_suffix = 'batch{}_nepochs{}_learnrate{}'.format(batch_size,n_epochs,learn_rate)
            cmd_params = list()
            cmd_params.append('python3')
            cmd_params.append(os.environ['NEWIFT_DIR'] + convnet_script)
            cmd_params.append(input_imgs)
            cmd_params.append(subset_dirs)
            cmd_params.append(str(n_splits))
            cmd_params.append(model_name)
            cmd_params.append(loss_func_name)
            cmd_params.append(optimizer_name)
            cmd_params.append(train_mode)
            cmd_params.append(str(batch_size))
            cmd_params.append(str(n_epochs))
            cmd_params.append(str(learn_rate))
            cmd_params.append(torch_device)
            cmd_params.append(output_suffix)

            print("@"*terminal_cols)
            print("RUNNING {} ...".format(cmd_params[1]))
            print("@"*terminal_cols)
            return_code = subprocess.call(cmd_params)
            if return_code != 0:
                sys.exit(-1)