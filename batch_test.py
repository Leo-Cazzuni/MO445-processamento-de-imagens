import os
import sys

for var in range(1,5):
	os.system(f'python3 exec.py {var} {var} 1 0 -1 encoder_layers={var}')