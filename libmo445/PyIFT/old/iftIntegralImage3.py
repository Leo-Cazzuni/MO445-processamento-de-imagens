import pyift as ift
import numpy as np
from sys import argv

if len(argv) != 7:
	ift.Error("Usage: iftIntegralImage <input-image.[png,pgm]> <width> <height> <deltaX> <deltaY> <output-image.[png,pgm]>", "main")

img 		   = ift.ReadImageByExt(argv[1])
integral_image = ift.IntegralImage(img)
cum_image	   = ift.CreateFImage(integral_image.shape[3], integral_image.shape[2], integral_image.shape[1])
Wx = round(float(argv[2]) / 2)
Wy = round(float(argv[3]) / 2)
Dx = int(argv[4])
Dy = int(argv[5])

pt = np.zeros([4, 3], dtype = int)
# for p in range(img.size):
for p in range(img.size):
	u = ift.GetVoxelCoord(img, p)
	pt[0, 0] = u[0] - Wx + Dx
	pt[0, 1] = u[1] - Wy + Dy
	pt[1, 0] = u[0] + Wx + Dx
	pt[1, 1] = u[1] - Wy + Dy
	pt[2, 0] = u[0] - Wx + Dx
	pt[2, 1] = u[1] + Wy + Dy
	pt[3, 0] = u[0] + Wx + Dx
	pt[3, 1] = u[1] + Wy + Dy
	if (ift.ValidVoxel(img, pt[0]) and 
		ift.ValidVoxel(img, pt[1]) and 
		ift.ValidVoxel(img, pt[2]) and 
		ift.ValidVoxel(img, pt[3])):
		x = p % integral_image.shape[3]
		y = p // integral_image.shape[3]
		cum_image[0, 0, y, x] = ift.GetIntegralValueInRegion(integral_image, pt, 4);

img = ift.FIImageToImage(cum_image, 65535)
iftWriteImageByExt(img, argv[6]);

hist = iftGrayHistogram(img, iftMaximumValue(img), 0)
ift.WriteHistogram(hist, "integral_hist.txt")

