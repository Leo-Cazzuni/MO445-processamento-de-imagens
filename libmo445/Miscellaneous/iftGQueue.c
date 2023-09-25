#include "ift.h"


void iftPrintGQueue(const iftGQueue *Q) {
    printf("minvalue: %d\n", Q->C.minvalue);
    printf("maxvalue: %d\n", Q->C.maxvalue);
    printf("nbuckets: %d\n", Q->C.nbuckets);
    for (int bucket = 0; bucket <= Q->C.nbuckets; bucket++) {
        printf("[%d] = [", bucket);
        int i = Q->C.first[bucket];

        while (i != IFT_NIL) {
            printf("%d, ", i);
            i = Q->L.elem[i].next;
        }
        puts("]");
    }
    puts("");
}



int main(int argc, const char *argv[]) {
    int n = 21;

    int v[n];
    for (int i = 0; i < n; i++) {
        v[i] = i;
    }

    iftGQueue *Q = iftCreateGQueue(n, n, v);

    for (int i = 0; i < n; i++) {
        iftInsertGQueue(&Q, i);
    }
    iftPrintGQueue(Q);
    puts("\n\n");

    printf("p = %d\n", iftRemoveGQueue(Q));
    printf("p = %d\n", iftRemoveGQueue(Q));
    printf("p = %d\n", iftRemoveGQueue(Q));
    iftRemoveGQueueElem(Q, 100);
    
    iftPrintGQueue(Q);


    return 0;
}





