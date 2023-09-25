/**
 * @file
 * @brief Generates the superpixels/supervoxels to a given Image.
 * @note See the source code in @ref iftSuperpixelsBySlic.c
 *
 * @example iftSuperpixelsBySlic.c
 * @brief Generates the superpixels/supervoxels to a given Image.
 * @author Samuel Martins
 * @date May 21, 2016
 */


#include "ift.h"

int main(int argc, const char *argv[]) {
//    iftDict *args = iftGetArgs(argc, argv);

    if (argc < 4 || argc > 5)
        iftError("Usage: iftSuperpixelsBySlic image.ppm nb_superpixels compacity gt_img","main");

    int input_n_clusters=atoi(argv[2]);
    double comp=atof(argv[3]);

//    iftGetRequiredArgs(args, &img_path, &input_n_clusters, &comp);

    iftImage *img = iftReadImageByExt(argv[1]);
    iftImage *aux;
//    iftGetOptionalArgs(args, img, &img_max_range);
    iftImage *gt_img=NULL;
    if (argc ==5){
        gt_img=iftReadImageByExt(argv[4]);
        aux=iftRelabelGrayScaleImage(gt_img,0);
        iftDestroyImage(&gt_img);
        gt_img=aux;
    }


    int img_max_range=iftNormalizationValue(iftMaximumValue(img));

    int n_clusters;

    iftImage *label = iftGenerateSuperpixelsBySlic(img, NULL, input_n_clusters, comp, img_max_range, &n_clusters,                      gt_img);

    if (gt_img){
        if (iftMinimumValue(label) > 0){
            for (int p=0;p<label->n;p++)
                label->val[p]--;
            printf("?????\n");
        }

        /*compute the accuracy*/
        int ncorrectsamples=0;
        for (int p=0;p<label->n;p++)
            if (gt_img->val[p] == label->val[p])
                ncorrectsamples++;
        printf("%.3f ",(float)ncorrectsamples/label->n);

        aux=label;
        label = iftSmoothRegionsByDiffusion(aux,img,0.5,2);
        iftDestroyImage(&aux);

        aux=iftVolumeOpen(label,250);
        iftDestroyImage(&label);
        label=aux;
        aux=iftVolumeClose(label,250);
        iftDestroyImage(&label);
        label=aux;

        aux= iftExtractLargestObjectInLabelImage(label);
        iftDestroyImage(&label);
        label=aux;

        printf("%.3lf\n",iftDiceSimilarity(label,gt_img));
    }

    iftWriteImageP2(label, "labels.pgm");

    iftWriteSuperpixelBorders(img, label, "regions.ppm");


//    iftDestroyDict(&args);
    iftDestroyImage(&img);
    iftDestroyImage(&gt_img);
    iftDestroyImage(&label);

    return 0;
}












