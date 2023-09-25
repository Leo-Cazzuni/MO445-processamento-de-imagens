#include "ift.h"

/**
   An example of Json Reading and Writing and how to get elements from the Json.
   This example opens the file "ift_dir/demo/Miscellaenous/json/example.json".
 */

int main(int argc, char **argv) {
    if (argc != 2)
        iftError("iftTestJson1 <input_json> (Use the file: \"ift_dir/demo/Miscellaenous/json/example.json\"", "main");
    char json_pathname[512];
    strcpy(json_pathname, "./json/example.json");

    iftJson *json = iftReadJson(json_pathname);

    iftPrintJson(json);
    iftPrintJsonMinified(json);
    puts("\n");

    unsigned int activate = iftGetJBool(json, "activate");
    void *null_value      = iftGetJNull(json, "null_value");
    char *paper_title     = iftGetJString(json, "papers:paper01:title");
    int year              = iftGetJInt(json, "papers:paper01:year");
    double impact         = iftGetJDouble(json, "papers:paper01:impact");

    printf("User Activate: %d, null_value? %d - paper_title: %s, year: %d, impact: %f\n",
        activate, null_value==NULL, paper_title, year, impact);
    free(paper_title);
    puts("\n");

    puts("paper02 - JDict Copy");
    iftJDict *jdict_copy = iftGetJDict(json, "papers:paper02");
    iftPrintJson(jdict_copy);
    puts("\n");
    iftDestroyJson(&jdict_copy);

    puts("paper02 - JDict Reference");
    iftJDict *jdict_ref = iftGetJDictReference(json, "papers:paper02");
    iftPrintJson(jdict_ref);
    puts("\n");

    puts("Integer Array");
    iftIntArray *iarr = iftGetJIntArray(json, "arrays:integer");
    printf("n = %lu\n", iarr->n);
    for (int i = 0; i < iarr->n; i++)
        printf("[%d] = %d\n", i, iarr->val[i]);

    puts("\nDouble Array");
    iftDblArray *darr = iftGetJFloatArray(json, "arrays:double");
    printf("n = %lu\n", darr->n);
    for (int i = 0; i < darr->n; i++)
        printf("[%d] = %f\n", i, darr->val[i]);

    puts("\nString Array");
    iftStrArray *sarr = iftGetJStringArray(json, "arrays:string");
    printf("n = %lu\n", sarr->n);
    for (int i = 0; i < sarr->n; i++)
        printf("[%d] = %s\n", i, sarr->val[i]);

    puts("\nInteger Matrix");
    iftIntMatrix *iM = iftGetJIntMatrix(json, "matrices:integer");
    for (int i = 0; i < iM->nrows; i++) {
        for (int j = 0; j < iM->ncols; j++)
            printf("%d ", iM->val[iftGetMatrixIndex(iM, j, i)]);
        puts("");
    }

    puts("\nDouble Matrix");
    iftDoubleMatrix *dM = iftGetJFloatMatrix(json, "matrices:double");
    for (int i = 0; i < dM->nrows; i++) {
        for (int j = 0; j < dM->ncols; j++)
            printf("%f ", dM->val[iftGetMatrixIndex(dM, j, i)]);
        puts("");
    }

    puts("\nString Matrix");
    iftStringMatrix *sM = iftGetJStringMatrix(json, "matrices:string");
    for (int i = 0; i < sM->nrows; i++) {
        for (int j = 0; j < sM->ncols; j++)
            printf("\"%s\" ", sM->val[iftGetMatrixIndex(sM, j, i)]);
        puts("");
    }

    iftDestroyIntArray(&iarr);
    iftDestroyDblArray(&darr);
    iftDestroyStrArray(&sarr);
    iftDestroyIntMatrix(&iM);
    iftDestroyMatrix(&dM);
    iftDestroyStringMatrix(&sM);

    iftDestroyJson(&json);

    return 0;
}
