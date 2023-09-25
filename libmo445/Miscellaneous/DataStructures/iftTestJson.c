#include "ift.h"

/**
   An example of Json Reading and Writing and how to get elements from the Json.
   This example opens the file "ift_dir/demo/Miscellaenous/json/example.json".
 */

int main(int argc, char **argv) {
    if (argc != 2)
        iftError("iftTestJson1 <input_json> (Use the file: \"ift_dir/demo/Miscellaenous/json/example.json\"", "main");
    char *json_pathname = iftCopyString(argv[1]);

    iftDict *json = iftReadJson(json_pathname);
    json->erase_elements = true; // enables the iftDestroyDict to deallocate all its elements

    iftPrintDict(json);
    iftPrintDictMinified(json);
    puts("\n");

    bool activate     = iftGetBoolValFromDict("activate", json);
    void *null_value  = iftGetPtrValFromDict("null_value", json);
    char *paper_title = iftGetStrValFromDict("papers:paper01:title", json);
    int year          = iftGetLongValFromDict("papers:paper01:year", json);
    double impact     = iftGetDblValFromDict("papers:paper01:impact", json);

    printf("User Activate: %s, null_value? %d - paper_title: %s, year: %d, impact: %f\n",
        iftBoolAsString(activate), null_value==NULL, paper_title, year, impact);
    free(paper_title);
    puts("\n");

    // When getting an array (or a matrix or a dict), it gets its reference and not a copy
    // Be careful, because if you deallocate the variable, its reference inserted into
    // dict will be pointing to an invalid region
    // If you set dict->erase_elementes = true, the iftDestroyDict will deallocate all its elements,
    // then, you don't need to do that with the variables
    
    puts("paper02 - JDict Copy");
    iftDict *papers_dict = iftGetDictFromDict("papers:paper02", json);
    iftPrintDict(papers_dict);
    puts("\n");

    puts("Integer Array");
    iftIntArray *iarr = iftGetIntArrayFromDict("arrays:integer", json);
    printf("n = %lu\n", iarr->n);
    for (int i = 0; i < iarr->n; i++)
        printf("[%d] = %d\n", i, iarr->val[i]);

    puts("\nDouble Array");
    iftDblArray *darr = iftGetDblArrayFromDict("arrays:double", json);
    printf("n = %lu\n", darr->n);
    for (int i = 0; i < darr->n; i++)
        printf("[%d] = %f\n", i, darr->val[i]);

    puts("\nString Array");
    iftStrArray *sarr = iftGetStrArrayFromDict("arrays:string", json);
    printf("n = %lu\n", sarr->n);
    for (int i = 0; i < sarr->n; i++)
        printf("[%d] = %s\n", i, sarr->val[i]);

    puts("\nInteger Matrix");
    iftIntMatrix *iM = iftGetIntMatrixFromDict("matrices:integer", json);
    for (int i = 0; i < iM->nrows; i++) {
        for (int j = 0; j < iM->ncols; j++)
            printf("%d ", iM->val[iftGetMatrixIndex(iM, j, i)]);
        puts("");
    }

    puts("\nDouble Matrix");
    iftMatrix *dM = iftGetDblMatrixFromDict("matrices:double", json);
    for (int i = 0; i < dM->nrows; i++) {
        for (int j = 0; j < dM->ncols; j++)
            printf("%f ", dM->val[iftGetMatrixIndex(dM, j, i)]);
        puts("");
    }

    puts("\nString Matrix");
    iftStrMatrix *sM = iftGetStrMatrixFromDict("matrices:string", json);
    for (int i = 0; i < sM->nrows; i++) {
        for (int j = 0; j < sM->ncols; j++)
            printf("\"%s\" ", sM->val[iftGetMatrixIndex(sM, j, i)]);
        puts("");
    }

    iftFree(json_pathname);

    puts("- Writing Json in \"./out.json\"");
    iftWriteJson(json, "./out.json");

    puts("- Writing Json Minify in \"./out_minify.json\"");
    iftWriteJsonMinified(json, "./out_minify.json");

    iftDestroyDict(&json);

    return 0;
}
