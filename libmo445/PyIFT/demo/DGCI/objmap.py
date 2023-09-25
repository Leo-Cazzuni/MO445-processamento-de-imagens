import pyift.pyift as ift
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


if __name__ == '__main__':

    parser = argparse.ArgumentParser(prog='Object saliency using optimum-path forest and seed selection.')
    parser.add_argument('-i', '--input-image', type=check_io_img, required=True)
    parser.add_argument('-m', '--markers-image', type=check_aux_img, required=True)
    parser.add_argument('-s', '--superpixel-image', type=check_aux_img, required=True)
    parser.add_argument('-o', '--output-image', type=check_io_img, default='output.pgm', required=False)
    parser.add_argument('-k', '--number-neighbors', type=int, default=24, required=False)
    parser.add_argument('-t', '--threshold', type=float, default=0.8, required=False)

    args = parser.parse_args()

    img = ift.ReadImageByExt(args.input_image)
    mrk = ift.ReadImageByExt(args.markers_image)
    spxl = ift.RelabelImage(ift.ReadImageByExt(args.superpixel_image))
    A = ift.Circular(1.5)
    
    # seed selection
    feat = ift.ExtractImageFeatures(img, spxl, A, False)
    Z = ift.MImageToDataSet(feat, spxl)
    ift.SetStatus(Z, ift.IFT_TRAIN)
    
    graph = ift.CreateKnnGraph(Z, args.number_neighbors)
    ift.UnsupTrain(graph, ift.NormalizedCutPtr())
    
    cluster = ift.DataSetToLabelImage(Z, spxl, False, ift.IFT_GROUP)
    seeds = ift.SelectSeedsForEnhancement(ift.LabeledSetFromSeedImage(mrk, True),
                                          cluster, 1, args.threshold)

    # objmap
    feat = ift.ExtractImageFeatures(img, None, A, False)
    Z = ift.MImageToDataSet(feat, None)
    ift.LabelDataSetFromSeeds(Z, seeds, None)
    Ztrain = ift.ExtractSamples(Z, ift.IFT_SUPERVISED)
    ift.SetStatus(Ztrain, ift.IFT_TRAIN)
    graph = ift.CreateCplGraph(Ztrain)
    ift.SupTrain(graph)

    ift.SetStatus(Z, ift.IFT_TEST)
    ift.ClassifyWithCertaintyValues(graph, Z)
    objmap = ift.DataSetObjectMap(Z, None, 255, 2)

    ift.WriteImageByExt(objmap, args.output_image)


