/**
 * @file
 * @brief Samples a dataset directory or CSV file according to a user chosen sampling type.
 *
 * @example iftFileSetSampler.c
 * @brief Samples a dataset directory or CSV file according to a user chosen sampling type.
 * @author Thiago Vallin Spina
 * @date Mar 9, 2016
 */
#include "ift.h"

#define  IFT_SAMPLING_TYPES "RANDOM"

iftDict *iftGetArgumentsFromCmdLine(int argc, const char **argv);

void iftSaveSampledFiles(iftFileSet *files, const iftSampler *sampler, const char *output_dir);

int main(int argc, const char** argv) {

    iftDict *args = NULL;
    iftFileSet *files = NULL;
    iftSampler *sampler = NULL;
    long niters = 0, ntraining = 0;

    /*--------------------------------------------------------*/

    int MemDinInicial, MemDinFinal;
    MemDinInicial = iftMemoryUsed();

    /*--------------------------------------------------------*/

    args = iftGetArgumentsFromCmdLine(argc, argv);

    const char *img_entry  = iftGetConstStrValFromDict("-i", args);
    const char *output_dir = iftGetConstStrValFromDict("-o", args);
    char *sampling_type    = iftUpperString(iftGetConstStrValFromDict("-t", args));

    if (iftDictContainKey("-s", args, NULL)) {
        files = iftLoadFileSetFromDirBySuffix(img_entry, iftGetConstStrValFromDict("-s", args));
    } else {
        files = iftLoadFileSetFromDirOrCSV(img_entry, true);
    }

    if(iftCompareStrings(sampling_type, "RANDOM")) {
        double train_percentage = 0.0;

        if(!iftDictContainKey("--train_perc", args, NULL))
            iftError("The training percentage is required for sampling type: %s", "main", sampling_type);
        if(!iftDictContainKey("--niters", args, NULL))
            iftError("The number of iterations is required for sampling type: %s", "main", sampling_type);


        niters = iftGetLongValFromDict("--niters", args);
        train_percentage = iftGetDblValFromDict("--train_perc", args);

        ntraining = iftMax(1, train_percentage*files->n);
        sampler = iftRandomSubsampling(files->n, niters, ntraining);

    } else {
        iftError("Please choose among the available sampling options: %s\n", "main", IFT_SAMPLING_TYPES);
    }

    if(sampler != NULL) {
        iftSaveSampledFiles(files, sampler, output_dir);

    }

    /* Cleaning up! */
    iftDestroyFileSet(&files);
    iftDestroySampler(&sampler);
    iftDestroyDict(&args);
    iftFree(sampling_type);

    MemDinFinal = iftMemoryUsed();
    iftVerifyMemory(MemDinInicial, MemDinFinal);

    return 0;
}


/************************** SOURCES **************************/
iftDict *iftGetArgumentsFromCmdLine(int argc, const char **argv) {
    iftCmdLineOpt cmd_line_opts[] = {
            {.short_name = "-i", .long_name = "--image_entry", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Folder with image files or CSV file"},
            {.short_name = "-t", .long_name = "--sampling_type", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help=IFT_SAMPLING_TYPES},
            {.short_name = "", .long_name = "--train_perc", .has_arg=true, .arg_type=IFT_DBL_TYPE,
                    .required=false, .help="Traning percentage for RANDOM sampling"},
            {.short_name = "", .long_name = "--niters", .has_arg=true, .arg_type=IFT_LONG_TYPE,
                    .required=false, .help="Number of iterations for for RANDOM sampling"},
            {.short_name = "-o", .long_name = "--output_dir", .has_arg=true, .arg_type=IFT_STR_TYPE,
                    .required=true, .help="Output folder where each sampling iteration will be saved using CSV files"}

    };
    int n_opts = sizeof(cmd_line_opts) / sizeof (iftCmdLineOpt);


    char program_description[IFT_STR_DEFAULT_SIZE] = "This program loads a set of file names and samples it according to a criterion.";

    // Parser Setup
    iftCmdLineParser *parser = iftCreateCmdLineParser(program_description, n_opts, cmd_line_opts);
    iftDict *args            = iftParseCmdLine(argc, argv, parser); // getting the passed options/arguments
    iftDestroyCmdLineParser(&parser);

    return args;
}

void iftSaveSampledFiles(iftFileSet *files, const iftSampler *sampler, const char *output_dir) {
    /** Computing the training/testing CSV files for each iteration **/
    for (int it = 0; it < sampler->niters; it++) {
        char output_iteration_dir[IFT_STR_DEFAULT_SIZE];
        char *output_dir_path = NULL;
        char *output_csv_path = NULL;

        iftFileSet *training_files = NULL;
        iftFileSet *testing_files = NULL;
        iftCSV *training_output = NULL;
        iftCSV *testing_output = NULL;

        /** Selecting the sampling for the current iteration **/
        iftSampleFileSet(files, sampler, it);

        /** Extracting samples to a FileSet and then to a CSV file **/
        training_files = iftExtractFileSamples(files, IFT_TRAIN);
        testing_files  = iftExtractFileSamples(files, IFT_TEST);

        training_output = iftFileSetToCSV(training_files);
        testing_output  = iftFileSetToCSV(testing_files);

        iftDestroyFileSet(&training_files);
        iftDestroyFileSet(&testing_files);

        /** Creating folders for each iteration/fold and saving CSV files **/
        sprintf(output_iteration_dir, "%04d", it + 1);

        /** Creating dirs **/
        output_dir_path = iftJoinPathnames(2, output_dir, output_iteration_dir);
        iftMakeDir(output_dir_path);

        /** Saving CSV files **/
        output_csv_path = iftJoinPathnames(2, output_dir_path, "train.csv");
        iftWriteCSV(training_output, output_csv_path, ';');
        iftFree(output_csv_path);

        output_csv_path = iftJoinPathnames(2, output_dir_path, "test.csv");
        iftWriteCSV(testing_output, output_csv_path, ';');
        iftFree(output_csv_path);

        /* Cleaning up! */
        iftFree(output_dir_path);
        iftDestroyCSV(&training_output);
        iftDestroyCSV(&testing_output);
    }
}