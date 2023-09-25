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
	args = ["iftSupervoxelDataSetFromDir"]
	args.extend(str_exp_params)
	print args
	call(args)

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("image_dir", type=str, help="Image directory")
	parser.add_argument("base_label_dir", type=str, help="Image directory")
	parser.add_argument("algo", type=str, help="algorithm")	
	parser.add_argument("sup_param", type=str, help="sup_param")	
	parser.add_argument("gt_dir", type=str, help="Image directory")	
	parser.add_argument("base_output_dir", type=str, help="Output directory")
	parser.add_argument("nproc", type=int, help="Number of processes")
	args = parser.parse_args()
	
	# read parameters
	image_dir = args.image_dir	
	base_label_dir = args.base_label_dir
	algo = args.algo
	sup_param = args.sup_param
	gt_dir = args.gt_dir
	base_output_dir = args.base_output_dir
	nproc = args.nproc
	
	pool = Pool(processes=nproc)

	list_exp_params = []
	
	nsup_list = np.array([10, 20, 40, 100, 200])

	# create list of experiment configurations		
	for nsup in nsup_list:
		label_dir = "{}{}_{}_{}/".format(base_label_dir, str(algo), str(sup_param), str(int(nsup)))
		output_dir = "{}{}_{}_{}/".format(base_output_dir, str(algo), str(sup_param), str(int(nsup)))
		if not os.path.exists(output_dir):
			os.makedirs(output_dir) 
		exp_params = []
		exp_params.append(image_dir)
		exp_params.append(label_dir)
		exp_params.append(gt_dir)
		exp_params.append(output_dir)	
		
		list_exp_params.append(exp_params)

	# Run experiments	
	if nproc == 1:
		for j in range(len(list_exp_params)):
			run_experiment(list_exp_params[j])
	else:
		pool.map(run_experiment, list_exp_params)
	
		
if __name__ == '__main__':
    main()