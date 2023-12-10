import os
import sys

a = [x for x in os.listdir() if 'filtered_r' in x]

for pasta in a:
	for file in os.listdir(pasta):
		if file not in ['000030.png','000046.png','000052.png','000058.png','000070.png']:
			os.system(f'rm {pasta}/{file}')