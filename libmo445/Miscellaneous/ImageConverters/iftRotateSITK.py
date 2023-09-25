
'''
Developed by: Azael de Melo e Sousa
22/03/2019
'''
import os
import sys
import numpy as np

import SimpleITK as sitk


def resample(image, transform):
    reference_image = image
    interpolator = sitk.sitkCosineWindowedSinc
    default_value = 100.0
    return sitk.Resample(image, reference_image, transform,
                         interpolator, default_value)


def main():
    if (len(sys.argv) != 3):
        sys.exit("python %s <sitk input_image> <sitk output_image>" % sys.argv[0])

    output_image_path = sys.argv[2]

    reader = sitk.ImageFileReader()
    reader.SetImageIO("MetaImageIO")
    reader.SetFileName(sys.argv[1])
    image = reader.Execute();

    origin = image.GetOrigin()    
    size = image.GetSize()
    image.SetOrigin((0,0,0))
    transform = sitk.VersorTransform((0,1,0), np.pi)    
    transform.SetCenter(image.TransformContinuousIndexToPhysicalPoint((size[0]/2,size[1]/2,size[2]/2)))
    outimage=sitk.Resample(image,size,transform,sitk.sitkLinear,[0,0,0], image.GetSpacing(), image.GetDirection())
    outimage.SetOrigin(origin)

    writer = sitk.ImageFileWriter()
    writer.SetImageIO("MetaImageIO")
    writer.SetFileName(output_image_path)
    writer.UseCompressionOff()    
    writer.Execute(outimage)

'''
    degrees = 180
    affine = sitk.AffineTransform(3)
    radians = np.pi * degrees / 180.
    affine.Rotate(axis1=0, axis2=1, angle=radians)
    outimage = resample(image, affine)
'''
    

'''
    writer = sitk.ImageFileWriter()
    writer.SetFileName(output_image_path)
    writer.Execute(nii)
'''

if __name__ == "__main__":
    main()
