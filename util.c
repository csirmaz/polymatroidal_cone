
// Utilities
// Utilities for integer vectors

T_VEC(zero_vector);

// Call this at the beginning of the program
void util_init(void) {
    VEC_LOOP(i) zero_vector[i] = 0;
}

// Return the greatest common divisor (always positive)
T_ELEM gcd(T_ELEM a, T_ELEM b) {
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

// Return the least common multiple
T_ELEM lcm(T_ELEM a, T_ELEM b) {
    return (a / gcd(a, b)) * b;
}

// Copy data from one vector to another
void vec_cpy(T_VEC(dest), T_VEC(src)) {
    memcpy(dest, src, sizeof(T_ELEM)*VECLEN);
}

// Set a row vector to zero
void vec_zero(T_VEC(r)) {
    vec_cpy(r, zero_vector);
}

// Scale a vector by a scalar
void vec_scale(T_VEC(v), T_ELEM s) {
    VEC_LOOP(i) v[i] *= s;
}

// Return if two vectors are the same
int vec_eq(T_VEC(a), T_VEC(b)) {
    VEC_LOOP(i) if(a[i] != b[i]) return 0;
    return 1;
}

// Simplify a vector
void simplify(T_VEC(r)) {
    T_ELEM c = gcd(r[0], r[1]);
    if(c == 1) return;
    for(int i=2; i<VECLEN; i++) {
        c = gcd(c, r[i]);
        if(c == 1) return;
    }
    if(c == 0) return;
    VEC_LOOP(i) r[i] /= c;
}

// Return the lcm for a vector
T_ELEM lcm_vec(T_VEC(r)) {
    T_ELEM c = lcm(r[0], r[1]);
    for(int i=2; i<VECLEN; i++) {
        c = lcm(c, r[i]);
    }
    return c;
}

// Print a vector
void print_vec(T_VEC(r)) {
    printf("[");
    VEC_LOOP(i) {
        if(i>0) printf(",");
        printf("%2d", r[i]);
    }
    printf("]");
}

// Dot product of two row vectors
T_ELEM dot(T_VEC(a), T_VEC(b)) {
    T_ELEM r = 0;
    VEC_LOOP(i) r += a[i] * b[i];
    return r;
}

// Dot product of two row vectors - optimized for mostly 0 b vector
T_ELEM dot_opt(T_VEC(a), T_VEC(b)) {
    T_ELEM r = 0;
    VEC_LOOP(i) if(b[i] != 0) r += a[i] * b[i];
    return r;
}

// a := a*x - b*y such that a[var_ix]==0
void solve_one(T_VEC(a), T_VEC(b), int var_ix) {
    T_ELEM c = gcd(a[var_ix], b[var_ix]);
    T_ELEM af = b[var_ix] / c;
    T_ELEM bf = a[var_ix] / c;
    // print_row(a); printf(" af=%lld\n", af);
    // print_row(b); printf(" bf=%lld\n", bf);
    int to_simplify = 0;
    VEC_LOOP(i) {
        a[i] = a[i]*af - b[i]*bf;
        if(a[i] != 0 && (a[i] > SIMPLIFY_ABOVE || a[i] < -SIMPLIFY_ABOVE)) { to_simplify = 1; }
    }
    // print_row(a);
    if(to_simplify) simplify(a);
}

// Assertion
void assert(int flag, char* message) {
    if(!flag) {
        printf("Assertion failure: %s\n", message);
        fflush(stdout);
        exit(1);
    }
}

pthread_t THREADS[NUM_THREADS];

// Launch threads with an arbitrary function that receives the thread number, then join them
void do_threaded(void *(*start_routine)(void *)) {
    #if NUM_THREADS > 1
        for(size_t i=0; i<NUM_THREADS; i++) {
            if(pthread_create(&THREADS[i], NULL, start_routine, (void*)i) != 0) {
                assert(0, "Could not launch thread");
            }
        }

        for(size_t i=0; i<NUM_THREADS; i++) {
            if(pthread_join(THREADS[i], NULL) != 0) {
                assert(0, "Could not join thread");
            }
        }
    #else
        start_routine(0);
    #endif
}
