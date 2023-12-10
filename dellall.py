import os
import sys

for dir_name in ['bag', 'layer2', 'layer1', 'layer4', 'layer3', 'salie', 'flim', 'objs', 'boxes', 'layer0']:
	if os.path.exists(dir_name):
		os.system(f"rm -r {dir_name}")
