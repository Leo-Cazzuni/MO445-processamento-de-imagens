import os
import sys

import SimpleITK as sitk

import pyift.pyift as ift


def main():
    if (len(sys.argv) < 3) or (len(sys.argv) > 4):
        sys.exit("python %s <dicom_dir/dicom_file> <out_image> <OPTIONAL: isotropic voxel size in mm>" % sys.argv[0])

    dicom_dir = sys.argv[1]
    out_img_path = sys.argv[2]

    parent_dir = os.path.dirname(out_img_path)
    if not os.path.exists(parent_dir) and parent_dir:
        os.makedirs(parent_dir)

    supported_extensions = ['.png','.scn','.nii','.nii.gz','.hdr','.hdr.gz']

    is_extension_ok = False

    for ext in supported_extensions:
        if (out_img_path.endswith(ext)):
            is_extension_ok = True
            break

    if (not is_extension_ok):
        sys.exit("ERROR: Output Filename should be one of the following extensions:\n \
                   .png, .scn, .nii, .nii.gz, .hdr, .hdr.gz\n \
                   input image: %s" % out_img_path)

    if os.path.isdir(dicom_dir):
        reader = sitk.ImageSeriesReader()
        series = reader.GetGDCMSeriesFileNames(dicom_dir)
        reader.SetFileNames(series)
    else:
        reader = sitk.ImageFileReader()
        reader.SetFileName(dicom_dir)
    
    img = reader.Execute()  # SimpleITK Image
    sitk.WriteImage(img, "tmp.nii.gz")

    img = ift.ReadImageByExt("tmp.nii.gz")

    if (len(sys.argv) == 4):
        d   = float(sys.argv[3])
        img = ift.Interp(img,img.dx/d,img.dy/d,img.dz/d)

    ift.WriteImageByExt(img, out_img_path)

    # put the checking:
    # if it is 3D and is png, raise an error

    # print("max = %d" % ift.ReadImageByExt(out_img_path).AsNumPy().max())

    os.remove("tmp.nii.gz")


if __name__ == "__main__":
    main()
