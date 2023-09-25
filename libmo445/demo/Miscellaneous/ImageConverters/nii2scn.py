import csv
import os
import pdb
import sys

import numpy as np

import SimpleITK as sitk


class Error(object):
    @staticmethod
    def print_error(error_msg, function):
        sys.exit("\nERROR in function {0}\n{1}".format(function, error_msg))



# We follow the Image Coordinate Convetions from Skimage
# http://scikit-image.org/docs/dev/user_guide/numpy_images.html

class Scene(object):
    def __init__(self, dx=0, dy=0, dz=0, val=[]):
        self.dx    = dx
        self.dy    = dy
        self.dz    = dz
        # 3D matrix (z, y, x) = (plane, row, col) = (z, y, x)
        self.val = val.astype("int") # create a copy with int64 values


    # it's called 1) The task is well-knouint8wn and well-established in the field. Several products are even existing for that purpose. In such a scenario, i would expect a significant technical contribution or showing clinical practicability via strong evaluation.with "print obj_instance"
    def __str__(self):
        msg = "dx = %f, dy = %f, dz = %f\n" %(self.dx, self.dy, self.dz)
        msg += "xsize = %d, ysize = %d, zsize = %d\n" %(self.val.shape[2], self.val.shape[1],
                                                        self.val.shape[0])
        msg += "length = %d\n" %(self.val.shape[0]*self.val.shape[1]*self.val.shape[2])
        msg += "{0}".format(self.val)
        return msg


    # it's called in the interactive terminal with obj_instance and you press the ENTER key
    def __repr__(self):
        msg = "dx = %f, dy = %f, dz = %f\n" %(self.dx, self.dy, self.dz)
        msg += "xsize = %d, ysize = %d, zsize = %d\n" %(self.val.shape[2], self.val.shape[1],
                                                        self.val.shape[0])
        msg += "length = %d\n" %(self.val.shape[0]*self.val.shape[1]*self.val.shape[2])
        msg += "{0}".format(self.val)
        return msg


    def write(self, filename):
        write(self, filename)



def read(filename): # class is a object not instanced from the Class
    """Read a Scene image from a filename.
    It is a class method, then it has a reference to its own class (Scene).

    Parameters
    ----------
    cls:
        Reference to its own class.

    filename: string
        Filename of the image to be read.
    """
    fp = open(filename, "r")

    img_type = fp.readline().split("\n")[0] # read the first line and delete the \n

    if (img_type != "SCN"):
        Error.print_error("Invalid image: it is not a SCN", "Scene.read_scene")
    else:
        ######## read sizes #######
        sizes = fp.readline().split("\n")[0] # e.g: "'10' '20' '30'
        sizes = sizes.split() # ['10', '20', '30']

        if len(sizes) != 3:
            Error.print_error("Number of sizes is different than 3", "Scene.read_scene")

        xsize, ysize, zsize = map(int, sizes) # xsize = 10, ysize = 20, zsize = 30 
        length = xsize*ysize*zsize

        ######## read displacements #######
        disps = fp.readline().split("\n")[0] # e.g: "'1.25' '1.25' '1.25'
        disps = disps.split() # ['1.25', '1.25', '1.25']

        if len(disps) != 3:
            Error.print_error("Number of displacements is different than 3", "Scene.read_scene")

        dx, dy, dz = map(np.float32, disps) # xsize = 1.25, ysize = 1.25, zsize = 1.25
         
        ######## read depth #######
        depth = fp.readline().split("\n")[0]
        depth = int(depth)
        
        ######## read the voxels #######
        if (depth == 8):
            val = np.fromfile(fp, dtype=np.uint8)
            if (len(val) != length):
                Error.print_error("Problems to read unsigned char voxel values", "Scene.read_scene")
        elif (depth == 16):
            val = np.fromfile(fp, dtype=np.uint16)
            if (len(val) != length):
                Error.print_error("Problems to read unsigned short voxel values", "Scene.read_scene")
        elif (depth == 32):
            val = np.fromfile(fp, dtype=np.int32)
            if (len(val) != length):
                Error.print_error("Problems to read int voxel values", "Scene.read_scene")
        else:
            Error.print_error("Input scene must be 8, 16, or 32 bit", "Scene.read_scene")

    fp.close()

    # reshape an 1D-array to 3D-array
    val = np.reshape(val, (zsize, ysize, xsize)) # coodinates: (z, y, x) = (plane, row, col) 
    img = Scene(dx, dy, dz, val)

    return img


def write(img, filename):
    """Write a Scene image.

    Parameters
    ----------
    img: Scene
        Scene Image to be saved.
    filename: string
        Filename of the image to be written.
    """
    if (img.val.min() < 0):
        print("Warning: Shifting image values from [%d,%d] to [%d,%d] on " \
            "the original image\n" % (img.val.min(), img.val.max(), 0, img.val.max()-img.val.min()))
        img.val -= img.val.min() 

    fp = open(filename, "w")
    ####### write the header ########
    fp.write("SCN\n")
    fp.write("%d %d %d\n" % (img.val.shape[2], img.val.shape[1], img.val.shape[0]))
    fp.write("%f %f %f\n" % (img.dx, img.dy, img.dz))

    ####### write the voxel values ########
    # reshape the 3D-array to 1D-array
    zsize, ysize, xsize = img.val.shape
    img.val = np.reshape(img.val, (img.val.shape[0]*img.val.shape[1]*img.val.shape[2]))
    if (img.val.max() < 256):
        fp.write("%d\n" % (8)) # write depth
        img.val = img.val.astype(np.uint8) # convert into uint8 (unsigned char)
    elif (img.val.max() < 65536):
        fp.write("%d\n" % (16)) # write depth
        img.val = img.val.astype(np.uint16) # convert into uint16 (unsigned short)
    else:
        fp.write("%d\n" % (32)) # write depth
        img.val = img.val.astype(np.uint32) # convert into uint32 (int)
    

    img.val.tofile(fp) # write binary image
    fp.close()

    # reshape an 1D-array to 3D-array
    img.val = np.reshape(img.val, (zsize, ysize, xsize)) # coodinates: (z, y, x) = (plane, row, col) 







def itk2scn(filename):
    filename = os.path.expanduser(filename)

    itk_img = sitk.ReadImage(filename)
    np_img  = sitk.GetArrayFromImage(itk_img)

    xsize, ysize, zsize = itk_img.GetSize()
    dx, dy, dz          = itk_img.GetSpacing()
    np_img.reshape((xsize, ysize, zsize))

    return Scene(dx, dy, dz, val=np_img)


def main():
    if len(sys.argv) != 3:
        sys.exit("python nii2scn.py <input_image.[nii,nii.gz]> <out_image.scn>")

    input_path = sys.argv[1]
    out_path   = sys.argv[2]

    if not input_path.endswith(".nii") and not input_path.endswith(".nii.gz"):
        sys.exit("Invalid extension for input path: %s\nTry .nii or .nii.gz" % input_path)

    if not out_path.endswith(".scn"):
        sys.exit("Invalid extension for output path: %s\nTry .scn" % out_path)


    print("- Input: %s" % input_path)
    print("- Output: %s\n" % out_path)

    print("- Converting VTK to SCN")
    out_img = itk2scn(input_path)
    print("- Writing Scn Image")
    out_img.write(out_path)



if __name__ == "__main__":
    main()
