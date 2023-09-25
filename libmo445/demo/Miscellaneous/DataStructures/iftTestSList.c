/**
 * @file
 * @brief It shows how to use the String Doubly Linked List.
 * @note See the source code in @ref iftTestSList.c
 * 
 * @example iftTestSList.c
 * @brief It shows how to use the String Doubly Linked List.
 * @author Samuel Martins
 */


#include "ift.h"


int main(int argc, char const *argv[]) {
    iftSList *SL = iftCreateSList();
    iftSNode *snode          = NULL;
    char *elem               = NULL;

    iftInsertSListIntoHead(SL, "abc");
    iftInsertSListIntoTail(SL, "def");
    iftInsertSListIntoTail(SL, "ghi");
    iftInsertSListIntoTail(SL, "jkl");

    iftPrintSList(SL);
    puts("Reversed print\n");
    iftPrintReversedSList(SL);

    /* Three Ways of scaning this Linked List */
    // First - Checking if an auxiliar pointer is != NULL
    puts("FIRST WAY");
    snode = SL->head;
    while (snode != NULL) {
        // some operation with the node
        printf("elem = %s\n", snode->elem);
        snode = snode->next;
    }    
    puts("\n");

    // Second - Using the number of nodes SL->n
    puts("SECOND WAY");
    snode = SL->head;
    for (int i = 0; i < SL->n; i++) {
        // some operation with the node
        printf("elem = %s\n", snode->elem);
        snode = snode->next;
    }
    puts("\n");

    // Third - Removing each node... There will not have nodes at the end
    // When there will not any node, the function return will be NULL
    // We could remove from the TAIL too
    puts("THIRD WAY");
    elem = iftRemoveSListHead(SL);
    while (elem != NULL) {
        printf("elem = %s\n", elem);
        free(elem); // it is necessary to deallocate the returned char*
        elem = iftRemoveSListHead(SL);
    }

    // now, the Linked List is empty
    iftPrintSList(SL);


    iftDestroySList(&SL);

    return 0;
}




