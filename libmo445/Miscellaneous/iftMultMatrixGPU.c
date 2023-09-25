//
// Created by peixinho on 2/26/16.
//

#include <ift.h>

#define N 2000

int main() {

    int i;
    size_t mem1 = iftMemoryUsed();

    //Called if you want to start CUDA device before the first function call.
    iftInitGPU();

    timer* t1;
    timer* t2;

    printf("Multiplying %dx%d matrices.\n", N, N);

    iftMatrix* m = iftCreateMatrix(N, N);
    iftMatrix* m2 = iftCreateMatrix(N, N);

    for (i = 0; i < m->n; ++i) {
        m->val[i] = iftRandomInteger(1,10);
    }

    for (i = 0; i < m2->n; ++i) {
        m2->val[i] = iftRandomInteger(1,10);
    }

    t1 = iftTic();
    iftMatrix* m3 = iftMultMatrices(m, m2);
    t2 = iftToc();
    printf("CPU Multiplication ");
    iftPrintFormattedTime(iftCompTime(t1, t2));

    t1 = iftTic();
    iftMatrix* m4 = iftMultMatricesGPU(m, m2);
    t2 = iftToc();
    printf("GPU Multiplication ");
    iftPrintFormattedTime(iftCompTime(t1, t2));

    iftDestroyMatrix(&m);
    iftDestroyMatrix(&m2);
    iftDestroyMatrix(&m3);
    iftDestroyMatrix(&m4);

    iftStopGPU();
    size_t mem2 = iftMemoryUsed();

    iftVerifyMemory(mem1, mem2);

    return 0;

}
