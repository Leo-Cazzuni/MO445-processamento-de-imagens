#include "ift.h"



int main(int argc, const char *argv[]) {
    if (argc != 5)
        iftError("%s <input_mimage> <seed_image> <mask> <output_svoxel_image>", "main", argv[0]);
    
    puts("- Reading MImage");
    iftMImage *mimg = iftReadMImage(argv[1]);
    
    puts("- Reading Seeds");
    iftImage *seeds_img = iftReadImageByExt(argv[2]);
    
    puts("- Reading Mask");
    iftImage *mask = iftReadImageByExt(argv[3]);
    
    iftAdjRel *A = iftSpheric(1.0);
    
    puts("- Executing ISF");
    iftIGraph *igraph = iftExplicitIGraph(mimg, mask, NULL, A);
    iftIGraphISF_Root(igraph, seeds_img, 0.08, 3, 10);
    
    iftImage *svoxels_img = iftIGraphLabel(igraph);
    iftWriteImageByExt(svoxels_img, argv[4]);
    
    puts("\n- Sanity Check!");
    
    int n_svoxels = iftMaximumValue(svoxels_img);
    
    
    iftAdjRel *B = iftSpheric(1.74);
    
    for (int svoxel = 1; svoxel <= n_svoxels; svoxel++) {
        iftImage *single_svoxel_img = iftExtractObject(svoxels_img, svoxel);
        iftImage *relabeld_img = iftFastLabelComp(single_svoxel_img, B);
        
        int n_comps = iftMaximumValue(relabeld_img);
        if (n_comps != 1) {
            printf("\t- Error: Supervoxel %d has %d components\n", svoxel, n_comps);
//            iftWriteImageByExt(relabeld_img, "tmp/relabeled_svoxel_%d.nii.gz", svoxel);
        }
        
        iftDestroyImage(&single_svoxel_img);
        iftDestroyImage(&relabeld_img);
    }
    
    
    iftDestroyMImage(&mimg);
    iftDestroyImage(&seeds_img);
    iftDestroyImage(&mask);
    iftDestroyAdjRel(&A);
    iftDestroyIGraph(&igraph);
    iftDestroyImage(&svoxels_img);
    iftDestroyAdjRel(&B);
    
    
    return 0;
}
