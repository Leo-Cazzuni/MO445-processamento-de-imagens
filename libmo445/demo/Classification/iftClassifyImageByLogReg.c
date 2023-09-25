/**
 * @file
 * @brief Classifies an Image (all voxels) using a Logistic Regression Classifier.
 *
 * The Image is converted to a Dataset where each sample is a voxel, and its feature vector is only the voxel's brightness value.
 * The Logistic Regression Classifier is trained by the program @ref iftTrainImageClassifierByLogReg.c (demo/Classification/iftTrainImageClassifierByLogReg.c)
 * The program might also compute the object map for each object's label based on the weights of the dataset's samples.
 * @note See the source code in @ref iftClassifyImageByLogReg.c
 *
 * @example iftClassifyImageByLogReg.c
 * @brief Classifies an Image (all voxels) using a Logistic Regression Classifier.
 *
 * The Image is converted to a Dataset where each sample is a voxel, and its feature vector is only the voxel's brightness value.
 * The Logistic Regression Classifier is trained by the program @ref iftTrainImageClassifierByLogReg.c (demo/Classification/iftTrainImageClassifierByLogReg.c)
 * The program might also compute the object map for each object's label based on the weights of the dataset's samples.
 * @note See the source code in @ref iftClassifyImageByLogReg.c
 * @author Samuel Martins
 * @date Jul 1, 2016
 */


#include "ift.h"


/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **clf_path, char **out_img_path);
void iftValidateInputs(const char *img_path, const char *clf_path, const char *out_img_path);
int iftGetImageMaxRange(const iftDict *args, const iftImage *img);
/*************************************************************/

iftMImage *iftExtractGrayFeatures(iftImage *img)
{
    iftMImage *mimg = iftCreateMImage(img->xsize,img->ysize,img->zsize,2);
    iftAdjRel *A;

    if (iftIs3DImage(img)){
        A  = iftSpheric(1.0);
    } else {
        A = iftCircular(1.0);
    }

#pragma omp parallel shared(img,mimg,A)
    for (int p=0; p < img->n; p++) {
        iftVoxel u = iftGetVoxelCoord(img,p);
        mimg->val[p][0] = img->val[p];
        mimg->val[p][1] = 0.0;
	int n = 0;
        for (int i=1; i < A->n; i++){
            iftVoxel v = iftGetAdjacentVoxel(A,u,i);
            if (iftValidVoxel(img,v)){
                int q = iftGetVoxelIndex(img,v);
		mimg->val[p][1] += fabs(img->val[q]-img->val[p]);
		n++;
            }
        }
	mimg->val[p][1] /= n;
    }

    iftDestroyAdjRel(&A);
    return(mimg);
}


int main(int argc, const char *argv[]) {
    iftDict *args = iftGetArgs(argc, argv);

    // mandatory args
    char *img_path        = NULL;
    char *clf_path        = NULL;
    char *out_img_path    = NULL;
    // optional args
    bool save_obj_maps   = iftDictContainKey("--save-object-maps", args, NULL);


    iftGetRequiredArgs(args, &img_path, &clf_path, &out_img_path);

    timer *t1 = iftTic();

    puts("- Reading Image");
    iftImage *img = iftReadImageByExt(img_path);

    if (iftDictContainKey("--smooth-factor", args, NULL)) {
        puts("- Smoothing Image");
        double smooth_factor = iftGetDblValFromDict("--smooth-factor", args);

        if (!iftAlmostZero(smooth_factor)) {
            iftKernel *K = NULL;
            if (iftIs3DImage(img))
                K = iftGaussianKernel(1.5, smooth_factor*iftMaximumValue(img));
            else
                K = iftGaussianKernel2D(1.5, smooth_factor*iftMaximumValue(img));
            iftImage *smoothed = iftFastLinearFilter(img, K, 0);

            iftDestroyKernel(&K);
            iftDestroyImage(&img);
            img = smoothed;
        }
    }

    puts("- Reading Log Reg");
    iftLogReg *logreg = iftReadLogReg(clf_path);

    puts("- Test Image To Dataset");

    /* iftMImage *mimg; */
    /* if (!iftIsColorImage(img)) { */
    /*   mimg = iftExtractGrayFeatures(img); */
    /* } else */
    /*   mimg = iftImageToMImage(img, LAB_CSPACE); */

    /* iftDataSet *Ztest = iftMImageToDataSet(mimg);  */

    iftDataSet *Ztest = iftImageToDataSet(img); // all samples have status TEST
    printf("  - n_samples: %d, n_feats: %d\n", Ztest->nsamples, Ztest->nfeats);

    puts("- Classifying Test Image By Log Reg");
    iftLogRegClassify(logreg, Ztest);
    iftImage *label_img = iftDataSetToLabelImage(Ztest, true);

    puts("- Saving Resulting Labeled Image");
    iftWriteImageByExt(label_img, out_img_path);
    
    if (save_obj_maps) {
        puts("- Saving Resulting Object Maps");
        char *base        = iftBasename(out_img_path);
        const char *ext   = iftFileExt(out_img_path);
        int img_max_range = iftGetImageMaxRange(args, img);
        printf("  - Image Range: [0, %d]\n", img_max_range);

        // for each class except the background (class 1 in Z)
        for (int label = 2; label <= logreg->nclasses; label++) {
            char obj_map_path[512];
            sprintf(obj_map_path, "%s_obj_map%d%s", base, label-1, ext);
            printf("  - %s\n", obj_map_path);

            iftImage *obj_map = iftDataSetObjectMap(Ztest, img_max_range, label);
            iftWriteImageByExt(obj_map, obj_map_path);            
            iftDestroyImage(&obj_map);
        }
        iftFree(base);
    }

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));


    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    //iftDestroyMImage(&mimg);
    iftDestroyLogReg(&logreg);
    iftDestroyDataSet(&Ztest);
    iftDestroyImage(&label_img);
    iftFree(img_path);
    iftFree(clf_path);
    iftFree(out_img_path);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
            "Classifies an Image (all voxels) using a Logistic Regression Classifier.\n" \
            "The Image is converted to a Dataset where each sample is a voxel, and its feature vector " \
            "is only the voxel's brightness value.\n" \
            "The program might also compute the object map for each object's label based on the weights " \
            "of the dataset's samples." \
            "PS: The Logistic Regression Classifier is trained by the program demo/Classification/iftTrainImageClassifierByLogReg.c";

    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--input-test-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Input Test Image to be classified."},
            {.short_name = "-c", .long_name = "--classifier", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Classifier's pathname (*.zip)."},
            {.short_name = "-o", .long_name = "--output-image", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Output Labeled Image's Pathname."},
            {.short_name = "-f", .long_name = "--smooth-factor", .has_arg=true, .arg_type=IFT_DBL_TYPE,
             .required=false, .help="Factor used to smooth the image (e.g., 0.3)."},
            {.short_name = "-s", .long_name = "--save-object-maps", .has_arg=false,
             .required=false, .help="Saves the resulting object maps for each class with the same " \
                                     "Output Labeled Image Basename."},
            {.short_name = "-b", .long_name = "--img-depth", .has_arg=true, .arg_type=IFT_LONG_TYPE,
             .required=false, .help="Input Image Depth in bits (8, 12, 16, ...)\n" \
                                    "Default: it tries to find the image depth automatically"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_path, char **clf_path, char **out_img_path) {
    *img_path     = iftGetStrValFromDict("--input-test-img", args);
    *clf_path     = iftGetStrValFromDict("--classifier", args);
    *out_img_path = iftGetStrValFromDict("--output-image", args);

    iftValidateInputs(*img_path, *clf_path, *out_img_path);

    puts("-----------------------");
    printf("- Input Image: \"%s\"\n", *img_path);
    printf("- Classifier: \"%s\"\n", *clf_path);
    printf("- Output Labeled Image: \"%s\"\n", *out_img_path);
    puts("-----------------------");
}


void iftValidateInputs(const char *img_path, const char *clf_path, const char *out_img_path) {
    if (!iftIsImageFile(img_path))
        iftError("Invalid Input Image: \"%s\"", "iftValidateInputs", img_path);

    if (iftFileExists(clf_path)) {
        if (!iftEndsWith(clf_path, ".zip")) {
            iftError("Invalid Extension for the Classifier's File: \"%s\"... Use *.zip",
                     "iftValidateInputs", clf_path);
        }
    }
    else iftError("Classifier's File \"%s\" does not exist", "iftValidateInputs", clf_path);

    if (!iftIsImagePathnameValid(out_img_path)) {
        iftError("Invalid Output Image Pathname: \"%s\"", "iftValidateInputs", out_img_path);
    }

    char *parent_dir = iftParentDir(out_img_path);
    if (!iftDirExists(parent_dir)) {
        iftMakeDir(parent_dir);
    }
    iftFree(parent_dir);
}


int iftGetImageMaxRange(const iftDict *args, const iftImage *img) {
    int img_max_range;

    if (iftDictContainKey("--img-depth", args, NULL)) {
        int img_depth = iftGetLongValFromDict("--img-depth", args);
        if (img_depth <= 0)
            iftError("Invalid Image Depth: %d... Try > 0", "iftGetOptionalArgs", img_depth);

        img_max_range = (1 << img_depth) - 1; // (2^img_depth) - 1
    }
    else img_max_range = iftNormalizationValue(iftMaximumValue(img));

    return img_max_range;
}
/*************************************************************/






