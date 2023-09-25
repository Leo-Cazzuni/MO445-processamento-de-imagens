import pdb
import os
import struct
import sys

# https://pyscience.wordpress.com/2014/09/08/dicom-in-python-importing-medical-image-data-into-numpy-with-pydicom-and-vtk/
try:
    import dicom 
except ImportError:
    raise ImportError("dicom package is not installed!\nTo install: pip install pydicom")

try:
    import numpy as np
except ImportError:
    raise ImportError("numpy package is not installed!")


class Error(object):
    @staticmethod
    def print_error(error_msg, function):
        sys.exit("\nERROR in function {0}\n{1}".format(function, error_msg))



class Scene(object):
    def __init__(self, xsize=0, ysize=0, zsize=0, dx=0, dy=0, dz=0, val=[]):
        self.xsize = xsize
        self.ysize = ysize
        self.zsize = zsize
        self.dx    = dx
        self.dy    = dy
        self.dz    = dz
        self.val   = np.array(val)

    # it's called with "print obj_instance"
    def __str__(self):
        msg = "dx = %d, dy = %d, dz = %d\n" %(self.dx, self.dy, self.dz)
        msg += "xsize = %d, ysize = %d, zsize = %d\n" %(self.xsize, self.ysize, self.zsize)
        msg += "length = %d\n" %(self.xsize*self.ysize*self.zsize)
        msg += "{0}".format(self.val)
        return msg

    # it's called in the interactive terminal with obj_instance and you press the ENTER key
    def __repr__(self):
        msg = "dx = %d, dy = %d, dz = %d\n" %(self.dx, self.dy, self.dz)
        msg += "xsize = %d, ysize = %d, zsize = %d\n" %(self.xsize, self.ysize, self.zsize)
        msg += "length = %d\n" %(self.xsize*self.ysize*self.zsize)
        msg += "{0}".format(self.val)
        return msg


    @classmethod
    def read(cls, filename): # class is a object not instanced from the Class
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

        img = cls(xsize, ysize, zsize, dx, dy, dz, val)

        return img


    def save(self, filename):
        """Write a Scene image.

        Parameters
        ----------
        filename: string
            Filename of the image to be written.
        """
        min_val = np.amin(self.val)
        max_val = np.amax(self.val)
        if (min_val < 0):
            print("Warning: Shifting image values from [%d,%d] to [%d,%d] on " \
                "the original image\n" % (min_val, max_val, 0, max_val-min_val))
            self.val -= min_val 
            max_val = max_val - min_val

        fp = open(filename, "w")
        ####### write the header ########
        fp.write("SCN\n")
        fp.write("%d %d %d\n" % (self.xsize, self.ysize, self.zsize))
        fp.write("%f %f %f\n" % (self.dx, self.dy, self.dz))



        ####### write the voxel values ########
        if (max_val < 256):
            fp.write("%d\n" % (8)) # write depth
            self.val = self.val.astype(np.uint8) # convert into uint8 (unsigned char)
            fmt = 'B'*len(self.val)
        elif (max_val < 65536):
            fp.write("%d\n" % (16)) # write depth
            self.val = self.val.astype(np.uint16) # convert into uint16 (unsigned short)
            fmt = 'H'*len(self.val)
        else:
            fp.write("%d\n" % (32)) # write depth
            self.val = self.val.astype(np.uint32) # convert into uint32 (int)
            fmt = 'I'*len(self.val)
        
        bin_values = struct.pack(fmt,*self.val) # convert the values into bytes
        fp.write(bin_values) # write
        fp.close()


    @classmethod
    def dicom_2_scene(cls, dicom_dir):
        """Convert the Dicom images from a directory in a Scene.
        If the directory has other not Dicom files, the function ignores them.

        Parameters
        ----------
        dicom_dir: string
            Directory of the Dicom images.

        Returns
        ----------
        scene: Scene
            The converted Scene image.
        """

        # load all dicom images from the dicom_dir
        dicom_imgs = []
        for dfile in os.listdir(dicom_dir):
            dfile = os.path.join(dicom_dir, dfile) # get the full path of the files
            
            if os.path.isfile(dfile): # it excludes folders
                try:
                    # if it is not a dicom img, an exception is going to be raised
                    dimg = dicom.read_file(dfile)
                    dicom_imgs.append(dimg)  
                except:
                    continue # ignore the not dicom files --> go to the next iteration
        if (len(dicom_imgs) == 0):
            Error.print_error("No Dicom images in the directory", "Scene.dicom_2_scene")


        # check if the dicom images have the same sizes and displacements
        cls.validate_dicom_images(dicom_imgs)

        # Sorting the dicom images according to their ImagePositionPatient in z-axis (Z Origin)
        z_positions = []
        for dimg in dicom_imgs:
            z_positions.append(float(dimg.ImagePositionPatient[2])) # [2] is the z-axis
        z_indices = np.argsort(np.array(z_positions)) # get the indices that would sort an array (in ascending order).

        sorted_dicom_imgs = []
        for i in z_indices:
            sorted_dicom_imgs.append(dicom_imgs[i])
        dicom_imgs = sorted_dicom_imgs


        # read the voxel values of each slice
        xsize, ysize, zsize = int(dicom_imgs[0].Columns), int(dicom_imgs[0].Rows), len(dicom_imgs) 
        dx = float(dicom_imgs[0].PixelSpacing[1])
        dy = float(dicom_imgs[0].PixelSpacing[0])
        dz = float(dicom_imgs[0].SliceThickness)

        val = np.zeros((ysize, xsize, zsize), dtype=dicom_imgs[0].pixel_array.dtype)
        for z, dimg in enumerate(dicom_imgs):
            # store the raw data of a slice into the numpy.array val of shape (ysize, xsize, zsize)
            val[:, :, z] = dimg.pixel_array

        # Check if all voxels were read
        if val.size != xsize*ysize*zsize:
            Error.print_error("Number of voxels != than xsize*ysize*zsize", "Scene.dicom_2_scene")

        scene = cls(xsize, ysize, zsize, dx, dy, dz, val)
        scene.val = np.reshape(scene.val, (xsize*ysize*zsize))

        return scene


    @staticmethod
    def validate_dicom_images(dicom_imgs):
        """Check if the dicom images have the same sizes and displacements, aborting
        the program if they have.
        Static Method.

        Parameters
        ----------
        dicom_dir: list of dicom object (from the package dicom)
        """
        xsizes = []
        ysizes = []
        dx     = []
        dy     = []
        dz     = []
        bits   = []
        for dimg in dicom_imgs:
            xsizes.append(int(dimg.Columns))
            ysizes.append(int(dimg.Rows))
            dx.append(float(dimg.PixelSpacing[1]))
            dy.append(float(dimg.PixelSpacing[0]))
            dz.append(float(dimg.SliceThickness))
            bits.append(dimg.BitsAllocated)
        if len(set(xsizes)) != 1:
            Error.print_error("Dicom images with different xsize (length)", "Scene.dicom_2_scene")
        if len(set(ysizes)) != 1:
            Error.print_error("Dicom images with different ysize (height)", "Scene.dicom_2_scene")
        if len(set(dx)) != 1:
            Error.print_error("Dicom images with different x displacement", "Scene.dicom_2_scene")
        if len(set(dy)) != 1:
            Error.print_error("Dicom images with different y displacement", "Scene.dicom_2_scene")
        if len(set(dz)) != 1:
            Error.print_error("Dicom images with different z displacement", "Scene.dicom_2_scene")
        if len(set(bits)) != 1:
            Error.print_error("Dicom images with different Number of Bits Allocated", "Scene.dicom_2_scene")




######################
def validate_inputs(dicom_dir, out_filename):
    """Check if the directories exist.
    If the Dicom Dir does not exist, stop the program.
    If the Output Dir does not exist, it creates this directory.

    Parameters
    ----------
    dicom_dir: string
        Directory of Dicom images to be converted to Scene.

    out_filename: string
        Output Filename where the Scene(s) will be stored.
    """
    if os.path.exists(dicom_dir):
        if not os.path.isdir(dicom_dir):
            error_msg = "Dicom dir: \"%s\" is not a directory!" % (dicom_dir)
            Error.print_error(error_msg, "validate_inputs")
    else:
        error_msg = "Dicom dir: \"%s\" does not exist!" % (dicom_dir)
        Error.print_error(error_msg, "validate_inputs")

    output_dir = os.path.dirname(out_filename)

    if not os.path.exists(output_dir):
        print("--> Creating the Output dir: \"%s\"\n\n" % (output_dir))
        os.makedirs(output_dir)




def main():
    if (len(sys.argv) != 3):
        sys.exit("iftDicon2Scene.py <dicom_directory> <out_filename.scn>")

    dicom_dir    = os.path.abspath(os.path.expanduser(sys.argv[1])) # expanduser enables the use of ~
    out_filename = os.path.abspath(os.path.expanduser(sys.argv[2]))

    validate_inputs(dicom_dir, out_filename)

    scene = Scene.dicom_2_scene(dicom_dir)
    scene.save(out_filename)

    print("Done...")
    


if __name__ == "__main__":
    sys.exit(main())