//
// Created by azaelmsousa on 08/07/20.
//

#include "ift.h"

int main(int argc, char *argv[])
{
    if (argc != 4)
        iftError("Usage: iftLabelToSeeds <...>\n[1] Input label\n[2] seeds class (-1 to make labels as class or a value n to be the class\n[3] Output seeds\n","main");

    iftImage *segm = iftReadImageByExt(argv[1]);
    int n = atoi(argv[2]);

    if (n < -1)
        iftError("seed class must be higher than -1.","main");

    iftLabeledSet *S = NULL;
    for (int i = 0; i < segm->n; i++){
        if (segm->val[i] > 0){
            if (n == -1)
                iftInsertLabeledSet(&S,i,segm->val[i]);
            else
                iftInsertLabeledSet(&S,i,n);
        }
    }

    iftWriteSeeds(S,segm,argv[3]);
    iftDestroyImage(&segm);
    iftDestroyLabeledSet(&S);

    return 0;
}