
// Solve random subsets of polymatroidal axioms

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define EPSILON 1e-10
#define T_FACTOR double
// long long int  format: %lld
// double  format: %lf
#define ABS(f) fabs(f)

// Any function modifiers, e.g. static inline
#define FUNCPARAMS 

// Whether to check for and report all-0 solutions
// #define CHECK_FOR_ZERO

#ifdef TEST0
    #include "tests/test0.c"
#else
    #ifdef TEST1
        #include "tests/test1.c"
    #else
        #ifdef TEST2
            #include "tests/test2.c"
        #else
            #ifdef TEST3
                #include "tests/test3.c"
            #else
                #include "axioms.c"
            #endif
        #endif
    #endif
#endif

#define T_ROW(a) T_FACTOR a[VARS]
// Loop through variables / elements in a row vector
#define LOOP(i) for(int i=0; i<VARS; i++)
// Loop through all axioms
#define ROW_LOOP(i) for(int i=0; i<AXIOMS; i++)
// Loop through the chosen axioms
#define CHOSEN_LOOP(i) for(int i=0; i<VARS-1; i++)

int rand_mask;

// The index of chosen axioms
int chosen_ix[AXIOMS];

// The chosen axioms to solve. We solve VARS-1 axioms; the last row is a buffer
T_FACTOR chosen_axioms[VARS][VARS];

struct timeval start_time, current_time;

// Return if value is zero
int FUNCPARAMS is_zero(T_FACTOR x) {
    return (ABS(x) < EPSILON);
}

// Set a row vector to zero
void FUNCPARAMS zero(T_ROW(r)) {
    LOOP(i) r[i] = 0;
}

// Divide a row vector by a scalar
void FUNCPARAMS divide(T_ROW(r), T_FACTOR f) {
    LOOP(i) r[i] /= f;
}

// a -= b*x for row vectors
void FUNCPARAMS subtract(T_ROW(a), T_ROW(b), T_FACTOR x) {
    LOOP(i) a[i] -= b[i] * x;
}

// Print a row vector
void print_row(T_ROW(r)) {
    printf("[");
    LOOP(i) {
        if(i>0) printf(",");
        printf("%5.2lf", r[i]);
    }
    printf("]");
}

// Dot product of two row vectors
T_FACTOR FUNCPARAMS dot(T_ROW(a), T_ROW(b)) {
    T_FACTOR r = 0;
    LOOP(i) r += a[i] * b[i];
    return r;
}

// Check whether a row vector satisfies all axioms
int FUNCPARAMS check_axioms(T_ROW(r)) {
    ROW_LOOP(a) {
        if(dot(axioms[a], r) < -EPSILON) {
            return 0; // false
        }
    }
    return 1; // all true
}

// Pick a random number for axioms
int FUNCPARAMS rnd_axiom_num(int limit) {
    int r;
    while(1) {
        r = (rand() & rand_mask);
        if(r < limit) return r;
    }
}

// Bubble sort the first segment of chosen_ix
void FUNCPARAMS sort_chosen_ix(void) {
    int tmp;
    while(1) {
        tmp = -1;
        for(int i=0; i<VARS-2; i++) {
            if(chosen_ix[i] > chosen_ix[i+1]) {
                tmp = chosen_ix[i];
                chosen_ix[i] = chosen_ix[i+1];
                chosen_ix[i+1] = tmp;
            }
        }
        if(tmp == -1) break;
    }
}

// Initialize, shuffle and sort chosen_ix to get VARS-1 random axioms
void FUNCPARAMS shuffle_chosen_ix(void) {
    
    ROW_LOOP(i) { chosen_ix[i] = i; }
    
    CHOSEN_LOOP(i) {
        int r = rnd_axiom_num(AXIOMS - i) + i;
        if(r != i) {
            int tmp = chosen_ix[i];
            chosen_ix[i] = chosen_ix[r];
            chosen_ix[r] = tmp;
        }
    }
    
    sort_chosen_ix();
}

// Print chosen_ix
void print_chosen_ix(void) {
    printf("AXIOMS=%d VARS=%d\n Chosen: {", AXIOMS, VARS);
    CHOSEN_LOOP(i) printf("%d,", chosen_ix[i]);
    printf("}\n");
}

// Print the axioms getting solved
void print_chosen_axioms(void) {
    CHOSEN_LOOP(i) {
        printf("  ");
        print_row(chosen_axioms[i]);
        printf(" Orig: %s\n", human_readable_axioms[chosen_ix[i]]);
    }
}

// Display elapsed time
void FUNCPARAMS elapsed_time(unsigned long counter) {
    gettimeofday(&current_time, NULL);
    double elapsed = (current_time.tv_sec - start_time.tv_sec) + (current_time.tv_usec - start_time.tv_usec) / 1000. / 1000.;
    printf("# %lu steps in %lf s, %lf ms/step\n", counter, elapsed, elapsed*1000./(double)counter);
}

// Assertion
void FUNCPARAMS assert(int flag, char* message) {
    if(!flag) {
        printf("Assertion failure: %s\n", message);
        exit(1);
    }
}

// Unit tests
void unit_test(void) {
    
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
    subtract(b, a, 1.); // a[.5,1] b[.5,0]
    assert(dot(a,b) == .25, "subtract");
    
    // Test rand_mask
    assert(rand_mask >= AXIOMS, "rand_mask");
    
    // Test shuffle_chosen_ix
    for(int j=0; j<100; j++) {
        shuffle_chosen_ix();
        CHOSEN_LOOP(i) {
            CHOSEN_LOOP(j) {
                if(i < j) { 
                    assert(chosen_ix[i] < chosen_ix[j], "chosen_ix");
                }
            }
        }
    }
    
}

int main(void) {
    
    gettimeofday(&start_time, NULL);
    
    // Calculate a mask for random numbers
    rand_mask = 1;
    while(rand_mask < AXIOMS) rand_mask <<= 1;
    rand_mask--;
    
    printf("# %s\n", LABEL);
    printf("# rand mask: %d\n", rand_mask);
    srand(time(NULL) + getpid());
    
    #ifdef UNIT_TEST
        unit_test();
        printf("OK - Unit tests\n");
    #else
    
    unsigned long tries = 0; // How many steps
    int free_var; // the index of the free variable in the solution
    T_FACTOR solution[VARS]; // the solution
    int freedoms; // how many freedoms the chosen system has
    int solved;  // how many of the chosen axioms are solved
    int inside;  // whether the ray found is inside the cone

    int axiom_solved_for[VARS-1]; // stores which axiom is stored for which variable
    int axiom_solved_for_init[VARS-1];
    CHOSEN_LOOP(i) axiom_solved_for_init[i] = -1;
    
    int variables_solved[VARS]; // stores which variables have been solved (bool, 1|0)
    int variables_solved_init[VARS];
    LOOP(i) variables_solved_init[i] = 0;
    
    unsigned long freedom_stats[VARS]; // Collect statistics on the number of freedoms encountered
    LOOP(i) freedom_stats[i] = 0;
    
    while(1) { // One experiment
    
        // Choose axioms
        shuffle_chosen_ix();
        // Copy axioms
        CHOSEN_LOOP(i) {
            memcpy(&chosen_axioms[i], &axioms[chosen_ix[i]], sizeof(T_FACTOR)*VARS);
        }
        #ifdef DEBUG
            printf("---------- Try #%lu --------------\n", tries);
            print_chosen_ix();
        #endif

        freedoms = 0;
        solved = 0;
        inside = 0;
        memcpy(axiom_solved_for, axiom_solved_for_init, sizeof(int)*(VARS-1)); // Initialize to [-1,-1,...]
        memcpy(variables_solved, variables_solved_init, sizeof(int)*VARS); // Initialize to [0,0,...]
        #ifdef CHECK_FOR_ZERO
            int all_zero = 1;
        #endif

        // Solve the system
        LOOP(var_ix) {
            #ifdef DEBUG
                printf("Solving for variable #%d (solved=%d)\nBegins:\n", var_ix, solved);
                print_chosen_axioms();
            #endif
            
            // Get the abs largest coefficient for the v'th variable
            T_FACTOR max_v = 0;
            int max_ix = -1;  // index of the largest value
            CHOSEN_LOOP(a) {
                if(axiom_solved_for[a] != -1) continue; // only look at the unsolved axioms
                T_FACTOR c = ABS(chosen_axioms[a][var_ix]);
                if(max_ix == -1 || max_v < c) {
                    max_v = c;
                    max_ix = a;
                }
            }
            #ifdef DEBUG
                printf("Largest coefficient (abs) %.2lf at axiom %d\n", max_v, max_ix);
            #endif
            
            if(max_v < EPSILON) {
                // No non-0 coefficients
                freedoms++;
                #ifdef DEBUG
                    printf("No non-0 coefficients, now %d freedoms\n", freedoms);
                #endif
                continue;
            }
            
            // Divide all by the chosen coefficient & subtract
            T_FACTOR r = chosen_axioms[max_ix][var_ix];
            divide(chosen_axioms[max_ix], r);
            CHOSEN_LOOP(a) {
                if(a != max_ix && !is_zero(chosen_axioms[a][var_ix])) {
                    // divide(chosen_axioms[a], r);
                    subtract(chosen_axioms[a], chosen_axioms[max_ix], chosen_axioms[a][var_ix]);
                }
            }
            #ifdef DEBUG
                printf("After divide and subtract:\n", var_ix);
                print_chosen_axioms();
            #endif

            axiom_solved_for[max_ix] = var_ix;
            variables_solved[var_ix] = 1;
            solved++;
        }

        #ifdef DEBUG
            printf("Result: solved=%d freedoms=%d\nFinal:\n", solved, freedoms);
            print_chosen_axioms();
            printf("Axioms solved for: [");
            CHOSEN_LOOP(i) printf("%d,", axiom_solved_for[i]);
            printf("]\n");
            printf("Variables solved: [");
            LOOP(i) printf("%d,", variables_solved[i]);
            printf("]\n");
        #endif

        #ifdef SOLVER_TARGET
            // Check against known results
            assert(solved == target_solved, "target_solved");
            assert(freedoms == target_freedoms, "target_freedoms");
            printf("OK - solved and freedoms checked against target\n");
        #endif
        
        assert(solved + freedoms == VARS, "solved+freedoms");
        
        #ifdef SOLVER_TEST
            // Verify that we have an identity matrix
            CHOSEN_LOOP(a) {
                LOOP(i) {
                    if(!variables_solved[i]) continue;
                    if(ABS(chosen_axioms[a][i] -
                        (i == axiom_solved_for[a] ? 1. : 0.)) >= EPSILON) {
                            printf("Identity failure at a=%d i=%d\n", a, i);
                            assert(0, "Identity");
                        }
                }
            }
            printf("OK - Identity\n");
        #endif

        // Extract the solution if freedoms==1
        if(freedoms == 1) {
            free_var = -1;
            // I expect this to be the last one in most cases
            if(variables_solved[VARS-1] == 0) {
                free_var = VARS-1;
            } else {
                LOOP(i) { if(variables_solved[i] == 0) { free_var = i; break; }}
                assert(free_var > -1, "free var not found");
            }
            #ifdef DEBUG
                printf("Free variable: %d\n", free_var);
            #endif
            
            // Collect the solution
            CHOSEN_LOOP(a) {
                solution[axiom_solved_for[a]] = -chosen_axioms[a][free_var];
                #ifdef CHECK_FOR_ZERO
                    if(!is_zero(chosen_axioms[a][free_var])) { all_zero = 0; }
                #endif
            }
            solution[free_var] = 1;
            #ifdef DEBUG
                printf("Solution: ");
                print_row(solution);
                printf("\n");
            #endif
            
            #ifdef SOLVER_TEST
                // Check the solution against the (original) chosen axioms
                CHOSEN_LOOP(a) {
                    T_FACTOR r = dot(axioms[chosen_ix[a]], solution);
                    #ifdef DEBUG
                        printf("Axiom #%d %d ", a, chosen_ix[a]);
                        print_row(axioms[chosen_ix[a]]);
                        printf(" gives for this solution %lf\n", r);
                    #endif
                    assert(is_zero(r), "Solution vs chosen axiom");
                }
                printf("OK - solution checked against chosen axioms\n");
            #endif

            #ifdef SOLVER_TARGET
                // Check against known results
                assert(free_var == target_free_var, "target_free_var");
                LOOP(i) assert(ABS(target_solution[i] - solution[i]) < .01, "target_solution");
                printf("OK - solution checked against target\n");
            #endif

            // Check the solution against all axioms
            inside = check_axioms(solution);

        } // end if freedoms==1

        // Output
        // 1. The chosen axioms
        CHOSEN_LOOP(a) printf("%d,", chosen_ix[a]);
        // 2. Number of freedoms
        // 3. Whether the ray is inside (relevant only if freedoms==1)
        printf("f%d,i%d\n", freedoms, inside);
        #ifdef CHECK_FOR_ZERO
            if(freedoms == 1) { printf("z%d\n", all_zero); }
        #endif

        tries++;
        if(inside) {
            freedom_stats[0]++;
        } else {
            freedom_stats[freedoms]++;
        }
        
        if(tries % 10000 == 0) {
            elapsed_time(tries);
            // Print freedom stats
            printf("# Freedom stats ");
            LOOP(i) printf("%lu,", freedom_stats[i]);
            printf("\n");
        }
        
        #ifdef SOLVER_TEST
            if(tries > 5) break;    
        #endif
    } // end while

    // endif unit tests
    #endif
}
