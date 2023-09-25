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
	parser.add_argument("metric", type=str, help="Metric", choices=['br', 'comp', 'top', 'ue'])
	parser.add_argument("res_slic", type=str, help="Results SLIC csv file")
	parser.add_argument("res_sib", type=str, help="Results SIB csv file")
	parser.add_argument("res_isf", type=str, help="Results ISF csv file")
	parser.add_argument("res_lrw", type=str, help="Results LRW csv file")
	parser.add_argument("res_fastsup", type=str, help="Results FASTSUP csv file")
	parser.add_argument("m", type=float, help="Compactness parameter")
	parser.add_argument("alpha_sib", type=float, help="Alpha value")
	parser.add_argument("alpha_isf", type=float, help="Alpha value")
	parser.add_argument("lazy_param", type=float, help="Threshold split value")
	parser.add_argument("rad_param", type=float, help="Radius parameter")
	parser.add_argument("img_format", type=str, help="Metric", choices=['png', 'pdf'])

	args = parser.parse_args()

	m = args.m
	alpha_sib = args.alpha_sib
	alpha_isf = args.alpha_isf
	lazy_param = args.lazy_param
	rad_param = args.rad_param
	dataset = args.dataset
	res_slic = args.res_slic
	res_sib = args.res_sib
	res_isf = args.res_isf
	res_lrw = args.res_lrw
	res_fastsup = args.res_fastsup
	metric = args.metric
	img_format = args.img_format

	dictmetric = {"br" : 0, "comp" : 2, "top" : 4, "ue" : 6}
	dictname = {"br" : "Boundary Recall", "comp" : "Compactness", "top" : "Topology", "ue" : "Undersegmentation Error"}
	indexmetric = dictmetric[metric]
	metricname = dictname[metric]

	legendloc = 4 # bottom right
	if metric == "top" or metric == "ue":
		legendloc = 1 # top right

	pylab.xlabel('Number superpixels')
	pylab.ylabel(metricname)
	#pylab.ylim(0.6,0.98)
	pylab.xlim(100,500)
	
	#arr_m = [ 24,  22,  20,  18,  16,  14,  12,  10.,   8,   6]
	arr_m = [ 22,  20,  18,  16,  14,  12,  10.,   8,   6]
	#arr_alpha = [ 0.02,  0.04,  0.06,  0.08,  0.1 ,  0.12,  0.14,  0.16,  0.18,  0.2 ]
	arr_alpha = [ 0.04,  0.06,  0.08,  0.1 ,  0.12,  0.14,  0.16,  0.18,  0.2 ]
	arr_lazy_param = [ 0.99, 0.999, 0.9999, 0.99999, 0.999999 ]

	arr_rad_param = [ 3.0 ]
	#m = 18
	index_m = arr_m.index(m)
	#alpha_sib = 0.08
	index_alpha_sib = arr_alpha.index(alpha_sib)
	#alpha_isf = 0.1
	index_alpha_isf = arr_alpha.index(alpha_isf)
	# lazy param = 0.99 default
	index_lazy_param = arr_lazy_param.index(lazy_param)

	index_rad_param = arr_rad_param.index(rad_param)
	
	nsupvalues = 9
	nparvalues = 9
	nparvalues_lrw = 1
	nparvalues_rad = 1

	minvalues = []
	maxvalues = []

	x1 = np.zeros(nsupvalues)
	x2 = np.zeros(nsupvalues)
	x3 = np.zeros(nsupvalues)
	x4 = np.zeros(nsupvalues)
	x5 = np.zeros(nsupvalues)
	
	# read data
	data1 = np.genfromtxt(res_slic, delimiter=',')
	newdata1 = np.zeros(nsupvalues)
	for j in range(nsupvalues):
		newdata1[j] = data1[(j+(index_m*nsupvalues)),indexmetric]
		x1[j] = data1[j,-1]
	data1 = newdata1
	print data1.shape
	minvalues.append(min(data1))
	maxvalues.append(max(data1))
	
	
	data2 = np.genfromtxt(res_sib, delimiter=',')
	newdata2 = np.zeros(nsupvalues)
	for j in range(nsupvalues):
		newdata2[j] = data2[(j+(index_alpha_sib*nsupvalues)),indexmetric]
		x2[j] = data2[j,-1]
	data2 = newdata2
	print data2.shape
	minvalues.append(min(data2))
	maxvalues.append(max(data2))
	

	data3 = np.genfromtxt(res_isf, delimiter=',')
	newdata3 = np.zeros(nsupvalues)
	for j in range(nsupvalues):
		newdata3[j] = data3[(j+(index_alpha_isf*nsupvalues)),indexmetric]
		x3[j] = data3[j,-1]
	data3 = newdata3
	print data3.shape
	minvalues.append(min(data3))
	maxvalues.append(max(data3))

	data4 = np.genfromtxt(res_lrw, delimiter=',')
	newdata4 = np.zeros(nsupvalues)
	for j in range(nsupvalues):
		newdata4[j] = data4[(j+(index_lazy_param*nsupvalues)),indexmetric]
		x4[j] = data4[j,-1]
	data4 = newdata4
	print data4.shape
	minvalues.append(min(data4))
	maxvalues.append(max(data4))	

	data5 = np.genfromtxt(res_fastsup, delimiter=',')
	newdata5 = np.zeros(nsupvalues)
	for j in range(nsupvalues):
		newdata5[j] = data5[(j+(index_rad_param*nsupvalues)),indexmetric]
		x5[j] = data5[j,-1]
	data5 = newdata5
	print data5.shape
	minvalues.append(min(data5))
	maxvalues.append(max(data5))
	
	# ranges
	#x1 = np.linspace(100,1000,10)
	#x1 = np.linspace(100,500,9)
	print x1
	
	#x2 = np.linspace(100,1000,10)
	#x2 = np.linspace(100,500,9)
	print x2

	#x3 = np.linspace(100,1000,10)
	#x3 = np.linspace(100,500,9)
	print x3

	#x4 = np.linspace(100,1000,10)
	#x4 = np.linspace(100,500,9)
	print x4

	print x5
	
	#xvalues = np.linspace(100,1000,10)
	xvalues = np.linspace(100,500,9)
	#xvalues = np.append(xvalues, [550])
	
	# config
	pylab.ylim(min(minvalues),max(maxvalues))
	if lazy_param == 0.999999 and metric=="br" and dataset=="grab":
		pylab.ylim(0.6,max(maxvalues))
	if lazy_param == 0.999999 and metric=="br" and dataset=="berk":
		pylab.ylim(0.5,max(maxvalues))
	
	pylab.xticks(xvalues)
	pylab.plot(x1,data1,'-.',c='r',linewidth=1.6) # SLIC

	pylab.plot(x4,data4,'-',c='k',linewidth=1.6) # LRW
	
	pylab.plot(x2,data2,'-',c='g',linewidth=1.6) # SIB
	
	pylab.plot(x3,data3,'--',c='b',linewidth=1.6) # ISF

	pylab.plot(x5,data5,'-',c='c',linewidth=1.6) # FASTSUP
	
	#pylab.legend(['SLIC (m={})'.format(arr_m[index_m]), 'SIB (alpha={})'.format(arr_alpha[index_alpha_sib]), 'ISF (alpha={})'.format(arr_alpha[index_alpha_isf]), 'LRW (alpha={})'.format(arr_lazy_param[index_lazy_param]) ],loc=legendloc)
	#pylab.legend(['SLIC'.format(arr_m[index_m]), 'LRW'.format(arr_lazy_param[index_lazy_param]), 'SIB'.format(arr_alpha[index_alpha_sib]), 'ISF'.format(arr_alpha[index_alpha_isf]) ],loc=legendloc)
	#pylab.legend(['SLIC'.format(arr_m[index_m]), 'LRW'.format(arr_lazy_param[index_lazy_param]), 'ISF1-MIX'.format(arr_alpha[index_alpha_sib]), 'ISF2-MIX'.format(arr_alpha[index_alpha_isf]) ],loc=legendloc)
	pylab.legend(['SLIC'.format(arr_m[index_m]), 'LRW'.format(arr_lazy_param[index_lazy_param]), 'ISF1'.format(arr_alpha[index_alpha_sib]), 'ISF2'.format(arr_alpha[index_alpha_isf]), 'FASTSUP'.format(arr_rad_param[index_rad_param]) ],loc=legendloc)

	#pylab.tight_layout()  
	pylab.savefig("plot_{}_m_{}_lrw{}_asib{}_aisf{}_rad{}_{}.{}".format(metric,arr_m[index_m], arr_lazy_param[index_lazy_param], arr_alpha[index_alpha_sib], arr_alpha[index_alpha_isf], arr_rad_param[index_rad_param], dataset, img_format))

if __name__ == '__main__':
	main()
