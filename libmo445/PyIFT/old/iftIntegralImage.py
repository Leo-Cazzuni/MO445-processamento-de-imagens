import pyift as ift
import numpy as np
from sys import argv

if len(argv) != 7:
	ift.Error("Usage: iftIntegralImage <input-image.[png,pgm]> <width> <height> <deltaX> <deltaY> <output-image.[png,pgm]>", "main")

img 		   = ift.ReadImageByExt(argv[1])
integral_image = ift.IntegralImage(img)
cum_image	   = ift.CreateFImage(integral_image.shape[0], integral_image.shape[1], integral_image.shape[2])
Wx = round(float(argv[2]) / 2)
Wy = round(float(argv[3]) / 2)
Dx = int(argv[4])
Dy = int(argv[5])

pt = []
for i in range(4):
	v = ift._voxel(0, 0, 0)
	pt.append(v)

u = ift._voxel(0, 0, 0)
v = ift._voxel(0, 0, 0)
# index 0 = x, 1 = y, 2 = z;

for p in range(img.size):
	u = ift.GetVoxelCoord(img, p)
	

