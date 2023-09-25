import os
import pdb
import sys

import numpy as np
import nibabel as nib
import scn


def read_tag_file(tag_path):
    f = open(tag_path)

    line = f.readline()
    while not "Points" in line:
        line = f.readline()

    tag = []

    # get real world coordinates
    for line in f:
        x, y, z = map(float, line.split("\n")[0].split("\r")[0].split(" ")[1:4])

        tag.append([x,y,z])

    f.close()

    return np.array(tag)


def convert_mnc_2_scn(mnc):
    out_scn = scn.Scene(val=mnc.get_data())
    out_scn.dx, out_scn.dy, out_scn.dz = mnc.get_header().get_zooms()

    return out_scn


def convert_tag_2_scn(tag, mnc):
    # creates a scn with a matriz of zeros with the same domain of mnc
    out_label_scn = scn.Scene(val=np.zeros(mnc.get_data().shape))
    out_label_scn.dx, out_label_scn.dy, out_label_scn.dz = mnc.get_header().get_zooms()

    # gets the inverse of the affine matriz from the image to convert
    # the real world coordinates (referece space) to the voxel coordinate space
    Ainv = np.linalg.inv(mnc.affine)

    for x, y, z in tag:
        i, j, k, _ = map(int, np.dot(Ainv, [x, y, z, 1])) # ignore the resulting homogeneous coordinates
        out_label_scn.val[i,j,k] = 1

    return out_label_scn




def main():
    if len(sys.argv) != 4:
        sys.exit("python %s <image.mnc> <labels.tag> <out_image_basename>" % __file__)


    mnc_path     = sys.argv[1]
    tag_path     = sys.argv[2]
    out_basename = sys.argv[3]

    print("-----------------------");
    print("- Minc Image: %s" % mnc_path)
    print("- Tag Image: %s" % tag_path)
    print("- Output Image Basename: %s" % out_basename)
    print("-----------------------\n");


    if not os.path.exists(os.path.dirname(out_basename)):
        os.makedirs(os.path.dirname(out_basename))

    print("- Loading Minc Image")
    mnc = nib.load(mnc_path)
    print("- Converting Minc to Scene")
    out_scn = convert_mnc_2_scn(mnc)
    print("- Writing Scene\n")
    out_scn.write(out_basename + ".scn")
    
    print("- Loading Tag File")
    tag = read_tag_file(tag_path)
    print("- Converting Tag to Label Scene")
    out_label_scn = convert_tag_2_scn(tag, mnc)
    print("- Writing Label Scene\n")
    out_label_scn.write(out_basename + "_label.scn")



    print("Done...")


if __name__ == "__main__":
    main()

