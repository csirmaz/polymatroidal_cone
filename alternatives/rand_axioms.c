
// Solve random subsets of polymatroidal axioms using integers

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define T_IELEM long long int
// long long int  format: %lld
// double  format: %lf
#define IABS(f) llabs(f)

// Any function modifiers, e.g. static inline
#define FUNCPARAMS 

// Whether to abort as soon as there are too many freedoms
// #define EARLY_STOP

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

#define T_ROW(a) T_ELEM a[VARS]

// Loop through variables / elements in a row vector
#define LOOP(i) for(int i=0; i<VARS; i++)
// Loop through all axioms
#define AXIOM_LOOP(i) for(int i=0; i<AXIOMS; i++)
// Loop through the chosen axioms
#define CHOSEN_LOOP(i) for(int i=0; i<VARS-1; i++)

// Simplify vectors if a number is above
#define SIMPLIFY_ABOVE 1024*1024

// Whether to remember and skip regions for frequent rays
// #define SKIP_FREQUENT
// Skip frequent rays if they give 0 for more than this many axioms
#define SKIP_FREQUENT_ABOVE 224
// Skip at most this many frequent rays
#define SKIP_FREQUENT_MAX 2

int rand_mask;

// The index of chosen axioms
int chosen_ix[AXIOMS];

// The chosen axioms to solve. We solve VARS-1 axioms; the last row is a buffer
T_ELEM chosen_axioms[VARS][VARS];

#ifdef SKIP_FREQUENT
    // Stores axioms that yield 0 for a frequent ray. 1 if yes, 0 if no
    int skip_axiom_sets[SKIP_FREQUENT_MAX][AXIOMS];
    // Stores how many frequent rays have been saved
    int skip_axiom_set_num = 0;
#endif

struct timeval prev_time, current_time;

// greatest common divisor
T_ELEM FUNCPARAMS gcd(T_ELEM a, T_ELEM b) {
    T_ELEM tmp;
    a = ABS(a);
    b = ABS(b);
    while(1) {
        if (b == 0) return a;
        if (a == 0) return b;
        tmp = b;
        b = a % b;
        a = tmp;
    }
}

// least common multiple
T_ELEM FUNCPARAMS lcm(T_ELEM a, T_ELEM b) {
    return (a / gcd(a, b)) * b;
}

// Set a row vector to zero
void FUNCPARAMS zero(T_ROW(r)) {
    LOOP(i) r[i] = 0;
}

// Simplify a row
void FUNCPARAMS simplify(T_ROW(r)) {
    T_ELEM c = gcd(r[0], r[1]);
    if(c == 1) return;
    for(int i=2; i<VARS; i++) {
        c = gcd(c, r[i]);
        if(c == 1) return;
    }
    if(c == 0) return;
    LOOP(i) r[i] /= c;
}

// Return the lcm for a row
T_ELEM lcm_row(T_ROW(r)) {
    T_ELEM c = lcm(r[0], r[1]);
    for(int i=2; i<VARS; i++) {
        c = lcm(c, r[i]);
    }
    return c;
}

// Print a row vector
void print_row(T_ROW(r)) {
    printf("[");
    LOOP(i) {
        if(i>0) printf(",");
        printf("%lld", r[i]);
    }
    printf("]");
}

// a = a*x - b*y such that there should be 0 at var_ix
void FUNCPARAMS subtract(T_ROW(a), T_ROW(b), int var_ix) {
    T_ELEM c = gcd(a[var_ix], b[var_ix]);
    T_ELEM af = b[var_ix] / c;
    T_ELEM bf = a[var_ix] / c;
    // print_row(a); printf(" af=%lld\n", af);
    // print_row(b); printf(" bf=%lld\n", bf);
    int to_simplify = 0;
    LOOP(i) {
        a[i] = a[i]*af - b[i]*bf;
        if(a[i] != 0 && (a[i] > SIMPLIFY_ABOVE || a[i] < -SIMPLIFY_ABOVE)) { to_simplify = 1; }
    }
    // print_row(a);
    if(to_simplify) simplify(a);
}

// Dot product of two row vectors
T_ELEM FUNCPARAMS dot(T_ROW(a), T_ROW(b)) {
    T_ELEM r = 0;
    LOOP(i) r += a[i] * b[i];
    return r;
}

// Check whether a row vector satisfies all axioms
int FUNCPARAMS check_axioms(T_ROW(r)) {
    int d;
    int zeros = 0;
    AXIOM_LOOP (a) {
        d = dot(axioms[a], r);
        if(d < 0) return -1; // Does not satisfy the axioms
        if(d == 0) zeros++;
    }
    
    #ifdef SKIP_FREQUENT
        if(zeros > SKIP_FREQUENT_ABOVE) {
            if(skip_axiom_set_num >= SKIP_FREQUENT_MAX) {
                printf("~ Could save this ray to skip as frequent, but SKIP_FREQUENT_MAX is exceeded\n");
            } else {
                printf("~ Saving this ray as the #%d frequent one (%d zeros, limit: %d)\n", skip_axiom_set_num, zeros, SKIP_FREQUENT_ABOVE);
                ROW_LOOP(a) { skip_axiom_sets[skip_axiom_set_num][a] = (dot(axioms[a], r) == 0); }
                skip_axiom_set_num++;
            }
        }
    #endif
    
    return zeros; // all true; return the number of axioms where we get zeros
}

// Pick a random number for axioms
int FUNCPARAMS rnd_num(int limit) {
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
    // Do a full shuffle
    CHOSEN_LOOP(i) {
        int r = rnd_num(AXIOMS - i) + i;
        if(r != i) {
            int tmp = chosen_ix[i];
            chosen_ix[i] = chosen_ix[r];
            chosen_ix[r] = tmp;
        }
    }
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
    double elapsed = (current_time.tv_sec - prev_time.tv_sec) + (current_time.tv_usec - prev_time.tv_usec) / 1000. / 1000.;
    gettimeofday(&prev_time, NULL);
    printf("# %lu tries in %lf s, %lf ms/try\n", counter, elapsed, elapsed*1000./(double)counter);
    fflush(stdout);
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
    
    // Test gcd()
    assert(gcd(2,3) == 1, "gcd1");
    assert(gcd(3,7) == 1, "gcd2");
    assert(gcd(2,6) == 2, "gcd3");
    assert(gcd(6,10) == 2, "gcd4");
    
    // Test lcm()
    assert(lcm(2,3) == 6, "lcm1");
    assert(lcm(6,10) == 30, "lcm2");
    
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
    
    // Test subtract()
    zero(a);
    zero(b);
    a[0] = 2; a[1] = 5;
    b[0] = 3; b[1] = 7;
    subtract(a, b, 0); // [6,15]-[6,14]=[0,1]
    assert(a[0] == 0, "subtract1");
    assert(a[1] == 1, "subtract2");
    assert(b[0] == 3, "subtract3");
    a[0] = 2; a[1] = 8;
    b[0] = 3; b[1] = 6;
    subtract(a, b, 1); // [6,24]-[12,24]=[-6,0]
    assert(a[0] == -6, "subtract1");
    assert(a[1] == 0, "subtract2");
    assert(b[0] == 3, "subtract3");

    // Test rand_mask
    assert(rand_mask >= AXIOMS, "rand_mask");
    
    // Test shuffle_chosen_ix
    for(int j=0; j<100; j++) {
        shuffle_chosen_ix();
        CHOSEN_LOOP(i) {
            CHOSEN_LOOP(j) {
                if(i < j) { 
                    assert(chosen_ix[i] != chosen_ix[j], "chosen_ix");
                }
            }
        }
    }
    
}

int main(void) {
    
    gettimeofday(&prev_time, NULL);

    AXIOM_LOOP (i) { chosen_ix[i] = i; }
    
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
    T_ELEM solution[VARS]; // the solution
    T_ELEM solution_divisor[VARS];
    int freedoms; // how many freedoms the chosen system has
    int solved;  // how many of the chosen axioms are solved

    int axiom_solved_for[VARS-1]; // stores which axiom is stored for which variable
    int axiom_solved_for_init[VARS-1];
    CHOSEN_LOOP(i) axiom_solved_for_init[i] = -1;
    
    int variables_solved[VARS]; // stores which variables have been solved (bool, 1|0)
    int variables_solved_init[VARS];
    LOOP(i) variables_solved_init[i] = 0;
    
    while(1) { // One experiment
    
        tries++;
        #define REPORT_EVERY 100000
        if(tries % REPORT_EVERY == 0) {
            elapsed_time(REPORT_EVERY);
        }
        
        // Choose axioms
        shuffle_chosen_ix();
        
        #ifdef SKIP_FREQUENT
            // Check if we are in a frequent-ray axiom set
            int in_a_set = 0;
            for(int s=0; s<skip_axiom_set_num; s++) {
                int in_this_set = 1;
                CHOSEN_LOOP(i) if(!skip_axiom_sets[s][chosen_ix[i]]) { in_this_set = 0; break; }
                if(in_this_set) {
                    in_a_set = 1;
                    break;
                }
            }
            if(in_a_set) continue;
        #endif
        
        // Check for columns of 0 (Full check faster overall than checking upper half)
        int has_zero_col = 0;
        LOOP(i) {
            int has_nonzero_val = 0;
            CHOSEN_LOOP(a) {
                if(axioms[chosen_ix[a]][i] != 0) { has_nonzero_val = 1; break; }
            }
            if(!has_nonzero_val) { has_zero_col = 1; break; }
        }
        if(has_zero_col) { continue; }
        
        // Copy axioms
        CHOSEN_LOOP(i) {
            memcpy(&chosen_axioms[i], &axioms[chosen_ix[i]], sizeof(T_ELEM)*VARS);
        }
        #ifdef DEBUG
            printf("---------- Try #%lu --------------\n", tries);
            print_chosen_ix();
        #endif

        freedoms = 0;
        solved = 0;
        memcpy(axiom_solved_for, axiom_solved_for_init, sizeof(int)*(VARS-1)); // Initialize to [-1,-1,...]
        memcpy(variables_solved, variables_solved_init, sizeof(int)*VARS); // Initialize to [0,0,...]

        // Solve the system
        LOOP(var_ix) {
            #ifdef DEBUG
                printf("Solving for variable #%d (solved=%d)\nBegins:\n", var_ix, solved);
                print_chosen_axioms();
            #endif
            
            // Get the abs largest coefficient for the v'th variable
            T_ELEM max_v = 0;
            int max_ix = -1;  // index of the largest value
            CHOSEN_LOOP(a) {
                if(axiom_solved_for[a] != -1) continue; // only look at the unsolved axioms
                T_ELEM c = ABS(chosen_axioms[a][var_ix]);
                if(max_ix == -1 || max_v < c) {
                    max_v = c;
                    max_ix = a;
                }
            }
            #ifdef DEBUG
                printf("Largest coefficient (abs) %lld at axiom %d\n", max_v, max_ix);
            #endif
            
            if(max_v == 0) {
                // No non-0 coefficients
                freedoms++;
                #ifdef DEBUG
                    printf("No non-0 coefficients, now %d freedoms\n", freedoms);
                #endif
                #ifdef EARLY_STOP
                    if(freedoms > 1) break;
                #endif
                continue;
            }
            
            // Subtract
            CHOSEN_LOOP(a) {
                if(a != max_ix && chosen_axioms[a][var_ix] != 0) {
                    subtract(chosen_axioms[a], chosen_axioms[max_ix], var_ix);
                }
            }
            #ifdef DEBUG
                printf("After subtract:\n", var_ix);
                print_chosen_axioms();
            #endif

            axiom_solved_for[max_ix] = var_ix;
            variables_solved[var_ix] = 1;
            solved++;
        } // end LOOP(var_ix) (solving the system)
        
        #ifdef EARLY_STOP
            if(freedoms > 1) continue;
        #endif

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
            // Verify that we have a kind of identity matrix
            CHOSEN_LOOP(a) {
                LOOP(i) {
                    if(!variables_solved[i]) continue;
                    if(axiom_solved_for[a] == i) {
                        assert(chosen_axioms[a][i] != 0, "Identity1");
                    } else {
                        assert(chosen_axioms[a][i] == 0, "Identity2");
                    }
                }
            }
            printf("OK - Identity\n");
        #endif

        // Extract the solution if freedoms==1
        #ifdef SOLVER_TEST
            if(tries > 5) break;    
        #endif
        if(freedoms != 1) continue;            
            
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
        int negatives = 0;
        int zeros = 0;
        CHOSEN_LOOP(a) {
            int ix = axiom_solved_for[a];
            solution[ix] = -chosen_axioms[a][free_var];
            solution_divisor[ix] = chosen_axioms[a][ix];
            if(solution[ix] == 0) {
                zeros++;
            }else{
                if((solution[ix] >= 0) != (solution_divisor[ix] >= 0)) negatives++;
            }
        }

        #ifdef DEBUG
            solution[free_var] = 1;
            solution_divisor[free_var] = 1;
            printf("Solution:  ");
            print_row(solution);
            printf("\nSolDivisor:");
            print_row(solution_divisor);
            printf("\nSolution negatives: %d zeros: %d\n", negatives, zeros);
        #endif

        if(negatives == 0) {
            solution[free_var] = 1;
        }else if(negatives + zeros == VARS-1) {
            solution[free_var] = -1;
        }else{
            // We have a mixture of signs which will never be "inside"
            continue;
        }
        solution_divisor[free_var] = 1;

        T_ELEM c = lcm_row(solution_divisor);
        if(c < 0) c = -c;
        if(negatives > 0) c = -c;  // flip the solution to an all positive
        LOOP(i) { solution[i] *= c / solution_divisor[i]; }
        simplify(solution);

        #ifdef DEBUG
            printf("Solution (merged): ");
            print_row(solution);
            printf("\n");
        #endif

        #ifdef SOLVER_TEST
            // Check the solution against the (original) chosen axioms
            CHOSEN_LOOP(a) {
                T_ELEM r = dot(axioms[chosen_ix[a]], solution);
                #ifdef DEBUG
                    printf("Axiom #%d %d ", a, chosen_ix[a]);
                    print_row(axioms[chosen_ix[a]]);
                    printf(" gives for this solution %lf\n", r);
                #endif
                assert(r == 0, "Solution vs chosen axiom");
            }
            printf("OK - solution checked against chosen axioms\n");
        #endif

        #ifdef SOLVER_TARGET
            // Check against known results
            assert(free_var == target_free_var, "target_free_var");
            LOOP(i) assert(ABS(target_solution[i] - ((float)solution[i])/((float)solution[free_var])) < .01, "target_solution");
            printf("OK - solution checked against target\n");
        #endif

        // Check the solution against all axioms
        int zero_axioms = check_axioms(solution);
        #ifdef DEBUG
            printf("Check: %d\n", zero_axioms);
        #endif
        if(zero_axioms == -1) {
            // We're not inside the cone
            continue;
        }

        // Only output the good rays
        sort_chosen_ix();
        printf("Tries: %lu Chosen: ", tries);
        CHOSEN_LOOP(a) printf("%d,", chosen_ix[a]);
        printf(" Ray: ");
        print_row(solution);
        printf(" ZeroAxioms: %d\n", zero_axioms);
        
        /*
        CHOSEN_LOOP(a) {
            printf("  ");
            print_row(axioms[chosen_ix[a]]);
            printf(" %s\n", human_readable_axioms[chosen_ix[a]]);
        }
        */
        fflush(stdout);

                
    } // end while

    // endif unit tests
    #endif
}
