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
    parser = argparse.ArgumentParser('Creates plots from the CSV files containing results of tests with different batch sizes and train modes')
    required_named = parser.add_argument_group('required named arguments')
    required_named.add_argument('-i', '--input_folder', type=str, help='Folder containing the CSV files with the test results', required=True)
    required_named.add_argument('-d', '--dataset_basename', type=str, help='Basename of the dataset that was used for the tests', required=True)
    required_named.add_argument('--acc_metrics', type=str, help='The accuracy metrics used for the tests (a list passed as text)', required=True)
    parser.add_argument("--dataset_short_name", type=str, help="Dataset short name to be used in the plot title")
    parser.add_argument('--time_metrics', type=str, default="[]", help='The time metrics used for the tests (a list passed as text)')
    parser.add_argument('--acc_val_range', type=str, default="[-3,103]", help='Range for the accuracy values in the plot')
    parser.add_argument('--time_val_range', type=str, default="[]", help='Range for the time values in the plot')
    parser.add_argument("-a", "--model_name", type=str, default="alexnet", help="Convnet model that was used for the tests")
    parser.add_argument("-l", "--loss_func_name", type=str, default="CrossEntropyLoss", help="Loss function that was used for the tests")
    parser.add_argument("-p", "--optimizer_name", type=str, default="SGD", help="Optimizer that was used for the tests")
    parser.add_argument("--sep_char", type=str, default=";", help="Separator character for the CSV file")
    parser.add_argument("--no_error_bars", action="store_true", default=False, help="Do not plot error bars")
    parser.add_argument("--line_format", type=str, default="o-", help="Format to be used in the plot lines: a single format for all the series. It must \
                        be a valid matplotlib style/marker/color combination")
    parser.add_argument("--line_format_list", type=str, default="[]", help="Format to be used in the plot lines: a list specifying the format for each \
                        series (a list passed as text). It must be a valid matplotlib style/marker/color combination")
    args = parser.parse_args()

    # read input parameters
    input_folder = args.input_folder.rstrip('/')
    dataset_basename = args.dataset_basename.rstrip('/')
    acc_metrics = eval(args.acc_metrics)
    dataset_short_name = args.dataset_short_name
    time_metrics = eval(args.time_metrics)
    acc_val_range = eval(args.acc_val_range)
    time_val_range = eval(args.time_val_range)
    model_name = args.model_name
    loss_func_name = args.loss_func_name
    optimizer_name = args.optimizer_name
    sep_char = args.sep_char
    no_error_bars = args.no_error_bars
    line_format = args.line_format
    line_format_list = eval(args.line_format_list)

    # create the plots from the CSV files (acc_metrics)
    x_label = 'Batch size'
    for metric in acc_metrics:
        y_label = metric
        csv_filename = os.path.join(input_folder, '{}_{}_{}_{}_{}.csv'.format(dataset_basename,model_name,loss_func_name,optimizer_name,metric))
        plot_filename = os.path.join(input_folder, '{}_{}_{}_{}_{}.png'.format(dataset_basename,model_name,loss_func_name,optimizer_name,metric))
        plot_title = '{} x {} ({})'.format(x_label, y_label, dataset_short_name) if len(dataset_short_name) > 0 else '"{} x {}"'.format(x_label, y_label)
        cmd = 'python3 {}/python-scripts/Plot/scatterplot_multiple_columns.py -i "{}" -o "{}" -r -l "x-axis" -t "{}" --x_vals_as_ticks --y_range \
              "{}" --y_grid --x_label "{}" --y_label "{}" --sep_char "{}" {} --line_format "{}" --line_format_list "{}"'.format(os.environ['NEWIFT_DIR'],
              csv_filename, plot_filename, plot_title, str(acc_val_range), x_label, y_label, sep_char, '--no_error_bars' if no_error_bars else '',
              line_format, str(line_format_list))

        print('Creating plot for metric "{}" ...'.format(metric))
        if os.system(cmd) != 0:
            sys.exit("Error in /python-scripts/Plot/scatterplot_multiple_columns.py")

    # create the plots from the CSV files (time_metrics)
    if time_metrics != []:
        for metric in time_metrics:
            y_label = metric
            csv_filename = os.path.join(input_folder, '{}_{}_{}_{}_{}.csv'.format(dataset_basename,model_name,loss_func_name,optimizer_name,metric))
            plot_filename = os.path.join(input_folder, '{}_{}_{}_{}_{}.png'.format(dataset_basename,model_name,loss_func_name,optimizer_name,metric))
            plot_title = '{} x {} ({})'.format(x_label, y_label, dataset_short_name) if len(dataset_short_name) > 0 else '"{} x {}"'.format(x_label, y_label)
            cmd = 'python3 {}/python-scripts/Plot/scatterplot_multiple_columns.py -i "{}" -o "{}" -r -l "x-axis" -t "{}" --x_vals_as_ticks --y_range \
                  "{}" --y_grid --x_label "{}" --y_label "{}" --sep_char "{}" {} --line_format "{}" --line_format_list "{}"'.format(os.environ['NEWIFT_DIR'],
                  csv_filename, plot_filename, plot_title, str(time_val_range), x_label, y_label, sep_char, '--no_error_bars' if no_error_bars else '',
                  line_format, str(line_format_list))

            print('Creating plot for metric "{}" ...'.format(metric))
            if os.system(cmd) != 0:
                sys.exit("Error in /python-scripts/Plot/scatterplot_multiple_columns.py")