
#define SO_MAX_ROWS ALL_AXIOMS
T_ELEM so_matrix[SO_MAX_ROWS][VECLEN];
int so_rows = 0; // How many rows are used in the matrix actually
T_VEC(solution);
T_VEC(solution_divisor);

#define SO_ROWS_FULL_LOOP(a) for(int a=0; a<SO_MAX_ROWS; a++)
#define SO_ROWS_LOOP(a) for(int a=0; a<so_rows; a++)

int axiom_solved_for[SO_MAX_ROWS]; // stores which axiom is stored for which variable
int axiom_solved_for_init[SO_MAX_ROWS];

int variables_solved[VARS]; // stores which variables have been solved (bool, 1|0)
int variables_solved_init[VARS];

/*
 * USAGE
 *  so_init();
 *  LOOP {
 *     so_init_matrix();
 *     LOOP so_add_to_matrix(vector);
 *     if(so_solve() == 1) { READ solution; }
 *  }
 */

void so_print_matrix(void) {
    // Print `so_matrix`
    SO_ROWS_LOOP(r) {
        printf("%d ", r);
        print_vec(so_matrix[r]);
        printf(" for=%d\n", axiom_solved_for[r]);
    }
}

void so_init(void) {
    // Call this once at the beginning
    SO_ROWS_FULL_LOOP(i) axiom_solved_for_init[i] = -1;
    VEC_LOOP(i) variables_solved_init[i] = 0;
}

void so_init_matrix(void) { 
    // Initialize the matrix for injecting expressions
    so_rows = 0;
}

void so_add_to_matrix(T_VEC(v)) {
    // Add an expression to the matrix
    memcpy(so_matrix[so_rows], v, sizeof(T_ELEM)*VECLEN);
    so_rows++;
}

int so_has_zero_columns(void) {
    // Check if the matrix has zero columns
    int zero_column;
    SO_ROWS_LOOP(a) {
        zero_column = 1;
        VEC_LOOP(i) {
            if(so_matrix[a][i] != 0) {
                zero_column = 0;
                break;
            }
        }
        if(zero_column) return 1;
    }
    return 0;
}

int so_solve(void) {
    // Solve the matrix
    // Returns:
    // 0: not good
    // 1: good (1 freedom); solution is in `solution`
    
    int freedoms = 0;
    memcpy(axiom_solved_for, axiom_solved_for_init, sizeof(int)*SO_MAX_ROWS); // Initialize to [-1,-1,...]
    memcpy(variables_solved, variables_solved_init, sizeof(int)*VARS); // Initialize to [0,0,...]
   
    VEC_LOOP(var_ix) {
        #ifdef SO_DEBUG
            printf("Solving for variable #%d\nBegins:\n", var_ix);
            so_print_matrix();
        #endif
        
        // Get the abs largest coefficient for the v'th variable
        T_ELEM max_v = 0;
        int max_ix = -1;  // index of the largest value
        SO_ROWS_LOOP(a) {
            if(axiom_solved_for[a] != -1) continue; // only look at the unsolved axioms
            T_ELEM c = ABS(so_matrix[a][var_ix]);
            if(max_ix == -1 || max_v < c) {
                max_v = c;
                max_ix = a;
            }
        }
        #ifdef SO_DEBUG
            printf("Largest coefficient (abs) %lld at axiom %d\n", max_v, max_ix);
        #endif

        if(max_v == 0) {
            // No non-0 coefficients
            // TODO We kind of never want this...?
            freedoms++;
            #ifdef SO_DEBUG
                printf("No non-0 coefficients, now %d freedoms\n", freedoms);
            #endif
            if(freedoms > 1) {
                #ifdef DEBUG
                    printf("X: Too many freedoms\n"); fflush(stdout);
                #endif
                return 0;
            }
            continue;
        }
        
        // Subtract
        SO_ROWS_LOOP(a) {
            if(a != max_ix && so_matrix[a][var_ix] != 0) {
                solve_one(so_matrix[a], so_matrix[max_ix], var_ix);
            }
        }
        
        axiom_solved_for[max_ix] = var_ix;
        variables_solved[var_ix] = 1;
        // solved++;
    } // end VEC_LOOP(var_ix)
    
    // Note that we do get here if the system is overspecified and in reality has no solution
    // However, we want this system to have 1 degree of freedom anyway
    #ifdef SO_DEBUG
        printf("Result: freedoms=%d\nFinal:\n", freedoms);
        so_print_matrix();
        printf("Axioms solved for: [");
        SO_ROWS_LOOP(i) printf("%d,", axiom_solved_for[i]);
        printf("]\n");
        printf("Variables solved: [");
        VEC_LOOP(i) printf("%d,", variables_solved[i]);
        printf("]\n");
    #endif
    
    if(freedoms != 1) {
        #ifdef DEBUG
            printf("X: Wrong number of freedoms\n"); fflush(stdout);
        #endif
        return 0;
    }
    
    // Extract the solution
    int free_var = -1;
    // I expect this to be the last one in most cases
    if(variables_solved[VARS-1] == 0) {
        free_var = VARS-1;
    } else {
        VEC_LOOP(i) { if(variables_solved[i] == 0) { free_var = i; break; }}
        assert(free_var > -1, "free var not found");
    }
    #ifdef SO_DEBUG
        printf("Free variable: %d\n", free_var);
    #endif

    // Collect the solution
    int negatives = 0;
    int zeros = 0;
    SO_ROWS_LOOP(a) {
        int ix = axiom_solved_for[a];
        if(ix == -1) continue;
        solution[ix] = -so_matrix[a][free_var];
        solution_divisor[ix] = so_matrix[a][ix];
        if(solution[ix] == 0) {
            zeros++;
        }else{
            if((solution[ix] >= 0) != (solution_divisor[ix] >= 0)) negatives++;
        }
    }

    #ifdef SO_DEBUG
        solution[free_var] = 1;
        solution_divisor[free_var] = 1;
        printf("Solution:  ");
        print_vec(solution);
        printf("\nSolDivisor:");
        print_vec(solution_divisor);
        printf("\nSolution negatives: %d zeros: %d\n", negatives, zeros);
    #endif

    // The following logic ensures we select a solution where all coordinates are positive (if possible)
    if(negatives == 0) {
        solution[free_var] = 1;
    }else if(negatives + zeros == VARS - 1) {
        solution[free_var] = -1;
    }else{
        // We have a mixture of signs which will never be "inside"
        #ifdef DEBUG
            printf("X: Ray not inside (signs)\n"); fflush(stdout);
        #endif
        return 0;
    }
    solution_divisor[free_var] = 1;

    T_ELEM c = lcm_vec(solution_divisor);
    if(c < 0) c = -c;
    if(negatives > 0) c = -c;  // flip the solution to an all positive
    VEC_LOOP(i) { solution[i] *= c / solution_divisor[i]; }
    simplify(solution);

    #ifdef SO_DEBUG
        printf("Solution (merged): ");
        print_vec(solution);
        printf("\n");
    #endif
    return 1;
}
