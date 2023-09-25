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
	args = ["slic_exp"]
	args.extend(str_exp_params)
	print args
	call(args)
	output_csv = str_exp_params[-1]
	nseeds_arr = np.genfromtxt(output_csv, delimiter=',').astype(np.float)
	return nseeds_arr

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("dataset_path", type=str, help="Dataset path")	
	parser.add_argument("output_path", type=str, help="Output path")
	parser.add_argument("nproc", type=int, help="Number of processes")
	args = parser.parse_args()
	
	# read parameters
	dataset_path = args.dataset_path	
	output_path = args.output_path
	nproc = args.nproc
	
	pool = Pool(processes=nproc)

	list_exp_params = []
	algo = "slic"

	#nsup_list = np.linspace(100,1000,10)
	# default values
	m_list = np.linspace(6,22,9)
	m_list = m_list[::-1]
	nsup_list = np.linspace(100,500,9)
	
	# other parameters
	#m_list = np.array([10])
	#nsup_list = np.array([10, 20, 40, 100, 200])
	#nsup_list = np.linspace(50,400,8)
	#nsup_list = np.linspace(50,500,10)
	#nsup_list = np.array([50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600, 650, 700, 750, 800])
	#nsup_list = np.array([50, 100, 150, 200, 250, 300, 350, 400, 450, 500, 550, 600])
	#nsup_list = np.linspace(100,500,9)

	# create list of experiment configurations	
	for m in m_list:
		for nsup in nsup_list:
			output_csv = "{}{}_{}_{}/nseeds.csv".format(output_path, algo, str(int(m)), str(int(nsup)))
			output_img_path = "{}{}_{}_{}/".format(output_path, algo, str(int(m)), str(int(nsup)))
			if not os.path.exists(output_img_path):
				os.makedirs(output_img_path)
			exp_params = []
			exp_params.append(dataset_path)
			exp_params.append(int(nsup))
			exp_params.append(m)
			exp_params.append(output_img_path)
			exp_params.append(output_csv)	
			
			list_exp_params.append(exp_params)

	# Run experiments	
	results = []
	if nproc == 1:
		for j in range(len(list_exp_params)):
			results.append(run_experiment(list_exp_params[j]) )
	else:
		results = pool.map(run_experiment, list_exp_params)
	
	
if __name__ == '__main__':
    main()