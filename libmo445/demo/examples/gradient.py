import pyift.pyift as ift
import numpy as np
import matplotlib.pyplot as plt
import sys

# remaining functions

def gradient(img, A):

    dil  = ift.Dilate(img,A,None)
    ero  = ift.Erode(img,A,None)
    grad = ift.Sub(dil,ero)

    return(grad)

# main function

if (len(sys.argv) != 4):
    print("python {} <input image> <adj. radius> <output gradient image>".format(sys.argv[0]))
    exit()
img = ift.ReadImageByExt(sys.argv[1])
A   = ift.Circular(float(sys.argv[2]))

if (ift.IsColorImage(img)):
    Y   = ift.ImageGray(img)
    Cb  = ift.ImageCb(img)
    Cr  = ift.ImageCr(img)
    G1  = gradient(Y,A).AsNumPy()
    G2  = gradient(Cb,A).AsNumPy()
    G3  = gradient(Cr,A).AsNumPy()
    grad = np.maximum(np.maximum(G1,G2),G3)
else:
    grad = gradient(img,A).AsNumPy()

    
plt.imshow(grad, cmap='gray', vmin=0, vmax=255)
plt.show()

#grad = ift.ImageFromNumPy(grad)
# For 3D images, you must use True
grad = ift.CreateImageFromNumPy(grad, False) 
    
ift.WriteImageByExt(grad,sys.argv[3])
