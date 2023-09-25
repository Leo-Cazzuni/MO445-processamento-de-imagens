//
// Created by Cesar Castelo on Mar 13, 2018
//

#include <ift.h>


int main(int argc, char** argv)
{
    if(argc != 4) {
        iftError("\nUsage: iftRemoveClassFromDataSet <...>\n"
                         "[1] inputDataSet: input dataset\n"
                         "[2] classId: class to be removed\n"
                         "[3] outputDataSet: output dataset\n",
                 "iftRemoveClassFromDataSet.c");
    }

    printf("- Reading dataset ...\n"); fflush(stdout);
    iftDataSet *ds1 = iftReadDataSet(argv[1]);
    printf("- Removing samples from class %d ...\n", atoi(argv[2])); fflush(stdout);
    iftDataSet *ds2 = iftRemoveClassFromDataSet(ds1, atoi(argv[2]));
    printf("- Writing dataset ...\n"); fflush(stdout);
    iftWriteDataSet(ds2, argv[3]);

    iftDestroyDataSet(&ds1);
    iftDestroyDataSet(&ds2);

    return 0;
}