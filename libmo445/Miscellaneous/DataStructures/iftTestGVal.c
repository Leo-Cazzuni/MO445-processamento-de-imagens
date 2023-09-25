/**
 * @file
 * @brief It shows how to use the Generic Value (iftGVal) 
 * @note See the source code in @ref iftTestGVal.c
 * 
 * @example iftTestGVal.c
 * @brief It shows how to use the Generic Value (iftGVal) 
 * @author Samuel Martins
 * @date Feb 1st, 2016
 */

#include "ift.h"


int main(int argc, const char *argv[]) {
    iftGVal gv;

    gv = iftCreateGVal(10);
    printf("gv = %ld\n", gv.long_val);

    gv = iftCreateGVal(10L);
    printf("gv = %ld\n", gv.long_val);

    gv = iftCreateGVal(10.5f);
    printf("gv = %lf\n", gv.dbl_val);

    gv = iftCreateGVal(10.5);
    printf("gv = %lf\n", gv.dbl_val);

    // it interprets the value as an integer, then its type is INT_TYPE
    // but, the print works normally
    gv = iftCreateGVal('a');
    printf("gv = %c\n", gv.char_val);

    gv = iftCreateGVal((char) 'a');
    printf("gv = %c\n", gv.char_val);

    // it's created a COPY... then, YOU MUST DEALLOCATE THE STRING
    gv = iftCreateGVal("literal string");
    printf("gv = %s\n", gv.str_val);
    free(gv.str_val);

    iftIntArray *iarr = iftCreateIntArray(3);
    iarr->val[0] = 10;
    iarr->val[1] = 11;
    iarr->val[2] = 12;
    gv = iftCreateGVal(iarr);
    iftIntArray *ref_iarr = iftGetIntArrayVal(gv);
    printf("gv = [%d, %d, %d]\n", ref_iarr->val[0], ref_iarr->val[1], ref_iarr->val[2]);
    iftDestroyIntArray(&iarr);

    iftDblArray *darr = iftCreateDblArray(3);
    darr->val[0] = 10.5;
    darr->val[1] = 11.5;
    darr->val[2] = 12.5;
    gv = iftCreateGVal(darr);
    iftDblArray *ref_darr = iftGetDblArrayVal(gv);
    printf("gv = [%lf, %lf, %lf]\n", ref_darr->val[0], ref_darr->val[1], ref_darr->val[2]);
    iftDestroyDblArray(&darr);

    iftStrArray *sarr = iftCreateStrArray(3);
    strcpy(sarr->val[0], "zero");
    strcpy(sarr->val[1], "first");
    strcpy(sarr->val[2], "second");
    gv = iftCreateGVal(sarr);
    iftStrArray *ref_sarr = iftGetStrArrayVal(gv);
    printf("gv = [%s, %s, %s]\n", ref_sarr->val[0], ref_sarr->val[1], ref_sarr->val[2]);
    iftDestroyStrArray(&sarr);

    iftIntMatrix *imat = iftCreateIntMatrix(2, 2);
    imat->val[0] = 0;
    imat->val[1] = 1;
    imat->val[2] = 2;
    imat->val[3] = 3;
    gv = iftCreateGVal(imat);
    iftIntMatrix *ref_imat = iftGetIntMatrixVal(gv);
    printf("[%d, %d]\n[%d, %d]\n", ref_imat->val[0], ref_imat->val[1], ref_imat->val[2], ref_imat->val[3]);
    iftDestroyIntMatrix(&imat);

    iftMatrix *mat = iftCreateMatrix(2, 2);
    mat->val[0] = 0.5;
    mat->val[1] = 1.5;
    mat->val[2] = 2.5;
    mat->val[3] = 3.5;
    gv = iftCreateGVal(mat);
    iftMatrix *ref_mat = iftGetDblMatrixVal(gv);
    printf("[%f, %f]\n[%f, %f]\n", ref_mat->val[0], ref_mat->val[1], ref_mat->val[2], ref_mat->val[3]);
    iftDestroyMatrix(&mat);

    iftStrMatrix *smat = iftCreateStrMatrix(2, 2);
    strcpy(smat->val[0], "00");
    strcpy(smat->val[1], "01");
    strcpy(smat->val[2], "10");
    strcpy(smat->val[3], "11");
    gv = iftCreateGVal(smat);
    iftStrMatrix *ref_smat = iftGetStrMatrixVal(gv);
    printf("[%s, %s]\n[%s, %s]\n", ref_smat->val[0], ref_smat->val[1], ref_smat->val[2], ref_smat->val[3]);
    iftDestroyStrMatrix(&smat);



    return 0;
}







