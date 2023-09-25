#!/usr/bin/python2.7
import matplotlib
import numpy as np
matplotlib.use('Agg')
import pylab
import csv
import argparse

def main():

	parser = argparse.ArgumentParser()
	parser.add_argument("dataset", type=str, help="Dataset")
	parser.add_argument("res_slic", type=str, help="Results SLIC csv file")
	parser.add_argument("res_isf", type=str, help="Results ISF csv file")
	parser.add_argument("m", type=float, help="Compactness parameter")
	parser.add_argument("alpha_isf", type=float, help="Alpha value")
	parser.add_argument("img_format", type=str, help="Metric", choices=['png', 'pdf'])

	args = parser.parse_args()

	m = args.m
	alpha_isf = args.alpha_isf
	dataset = args.dataset
	res_slic = args.res_slic
	res_isf = args.res_isf
	img_format = args.img_format
	
	legendloc = 4 # bottom right
	
	pylab.xlabel('Number superpixels')
	pylab.ylabel('Accuracy')
	pylab.ylim(0.8,1)
	pylab.xlim(0, 210)

	minvalues = []
	maxvalues = []

	x1 = np.array([10, 20, 40, 100, 200])
	x2 = np.array([10, 20, 40, 100, 200])

	data1 = np.genfromtxt(res_slic, delimiter=',')
	data2 = np.genfromtxt(res_isf, delimiter=',')

	xvalues = np.array([10, 20, 40, 100, 200])
	
	pylab.xticks(xvalues)
	pylab.plot(x1,data1,'-.',c='r',linewidth=1.6) # SLIC
	
	pylab.plot(x2,data2,'--',c='b',linewidth=1.6) # ISF
	
	pylab.legend(['SLIC', 'ISF'],loc=legendloc)

	#pylab.tight_layout()  
	pylab.savefig("plot_supclass_m_{}_aisf{}_{}.{}".format(m, alpha_isf, dataset, img_format))

if __name__ == '__main__':
	main()