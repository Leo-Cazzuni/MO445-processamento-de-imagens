/**
 * based on
 * http://eradman.com/posts/tdd-in-c.html
 * http://www.jera.com/techinfo/jtns/jtn002.html
 */

#include "unitytest.h"

// global variables to control the test suit statistics
int *g_n_tests;
int *g_n_assertions;
int *g_n_fails;
int *g_n_errors;



void letsInitTestSuit() {
    g_n_tests      = mmap(NULL, sizeof(*g_n_tests), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    g_n_assertions = mmap(NULL, sizeof(*g_n_assertions), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    g_n_fails      = mmap(NULL, sizeof(*g_n_fails), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
    g_n_errors     = mmap(NULL, sizeof(*g_n_errors), PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);

    *g_n_tests      = 0;
    *g_n_assertions = 0;
    *g_n_fails      = 0;
    *g_n_errors     = 0;
}



void letsPrintTestSuitStats(const char *test_suit_name) {
    fprintf(stdout, "\n\nTest Suit: ");
    fprintf(stdout, KCYN "%s\n" KRESET, test_suit_name);

    if ((*g_n_fails == 0) && (*g_n_errors == 0)) {
        fprintf(stdout, KGRN "%d tests, %d assertions, %d failures, %d errors\n" KRESET,
                *g_n_tests, *g_n_assertions, *g_n_fails, *g_n_errors);
    }
    else {
        fprintf(stdout, KRED "%d tests, %d assertions, %d failures, %d errors\n" KRESET,
                *g_n_tests, *g_n_assertions, *g_n_fails, *g_n_errors);
    }
    puts("...........");
}



