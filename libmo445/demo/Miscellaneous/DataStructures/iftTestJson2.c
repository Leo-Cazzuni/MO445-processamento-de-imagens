#include "ift.h"


/**
 * An example of how to create a Json and add elements to it.
 * This created json is stored in "./json/out.json" and "./json/out_minified.json".
 */
int main(int argc, const char *argv[]) {
    iftJson *json = iftCreateJsonRoot();

    /******** Adding Basic Elements *********/
    iftAddStringToJson(json, "name", "Steven Spielberg");
    iftAddIntToJson(json, "age", 68);
    iftAddDoubleToJson(json, "height", 1.78);
    iftAddBoolToJson(json, "active", 1);
    iftAddNullToJson(json, "null_value");
    puts("- Main Json");
    iftPrintJson(json);

    // /******** Adding Dictionaries *********/
    // one way of passing JDict is to pass a reference to Json
    // there is no memory leak, because the Json incorporates the reference
    iftAddJDictReferenceToJson(json, "movies", iftCreateJDict());

    // another way of passing JDict reference to Json
    iftJDict *jdict_ref = iftCreateJDict();
    iftAddStringToJson(jdict_ref, "title", "E.T. the Extra-Terrestrial");
    iftAddIntToJson(jdict_ref, "year", 1982);
    iftAddJDictReferenceToJson(json, "movies:movie01", jdict_ref);
    // DO NOT DEALLOCATE the jdict REFERENCE, otherwise the json will get inconsistent
    jdict_ref = NULL;

    puts("\n- Added JDict References to the Main Json");
    iftPrintJson(json);

    iftJDict *jcopy = iftCreateJDict();
    iftAddStringToJson(jcopy, "title", "Jurassic Park");
    iftAddIntToJson(jcopy, "number", 1993);
    iftAddJDictToJson(json, "movies:movie02", jcopy);
    iftDestroyJson(&jcopy);

    puts("\n- Added a JDict to the Main Json by copying it into");
    iftPrintJson(json);
    /*********************************/


    /******** Adding Arrays *********/
    iftIntArray *iarr = iftCreateIntArray(5);
    for (int i = 0; i < iarr->n; i++)
        iarr->val[i] = i;

    iftDblArray *darr = iftCreateDblArray(6);
    for (int i = 0; i < darr->n; i++)
        darr->val[i] = (i + (i*0.10));

    iftStrArray *sarr = iftCreateStrArray(4);
    strcpy(sarr->val[0], "First String");
    strcpy(sarr->val[1], "Second String");
    strcpy(sarr->val[2], "Third String");
    strcpy(sarr->val[3], "Fourth String");

    // there is no memory leak, because the Json incorporates the reference
    iftAddJDictReferenceToJson(json, "arrays", iftCreateJDict());
    iftAddIntArrayToJson(json, "arrays:integer", iarr);
    iftAddFloatArrayToJson(json, "arrays:double", darr);
    iftAddStringArrayToJson(json, "arrays:string", sarr);

    iftDestroyIntArray(&iarr);
    iftDestroyDblArray(&darr);
    iftDestroyStrArray(&sarr);
    
    puts("\n- Added Int, Double, and String Arrays to the Main Json");
    iftPrintJson(json);


    // Adding elements directly into an Array (without passing any key)
    iftJNode *jarray = iftGetJNodeReference(json, "arrays:integer");
    iftAddIntToJArrayNode(jarray, 1000);
    jarray = NULL;
    // there is no memory leak because the Json Array Nodes used are just references
    iftAddDoubleToJArrayNode(iftGetJNodeReference(json, "arrays:double"), 0.111111);
    iftAddStringToJArrayNode(iftGetJNodeReference(json, "arrays:string"), "THAT'S A STRING");

    // Adding elements into an JArray passing the key of the JArray
    iftAddIntToJArray(json, "arrays:integer", 99999);
    iftAddDoubleToJArray(json, "arrays:double", 0.99999);
    iftAddStringToJArray(json, "arrays:string", "LAST STRING");

    puts("\n- Added Int, Double, and String values into the arrays in the Main Json");
    iftPrintJson(json);
    /*********************************/


    /******** Adding Matrices *********/
    iftIntMatrix *iM = iftCreateIntMatrix(2, 3);
    iM->val[0] = 0;  iM->val[1] = 1;
    iM->val[2] = 10; iM->val[3] = 11;
    iM->val[4] = 20; iM->val[5] = 21;

    iftMatrix *dM = iftCreateMatrix(3, 2);
    dM->val[0] = 0.25;  dM->val[1] = 1.50; dM->val[2] = 2.75;
    dM->val[3] = 10.25; dM->val[4] = 11.5; dM->val[5] = 11.75;

    iftStringMatrix *sM = iftCreateStringMatrix(1, 4);
    strcpy(sM->val[0], "apple");
    strcpy(sM->val[1], "banana");
    strcpy(sM->val[2], "grape");
    strcpy(sM->val[3], "orange");

    // there is no memory leak, because the Json incorporates the reference
    iftAddJDictReferenceToJson(json, "matrices", iftCreateJDict());
    iftAddIntMatrixToJson(json, "matrices:integer", iM);
    iftAddDoubleMatrixToJson(json, "matrices:double", dM);
    iftAddStringMatrixToJson(json, "matrices:string", sM);

    iftDestroyIntMatrix(&iM);
    iftDestroyMatrix(&dM);
    iftDestroyStringMatrix(&sM);

    puts("\n- Added Int, Double, and String Matrices to the Main Json");
    iftPrintJson(json);
    /*********************************/


    /******** Adding a Directory of Files To Json *********/
    iftDir *dir = iftLoadDir(".", 1); // loads all files of the first level from the current dir
    iftJNode *jarray2 = iftCreateJArray(); // creates a JNode with type = IFT_JSON_ARRAY
    for (int i = 0; i < dir->nfiles; i++)
        iftAddStringToJArrayNode(jarray2, dir->files[i]->path);
    iftAddJNodeReferenceToJson(json, "pathnames", jarray2);

    puts("\n- Added a Directory of Pathnames to the Main Json");
    iftPrintJson(json);
    /*********************************/


    /******** Changing existent keys *********/
    // sets an existent key
    iftAddIntToJson(json, "age", 100);
    // sets an existent key of a DIFFERENT type
    iftAddStringToJson(json, "active", "It is active");

    puts("\n- Changed an existent key from Main Json with a same and a different type: Assigned \"age\" and \"name\"");
    iftPrintJson(json);
    /*********************************/


    /******** Deleting some Elements *********/
    iftDeleteJNode(json, "active");
    iftDeleteJNode(json, "arrays:integer");
    iftDeleteJNode(json, "matrices");
    puts("\n- Deleted the elements with keys: \"active\", \"arrays:integer\", and \"matrices\"");
    iftPrintJson(json);
    /*********************************/

    /******** Saving the final JSON *********/
    iftWriteJson(json, "./json/out.json");
    iftWriteJsonMinified(json, "./json/out_minified.json");
    /*********************************/

    iftDestroyJson(&json);

    return 0;
}









