'''
Developed by: Azael de Melo e Sousa
04/07/2019
'''
import shutil
import os
import sys

import SimpleITK as sitk

def main():

	if (len(sys.argv) != 3):
		sys.exit("\nDependencies: iftNormalizeImage and iftInterp\n\nUsage: python %s <dicom dir> <output image>\nNumber of args: %d" % (sys.argv[0],len(sys.argv)))

	dirname = sys.argv[1]

	series_IDs = sitk.ImageSeriesReader_GetGDCMSeriesIDs(dirname)

	print("Converting Image")
	if not series_IDs:
		print('No series in directory ' + '\'' + dirname + '\'')

	series_file_names = sitk.ImageSeriesReader.GetGDCMSeriesFileNames(dirname, series_IDs[0])
	series_reader = sitk.ImageSeriesReader()
	series_reader.SetFileNames(series_file_names)
	series_reader.LoadPrivateTagsOn()
	image3D=series_reader.Execute()
	spacing = image3D.GetSpacing()

	sitk.WriteImage(image3D,sys.argv[2])

	#for series in series_IDs:
	#	sitk.WriteImage(sitk.ReadImage(sitk.ImageSeriesReader_GetGDCMSeriesFileNames(dirname, series)),sys.argv[2])

	print("Done\n")

	create_tmp_file = False
	if (shutil.which("iftNormalizeImage") is None):
		print("iftNormalizeImage not compiled, skiping normalization.")
	else:
		create_tmp_file = True
		cmd = "iftNormalizeImage -i "+sys.argv[2]+" -a 0 -b 4095 -o temp.nii"
		print("Running command: "+cmd)
		if (os.system(cmd) == -1):
			print("Error at iftNormalizeImage. Please compile the program iftNormalizeImage")
			exit(-1)

	if (shutil.which("iftInterp") is None):
		print("iftInterp not compiled, skiping interpolation.")
	else:
		create_tmp_file = True
		minimum = min(spacing)
		cmd = "iftInterp temp.nii VOXEL %f %f %f %s" % (minimum, minimum, minimum, sys.argv[2])
		print("Running command: "+cmd)
		if (os.system(cmd) == -1):
			print("Error at iftInterp. Please compile the program iftInterp")
			exit(-1)	 


	if (create_tmp_file):
		cmd = "rm temp.nii"
		if (os.system(cmd) == -1):
			print("Error removing temp")
			exit(-1)	 

if __name__ == "__main__":
	main()

