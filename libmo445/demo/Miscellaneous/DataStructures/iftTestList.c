#include "ift.h"


int main(int argc, char const *argv[]) {
    iftList *L     = iftCreateList();
    iftNode *node = NULL;
    int elem;

    iftInsertListIntoHead(L, 2);
    iftInsertListIntoTail(L, 3);
    iftInsertListIntoHead(L, 1);
    iftInsertListIntoTail(L, 4);

    iftPrintList(L);
    puts("Reversed print\n");
    iftPrintReversedList(L);

    /* Three Ways of scaning this Linked List */
    // First - Checking if an auxiliar pointer is != NULL
    puts("FIRST WAY");
    node = L->head;
    while (node != NULL) {
       // some operation with the node
       printf("elem = %d\n", node->elem);
       node = node->next;
    }
    puts("\n");

    // Second - Using the number of nodes L->n
    puts("SECOND WAY");
    node = L->head;
    for (int i = 0; i < L->n; i++) {
       // some operation with the node
       printf("elem = %d\n", node->elem);
       node = node->next;
    }
    puts("\n");

    // Third - Removing each node... There will not have nodes at the end
    // When there will not any node, the function return will be NULL
    // We could remove from the TAIL too
    puts("THIRD WAY");
    while (L->n != 0) {
       elem = iftRemoveListHead(L);
       printf("elem = %d\n", elem);
    }

    // now, the Linked List is empty
    iftPrintList(L);


    iftDestroyList(&L);

    return 0;
}