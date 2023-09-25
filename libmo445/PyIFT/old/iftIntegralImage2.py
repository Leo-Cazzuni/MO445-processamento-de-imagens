import pyift as ift
import numpy as np
from sys import argv

# from pympler import summary, muppy

# all_objects = muppy.get_objects()
# sum1 = summary.summarize(all_objects)
# summary.print_(sum1)

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

for p in range(img.shape[3] * img.shape[2]):
	u = np.array([p % img.shape[3], p //img.shape[3], 0])
	pt[0, 0] = u[0] - Wx + Dx
	pt[0, 1] = u[1] - Wy + Dy
	pt[1, 0] = u[0] + Wx + Dx
	pt[1, 1] = u[1] - Wy + Dy
	pt[2, 0] = u[0] - Wx + Dx
	pt[2, 1] = u[1] + Wy + Dy
	pt[3, 0] = u[0] + Wx + Dx
	pt[3, 1] = u[1] + Wy + Dy
	# print("Array of Python")
	# print(pt)
	if (ift.ValidVoxel(img, pt[0]) and 
		ift.ValidVoxel(img, pt[1]) and 	
		ift.ValidVoxel(img, pt[2]) and 
		ift.ValidVoxel(img, pt[3])):
		x = p % integral_image.shape[3]
		y = p // integral_image.shape[3]
		cum_image[0, 0, y, x] = ift.GetIntegralValueInRegion(integral_image, pt, 4);
	# if (p % 1000 == 0):
	# 	all_objects = muppy.get_objects()
	# 	sum2 = summary.summarize(all_objects)
	# 	summary.print_(sum2)

img = ift.FImageToImage(cum_image, 65535)
ift.WriteImageByExt(img, argv[6]);
hist = ift.GrayHistogram(img, ift.MaximumValue(img), False)
ift.WriteHistogram(hist, "integral_hist.txt")
