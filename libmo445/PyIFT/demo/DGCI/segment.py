import pyift.pyift as ift
import argparse


def str2bool(v):
    if v.lower() in ('yes', 'true', 't', 'y', '1'):
        return True
    elif v.lower() in ('no', 'false', 'f', 'n', '0'):
        return False
    else:
        raise argparse.ArgumentTypeError('Boolean value expected.')


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


if __name__ == '__main__':

    parser = argparse.ArgumentParser(prog='Image segmentation using Dynamic Trees.')
    parser.add_argument('-i', '--input-image', type=check_io_img, required=True)
    parser.add_argument('-m', '--markers-image', type=check_aux_img, required=True)
    parser.add_argument('-o', '--output-image', type=check_io_img, default='output.pgm', required=False)
    parser.add_argument('-c', '--closest-root', type=str2bool, default='false', required=False)
    parser.add_argument('-gm', '--gamma', type=float, default=0.0, required=False)
    parser.add_argument('-s', '--saliency-image', type=check_aux_img, default='', required=False)
    parser.add_argument('-a', '--alpha', type=float, default=0.5, required=False,
                        help='Alpha needs to be tuned according to original image and saliency depth.')
    parser.add_argument('-d', '--delta', type=int, default=0, required=False)

    args = parser.parse_args()

    if args.gamma < 0.0:
        raise argparse.ArgumentTypeError('Gamma must be greater than 0.0.')

    if args.alpha < 0.0 or args.alpha > 1.0:
        raise argparse.ArgumentTypeError('Alpha must be between 0.0 and 1.0.')

    orig  = ift.ReadImageByExt(args.input_image)
    mrk   = ift.ReadImageByExt(args.markers_image)
    seeds = ift.LabeledSetFromSeedImage(mrk, True)

    if args.saliency_image != '':
        objmap = ift.ReadImageByExt(args.saliency_image)
    else:
        objmap = None

    mimg = ift.ImageToMImage(orig, ift.LABNorm_CSPACE)
    A = ift.Circular(1.0)
    
    if args.closest_root:
        segm = ift.DynTreeClosestRoot(mimg, A, seeds, args.delta, args.gamma, objmap, args.alpha)
    else:
        segm = ift.DynTreeRoot(mimg, A, seeds, args.delta, args.gamma, objmap, args.alpha)
    
    ift.WriteImageByExt(ift.Normalize(segm, 0, 255), args.output_image)
