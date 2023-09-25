import argparse
import matplotlib.pyplot as plt
import matplotlib.ticker
import numpy as np
import pandas
import subprocess
import sys

#===============#
# main function
#===============#
if __name__ == "__main__":
    # get the size of the terminal window
    terminal_rows, terminal_cols = subprocess.check_output(['stty', 'size']).decode().split()
    terminal_cols = int(terminal_cols)

    # verify input parameters
    parser = argparse.ArgumentParser('Creates a plot with multiple series from a CSV file')
    required_named = parser.add_argument_group('required named arguments')
    required_named.add_argument('-i', '--input_data', type=str, help='CSV (semi-colon) file containing the x and y values (first column is the \
                                x-axis). If the values contain "+-" standard deviation will be considered in the plot', required=True)
    required_named.add_argument('-o', '--output_img', type=str, help='Output image (.png)', required=True)
    parser.add_argument("--sep_char", type=str, default=";", help="Separator character for the CSV file")
    parser.add_argument("-c", "--columns_plot", type=str, default="all", help="The columns to be plotted (a list passed as text) starting from 1, \
                        0 is the x-axis")
    parser.add_argument("-r", "--first_row_names", action="store_true", default=False, help="Use the first row as the series names")
    parser.add_argument("-l", "--log_scale", type=str, default="no", choices=['x-axis','y-axis','x-y-axis'], help="Whether or not to use log \
                        scale in the axes")
    parser.add_argument("-t", "--plot_title", type=str, default="Scatter plot", help="Title for the plot")
    parser.add_argument("--minor_ticks", action="store_true", default=False, help="Show minor ticks in the axes")
    parser.add_argument("--axes_ticks_rotation", type=float, default=0, help="Change axes ticks rotation")
    parser.add_argument("--x_vals_as_ticks", action="store_true", default=False, help="Use x-values as x-ticks")
    parser.add_argument("--y_vals_as_ticks", action="store_true", default=False, help="Use y-values as y-ticks (takes the first y-series in the data)")
    parser.add_argument("--x_label", type=str, help="Label for the x-axis")
    parser.add_argument("--y_label", type=str, help="Label for the y-axis")
    parser.add_argument("--x_range", type=str, default="[]", help="Range for the x-values (a two-element list passed as text)")
    parser.add_argument("--y_range", type=str, default="[]", help="Range for the y-values (a two-element list passed as text)")
    parser.add_argument("--x_grid", action="store_true", default=False, help="Show x-axis grid lines")
    parser.add_argument("--y_grid", action="store_true", default=False, help="Show y-axis grid lines")
    parser.add_argument("--no_error_bars", action="store_true", default=False, help="Do not plot error bars")
    parser.add_argument("--line_format", type=str, default="o-", help="Format to be used in the plot lines: a single format for all the series. It must \
                        be a valid matplotlib style/marker/color combination")
    parser.add_argument("--line_format_list", type=str, default="[]", help="Format to be used in the plot lines: a list specifying the format for each \
                        series (a list passed as text). It must be a valid matplotlib style/marker/color combination")
    parser.add_argument("--legend_loc", type=str, default="best", choices=['best','upper right','upper left','lower left','lower right','right',
                        'center left','center right','lower center','upper center','center'], help="Legend location inside the plot")
    args = parser.parse_args()

    # read input parameters
    input_data = args.input_data
    output_img = args.output_img
    sep_char = args.sep_char
    columns_plot = args.columns_plot
    first_row_names = args.first_row_names
    log_scale = args.log_scale
    plot_title = args.plot_title
    minor_ticks = args.minor_ticks
    axes_ticks_rotation = args.axes_ticks_rotation
    x_vals_as_ticks = args.x_vals_as_ticks
    y_vals_as_ticks = args.y_vals_as_ticks
    x_label = args.x_label
    y_label = args.y_label
    x_range = eval(args.x_range)
    y_range = eval(args.y_range)
    x_grid = args.x_grid
    y_grid = args.y_grid
    no_error_bars = args.no_error_bars
    line_format = args.line_format
    line_format_list = eval(args.line_format_list)
    legend_loc = args.legend_loc
    
    # read the input files
    if first_row_names:
        data = pandas.read_csv(input_data, sep=sep_char)
    else:
        data = pandas.read_csv(input_data, sep=sep_char, header=None)

    fig, ax = plt.subplots()

    # setup the log scale
    if log_scale == 'x-axis':
        plt.xscale('log')
    elif log_scale == 'y-axis':
        plt.xscale('log')
    elif log_scale == 'x-y-axis':
        plt.xscale('log')
        plt.xscale('log')

    # verify columns to plot
    if columns_plot == 'all':
        columns_plot = range(1, len(data.columns))
    else:
        columns_plot = eval(columns_plot)

    # minor ticks in axes
    if minor_ticks:
        plt.minorticks_on()
    else:
        plt.minorticks_off()

    # set x and y limit values
    if x_range != []:
        plt.xlim(x_range[0],x_range[1])
    if y_range != []:
        plt.ylim(y_range[0],y_range[1])

    # set grid lines
    if x_grid:
        ax.xaxis.grid(True)
    if y_grid:
        ax.yaxis.grid(True)

    # set the format for the lines
    if line_format_list != []:
        if len(line_format_list) != len(columns_plot):
            sys.exit('The list with line formats must have the same length as the list of columns to plot')
    else:
        line_format_list = [line_format for i in range(len(columns_plot))]

    # create the data series
    x = data[data.columns[0]].to_numpy('float')

    # use x-values as ticks
    if x_vals_as_ticks:
        plt.xticks(x, rotation=axes_ticks_rotation)

    for i in range(len(columns_plot)):
        col = data[data.columns[columns_plot[i]]]
        
        # verify the use of error bars
        if type(col[0]) == str and col[0].find("+-") != -1:
            y = [float(val.split('+-')[0]) for val in col]
            e = [float(val.split('+-')[1]) for val in col]
            if no_error_bars:
                plt.plot(x, y, line_format_list[i])
            else:
                plt.errorbar(x, y, e, fmt=line_format_list[i], capsize=4)
        else:
            y = col.to_numpy('float')
            plt.plot(x, y, line_format_list[i])

        # use y-values as ticks
        if y_vals_as_ticks:
            plt.yticks(y, rotation=axes_ticks_rotation)
            y_vals_as_ticks = False

    # set other params for the plot
    plt.title(plot_title)
    plt.xlabel(x_label)
    plt.ylabel(y_label)
    ax.xaxis.set_major_formatter(matplotlib.ticker.ScalarFormatter())
    plt.legend(list(data.columns[columns_plot]), loc=legend_loc)
    plt.savefig(output_img)