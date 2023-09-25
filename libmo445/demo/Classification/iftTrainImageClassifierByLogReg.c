/**
 * @file
 * @brief Trains a Logistic Regression Classifier from a set of Voxels (markers) of an Input Image.
 *
 * The samples from the training dataset are the markers, and their feature vectors correspond to the
 * brightness value from each one in the Input Image.
 * The labels from the markers are used as the true labels from the dataset's samples.
 * The markers could be obtained using VISVA or Usis, and they are stored by the function iftWriteSeeds().
 * @note See the source code in @ref iftTrainImageClassifierByLogReg.c
 *
 * @example iftTrainImageClassifierByLogReg.c
 * @brief Trains a Logistic Regression Classifier from a set of Voxels (markers) of an Input Image.
 *
 * The samples from the training dataset are the markers, and their feature vectors correspond to the
 * brightness value from each one in the Input Image.
 * The labels from the markers are used as the true labels from the dataset's samples.
 * The markers could be obtained using VISVA or Usis, and they are stored by the function iftWriteSeeds().
 * @note See the source code in @ref iftTrainImageClassifierByLogReg.c
 * @author Samuel Martins
 * @date Jul 1, 2016
 */


#include "ift.h"

/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(const iftDict *args, char **img_path, char **markers_path, char **clf_path);
void iftValidateInputs(const char *img_path, const char *markers_path, const char *out_clf_path);
void iftGetOptionalArgs(const iftDict *args, int *max_n_train, double *smooth_factor);
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
    char *img_path     = NULL;
    char *markers_path = NULL;
    char *out_clf_path     = NULL;

    // optional args
    int max_n_train;
    double smooth_factor;

    iftGetRequiredArgs(args, &img_path, &markers_path, &out_clf_path);
    iftGetOptionalArgs(args, &max_n_train, &smooth_factor);

    timer *t1 = iftTic();

    puts("- Reading Image");
    iftImage *img = iftReadImageByExt(img_path);

    if (smooth_factor > 0.0) {
        puts("- Smoothing Image");

        iftKernel *K = NULL;
        if(iftIs3DImage(img))
            K = iftGaussianKernel(1.5, smooth_factor*iftMaximumValue(img));
        else
            K = iftGaussianKernel2D(1.5, smooth_factor*iftMaximumValue(img));

        iftImage *smoothed = iftFastLinearFilter(img, K, 0);

        iftDestroyKernel(&K);
        iftDestroyImage(&img);
        img = smoothed;
    }

    puts("- Reading Markers");
    iftLabeledSet *markers = iftReadSeeds(img, markers_path);

    puts("- Getting DataSet");
    
    /* iftMImage *mimg; */
    /* if (!iftIsColorImage(img)) { */
    /*   mimg = iftExtractGrayFeatures(img); */
    /* } else */
    /*   mimg = iftImageToMImage(img, LAB_CSPACE); */

    /* iftDataSet *Z = iftMImageSeedsToDataSet(mimg, markers); */
    
    iftDataSet *Z = iftImageSeedsToDataSet(img, markers);

    if (!iftSelectSupTrainSamplesWithNoOutliers(Z,max_n_train))
        iftError("Seeds do not represent a valid training set", "main");

    printf("  - n_train_samples: %d/%d, n_feats: %d, n_classes: %d\n", Z->ntrainsamples, Z->nsamples, Z->nfeats, Z->nclasses);

    puts("- Training Log Reg");
    iftLogReg *logreg = iftLogRegTrain(Z);

    puts("- Saving Log Reg");
    iftWriteLogReg(logreg, out_clf_path);

    puts("\nDone...");
    puts(iftFormattedTime(iftCompTime(t1, iftToc())));

    // DESTROYERS
    iftDestroyDict(&args);
    iftDestroyImage(&img);
    //iftDestroyMImage(&mimg);
    iftDestroyLabeledSet(&markers);
    iftDestroyDataSet(&Z);
    iftDestroyLogReg(&logreg);
    iftFree(img_path);
    iftFree(markers_path);
    iftFree(out_clf_path);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Trains a Logistic Regression Classifier from a set of Voxels (markers) of an Input Image.\n" \
        "The samples from the training dataset are the markers, and their feature vectors correspond to the " \
        "brightness value from each one in the Input Image.\n" \
        "The labels from the markers are used as the true labels from the dataset's samples.\n\n" \
        "The markers could be obtained using VISVA or Usis, and they are stored by the function iftWriteSeeds()";

    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--input-img", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Input Image where the markers were obtained."},
            {.short_name = "-m", .long_name = "--markers-path", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="File (*.txt) with the chosen markers to train the classifier.\n" \
                                    "Use VISVA or Usis software to get it, or the function iftWriteSeeds()."},
            {.short_name = "-f", .long_name = "--smooth-factor", .has_arg=true, .arg_type=IFT_DBL_TYPE,
                    .required=false, .help="Factor used to smooth the image (e.g., 0.3)."},
            {.short_name = "-o", .long_name = "--output-clf-path", .has_arg=true, .arg_type=IFT_STR_TYPE,
             .required=true, .help="Pathname from the trained classifier (*.logreg.zip)."},
            {.short_name = "-n", .long_name = "--max-num-train-markers", .has_arg=true, .arg_type=IFT_LONG_TYPE,
             .required=false, .help="Maximum Number of Markers used to train the Classifier.\n" \
                                     "Default: 500"}
    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}


void iftGetRequiredArgs(const iftDict *args, char **img_path, char **markers_path, char **out_clf_path) {
    *img_path     = iftGetStrValFromDict("--input-img", args);
    *markers_path = iftGetStrValFromDict("--markers-path", args);
    *out_clf_path = iftGetStrValFromDict("--output-clf-path", args);

    iftValidateInputs(*img_path, *markers_path, *out_clf_path);

    puts("-----------------------");
    printf("- Input Image: \"%s\"\n", *img_path);
    printf("- Markers: \"%s\"\n", *markers_path);
    printf("- Output Classifier: \"%s\"\n", *out_clf_path);
    puts("-----------------------");

}


void iftValidateInputs(const char *img_path, const char *markers_path, const char *out_clf_path) {
    if (!iftIsImageFile(img_path))
        iftError("Invalid Input Image: \"%s\"", "iftValidateInputs", img_path);

    if (iftFileExists(markers_path)) {
        if (!iftEndsWith(markers_path, ".txt")) {
            iftError("Invalid Extension for the Markers File: \"%s\"... Use *.txt",
                     "iftValidateInputs", markers_path);
        }
    }
    else iftError("Markers File \"%s\" does not exist", "iftValidateInputs", markers_path);

    if (!iftEndsWith(out_clf_path, ".zip")) {
        iftError("Invalid Extension for the Log Reg Classifier: \"%s\"... ... Use *.zip",
                 "iftValidateInputs", out_clf_path);
    }

    char *parent_dir = iftParentDir(out_clf_path);
    if (!iftDirExists(parent_dir)) {
        iftMakeDir(parent_dir);
    }
    iftFree(parent_dir);
}

void iftGetOptionalArgs(const iftDict *args, int *max_n_train, double *smooth_factor) {
    if (iftDictContainKey("--smooth-factor", args, NULL)) {
        *smooth_factor = iftGetDblValFromDict("--smooth-factor", args);
    }
    else
        *smooth_factor = 0.3;

    if (iftDictContainKey("--max-num-train-markers", args, NULL)) {
        *max_n_train = iftGetLongValFromDict("--max-num-train-markers", args);
    }
    else *max_n_train = 500;

    printf("- Max. Num of Train Markers/Samples: %d\n", *max_n_train);
    printf("- Smooth factor: %lf\n", *smooth_factor);
    puts("-----------------------\n");
}
/*************************************************************/




