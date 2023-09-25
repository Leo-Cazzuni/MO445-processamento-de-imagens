import pyift.pyift as ift
import numpy as np
import argparse


def check_io_img(s):
    if '.png' in s or '.pgm' in s or '.jpg' in s or '.ppm' in s:
        return s
    else:
        raise argparse.ArgumentTypeError('Image must be a .ppm, .pgm, .png or .jpg file.')


def check_aux_img(s):
    if '.pgm' in s or s == '':
        return s
    else:
        raise argparse.ArgumentTypeError('Auxiliary images must be a .pgm file.')


def RISF(img, n_superpixels, n_iters=10, A=None, small_threshold=0.05, 
         seed_sampling=ift.IFT_ISF_GEODESIC_SAMPLING,
         pathcost=ift.IFT_ISF_ADD_MEAN_PATHCOST,
         seeds_recomp=ift.IFT_ISF_CENTROID_SEEDRECOMP):
    
    opt = ift.InitISFOptions(n_superpixels, n_iters, 0, small_threshold, 
                             seed_sampling, pathcost, seeds_recomp)
    
    refImg = ift.CopyImage(img)
    n = refImg.xsize * refImg.ysize * refImg.zsize
    refNp = np.linspace(0, n,num=n, dtype=np.int32).reshape((refImg.ysize, refImg.xsize))
    refImg.FromNumPy(refNp)

    if A is None:
        Adj = ift.Circular(1.0)
    else:
        Adj = A
    
    igraph = ift.InitSuperpixelIGraph(refImg)
    ift.SetSuperpixelIGraphImplicitAdjacency(igraph, Adj)
#     feats = ift.iftComputeSuperpixelFeaturesByGeometricCenter(refImg)
    feats = ift.ComputeSuperpixelFeaturesByColorSpaceMean(refImg, img, ift.LABNorm_CSPACE)
#     mimg = ift.ImageToMImage(img, ift.RGB_CSPACE)    
#     feats = ift.ComputeSuperpixelFeaturesByMImageMean(mimg, img)
    ift.SetSuperpixelIGraphFeatures(igraph, feats, ift.WeightedL2NormPtr(), 0.2, 12.0)
    suppixel = ift.SuperpixelSegmentationByRISF(igraph, opt, img)

    B = ift.Circular(1.5)
    return ift.RelabelRegions(suppixel, B)


if __name__ == '__main__':

    parser = argparse.ArgumentParser(prog='Image segmentation using optimum-path trees and seed selection.')
    parser.add_argument('-i', '--input-image', type=check_io_img, required=True)
    parser.add_argument('-n', '--number-superpixels', type=int, required=True)
    parser.add_argument('-o', '--output-image', type=check_aux_img, default='output.pgm', required=False)

    args = parser.parse_args()

    img = ift.ReadImageByExt(args.input_image)
    n = args.number_superpixels
    spxl = RISF(img, n)
    
    ift.WriteImageByExt(spxl, args.output_image)

