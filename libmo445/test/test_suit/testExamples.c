/**
 * Examples of Unity Test.
 * The functions CANNOT HAVE PARAMETERS.
 * All data used by the function must be created/loaded inside it.
 * Use the directory ift_root/test/data to store small data, such as images, datasets, etc.
 */

#include "unitytest.h"


// The Test functions CANNOT have parameters and must return void
void testSum() {
    int a = 10;
    int b = 5;
    int sum = a + b;

    // first param is a Message for Failure
    // second param is a boolean --- resulted of a comparison --- to be tested, which should be true
    LETS_ASSERT("Invalid Sum", sum == 15);
}


void testSub() {
    int a = 10;
    int b = 5;
    int sub1 = a - b;
    int sub2 = b - a;

    // you can declate as assertions as you wish
    LETS_ASSERT("Invalid Subtraction: 10 - 5 != 5", sub1 == 5);
    LETS_ASSERT("Invalid Subtraction: 5 - 10 != -5", sub2 == -5);
}


void testFail() {
    int a = 11;
    int b = 5;
    int sub = a - b;

    char msg[512];
    sprintf(msg, "Invalid Subtraction: %d - %d = %d should be 5", a, b, sub);

    // you can declate as assertions as you wish
    LETS_ASSERT(msg, sub == 5);
}


// it will cause an error
void testReadImage() {
    iftImage *img = NULL;
    img = iftReadImage("aaa.scn");
    LETS_ASSERT("AAAA", img != NULL);
}



// test suit for the functions implemented in this file
int main(int argc, const char *argv[]) {
    // Call this function to initialize the test suit
    letsInitTestSuit();

    // functions to be tested (they must be: void your_test_function())
    LETS_TEST(testSum);
    LETS_TEST(testSub);
    LETS_TEST(testFail); //uncomment it to see a FAILURE in the test suit
    // LETS_TEST(testReadImage); //uncomment it to see an ERROR in the test suit

    // prints the statistics of the test suit
    // __FILE__ is the filename of this test suit 
    letsPrintTestSuitStats(__FILE__);

    return 0;
}




