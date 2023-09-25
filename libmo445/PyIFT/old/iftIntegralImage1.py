import pyift as ift
import numpy as np
from sys import argv

if len(argv) != 7:
	ift.Error("Usage: iftIntegralImage <input-image.[png,pgm]> <width> <height> <deltaX> <deltaY> <output-image.[png,pgm]>", "main")

img 		   = ift.ReadImageByExt(argv[1])
integral_image = ift.IntegralImage(img)
cum_image	   = ift.CreateFImage(integral_image.shape[3], integral_image.shape[3], integral_image.shape[1])
Wx = round(float(argv[2]) / 2)
Wy = round(float(argv[3]) / 2)
Dx = int(argv[4])
Dy = int(argv[5])

pt = []
for i in range(4):
	pt.append(ift.VoxelNew())

v = ift.VoxelNew()

# for p in range(img.size):
for p in range(img.size):
	u = ift.GetVoxelCoord(img, p)
	ift.VoxelSetX(pt[0], ift.VoxelGetX(u) - Wx + Dx)
	ift.VoxelSetY(pt[0], ift.VoxelGetY(u) - Wy + Dy)
	ift.VoxelSetX(pt[1], ift.VoxelGetX(u) + Wx + Dx)
	ift.VoxelSetY(pt[1], ift.VoxelGetY(u) - Wy + Dy)
	ift.VoxelSetX(pt[2], ift.VoxelGetX(u) - Wx + Dx)
	ift.VoxelSetY(pt[2], ift.VoxelGetY(u) + Wy + Dy)
	ift.VoxelSetX(pt[3], ift.VoxelGetX(u) + Wx + Dx)
	ift.VoxelSetY(pt[3], ift.VoxelGetY(u) + Wy + Dy)
	if (ift.ValidVoxel(img, pt[0]) and 
		ift.ValidVoxel(img, pt[1]) and 
		ift.ValidVoxel(img, pt[2]) and 
		ift.ValidVoxel(img, pt[3])):
		x = p % integral_image.shape[3]
		y = p // integral_image.shape[3]
		cum_image[0, 0, y, x] = ift.GetIntegralValueInRegion(integral_image, pt, 4);
	# ift.VoxelDelete(u)
print(gc.get_stats())
print(":(")
img = ift.FIImageToImage(cum_image, 65535)
iftWriteImageByExt(img, argv[6]);

hist = iftGrayHistogram(img, iftMaximumValue(img), 0)
ift.WriteHistogram(hist, "integral_hist.txt")

# ift.VoxelDelete(u)
# ift.VoxelDelete(v)
# for i  in range(len(pt)):
# 	ift.VoxelDelete(pt[i])
