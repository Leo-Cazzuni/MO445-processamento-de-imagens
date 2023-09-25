#include "ift.h"

bool compareIntegers(void *value1,void *value2)
{
    int n1 = (*(int*)value1);
    int n2 = (*(int*)value2);
    return (n1 == n2);
}

void destroyImage(void *image){
    iftImage** aux = ((iftImage**)image);
    iftDestroyImage(aux);
}


int main(int argc, char const *argv[]) {

    //generating numbers
    iftLinkedList *list = iftCreateLinkedList(sizeof(int),NULL,true);
    for(int i = 1; i <= 10; i++) {
        iftAppendElement(list,&i);
    }
    //iterate over the list nodes (foward)
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }
    printf("\n");
    iftResetIterator(list);
    //iterate over the list nodes (backwards)
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetPreviousElement(list));
        printf("%d ",a);
    }

    printf("\n");
    iftResetIterator(list);
    iftRemoveHead(list);
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }


    printf("\n");
    iftResetIterator(list);
    iftRemoveTail(list);
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }


    printf("\n");
    iftResetIterator(list);
    int n1 = 30;
    int n2 = 60;
    iftInsertElementAt(list, &n1, 3);
    iftInsertElementAt(list, &n2, 6);
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }


    printf("\n");
    iftResetIterator(list);
    iftRemoveElementAt(list, 6);
    iftRemoveElementAt(list, 3);
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }


    printf("\n");
    int n = *((int*)iftGetElement(list,5));
    printf("%d\n",n);
    iftRemoveElementByReference(list,&n);
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }


    printf("\n");
    list->comparatorFunction = compareIntegers;
    iftRemoveElementGivenValue(list, &n);
    for (unsigned  int i = 0; i < list->length*2; ++i) {
        int a = *((int*)iftGetNextElement(list));
        printf("%d ",a);
    }
    printf("\n");


    iftImage* image1 = iftReadImageByExt("../Rectangle.pgm");
    iftImage* image2 = iftReadImageByExt("../Rectangle.pgm");
    iftImage* image3 = iftReadImageByExt("../Rectangle.pgm");

    iftLinkedList *list_image = iftCreateLinkedList(sizeof(iftImage*),destroyImage,false);
    iftAppendElement(list_image,&image1);
    iftAppendElement(list_image,&image2);
    iftAppendElement(list_image,&image3);

    iftImage* aux = *((iftImage**)iftGetNextElement(list_image));
    iftWriteImageByExt(aux,"img1.pgm");

    aux = *((iftImage**)iftGetNextElement(list_image));
    for (int j = 0; j < aux->n; ++j) {
        aux->val[j] = 255 - aux->val[j];
    }
    iftWriteImageByExt(aux,"img2.pgm");

    aux = *((iftImage**)iftGetNextElement(list_image));
    for (int j = 0; j < aux->n; ++j) {
        aux->val[j] =  iftRandomInteger(0,255);
    }
    iftWriteImageByExt(aux,"img3.pgm");

    iftDestroyLinkedList(&list);
    iftDestroyLinkedList(&list_image);

    return 0;
}




