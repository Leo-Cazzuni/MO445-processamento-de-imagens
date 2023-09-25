import numpy as np
import os
import pdb

import scn

import SimpleITK as sitk

# http://insightsoftwareconsortium.github.io/SimpleITK-Notebooks/


def itk2scn(filename):
    filename = os.path.expanduser(filename)

    itk_img = sitk.ReadImage(filename)
    np_img  = sitk.GetArrayFromImage(itk_img)

    ndims = itk_img.GetDimension()

    if ndims == 3: # 3D
        xsize, ysize, zsize = itk_img.GetSize()
        np_img.reshape((xsize, ysize, zsize))
        
        dx, dy, dz          = itk_img.GetSpacing()
    else: # 2D
        xsize, ysize = itk_img.GetSize()
        np_img.reshape((xsize, ysize))
        
        dx, dy = itk_img.GetSpacing()
        dz     = 0.0

    return scn.Scene(dx, dy, val=np_img, copy=False)










