import pyift.pyift as ift
import os
from os.path import join as pjoin


if __name__ == '__main__':

    orig_dir = 'orig'
    scrib_1_dir = 'scribbles-set-1'
    scrib_2_dir = 'scribbles-set-2'
    out_1_dir = 'out-scrib-1'
    out_2_dir = 'out-scrib-2'

    origs_dir = sorted([pjoin(orig_dir, f) for f in os.listdir(orig_dir)])
    seeds_1_dir = sorted([pjoin(scrib_1_dir, f) for f in os.listdir(scrib_1_dir) if f.endswith('.txt')])
    seeds_2_dir = sorted([pjoin(scrib_2_dir, f) for f in os.listdir(scrib_2_dir) if f.endswith('.txt')])

    A = ift.Circular(1.0)

    if not os.path.exists(out_1_dir):
        os.makedirs(out_1_dir)

    if not os.path.exists(out_2_dir):
        os.makedirs(out_2_dir)

    for idx, path in enumerate(origs_dir):

        img = ift.ReadImageByExt(path)
        mimg = ift.ImageToMImage(img, ift.LABNorm_CSPACE)

        seeds_1 = ift.ReadSeeds(img, seeds_1_dir[idx])
        seeds_2 = ift.ReadSeeds(img, seeds_2_dir[idx])

        print('\nSegmenting image {} ...'.format(idx + 1))
        print('Scribbles 1')

        print('Dynamic Tree')
        segm = ift.DynTreeRoot(mimg, A, seeds_1, 1, 0.0, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_1_dir, '{:02d}'.format(idx + 1) + '-dyntree.ppm'))

        print('Dynamic Tree + Neighbor Dist')
        segm = ift.DynTreeRoot(mimg, A, seeds_1, 1, 0.25, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_1_dir, '{:02d}'.format(idx + 1) + '-dyntree-dist.ppm'))

        print('Dynamic Closest Tree')
        segm = ift.DynTreeClosestRoot(mimg, A, seeds_1, 1, 0.0, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_1_dir, '{:02d}'.format(idx + 1) + '-dynclosest.ppm'))

        print('Dynamic Closest Tree + Neighbor Dist')
        segm = ift.DynTreeClosestRoot(mimg, A, seeds_1, 1, 0.25, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_1_dir, '{:02d}'.format(idx + 1) + '-dynclosest-dist.ppm'))

        print('Scribbles 2')

        print('Dynamic Tree')
        segm = ift.DynTreeRoot(mimg, A, seeds_2, 1, 0.0, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_2_dir, '{:02d}'.format(idx + 1) + '-dyntree-.ppm'))

        print('Dynamic Tree + Neighbor Dist')
        segm = ift.DynTreeRoot(mimg, A, seeds_2, 1, 0.25, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_2_dir, '{:02d}'.format(idx + 1) + '-dyntree-dist.ppm'))

        print('Dynamic Closest Tree')
        segm = ift.DynTreeClosestRoot(mimg, A, seeds_2, 1, 0.0, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_2_dir, '{:02d}'.format(idx + 1) + '-dynclosest.ppm'))

        print('Dynamic Closest Tree + Neighbor Dist')
        segm = ift.DynTreeClosestRoot(mimg, A, seeds_2, 1, 0.25, None, 0.0)
        ift.WriteImageByExt(ift.Mask(img, segm),
                            pjoin(out_2_dir, '{:02d}'.format(idx + 1) + '-dynclosest-dist.ppm'))


