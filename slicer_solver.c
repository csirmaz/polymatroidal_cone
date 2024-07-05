
// Solve a collection of linear equations without a constant using integers.
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
T_IVEC(so_matrix_coll[NUM_THREADS][SO_MAX_ROWS]);
T_RAYIX so_rows_coll[NUM_THREADS]; // How many rows are used in the matrix actually
T_IVEC(so_solution_coll[NUM_THREADS]);
T_IVEC(so_solution_divisor_coll[NUM_THREADS]);

int so_axiom_solved_for_coll[NUM_THREADS][SO_MAX_ROWS]; // stores which axiom is stored for which variable
int so_axiom_solved_for_init[SO_MAX_ROWS];

int so_variables_solved_coll[NUM_THREADS][VARS]; // stores which variables have been solved (bool, 1|0)

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


// a := a*x - b*y such that a[var_ix]==0
// Return 1 if results are unstable
int so_solve_one(T_IVEC(a), T_IVEC(b), int var_ix) {
    T_IELEM c = gcd(a[var_ix], b[var_ix]);
    T_IELEM af = b[var_ix] / c;
    T_IELEM bf = a[var_ix] / c;
    // print_row(a); printf(" af=%lld\n", af);
    // print_row(b); printf(" bf=%lld\n", bf);
    int to_simplify = 0;
    VEC_LOOP(i) {
        a[i] = a[i]*af - b[i]*bf;
        if(a[i] != 0 && (a[i] > SIMPLIFY_ABOVE || a[i] < -SIMPLIFY_ABOVE)) { to_simplify = 1; }
    }
    // print_row(a);
    if(to_simplify) { return 1 - vec_isimplify(a); }
    return 0;
}


void so_print_matrix(T_THREAD_NUM thread_num) {
    // Print `so_matrix`
    for(int r=0; r<so_rows_coll[thread_num]; r++) {
        printf("  | %2d ", r);
        vec_iprint(so_matrix_coll[thread_num][r]);
        printf(" for=%d\n", so_axiom_solved_for_coll[thread_num][r]);
    }
}

void so_init(void) {
    // Call this once at the beginning
    SO_ROWS_FULL_LOOP(i) so_axiom_solved_for_init[i] = -1;
    // VEC_LOOP(i) variables_solved_init[i] = 0;
}

void so_init_matrix(T_THREAD_NUM thread_num) { 
    // Initialize the matrix for injecting expressions
    so_rows_coll[thread_num] = 0;
}

void so_add_to_matrix(T_THREAD_NUM thread_num, T_IVEC(v)) {
    // Add an expression to the matrix
    memcpy(so_matrix_coll[thread_num][so_rows_coll[thread_num]], v, sizeof(T_IELEM)*VECLEN);
    so_rows_coll[thread_num]++;
}

void so_assign_f_to_matrix(T_THREAD_NUM thread_num, T_FVEC(v)) {
    // Add an expression to the (int) matrix from float
    VEC_LOOP(i) so_matrix_coll[thread_num][so_rows_coll[thread_num]][i] = v[i];
    so_rows_coll[thread_num]++;    
}

static inline int so_solve_impl(
    T_THREAD_NUM thread_num,
    T_RAYIX so_rows,
    int axiom_solved_for[SO_MAX_ROWS],
    int variables_solved[VARS],
    T_IELEM so_matrix[SO_MAX_ROWS][VECLEN],
    T_IELEM solution[VECLEN],
    T_IELEM solution_divisor[VECLEN]
) {
    // Solve the matrix
    // Returns:
    // -2: overflow
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    // 2 and above: many freedoms
#else
static inline int so_solve_early_impl(
    T_THREAD_NUM thread_num,
    T_RAYIX so_rows,
    int axiom_solved_for[SO_MAX_ROWS],
    int variables_solved[VARS],
    T_IELEM so_matrix[SO_MAX_ROWS][VECLEN],
    T_IELEM solution[VECLEN],
    T_IELEM solution_divisor[VECLEN]    
) {
    // Solve the matrix
    // Returns:
    // -2: overflow
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 or >1 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
#endif
    
    int freedoms = 0;

    memcpy(axiom_solved_for, so_axiom_solved_for_init, sizeof(int)*SO_MAX_ROWS); // Initialize to [-1,-1,...]
    // memcpy(variables_solved, variables_solved_init, sizeof(int)*VARS); // Initialize to [0,0,...]
    bzero(variables_solved, sizeof(int)*VARS); // Initialize to [0,0,...]
   
    VEC_LOOP(var_ix) {
        printf("  | Solving for variable #%d so_rows=%zu thread=%d\n  | Begins:\n", var_ix, so_rows, thread_num); // SO_DEBUG
        so_print_matrix(thread_num); // SO_DEBUG
        
        // Get the abs largest coefficient for the v'th variable
        T_IELEM max_v = 0;
        int max_ix = -1;  // index of the largest value
        SO_ROWS_LOOP(a) {
            if(axiom_solved_for[a] != -1) continue; // only look at the unsolved axioms
            T_IELEM c = IABS(so_matrix[a][var_ix]);
            if(max_ix == -1 || max_v < c) {
                max_v = c;
                max_ix = a;
            }
        }
        printf("  | Largest coefficient (abs) %d at axiom %d\n", max_v, max_ix); // SO_DEBUG

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
                if(so_solve_one(so_matrix[a], so_matrix[max_ix], var_ix)) { return -2; }
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
    vec_iprint(solution); // SO_DEBUG
    printf("\n  | SolDivisor:"); // SO_DEBUG
    vec_iprint(solution_divisor); // SO_DEBUG
    printf("\n"); // SO_DEBUG
    
    T_IELEM c = lcm_vec(solution_divisor);
    if(c < 0) c = -c;
    VEC_LOOP(i) { solution[i] *= c / solution_divisor[i]; }
    vec_isimplify (solution);

    printf("  | Solution (merged): "); // SO_DEBUG
    vec_iprint(solution); // SO_DEBUG
    printf("\n"); // SO_DEBUG

    return 1;
}

#ifndef SO_EARLYSTOP
int so_solve(T_THREAD_NUM thread_num) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    // 2 and above: many freedoms
    return so_solve_impl(
        thread_num,
        so_rows_coll[thread_num],
               so_axiom_solved_for_coll[thread_num],
               so_variables_solved_coll[thread_num],
        so_matrix_coll[thread_num],
               so_solution_coll[thread_num],
               so_solution_divisor_coll[thread_num]
    );
}
#else
int so_solve_early(T_THREAD_NUM thread_num) {
    // Solve the matrix
    // Returns:
    // -1: 1 freedom but there's a mixture of sings for the coordinates
    // 0: 0 or >1 freedoms
    // 1: good (1 freedom & all positive coordinates); solution is in `solution`
    return so_solve_early_impl(
        thread_num,
        so_rows_coll[thread_num],
               so_axiom_solved_for_coll[thread_num],
               so_variables_solved_coll[thread_num],
        so_matrix_coll[thread_num],
               so_solution_coll[thread_num],
               so_solution_divisor_coll[thread_num]
    );
}
#endif
