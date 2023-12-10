import os
import sys

if (len(sys.argv) != 7):
    print("python exec <P1> <P2>")
    print("P1: number of layers (if negative, do not encode layers again)")
    print("P2: layer for the results")
    print("P3: points per marker? (1, 2, ...)")
    print("P4: model_type (0, 1, 2)")
    print("P5: preproc rad (if negative skips preproc)")
    print("P6: results dir")
    exit()

nlayers      = int(sys.argv[1])
target_layer = int(sys.argv[2])
npts_per_marker = int(sys.argv[3])
model_type   = int(sys.argv[4])
median_adj_rad = float(sys.argv[5])
result_path = sys.argv[6]

os.system('python3 dellall.py')
if os.path.exists(f'{result_path}'):
    os.system(f'rm -r {result_path}')
os.system(f'mkdir {result_path}')

if median_adj_rad>0:
    os.system(f"rm -r filtered")
    print('preproc')
    os.system(f"preproc images {median_adj_rad} filtered")

print('bag_of_feature_points')
line = f"bag_of_feature_points filtered markers {npts_per_marker} {result_path}/bag"
os.system(line)

for layer in range(1,nlayers+1):
    print('create_layer_model')
    line = f"create_layer_model {result_path}/bag arch.json {layer} {result_path}/flim"
    os.system(line)
    if(model_type == 1):
        print('merge_layer_models')
        line = f"merge_layer_models arch.json {layer} {result_path}/flim"
        os.system(line)
        print('encode_merged_layer')
        line = f"encode_merged_layer arch.json {layer} {result_path}/flim"
        os.system(line)
    else:
        print('encode_layer')
        line = f"encode_layer arch.json {layer} {result_path}/flim"
        os.system(line)
    print('decode_layer')
    line = f"decode_layer {layer} arch.json {result_path}/flim {model_type} {result_path}/salie"
    os.system(line)

print('decode_layer')
line = f"decode_layer {target_layer} arch.json {result_path}/flim {model_type} {result_path}/salie"
os.system(line)
print('detection')
line = f"detection {result_path}/salie {target_layer} {result_path}/boxes"
os.system(line)
print('delineation')
line = f"delineation {result_path}/salie {target_layer} {result_path}/objs 1"
os.system(line)
        
