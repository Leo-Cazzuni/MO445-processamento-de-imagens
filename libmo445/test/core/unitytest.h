/**
 * Library for Unity Test in C.
 * @author Samuel Martins
 * @date Dec 13, 2016
 *
 * It runs each test function (executed by LETS_TEST) in a child process (by fork()),
 * in order to avoid to abort the test suit if errors, exits, segmentation faults occur.
 */

#ifndef UNITY_TEST_H
#define UNITY_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdio.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "ift.h"

#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"
#define KRESET "\033[0m"

/**
 * Declaration of global variables to control the Test Suit statistics.
 * Used pointers because the memory mapping function mmap(), used to share such global variables
 * between child and parent process (fork()), works with void*.
 */
extern int *g_n_tests;
extern int *g_n_assertions;
extern int *g_n_fails;
extern int *g_n_errors; // caused by iftError in functions


#define LETS_ASSERT(msg, test) \
            do { \
                (*g_n_assertions)++; \
                if (!(test)) { \
                    (*g_n_fails)++; \
                    fprintf(stderr, KRED "#%d) Failure:\n" KRESET, (*g_n_fails)); \
                    fprintf(stderr, KCYN "  %s - %s() line %d\n" KRESET, __FILE__, __func__, __LINE__); \
                    fprintf(stderr, "  %s\n", msg); \
                    fprintf(stderr, "--------------\n"); \
                } \
            } while (0)


#define LETS_TEST(test) \
            do { \
                (*g_n_tests)++; \
                pid_t pid = fork(); \
                if (pid == -1) { perror("Problems when executing fork()"); } \
                else { \
                    int curr_n_errors = (*g_n_errors); \
                    LETS_TEST_ON_FORK(test, pid); \
                    if (curr_n_errors != (*g_n_errors)) { /* an error occurred */ \
                        fprintf(stderr, KRED "#%d) Error:\n" KRESET, (*g_n_errors)); \
                        fprintf(stderr, KCYN "  %s - %s() line %d\n" KRESET, __FILE__, __func__, __LINE__); \
                        fprintf(stderr, "--------------\n"); \
                    } \
                } \
            } while (0)


#define LETS_TEST_ON_FORK(test, pid) \
            do { \
                /* Child Process: run the Test Function */ \
                if (pid == 0) { \
                    (*g_n_errors)++; /* it suppose that an error will occur in function (e.g: due to segmentation, iftError, ...) */ \
                    test(); \
                    /* if the child process gets here, it still live and no error occurred */ \
                    /* then, we remove the supposed error and the child process kills itself */ \
                    (*g_n_errors)--; \
                    kill(getpid(), SIGKILL); \
                } \
                /* Parent Process: only waits the execution of the Child Pŕocess */ \
                else { \
                    int status; \
                    /* waits for the Child Process. In our case, the child process always will die. */ \
                    if (wait(&status) == -1) { perror("Problems when executing wait()"); } \
                } \
            } while (0)


/**
 * Function to initialize the (global shared) statistic parameters of the Test Suit.
 * It must be called before running the tests.
 */
void letsInitTestSuit();

// Prints the Test Suit Statistics
void letsPrintTestSuitStats();


#ifdef __cplusplus
}
#endif

#endif
