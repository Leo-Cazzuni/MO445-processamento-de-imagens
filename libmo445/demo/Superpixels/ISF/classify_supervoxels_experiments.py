#!/usr/bin/python2.7
import os
import sys
import argparse
import time
import numpy as np

from subprocess import call
from multiprocessing import Pool

def run_experiment(exp_params):
	str_exp_params = [str(e) for e in exp_params]
	args = ["iftClassifySupervoxelsFromDir"]
	args.extend(str_exp_params)
	print args
	call(args)

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("gt_dir", type=str, help="Image directory")	
	parser.add_argument("base_label_dir", type=str, help="Image directory")
	parser.add_argument("algo", type=str, help="algorithm")	
	parser.add_argument("sup_param", type=str, help="sup_param")	
	parser.add_argument("base_dataset_dir", type=str, help="Image directory")
	parser.add_argument("perc_train", type=float, help="Output directory")
	parser.add_argument("output_mean_acc", type=str, help="Output mean accuracy csv file")
	parser.add_argument("nproc", type=int, help="Number of processes")
	parser.add_argument("-md", "--markers_dir", type=str, help="Markers directory")
	args = parser.parse_args()
	
	# read parameters
	gt_dir = args.gt_dir	
	base_label_dir = args.base_label_dir
	algo = args.algo
	sup_param = args.sup_param
	base_dataset_dir = args.base_dataset_dir
	perc_train = args.perc_train
	output_mean_acc = args.output_mean_acc
	nproc = args.nproc
	markers_dir = None
	markers_dir = args.markers_dir

	pool = Pool(processes=nproc)

	list_exp_params = []
	
	nsup_list = np.array([10, 20, 40, 100, 200])
	nsup_values = nsup_list.shape[0]

	# create list of experiment configurations		
	for nsup in nsup_list:
		label_dir = "{}{}_{}_{}/".format(base_label_dir, str(algo), str(sup_param), str(int(nsup)))
		dataset_dir = "{}{}_{}_{}/".format(base_dataset_dir, str(algo), str(sup_param), str(int(nsup)))
		acc_path = "{}{}_{}_{}/acc.csv".format(base_dataset_dir, str(algo), str(sup_param), str(int(nsup)))
		exp_params = []
		exp_params.append(gt_dir)
		exp_params.append(label_dir)
		exp_params.append(dataset_dir)
		exp_params.append(perc_train)
		exp_params.append(dataset_dir)
		exp_params.append(acc_path)	

		if (markers_dir is not None):
			exp_params.append(markers_dir)

		list_exp_params.append(exp_params)

	# Run experiments	
	if nproc == 1:
		for j in range(len(list_exp_params)):
			run_experiment(list_exp_params[j])
	else:
		pool.map(run_experiment, list_exp_params)
	
	# save mean accuracies
	acc_mean = np.zeros(nsup_values)
	idx_nsup = 0
	for nsup in nsup_list:
		acc_path = "{}{}_{}_{}/acc.csv".format(base_dataset_dir, str(algo), str(sup_param), str(int(nsup)))
		acc_arr = np.genfromtxt(acc_path, delimiter=',').astype(np.float)
		acc_mean[idx_nsup] = acc_arr[-1]
		idx_nsup += 1

	np.savetxt(output_mean_acc, acc_mean, fmt = '%.6f', delimiter = ',')
		
if __name__ == '__main__':
    main()