
#include "ift.h"

/************************** HEADERS **************************/
iftDict *iftGetArgs(int argc, const char *argv[]);
void iftGetRequiredArgs(iftDict *args, char **img_path, char **markers_path, char **clf_path, double *C_SVC);
void iftValidateInputs(const char *img_path, const char *markers_path, const char *out_clf_path);
void iftGetOptionalArgs(const iftDict *args, int *max_n_train, double *smooth_factor);

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
    iftImage *img = NULL, *smoothed = NULL;
    iftMImage *mimg = NULL;
    iftSVM *svm = NULL;
    iftLabeledSet *training_set = NULL;
    iftDataSet *Z = NULL, *Ztmp = NULL, *Zref = NULL, *Ztrain = NULL;
    size_t mem_start, mem_end;
    timer *t1, *t2;

    mem_start = iftMemoryUsed();

    iftDict *args = iftGetArgs(argc, argv);

    // mandatory args
    char *img_path     = NULL;
    char *markers_path = NULL;
    char *out_clf_path     = NULL;
    double C;

    // optional args
    int max_n_train;
    double smooth_factor;


    iftGetRequiredArgs(args, &img_path, &markers_path, &out_clf_path, &C);
    iftGetOptionalArgs(args, &max_n_train, &smooth_factor);

    t1 = iftTic();

    img = iftReadImageByExt(img_path);

    if (smooth_factor > 0.0) {
        /* Smoothing image */
        iftKernel *K = NULL;
        if(iftIs3DImage(img))
            K = iftGaussianKernel(1.5, smooth_factor*iftMaximumValue(img));
        else
            K = iftGaussianKernel2D(1.5, smooth_factor*iftMaximumValue(img));

        smoothed = iftLinearFilter(img, K);

        iftDestroyKernel(&K);
        iftDestroyImage(&img);

        img = smoothed;
    }

    if (!iftIsColorImage(img)) {
        // mimg = iftExtractGrayFeatures(img);
        mimg = iftImageToMImage(img, GRAY_CSPACE);
    } else
        mimg = iftImageToMImage(img, LAB_CSPACE);

    training_set = iftReadSeeds(img, markers_path);

    Z = iftMImageSeedsToDataSet(mimg, training_set);
    iftDestroyLabeledSet(&training_set);

    if (!iftSelectSupTrainSamplesWithNoOutliers(Z, max_n_train))
        iftError("Seeds do not represent a valid training set", "main");

    svm = iftCreatePreCompSVC(C);

    Ztmp = iftExtractSamples(Z, IFT_TRAIN);
    iftDestroyDataSet(&Z);
    Z = Ztmp;

    Zref = iftNormalizeContrastDataSet(Z);
    iftDestroyDataSet(&Z);

    /** Note: we only train an OVO classifier since we intend to compute a binary object map afterwards **/
    printf("- Kernelizing DataSet\n");
    uchar traceNormalize = 0;
    float ktrace;
    Ztrain = iftKernelizeDataSet(Zref, Zref, LINEAR, traceNormalize, &ktrace);
    iftSVMTrainOVO(svm, Ztrain, Zref);

    iftWriteSVM(svm, out_clf_path);

    iftDestroyLabeledSet(&training_set);
    iftDestroyImage(&img);
    iftDestroyMImage(&mimg);
    iftDestroyDataSet(&Ztrain);
    iftDestroyDataSet(&Zref);
    iftDestroyDict(&args);
    iftDestroySVM(svm);
    iftFree(img_path);
    iftFree(markers_path);
    iftFree(out_clf_path);

    mem_end = iftMemoryUsed();
    iftVerifyMemory(mem_start,mem_end);

    t2 = iftToc();
    puts(iftFormattedTime(iftCompTime(t1,t2)));


    return 0;
}

/************************** SOURCES **************************/
iftDict *iftGetArgs(int argc, const char *argv[]) {
    char program_description[2048] = \
        "Trains an SVM Classifier from a set of Voxels (markers) of an Input Image.\n" \
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
            {.short_name = "-C", .long_name = "--C-SVM", .has_arg=true, .arg_type=IFT_DBL_TYPE,
                    .required=true, .help="C constant for SVM"},
            {.short_name = "-f", .long_name = "--smooth-factor", .has_arg=true, .arg_type=IFT_DBL_TYPE,
                    .required=false, .help="Factor used to smooth the image (e.g., 0.3)."},
            {.short_name = "-o", .long_name = "--output-clf-path", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Pathname from the trained classifier (*.svm.zip)."},
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


void iftGetRequiredArgs(iftDict *args, char **img_path, char **markers_path, char **out_clf_path, double *C_SVC) {
    *img_path     = iftGetStrValFromDict("--input-img", args);
    *markers_path = iftGetStrValFromDict("--markers-path", args);
    *out_clf_path = iftGetStrValFromDict("--output-clf-path", args);
    *C_SVC        = iftGetDblValFromDict("--C-SVM", args);

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
