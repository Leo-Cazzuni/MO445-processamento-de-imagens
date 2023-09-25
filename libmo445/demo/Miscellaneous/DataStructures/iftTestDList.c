/**
 * @file iftTestDList.c
 * @brief Simple demo to test a Doubly Linked-List of doubles.
 * @note See the source code in @ref iftTestDList.c
 *
 * @example iftTestDList.c
 * @brief Simple demo to test a Doubly Linked-List of doubles.
 * @author Samuel Martins
 * @date Sep 19, 2015
 */

#include "ift.h"


int main(int argc, char const *argv[]) {
    iftDList *L     = iftCreateDList();
    iftDNode *dnode = NULL;
    double elem;

    iftInsertDListIntoHead(L, 2.0);
    iftInsertDListIntoTail(L, 3.0);
    iftInsertDListIntoHead(L, 1.0);
    iftInsertDListIntoTail(L, 4.0);

    iftPrintDList(L);
    puts("Reversed print\n");
    iftPrintReversedDList(L);

    /* Three Ways of scaning this Linked DList */
    // First - Checking if an auxiliar pointer is != NULL
    puts("FIRST WAY");
    dnode = L->head;
    while (dnode != NULL) {
       // some operation with the node
       printf("elem = %lf\n", dnode->elem);
       dnode = dnode->next;
    }
    puts("\n");

    // Second - Using the number of nodes L->n
    puts("SECOND WAY");
    dnode = L->head;
    for (int i = 0; i < L->n; i++) {
       // some operation with the node
       printf("elem = %lf\n", dnode->elem);
       dnode = dnode->next;
    }
    puts("\n");

    // Third - Removing each node... There will not have nodes at the end
    // When there will not any node, the function return will be NULL
    // We could remove from the TAIL too
    puts("THIRD WAY");
    while (L->n != 0) {
       elem = iftRemoveDListHead(L);
       printf("elem = %lf\n", elem);
    }

    // now, the Linked DList is empty
    iftPrintDList(L);


    iftDestroyDList(&L);

    return 0;
}