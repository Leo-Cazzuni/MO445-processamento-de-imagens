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
	args = ["iftDatasetSuperpixelMetrics"]
	args.extend(str_exp_params)
	print args
	call(args)
	output_csv = str_exp_params[-1]
	metrics = np.genfromtxt(output_csv, delimiter=',').astype(np.float)
	return metrics

def main():
	parser = argparse.ArgumentParser()
	parser.add_argument("superpixels_path", type=str, help="Superpixel images path")
	parser.add_argument("algo", type=str, help="Superpixel algorithm")
	parser.add_argument("border_path", type=str, help="Border path")
	parser.add_argument("label_path", type=str, help="Label path")	
	parser.add_argument("nproc", type=int, help="Number of processes")
	parser.add_argument("output_resume", type=str, help="Output resume")
	args = parser.parse_args()	
	
	superpixels_path = args.superpixels_path
	border_path = args.border_path
	label_path = args.label_path
	algo = args.algo
	nproc = args.nproc
	output_resume = args.output_resume
	ncols_result = 10
	
	pool = Pool(processes=nproc)

	list_exp_params = []

	#sup_param_list = np.linspace(0.02,0.2,10)	
	sup_param_list = np.linspace(0.04,0.2,9)
	if algo == "slic" or algo == "ourslic" or algo == "slic2":
		sup_param_list = np.linspace(6,22,9)
		sup_param_list = sup_param_list[::-1]
	elif algo == "fastsup":
		rad_param_list = [ 3.0 ]
		sup_param_list = np.array(rad_param_list)
	elif algo == "lrw":
		lazy_param_list = [ 0.99, 0.999, 0.9999, 0.99999, 0.999999 ]
		sup_param_list = np.array(lazy_param_list)
	elif algo == "vcells":
		sup_param_list = np.linspace(1,11,6)
	#nsup_list = np.linspace(100,1000,10)
	nsup_list = np.linspace(100,500,9)

	# create list of experiment configurations 
	for sup_param in sup_param_list:
		for nsup in nsup_list:
			if algo == "slic" or algo == "ourslic" or algo == "slic2" or algo == "fastsup":
				sup_param = int(sup_param)
			output_csv_path = "{}{}_{}_{}/metrics.csv".format(superpixels_path, algo, str(sup_param), str(int(nsup)))
			sup_img_path = "{}{}_{}_{}/".format(superpixels_path, algo, str(sup_param), str(int(nsup)))			
			exp_params = []
			exp_params.append(sup_img_path)
			exp_params.append(border_path)
			exp_params.append(label_path)
			exp_params.append(output_csv_path)	
			
			list_exp_params.append(exp_params)

	
	# Retrieve nseeds results
	list_nseeds_results = []
	for sup_param in sup_param_list:
		for nsup in nsup_list:
			if algo == "slic" or algo == "ourslic" or algo == "slic2" or algo == "fastsup":
				sup_param = int(sup_param)
			nseeds_csv_path = "{}{}_{}_{}/nseeds.csv".format(superpixels_path, algo, str(sup_param), str(int(nsup)))
			nseeds_results = np.genfromtxt(nseeds_csv_path, delimiter=',')
			list_nseeds_results.append(nseeds_results)

	# Run experiments	
	results = []
	if nproc == 1:
		for j in range(len(list_exp_params)):
			results.append(run_experiment(list_exp_params[j]) )
	else:
		results = pool.map(run_experiment, list_exp_params)
	

	results_arr = np.zeros((len(list_exp_params), ncols_result))
	for j in range(len(list_exp_params)):
		# Convert the results in standard format for plotting
		all_metrics_one_config = results[j]
		results_format = np.zeros((1, ncols_result))
		# -2 and -1 rows contain the mean and stdev results
		results_format[0, 0] = all_metrics_one_config[-2, 0]
		results_format[0, 1] = all_metrics_one_config[-1, 0]
		results_format[0, 2] = all_metrics_one_config[-2, 1]
		results_format[0, 3] = all_metrics_one_config[-1, 1]
		results_format[0, 4] = all_metrics_one_config[-2, 2]
		results_format[0, 5] = all_metrics_one_config[-1, 2]
		results_format[0, 6] = all_metrics_one_config[-2, 3]
		results_format[0, 7] = all_metrics_one_config[-1, 3]

		nseeds_results = list_nseeds_results[j]
		# -1 row contain the mean results
		results_format[0, 8] = nseeds_results[-1, 0]
		results_format[0, 9] = nseeds_results[-1, 1]

		results_arr[j,:] = results_format
	
	# write results
	if output_resume:
		np.savetxt(output_resume, results_arr, fmt = '%.6f', delimiter = ',')
	
if __name__ == '__main__':
	main()
