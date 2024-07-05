
// Solve a collection of linear equations without a constant using floats.
// Determine the freedom (D-rank), and if it is 1, return a vector (if possible) that satisfies
// all equations and has at least one positive coordinate.

#ifndef SO_MAX_ROWS

#define SO_MAX_ROWS AXIOMS
#define SO_ROWS_FULL_LOOP(a) for(int a=0; a<SO_MAX_ROWS; a++)
#define SO_ROWS_LOOP(a) for(int a=0; a<so_rows; a++)

#endif

/* If SO_EARLYSTOP is defined, this file only sets up a variant of so_solve()
 * named so_solve_early() that stops if the freedoms exceed 1.
 */
#ifndef SO_EARLYSTOP

// "_coll" are collections - one for each thread
T_FVEC(sof_matrix_coll[NUM_THREADS][SO_MAX_ROWS]);
T_RAYIX sof_rows_coll[NUM_THREADS]; // How many rows are used in the matrix actually
T_FVEC(sof_solution_coll[NUM_THREADS]);

int sof_axiom_solved_for_coll[NUM_THREADS][SO_MAX_ROWS]; // stores which axiom is stored for which variable
int sof_axiom_solved_for_init[SO_MAX_ROWS];

int sof_variables_solved_coll[NUM_THREADS][VARS]; // stores which variables have been solved (bool, 1|0)

/*
 * USAGE
 *  sof_init();
 *  LOOP {
 *     sof_init_matrix();
 *     LOOP sof_add_to_matrix(vector);
 *     int freedoms = sof_solve();
 *     if(freedoms == 1) { READ solution; }
 *  }
 */


// a := a - b*f such that a[var_ix]==0
void sof_solve_one(T_FVEC(a), T_FVEC(b), int var_ix) {
    T_FELEM bf = a[var_ix] / b[var_ix];
    VEC_LOOP(i) {
        a[i] -= b[i]*bf;
    }
}

void sof_print_matrix(T_THREAD_NUM thread_num) {
    // Print `sof_matrix`
    for(int r=0; r<sof_rows_coll[thread_num]; r++) {
        printf("  | %2d ", r);
        vec_fprint(sof_matrix_coll[thread_num][r]);
        printf(" for=%d\n", sof_axiom_solved_for_coll[thread_num][r]);
    }
}

void sof_init(void) {
    // Call this once at the beginning
    SO_ROWS_FULL_LOOP(i) sof_axiom_solved_for_init[i] = -1;
    // VEC_LOOP(i) variables_solved_init[i] = 0;
}

void sof_init_matrix(T_THREAD_NUM thread_num) { 
    // Initialize the matrix for injecting expressions
    sof_rows_coll[thread_num] = 0;
}

void sof_add_to_matrix(T_THREAD_NUM thread_num, T_FVEC(v)) {
    // Add an expression to the matrix
    memcpy(sof_matrix_coll[thread_num][sof_rows_coll[thread_num]], v, sizeof(T_FELEM)*VECLEN);
    sof_rows_coll[thread_num]++;
}

static inline int sof_solve_impl(
    T_THREAD_NUM thread_num,
    T_RAYIX so_rows,
    int axiom_solved_for[SO_MAX_ROWS],
    int variables_solved[VARS],
    T_FELEM sof_matrix[SO_MAX_ROWS][VECLEN],
    T_FELEM solution[VECLEN]
) {
    // Solve the matrix (float)
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    // 2 and above: many freedoms
#else
static inline int sof_solve_early_impl(
    T_THREAD_NUM thread_num,
    T_RAYIX so_rows,
    int axiom_solved_for[SO_MAX_ROWS],
    int variables_solved[VARS],
    T_FELEM sof_matrix[SO_MAX_ROWS][VECLEN],
    T_FELEM solution[VECLEN]
) {
    // Solve the matrix (float)
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 or >1 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
#endif
    
    int freedoms = 0;

    memcpy(axiom_solved_for, sof_axiom_solved_for_init, sizeof(int)*SO_MAX_ROWS); // Initialize to [-1,-1,...]
    // memcpy(variables_solved, variables_solved_init, sizeof(int)*VARS); // Initialize to [0,0,...]
    bzero(variables_solved, sizeof(int)*VARS); // Initialize to [0,0,...]
   
    VEC_LOOP(var_ix) {
        printf("  | Solving for variable #%d so_rows=%zu thread=%d\n  | Begins:\n", var_ix, so_rows, thread_num); // SO_DEBUG
        so_print_matrix(thread_num); // SO_DEBUG
        
        // Get the abs largest coefficient for the v'th variable
        T_FELEM max_v = 0;
        int max_ix = -1;  // index of the largest value
        SO_ROWS_LOOP(a) {
            if(axiom_solved_for[a] != -1) continue; // only look at the unsolved axioms
            T_FELEM c = FABS(sof_matrix[a][var_ix]);
            if(max_ix == -1 || max_v < c) {
                max_v = c;
                max_ix = a;
            }
        }
        printf("  | Largest coefficient (abs) %f at axiom %d\n", max_v, max_ix); // SO_DEBUG

        if(f_is_zero(max_v)) {
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
            if(a != max_ix && f_is_nonzero(sof_matrix[a][var_ix])) {
                sof_solve_one(sof_matrix[a], sof_matrix[max_ix], var_ix);
            }
        }
        
        axiom_solved_for[max_ix] = var_ix;
        variables_solved[var_ix] = 1;
    } // end VEC_LOOP(var_ix)
    
    // Note that we do get here if the system is overspecified and in reality has no solution
    // However, we want this system to have 1 degree of freedom anyway
    printf("  | Result: freedoms=%d\n  | Final:\n", freedoms); // SO_DEBUG
    sof_print_matrix(thread_num); // SO_DEBUG
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
        assert(f_is_nonzero(sof_matrix[a][ix]), "solution div 0");
        solution[ix] = -sof_matrix[a][free_var] / sof_matrix[a][ix];
    }

    solution[free_var] = 1;
    
    printf("  | Solution:  "); // SO_DEBUG
    vec_fprint(solution); // SO_DEBUG
    printf("\n"); // SO_DEBUG
    
    return 1;
}

#ifndef SO_EARLYSTOP
int sof_solve(T_THREAD_NUM thread_num) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    // 2 and above: many freedoms
    return sof_solve_impl(
        thread_num,
        sof_rows_coll[thread_num],
        sof_axiom_solved_for_coll[thread_num],
        sof_variables_solved_coll[thread_num],
        sof_matrix_coll[thread_num],
        sof_solution_coll[thread_num]
    );
}
#else
int sof_solve_early(T_THREAD_NUM thread_num) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 or >1 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    return sof_solve_early_impl(
        thread_num,
        sof_rows_coll[thread_num],
        sof_axiom_solved_for_coll[thread_num],
        sof_variables_solved_coll[thread_num],
        sof_matrix_coll[thread_num],
        sof_solution_coll[thread_num]
    );
}
#endif
