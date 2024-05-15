#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

#define EPSILON 1e-10
#define T_FACTOR double
// long long int  format: %lld
// double  format: %lf
#define ABS(f) fabs(f)

#include "axioms.c"

#define T_ROW(a) T_FACTOR a[VARS]
// Loop through variables / elements in a row vector
#define LOOP(i) for(int i=0; i<VARS; i++)
// Loop through all axioms
#define ROW_LOOP(i) for(int i=0; i<AXIOMS; i++)
// Loop through the chosen axioms
#define CHOSEN_LOOP(i) for(int i=0; i<VARS-1; i++)
#define SOLVING_LOOP(i) for(int i=solved; i<VARS-1; i++)

int rand_mask;

// The index of chosen axioms
int chosen_ix[AXIOMS];

/*
// greatest common divisor
T_FACTOR gcd(T_FACTOR a, T_FACTOR b) {
    if (b == 0) return a;
    return gcd(b, a % b);
}

// least common multiple
T_FACTOR lcm(T_FACTOR a, T_FACTOR b) {
    return (a / gcd(a, b)) * b;
}
*/

// Set a row vector to zero
void zero(T_ROW(r)) {
    LOOP(i) r[i] = 0;
}

// Divide a row vector
void divide(T_ROW(r), T_FACTOR f) {
    LOOP(i) r[i] /= f;
}

// a -= b for row vectors
void subtract(T_ROW(a), T_ROW(b)) {
    LOOP(i) a[i] -= b[i];
}

// Print a row vector
void print_row(T_ROW(r)) {
    printf("[");
    LOOP(i) {
        if(i>0) printf(",");
        printf("%lf", r[i]);
    }
    printf("]");
}

// Dot product of two row vectors
T_FACTOR dot(T_ROW(a), T_ROW(b)) {
    T_FACTOR r = 0;
    LOOP(i) r += a[i] * b[i];
    return r;
}

// Check whether a row vector satisfies all axioms
int check_axioms(T_ROW(r)) {
    ROW_LOOP(a) {
        if(dot(axioms[a], r) < -EPSILON) {
            return 0; // false
        }
    }
    return 1; // all true
}

// Pick a random number for axioms
int rnd_axiom_num(int limit) {
    int r;
    while(1) {
        r = (rand() & rand_mask);
        if(r < limit) return r;
    }
}

// Initialize and shuffle chosen_ix to get VARS-1 random axioms
void shuffle_chosen_ix(void) {
    
    ROW_LOOP(i) { chosen_ix[i] = i; }
    
    CHOSEN_LOOP(i) {
        int r = rnd_axiom_num(AXIOMS-i) + i;
        if(r != i) {
            int tmp = chosen_ix[i];
            chosen_ix[i] = chosen_ix[r];
            chosen_ix[r] = tmp;
        }
    }
}

// Print chosen_ix
void print_chosen_ix(void) {
    printf("AXIOMS=%d VARS=%d\n{", AXIOMS, VARS);
    CHOSEN_LOOP(i) printf("%d,", chosen_ix[i]);
    printf("}\n");
}

// Assertion
void assert(int flag, char* message) {
    if(!flag) {
        printf("Assertion failure: %s\n", message);
        exit(1);
    }
}

// Unit tests
void test(void) {
    
    // Test dot()
    T_ROW(a);
    T_ROW(b);
    zero(a);
    zero(b);
    assert(dot(a,b) == 0, "dot1");
    a[0] = 1;
    assert(dot(a,b) == 0, "dot2");
    b[0] = 1;
    assert(dot(a,b) == 1, "dot3");
    a[1] = 2;
    b[1] = 1; // a[1,2] b[1,1]
    assert(dot(a,b) == 3, "dot4");
    
    // Test divide()
    divide(a, 2); // a[.5,1] b[1,1]
    assert(dot(a,b) == 1.5, "divide");
    
    // Test subtract()
    subtract(b, a); // a[.5,1] b[.5,0]
    assert(dot(a,b) == .25, "subtract");
    
    // Test rand_mask
    assert(rand_mask >= AXIOMS, "rand_mask");
    
    // Test shuffle_chosen_ix
    for(int j=0; j<100; j++) {
        shuffle_chosen_ix();
        for(int i=1; i<VARS-1; i++) {
            assert(chosen_ix[0] != chosen_ix[i], "chosen_ix1");
            assert(chosen_ix[i-1] != chosen_ix[VARS-2], "chosen_ix2");
        }
    }
    
}

int main(void) {
    
    // Calculate a mask for random numbers
    rand_mask = 1;
    while(rand_mask < AXIOMS) rand_mask <<= 1;
    rand_mask--;
    
    printf("# %s\n", LABEL);
    printf("# rand mask: %d\n", rand_mask);
    srand(time(NULL) + getpid());
    
    test();
    
    // The chosen axioms to solve. We solve VARS-1 axioms; the last row is a buffer
    T_FACTOR chosen_axioms[VARS][VARS];
    
    while(1) { // One experiment
    
        // Choose axioms
        shuffle_chosen_ix();
        // Copy axioms
        CHOSEN_LOOP(i) {
            memcpy(&chosen_axioms[i], &axioms[chosen_ix[i]], sizeof(T_FACTOR)*VARS);
        }
    
        int freedoms = 0;
        int solved = 0;
    
        // Solve the system
        LOOP(var_ix) {
            // Get the abs largest coefficient for the v'th variable
            T_FACTOR max_v = 0;
            int max_ix = -1;  // index of the largest value
            SOLVING_LOOP(a) {
                T_FACTOR c = ABS(chosen_axioms[a][var_ix]);
                if(max_ix == -1 || max_v < c) {
                    max_v = c;
                    max_ix = a;
                }
            }
            
            if(max_v < EPSILON) {
                // No non-0 coefficients
                freedoms++;
                break;
            }
            
            // Divide all by the chosen coefficient
            T_FACTOR r = chosen_axioms[max_ix][var_ix];
            SOLVING_LOOP(a) divide(chosen_axioms[a], r);
            // Subtract
            SOLVING_LOOP(a) {
                if(a != max_ix) subtract(chosen_axioms[a], chosen_axioms[max_ix]);
            }
            
            // Move the solved axiom to the top
            if(max_ix != solved) {
                // tmp <- chosen_axioms[max_ix] (currently solved)
                memcpy(&chosen_axioms[VARS-1], &chosen_axioms[max_ix], sizeof(T_FACTOR)*VARS);
                // chosen_axioms[max_ix] <- chosen_axioms[solved] (first in the block being solved)
                memcpy(&chosen_axioms[max_ix], &chosen_axioms[solved], sizeof(T_FACTOR)*VARS);
                // chosen_axioms[solved] <- tmp
                memcpy(&chosen_axioms[solved], &chosen_axioms[VARS-1], sizeof(T_FACTOR)*VARS);
            }
            solved++;
            
        }
        
        break; // TODO
    }

    shuffle_chosen_ix();
    print_chosen_ix();
    printf("Hello world %d\n", 1);
}

/*

    int x[4]
    -> x can be treated as a pointer, === &x[0]
    -> &x[i] to get address of element
    -> *x === x[0]
    
    int *a[10] === 10 int* pointers
    inf (*a)[10] === pointer to int[10]
    -> (*a)[i] to get element
    
*/
