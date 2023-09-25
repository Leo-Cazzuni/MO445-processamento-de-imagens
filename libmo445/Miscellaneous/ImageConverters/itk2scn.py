import csv
import numpy as np
import os
import pdb
import sys

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

    return scn.Scene(dx=dx, dy=dy, dz=dz, val=np_img, copy=False)





def main():
    if len(sys.argv) != 3:
        sys.exit("python itk2scn.py <input_image.[mha | csv]> <out_image.scn | out_dir>")

    input_path = sys.argv[1]
    out_path   = sys.argv[2]

    print("- Input: %s" % input_path)
    print("- Output: %s\n" % out_path)

    # converting a single vtk image
    if input_path.endswith(".mha"):
        if not out_path.endswith(".scn"):
            sys.exit("ERROR\nOutput is not *.scn")
        
        print("- Converting VTK to SCN")
        out_img = itk2scn(input_path)
        print("- Writing Scn Image")
        out_img.write(out_path)
    # converting a list vtk images
    elif input_path.endswith(".csv"):
        img_set = []
        with open(input_path) as f:
            reader = csv.reader(f)
            for row in reader:
                for path in row:
                    if path.endswith(".mha"):
                        img_set.append(path)

        for img_path in img_set:
            base         = os.path.splitext(os.path.basename(img_path))[0]
            out_img_path = os.path.join(out_path, base + ".scn")
            
            print("- Converting VTK to SCN")
            print(img_path)
            out_img = itk2scn(img_path)

            print("\n- Writing Scn Image")
            print(out_img_path)
            out_img.write(out_img_path)

    else:
        sys.exit("ERROR\nInput should be *.mha or *.csv")

if __name__ == "__main__":
    sys.exit(main())
