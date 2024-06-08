
// Solve a collection of linear equations without a constant using integers.
// Determine the freedom (D-rank), and if it is 1, return a vector (if possible) that satisfies
// all equations and has at least one positive coordinate.

/* If SO_EARLYSTOP is defined, this file only sets up a variant of so_solve()
 * named so_solve_early() that stops if the freedoms exceed 1.
 */
#ifndef SO_EARLYSTOP

#define SO_MAX_ROWS AXIOMS
// "_coll" are collections - one for each thread
T_ELEM so_matrix_coll[NUM_THREADS][SO_MAX_ROWS][VECLEN];
T_RAYIX so_rows_coll[NUM_THREADS]; // How many rows are used in the matrix actually
T_ELEM solution_coll[NUM_THREADS][VECLEN];
T_ELEM solution_divisor_coll[NUM_THREADS][VECLEN];

#define SO_ROWS_FULL_LOOP(a) for(int a=0; a<SO_MAX_ROWS; a++)
#define SO_ROWS_LOOP(a) for(int a=0; a<so_rows; a++)

int axiom_solved_for_coll[NUM_THREADS][SO_MAX_ROWS]; // stores which axiom is stored for which variable
int axiom_solved_for_init[SO_MAX_ROWS];

int variables_solved_coll[NUM_THREADS][VARS]; // stores which variables have been solved (bool, 1|0)
int variables_solved_init[VARS];

/*
 * USAGE
 *  so_init();
 *  LOOP {
 *     so_init_matrix();
 *     LOOP so_add_to_matrix(vector);
 *     int freedoms = so_solve();
 *     if(freedoms == 1) { READ solution; }
 *  }
 */


void so_print_matrix(size_t thread_num) {
    // Print `so_matrix`
    for(int r=0; r<so_rows_coll[thread_num]; r++) {
        printf("  | %2d ", r);
        print_vec(so_matrix_coll[thread_num][r]);
        printf(" for=%d\n", axiom_solved_for_coll[thread_num][r]);
    }
}

void so_init(void) {
    // Call this once at the beginning
    SO_ROWS_FULL_LOOP(i) axiom_solved_for_init[i] = -1;
    VEC_LOOP(i) variables_solved_init[i] = 0;
}

void so_init_matrix(size_t thread_num) { 
    // Initialize the matrix for injecting expressions
    so_rows_coll[thread_num] = 0;
}

void so_add_to_matrix(size_t thread_num, T_VEC(v)) {
    // Add an expression to the matrix
    memcpy(so_matrix_coll[thread_num][so_rows_coll[thread_num]], v, sizeof(T_ELEM)*VECLEN);
    so_rows_coll[thread_num]++;
}

static inline int so_solve_impl(
    size_t thread_num,
    T_RAYIX so_rows,
    int axiom_solved_for[SO_MAX_ROWS],
    int variables_solved[VARS],
    T_ELEM so_matrix[SO_MAX_ROWS][VECLEN],
    T_ELEM solution[VECLEN],
    T_ELEM solution_divisor[VECLEN]
) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    // 2 and above: many freedoms
#else
static inline int so_solve_early_impl(
    size_t thread_num,
    T_RAYIX so_rows,
    int axiom_solved_for[SO_MAX_ROWS],
    int variables_solved[VARS],
    T_ELEM so_matrix[SO_MAX_ROWS][VECLEN],
    T_ELEM solution[VECLEN],
    T_ELEM solution_divisor[VECLEN]    
) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 or >1 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
#endif
    
    int freedoms = 0;

    memcpy(axiom_solved_for, axiom_solved_for_init, sizeof(int)*SO_MAX_ROWS); // Initialize to [-1,-1,...]
    memcpy(variables_solved, variables_solved_init, sizeof(int)*VARS); // Initialize to [0,0,...]
   
    VEC_LOOP(var_ix) {
        printf("  | Solving for variable #%d so_rows=%zu thread=%zu\n  | Begins:\n", var_ix, so_rows, thread_num); // SO_DEBUG
        so_print_matrix(thread_num); // SO_DEBUG
        
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
        printf("  | Largest coefficient (abs) %lld at axiom %d\n", max_v, max_ix); // SO_DEBUG

        if(max_v == 0) {
            // No non-0 coefficients
            freedoms++;
            printf("  | No non-0 coefficients, now %d freedoms\n", freedoms); // SO_DEBUG
            #ifdef SO_EARLYSTOP
                if(freedoms > 1) { 
                    printf("  | X: Too many freedoms, %d\n", freedoms); fflush(stdout); // SO_DEBUG
                    return 0; 
                }
            #endif
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
    printf("  | Result: freedoms=%d\n  | Final:\n", freedoms); // SO_DEBUG
    so_print_matrix(thread_num); // SO_DEBUG
    printf("  | Axioms solved for: ["); // SO_DEBUG
    SO_ROWS_LOOP(i) printf("%d,", axiom_solved_for[i]); // SO_DEBUG
    printf("]\n"); // SO_DEBUG
    printf("  | Variables solved: ["); // SO_DEBUG
    VEC_LOOP(i) printf("%d,", variables_solved[i]); // SO_DEBUG
    printf("]\n"); // SO_DEBUG
    
    if(freedoms != 1) {
        printf("  | X: Wrong number of freedoms: %d\n", freedoms); fflush(stdout); // SO_DEBUG
        return freedoms;
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
    printf("  | Free variable at: %d\n", free_var); // SO_DEBUG

    // Collect the solution
    // We want all-positive solutions. Since then the free variable needs to be positive,
    // there cannot be negative coordinates at all (we cannot flip the signs).
    SO_ROWS_LOOP(a) {
        int ix = axiom_solved_for[a];
        if(ix == -1) continue;
        solution[ix] = -so_matrix[a][free_var];
        solution_divisor[ix] = so_matrix[a][ix];
        assert(solution_divisor[ix] != 0, "solution div 0");
    }

    solution[free_var] = 1;
    solution_divisor[free_var] = 1;
    
    printf("  | Solution:  "); // SO_DEBUG
    print_vec(solution); // SO_DEBUG
    printf("\n  | SolDivisor:"); // SO_DEBUG
    print_vec(solution_divisor); // SO_DEBUG
    printf("\n"); // SO_DEBUG
    
    T_ELEM c = lcm_vec(solution_divisor);
    if(c < 0) c = -c;
    VEC_LOOP(i) { solution[i] *= c / solution_divisor[i]; }
    simplify(solution);

    printf("  | Solution (merged): "); // SO_DEBUG
    print_vec(solution); // SO_DEBUG
    printf("\n"); // SO_DEBUG

    return 1;
}

#ifndef SO_EARLYSTOP
int so_solve(size_t thread_num) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    // 2 and above: many freedoms
    return so_solve_impl(
        thread_num,
        so_rows_coll[thread_num],
        axiom_solved_for_coll[thread_num],
        variables_solved_coll[thread_num],
        so_matrix_coll[thread_num],
        solution_coll[thread_num],
        solution_divisor_coll[thread_num]
    );
}
#else
int so_solve_early(size_t thread_num) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 or >1 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    return so_solve_early_impl(
        thread_num,
        so_rows_coll[thread_num],
        axiom_solved_for_coll[thread_num],
        variables_solved_coll[thread_num],
        so_matrix_coll[thread_num],
        solution_coll[thread_num],
        solution_divisor_coll[thread_num]
    );
}
#endif
