
// Utilities
// Utilities for integer vectors

T_IVEC(zero_vector);

// Assertion
void assert(int flag, char* message) {
    if(!flag) {
        printf("Assertion failure: %s\n", message);
        fflush(stdout);
        exit(1);
    }
}


// Call this at the beginning of the program
void util_init(void) {
    VEC_LOOP(i) { zero_vector[i] = 0; }
    
    T_IELEM half = (1 << (sizeof(T_IELEM)*8 / 2));
    if(SIMPLIFY_ABOVE < (half >> 1) || SIMPLIFY_ABOVE > half) {
        printf("SIMPLIFY_ABOVE is suboptimal: SIMPLIFY_ABOVE=%d 'half'=%d\n", SIMPLIFY_ABOVE, half);
    }
    assert(SIMPLIFY_ABOVE <= half, "SIMPLIFY_ABOVE is too large");
}


// Copy data from one vector to another
void vec_icpy (T_IVEC(dest), T_IVEC(src)) {
    memcpy(dest, src, sizeof(T_IELEM)*VECLEN);
}

void vec_fcpy (T_FVEC(dest), T_FVEC(src)) {
    memcpy(dest, src, sizeof(T_FELEM)*VECLEN);
}

// Set a row vector to zero
void vec_izero(T_IVEC(r)) {
    vec_icpy (r, zero_vector);
}

// Scale a vector by a scalar
void vec_iscale(T_IVEC(v), T_IELEM s) {
    VEC_LOOP(i) v[i] *= s;
}

void vec_fscale(T_FVEC(v), T_FELEM s) {
    VEC_LOOP(i) v[i] *= s;
}

// Return if two vectors are the same
int vec_ieq(T_IVEC(a), T_IVEC(b)) {
    VEC_LOOP(i) if(a[i] != b[i]) return 0;
    return 1;
}

// Print a vector
void vec_iprint(T_IVEC(r)) {
    printf("[");
    VEC_LOOP(i) {
        if(i>0) printf(",");
        printf("%2d", r[i]);
    }
    printf("]");
}

// Dot product of two row vectors
T_IELEM idot(T_IVEC(a), T_IVEC(b)) {
    T_IELEM r = 0;
    VEC_LOOP(i) r += a[i] * b[i];
    return r;
}

// Dot product of two row vectors - optimized for mostly 0 b vector
T_IELEM idot_opt(T_IVEC(a), T_IVEC(b)) {
    T_IELEM r = 0;
    VEC_LOOP(i) if(b[i] != 0) r += a[i] * b[i];
    return r;
}

// Return the greatest common divisor (always positive)
T_IELEM gcd(T_IELEM a, T_IELEM b) {
    T_IELEM tmp;
    a = IABS(a);
    b = IABS(b);
    while(1) {
        if (b == 0) return a;
        if (a == 0) return b;
        tmp = b;
        b = a % b;
        a = tmp;
    }
}

// Return the least common multiple
T_IELEM lcm(T_IELEM a, T_IELEM b) {
    return (a / gcd(a, b)) * b;
}

// Simplify a vector. Return if we managed to simplify it
int vec_isimplify(T_IVEC(r)) {
    T_IELEM c = gcd(r[0], r[1]);
    if(c == 1) return 0;
    for(int i=2; i<VECLEN; i++) {
        c = gcd(c, r[i]);
        if(c == 1) return 0;
    }
    if(c == 0) return 0;
    VEC_LOOP(i) r[i] /= c;
    return 1;
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
