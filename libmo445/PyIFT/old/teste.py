import pyift as ift
import gc

print(gc.get_stats())

img = ift.ReadImageByExt("Lenna.png")
integral_image = ift.IntegralImage(img)

pt = []
for i in range(4):
	pt.append(ift.VoxelNew())

for i in pt:
	print(ift.VoxelGetX(i))

print(ift.GetIntegralValueInRegion(integral_image, pt, len(pt)))
print(gc.get_stats())