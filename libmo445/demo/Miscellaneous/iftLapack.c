#include "ift.h"

/* SGESDD prototype */
extern void sgesdd_( char* jobz, int* m, int* n, float* a,
int* lda, float* s, float* u, int* ldu, float* vt, int* ldvt,
float* work, int* lwork, int* iwork, int* info );
#define M 10
#define N 2
#define LDA M
#define LDU M
#define LDVT N

/* Main program */
int main() {
/* Locals */
int m = M, n = N, lda = LDA, ldu = LDU, ldvt = LDVT, info, lwork;
float wkopt;
float* work;
/* Local arrays */
/* iwork dimension should be at least 8*min(m,n) */
int iwork[8*N];
static float s[N], u[LDU*M], vt[LDVT*N];
static float a[LDA*N];

int i;
FILE *f;
float number;
time_t t1,t2;

/* Read numbers from matrix file into a variable */
f=fopen("matrix.txt","r");

if (!f) {
fprintf(stderr, "Unable to open input file \n");
return 1;
}

for(i=0; !feof(f); i++) {
fscanf(f,"%f\n", &number);
a[i] = number;
}

fclose(f);

(void) time(&t1);


/* Executable statements */
printf( " SGESDD Example Program Results\n" );
/* Query and allocate the optimal workspace */
lwork = -1;
sgesdd_( "Singular vectors", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, &wkopt,
&lwork, iwork, &info );
lwork = (int)wkopt;
work = (float*)malloc( lwork*sizeof(float) );
/* Compute SVD */
sgesdd_( "Singular vectors", &m, &n, a, &lda, s, u, &ldu, vt, &ldvt, work,
&lwork, iwork, &info );
/* Check for convergence */
if( info > 0 ) {
printf( "The algorithm computing SVD failed to converge.\n" );
exit( 1 );
}
(void) time(&t2);

 printf("\n Time difference %ld seconds \n", (long int)(t2-t1));
free( (void*)work );
exit( 0 );
} /* End of SGESDD Example */

