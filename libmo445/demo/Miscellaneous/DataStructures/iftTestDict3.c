#include "ift.h"


/**
 * Example that show how to insert and get sub-keys into a dict.
 */
int main(int argc, const char *argv[]) {
    iftDict *dict = iftCreateDict();

    iftInsertIntoDict("name", "batman", dict);
    iftInsertIntoDict(10, "Ten", dict);
    iftInsertIntoDict("sub:sub-sub:test", 100, dict);
    iftInsertIntoDict("sub:sub-sub:yeah", "yeah", dict);
    iftInsertIntoDict("sub:abc", 5.5, dict);

    iftPrintDict(dict);

    printf("\nname: %s\n", iftGetStrValFromDict("name", dict));
    printf("abc: %f\n", iftGetDblValFromDict("sub:abc", dict));
    printf("test: %ld\n", iftGetLongValFromDict("sub:sub-sub:test", dict));

    iftWriteJson(dict, "test.json");

    iftDestroyDict(&dict);

    return 0;
}



