
'''
Developed by: Azael de Melo e Sousa
22/03/2019
'''

import os
import sys

import SimpleITK as sitk

import pyift.pyift as ift


def main():
    if (len(sys.argv) < 3) or (len(sys.argv) > 4):
        sys.exit("python %s <itk input_image> <sitk output_image>" % sys.argv[0])

    output_image_path = sys.argv[2]

    reader = sitk.ImageFileReader()
    #reader.SetImageIO("NiftiImageIO")
    reader.SetFileName(sys.argv[1])
    image = reader.Execute();

    writer = sitk.ImageFileWriter()
    writer.SetFileName("temp.nii")
    writer.UseCompressionOn()
    writer.Execute(image)




    img = ift.ReadImageByExt("temp.nii")

    img_np = img.AsNumPy()
    img_np[img_np == 3] = 0

    ift.WriteImageByExt(img,output_image_path)

    os.remove("temp.nii")

'''
    writer = sitk.ImageFileWriter()
    writer.SetFileName(output_image_path)
    writer.Execute(nii)
'''

if __name__ == "__main__":
    main()
