import os
import sys

for rad in ['0.1', '0.25', '0.5', '1', '1.5', '2', '5','10']:
	os.system(f"preproc images {rad} \"filtered_r={rad}\"")
