
// Keep slicing a cone by axioms until we get all rays

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define T_ELEM long long int
// long long int  format: %lld
// double  format: %lf
#define ABS(f) llabs(f)

#include "axioms.c"

// Length of a vector
#define VECLEN VARS
#define T_VEC(a) T_ELEM a[VECLEN]
// Loop through variables / elements in a row vector
#define VEC_LOOP(i) for(int i=0; i<VECLEN; i++)
// Loop through all axioms
#define ROW_LOOP(i) for(int i=0; i<AXIOMS; i++)

// Simplify vectors if a number is above
#define SIMPLIFY_ABOVE 1024*1024

#include "util.c"
#include "ray_store.c"

// Axioms extended with identities
T_ELEM ext_axioms[AXIOMS+VARS][VARS];


int main(void) {
    printf("Slicer starting\n");
    
    // Initialize ext_axioms
    VEC_LOOP(j) VEC_LOOP(i) ext_axioms[j][i] = (i==j ? 1 : 0);
    ROW_LOOP(j) VEC_LOOP(i) ext_axioms[VARS+j][i] = axioms[j][i];
    
    rs_assert_bitmap_size(VARS+AXIOMS); // total number of faces
    
    // TODO when can we forget about the identity axioms/faces?
    
    
}




/*
-
-    int x[4]
-    -> x can be treated as a pointer, === &x[0]
-    -> &x[i] to get address of element
-    -> *x === x[0]
-    
-    int *a[10] === 10 int* pointers
-    inf (*a)[10] === pointer to int[10]
-    -> (*a)[i] to get element
-    
*/
