/**
 * @file
 * @brief A program that converts an OPF dataset file from the old format (without field function_number, prior to Feb 27, 2016) to the new one (ending with .zip file extension).
 * @note See the source code in @ref iftConvertOldOPFDataSetFormatWithoutFunctionNumberToZip.c
 *
 * @example iftConvertOldOPFDataSetFormatWithoutFunctionNumberToZip.c
 * @brief A program that converts an OPF dataset file from the old format (without field function_number, prior to Feb 27, 2016) to the new one.
 * @author Thiago Vallin Spina
 * @date July 06, 2016
 */

#include "ift.h"


iftDataSet *iftReadOldOPFDataSetFormatWithoutFunctionNumber(const char *filename);

int main(int argc, const char *argv[]) {
    size_t memStart, memEnd;

    if (argc != 4)
        iftError("%s <old_dataset.data> <function number> <new_dataset.zip>", "main", argv[0]);


    memStart = iftMemoryUsed();

    if(iftCompareStrings(iftFileExt(argv[1]), ".zip"))
        iftError("Data set already in the new zipped format", "main");

    if(!iftCompareStrings(iftFileExt(argv[3]), ".zip"))
        iftError("Output data set filename must end with \".zip\"", "main");

    iftDataSet *Z = iftReadOldOPFDataSetFormatWithoutFunctionNumber(argv[1]);
    iftSetDistanceFunction(Z, atoi(argv[2]));
    iftWriteOPFDataSet(Z, argv[3]);

    // DESTROYERS
    iftDestroyDataSet(&Z);

    memEnd = iftMemoryUsed();

    iftVerifyMemory(memStart, memEnd);

    return 0;
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
size_t iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumber(FILE *fp) {
    size_t expected_size, actual_size;

    expected_size = iftExpectedOldOPFDataSetFileSizeWithoutFunctionNumber(fp);

    // Computing the actual file size and rewinding pointer
    fseek(fp, 0, SEEK_END);
    actual_size = ftell(fp);
    rewind(fp); // Rewinding file

    // If the expected_size and actual_size match, then the file meets the new standard and 0 is returned
    return labs(expected_size - actual_size);
}

iftDataSet *iftReadOldOPFDataSetFilePointerWithoutFunctionNumber(FILE *fp) {

    iftDataSet* Z = NULL;
    int  nsamples, nclasses, nfeats, distind, function_number = IFT_NIL, s;
    size_t      file_size_diff;

//    iftDeprecated("iftReadOldOPFDataSetFilePointerWithoutFunctionNumber", "iftConvertOldOPFDataSetFormatToZip",
//                  "This function does *not* read the distance function field number, and may be used to read older datasets saved prior to its creation.");

    file_size_diff = iftVerifyOldOPFDataSetFileFormatWithoutFunctionNumber(fp);

    if(file_size_diff == 0) {
        iftError(
                "The OPF dataset file already has the function number field. Please use iftConvertOPFDataSetToZipFile to convert it to the new zip format.",
                "iftReadOldOPFDataSetFilePointerWithoutFunctionNumber");
    } else if (file_size_diff != sizeof(int)) {
        iftError("Dataset file does not have an expected OPF dataset file size! The size difference is of %lu bytes.",
                 "iftReadOldOPFDataSetFilePointerWithoutFunctionNumber", file_size_diff);
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
        iftError(MSG_FILE_OPEN_ERROR, "iftReadOPFDataSet", filename);
    iftDataSet* Z = NULL;

    Z = iftReadOldOPFDataSetFilePointerWithoutFunctionNumber(fp);

    fclose(fp);

    return Z;
}

/******************************************************************/