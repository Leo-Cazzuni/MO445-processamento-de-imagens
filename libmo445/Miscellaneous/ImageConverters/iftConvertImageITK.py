import os
import sys

import SimpleITK as sitk

import pyift.pyift as ift


def main():
    if (len(sys.argv) < 3) or (len(sys.argv) > 4):
        sys.exit("python %s <ift input_image> <sitk output_image>" % sys.argv[0])

    output_image_path = sys.argv[2]

    img = ift.ReadImageByExt(sys.argv[1])

    img_np = img.AsNumPy()
    img_np[img_np == 3] = 0

    ift.WriteImageByExt(img,"temp.nii")




    reader = sitk.ImageFileReader()
    reader.SetImageIO("NiftiImageIO")
    reader.SetFileName("temp.nii")
    image = reader.Execute();

    writer = sitk.ImageFileWriter()
    writer.SetFileName(output_image_path)
    writer.Execute(image)

    os.remove("temp.nii")

'''
    writer = sitk.ImageFileWriter()
    writer.SetFileName(output_image_path)
    writer.Execute(nii)
'''

if __name__ == "__main__":
    main()