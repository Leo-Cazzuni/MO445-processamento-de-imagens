
#include "unitytest.h"




void testMatrixSum() {
    float va[9] = {1, 1, 1, 2, 2, 2, 3, 3, 3};
    float vb[9] = {1, 1, 1, 2, 2, 2, 3, 3, 3};
    float vc[9] = {2, 2, 2, 4, 4, 4, 6, 6, 6};

    iftMatrix *A = iftCreateMatrix(3, 3);
    iftMatrix *B = iftCreateMatrix(3, 3);
    iftMatrix *C = iftCreateMatrix(3, 3);

    float *old_va = A->val;
    float *old_vb = B->val;
    float *old_vc = C->val;
    A->val = va;    
    B->val = vb;
    C->val = vc;

    iftMatrix *AB = iftSumMatrices(A, B);
    LETS_ASSERT("Invalid Sum of Matrices", iftIsMatricesEqual(A, B));

    // cleaning up
    A->val = B->val = C->val = NULL;
    iftDestroyMatrix(&A);
    iftDestroyMatrix(&B);
    iftDestroyMatrix(&C);
    iftDestroyMatrix(&AB);
    iftFree(old_va);
    iftFree(old_vb);
    iftFree(old_vc);
}





int main(int argc, const char *argv[]) {
    letsInitTestSuit();

    LETS_TEST(testMatrixSum);

    letsPrintTestSuitStats(__FILE__);

    return 0;
}









