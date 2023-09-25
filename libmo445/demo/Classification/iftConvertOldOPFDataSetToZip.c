/**
 * @file
 * @brief A program that converts an OPF dataset file from the old format (*with* field function_number) to the new one (ending with .zip file extension). For data sets saved before Feb 27, 2016 use iftConvertOldOPFDataSetFormatWithoutFunctionNumberToZip.c.
 * @note See the source code in @ref iftConvertOldOPFDataSetToZip.c
 *
 * @example iftConvertOldOPFDataSetToZip.c
 * @brief A program that converts an OPF dataset file from the old format (*with* field function_number) to the new one (ending with .zip file extension). For data sets saved before Feb 27, 2016 use iftConvertOldOPFDataSetFormatWithoutFunctionNumberToZip.c. * @author Thiago Vallin Spina
 * @date July 06, 2016
 */

#include "ift.h"

iftDataSet *iftReadOldUnzippedOPFDataSet(const char *filename);
iftDataSet *iftReadOldOPFDataSetFormatWithoutFunctionNumber(const char *filename);
size_t iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumber(const char *filename);
int iftVerifyOldOPFDataSetFileDifference(size_t file_size_diff);

int main(int argc, const char *argv[]) {
    if (argc != 3 && argc != 4)
        iftError(
                "iftConvertOldOPFDataSet <old_dataset.data> <new_dataset.zip> [optional: function_number for old data sets without it]",
                "main");

    if(iftCompareStrings(iftFileExt(argv[1]), ".zip"))
        iftError("Data set already in the new zipped format", "main");

    iftDataSet *Z = NULL;


    if(argc == 3) {
        size_t file_size_diff = iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumber(argv[1]);
        int ok = iftVerifyOldOPFDataSetFileDifference(file_size_diff);

        if(ok == 2)
            iftError("Please verify if the file corresponds to a valid data set!", "main");
        else if(ok == 1) {
            Z = iftReadOldUnzippedOPFDataSet(argv[1]);
        } else {
            iftError("The data set does not have a function number! Please pass it as an argument", "main");
        }
    } else {
        /* If the data set is already in the new format then we simply read it using the old function number and replace it if necessary */
        Z = iftReadOldOPFDataSetFormatWithoutFunctionNumber(argv[1]);
        iftWarning("Setting the new function number as %d, before it was %d", "main", atoi(argv[3]), Z->function_number);
        iftSetDistanceFunction(Z, atoi(argv[3]));
    }


    iftWriteOPFDataSet(Z, argv[2]);
    iftDestroyDataSet(&Z);

    return 0;
}


/**
 * @brief Read a dataset file.
 * @sa iftReadDataSet().
 * @param fp File pointer to the dataset.
 * @return The loaded dataset.
 */
iftDataSet *iftReadOldUnzippedOPFDataSetFilePointer(FILE *fp) {
    iftDataSet* Z = NULL;
    int  nsamples, nclasses, nfeats, distind, function_number = IFT_NIL, s;

    if(fread(&nsamples, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of samples", "iftReadOldUnzippedOPFDataSetFilePointer");
    if(fread(&nclasses, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of classes", "iftReadOldUnzippedOPFDataSetFilePointer");
    if(fread(&nfeats, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of features", "iftReadOldUnzippedOPFDataSetFilePointer");
    if(fread(&function_number, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the function number", "iftReadOldUnzippedOPFDataSetFilePointer");

    if(function_number <= 0)
        iftError("Function number is %d, which cannot be identified as any standard distance function for OPF."
                 "Please verify the procedure for ", "iftReadOldUnzippedOPFDataSetFilePointer", function_number);

    Z = iftCreateDataSet(nsamples, nfeats);
    Z->nclasses = nclasses;

    /* Read features of each sample */

    for (s=0; s < nsamples; s++){

        if(fread(&distind, sizeof(int), 1, fp) != 1)
            iftError("Reading error", "iftReadOldUnzippedOPFDataSetFilePointer"); /* Pre-computed distances are not being used this way. */
        if(fread(&(Z->sample[s].truelabel), sizeof(int), 1, fp)!=1)
            iftError("Reading error", "iftReadOldUnzippedOPFDataSetFilePointer");

        if(fread(Z->sample[s].feat, sizeof(float), nfeats, fp)!=nfeats)
            iftError("Reading error", "iftReadOldUnzippedOPFDataSetFilePointer");

        Z->sample[s].id = s; /* for distance table access */

    }

    iftSetDistanceFunction(Z, function_number);

    return(Z);
}


iftDataSet *iftReadOldUnzippedOPFDataSet(const char *filename) {
    if (filename == NULL)
        iftError("Filename is NULL", "iftReadOldUnzippedOPFDataSet");

    FILE *fp = fopen(filename, "rb");
    if (fp == NULL)
        iftError(MSG_FILE_OPEN_ERROR, "iftReadOldUnzippedOPFDataSet", filename);

    iftDataSet *Z = iftReadOldUnzippedOPFDataSetFilePointer(fp);

    fclose(fp);

    return Z;
}


/**
 * @brief Computes the expected OPF dataset file size by reading some header fields.
 *
 * @param fp The pointer to an OPF dataset file.
 *
 * @return The expected size of the dataset file in bytes, which will differ from the last one by one integer field.
 */
size_t iftExpectedOldOPFDataSetFileSizeWithoutFunctionNumber(FILE *fp) {
    size_t expected_size = 0, header_size = 4; // 4 fields in the new header, including function_number field
    int  nsamples, nclasses, nfeats;

    rewind(fp); // Rewinding file to ensure that the file's header will be read

    if(fread(&nsamples, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of samples", "iftExpectedOldOPFDataSetFileSizeWithoutFunctionNumber");
    if(fread(&nclasses, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of classes", "iftExpectedOldOPFDataSetFileSizeWithoutFunctionNumber");
    if(fread(&nfeats, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of features", "iftExpectedOldOPFDataSetFileSizeWithoutFunctionNumber");

    expected_size = header_size * sizeof(int) + nsamples*(2*sizeof(int) + nfeats*sizeof(float));

    rewind(fp); // Rewinding file once again

    return expected_size;
}

/**
 * @brief Verifies if a file corresponds to an OPF dataset file format following the new convention.
 *
 * @return 0 if the file meets the expected size or the number of bytes that it differs from the expected value.
 */
size_t iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumberFilePointer(FILE *fp) {
    size_t expected_size, actual_size;

    expected_size = iftExpectedOldOPFDataSetFileSizeWithoutFunctionNumber(fp);

    // Computing the actual file size and rewinding pointer
    fseek(fp, 0, SEEK_END);
    actual_size = ftell(fp);
    rewind(fp); // Rewinding file

    // If the expected_size and actual_size match, then the file meets the new standard and 0 is returned
    return labs(expected_size - actual_size);
}

size_t iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumber(const char *filename) {
    FILE *fp = NULL;
    size_t diff;

    fp = fopen(filename,"rb");
    if (fp == NULL)
        iftError(MSG_FILE_OPEN_ERROR, "iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumber", filename);

    diff = iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumberFilePointer(fp);
    fclose(fp);

    return diff;
}

int iftVerifyOldOPFDataSetFileDifference(size_t file_size_diff)
{
    int ok = 0;

    if(file_size_diff == 0) {
        fprintf(stderr, "The OPF dataset file already has the function number field.");
        ok = 1;
    } else if (file_size_diff != sizeof(int)) {
        fprintf(stderr,"Dataset file does not have an expected OPF dataset file size! The size difference is of %lu bytes.", file_size_diff);
        ok = 2;
    }

    return ok;
}

iftDataSet *iftReadOldOPFDataSetFilePointerWithoutFunctionNumber(FILE *fp) {

    iftDataSet* Z = NULL;
    int  nsamples, nclasses, nfeats, distind, function_number = IFT_NIL, s;
    size_t      file_size_diff;

//    iftDeprecated("iftReadOldOPFDataSetFilePointerWithoutFunctionNumber", "iftConvertOldOPFDataSetFormatToZip",
//                  "This function does *not* read the distance function field number, and may be used to read older datasets saved prior to its creation.");

    file_size_diff = iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumberFilePointer(fp);

    int ok = iftVerifyOldOPFDataSetFileDifference(file_size_diff);
    if(ok == 2) {
        iftError("Please verify if the file corresponds to a valid data set",
                 "iftReadOldOPFDataSetFilePointerWithoutFunctionNumber");
    } else if(ok == 1) { // Reading the file with the function number
        Z = iftReadOldUnzippedOPFDataSetFilePointer(fp);

        return Z;
    }

    /* Read # of samples, classes and feats*/

    if(fread(&nsamples, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of samples", "iftReadOPFDataSet");
    if(fread(&nclasses, sizeof(int), 1, fp)!=1)
        iftError("Error when reading the number of classes", "iftReadOPFDataSet");
    if(fread(&nfeats, sizeof(int), 1, fp)!=1) iftError("Error when reading the number of features", "iftReadOPFDataSet");

    Z = iftCreateDataSet(nsamples, nfeats);
    Z->nclasses = nclasses;
    function_number = 1;

    /* Read features of each sample */

    for (s=0; s < nsamples; s++){

        if(fread(&distind, sizeof(int), 1, fp) != 1) iftError("Reading error", "iftReadOPFDataSet"); /* Pre-computed distances are not being used this way. */
        if(fread(&(Z->sample[s].truelabel), sizeof(int), 1, fp)!=1) iftError("Reading error", "iftReadOPFDataSet");

        if(fread(Z->sample[s].feat, sizeof(float), nfeats, fp)!=nfeats){
            iftError("Reading error", "iftReadOPFDataSet");
        }

        Z->sample[s].id = s; /* for distance table access */

    }

    iftSetDistanceFunction(Z, function_number);

    return(Z);
}

iftDataSet *iftReadOldOPFDataSetFormatWithoutFunctionNumber(const char *filename)
{
    FILE       *fp = NULL;

    fp = fopen(filename,"rb");
    if (fp == NULL)
        iftError(MSG_FILE_OPEN_ERROR, "iftReadOldOPFDataSetFormatWithoutFunctionNumber", filename);
    iftDataSet* Z = NULL;

    Z = iftReadOldOPFDataSetFilePointerWithoutFunctionNumber(fp);

    fclose(fp);

    return Z;
}
