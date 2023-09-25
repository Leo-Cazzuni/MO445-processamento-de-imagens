import sys

try:
    import numpy as np
except ImportError:
    raise ImportError("numpy package is not installed!")


class Error(object):
    @staticmethod
    def print_error(error_msg, function):
        sys.exit("\nERROR in function {0}\n{1}".format(function, error_msg))



# We follow the Image Coordinate Convetions from Skimage
# http://scikit-image.org/docs/dev/user_guide/numpy_images.html

class Scene(object):
    def __init__(self, dx=0, dy=0, dz=0, val=[], copy=True):
        self.dx    = dx
        self.dy    = dy
        self.dz    = dz
        # 3D matrix (z, y, x) = (plane, row, col) = (z, y, x)
        if copy:
            self.val = np.array(val)
        else:
            self.val = val


    # it's called with "print obj_instance"
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
    min_val = np.amin(img.val)
    max_val = np.amax(img.val)
    if (min_val < 0):
        print("Warning: Shifting image values from [%d,%d] to [%d,%d] on " \
            "the original image\n" % (min_val, max_val, 0, max_val-min_val))
        img.val -= min_val 
        max_val = max_val - min_val

    fp = open(filename, "w")
    ####### write the header ########
    fp.write("SCN\n")
    fp.write("%d %d %d\n" % (img.val.shape[2], img.val.shape[1], img.val.shape[0]))
    fp.write("%f %f %f\n" % (img.dx, img.dy, img.dz))

    ####### write the voxel values ########
    # reshape the 3D-array to 1D-array
    zsize, ysize, xsize = img.val.shape
    img.val = np.reshape(img.val, (img.val.shape[0]*img.val.shape[1]*img.val.shape[2]))
    if (max_val < 256):
        fp.write("%d\n" % (8)) # write depth
        img.val = img.val.astype(np.uint8) # convert into uint8 (unsigned char)
    elif (max_val < 65536):
        fp.write("%d\n" % (16)) # write depth
        img.val = img.val.astype(np.uint16) # convert into uint16 (unsigned short)
    else:
        fp.write("%d\n" % (32)) # write depth
        img.val = img.val.astype(np.uint32) # convert into uint32 (int)
    

    img.val.tofile(fp) # write binary image
    fp.close()

    # reshape an 1D-array to 3D-array
    img.val = np.reshape(img.val, (zsize, ysize, xsize)) # coodinates: (z, y, x) = (plane, row, col) 



