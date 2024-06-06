
// Module: stores a variable number of rays

// type for bitmaps (one element)
#define T_BITMAP_ELEM uint64_t
// number of variables/elements in a bitmap
#define NUM_BITMAP 4
#define T_BITMAP(a) T_BITMAP_ELEM a[NUM_BITMAP]

// How many bits in one element of a bitmap
#define BITMAP_ELEM_BITS (sizeof(T_BITMAP_ELEM)*8)
// How many bits in total in a bitmap
#define BITMAP_BITS (BITMAP_ELEM_BITS*NUM_BITMAP)
// How many bytes in total in a bitmap
#define BITMAP_BYTES (sizeof(T_BITMAP_ELEM)*NUM_BITMAP)

#define BITMAP_ONE ((T_BITMAP_ELEM)1u)
#define BITMAP_ZERO ((T_BITMAP_ELEM)0u)

// log2(T_BITMAP_ELEM_BITS)
#define BITMAP_ELOG 6
#define BITMAP_IMASK ((1<<BITMAP_ELOG)-1)

// memory allocation step
#define RS_ALLOC_STEP 100000

struct ray_record {
    T_VEC(coords);     // coordinates of the ray
    T_BITMAP(faces);   // which faces the ray is on (stores all faces, not just the ones already used)
    char used;         // 0 if slot is unused; 1 if used
};

struct ray_record *RS_STORE;
T_RAYIX RS_STORE_SIZE = 0; // total allocated size of the store
T_RAYIX RS_STORE_RANGE = 0; // first known unused slot (may have holes below) -- After garbage collection this is also the number of rays in the store
T_BITMAP(zero_bitmap) = {0,0,0,0}; // see NUM_BITMAP

// Possible values for ray_record.used:
#define U_USED 1 // or "zero" - on the face
#define U_POS 2
#define U_NEG 3

void rs_init(int required_bitmap_size) {
    // Run this at the very beginning of the code
    // Check that the bitmap is at least this size
    printf("bitmap_size=%ld required_bm_size=%d\n", BITMAP_BITS, required_bitmap_size );
    assert( BITMAP_BITS >= required_bitmap_size, "bitmap size");
    // Check RS_BITMAP_ELOG
    assert( BITMAP_ELEM_BITS == (1 << BITMAP_ELOG ), "RS_BITMAP_ELOG");
}

void _rs_extend_store(void) {
    // Extend the store by RS_ALLOC_STEP records
    if(RS_STORE_SIZE == 0) {
        RS_STORE = malloc(sizeof(struct ray_record) * RS_ALLOC_STEP);
    } else {
        RS_STORE = realloc(RS_STORE, sizeof(struct ray_record) * (RS_STORE_SIZE + RS_ALLOC_STEP));
    }
    if(RS_STORE == NULL) {
        printf("Cannot allocate more memory to ray store\n");
        exit(1);
    }
    RS_STORE_SIZE += RS_ALLOC_STEP;
    printf("Extended ray store to ray_store_size=%zu\n", RS_STORE_SIZE);
    fflush(stdout);
}

inline void bitmap_cpy(T_BITMAP(dest), T_BITMAP(src)) {
    // Copy one bitmap over another
    memcpy(dest, src, BITMAP_BYTES );
}

void bitmap_zero(T_BITMAP(bm)) {
    // Set bitmap to zero
    // TODO Check which one is faster
    // rs_bitmap_cpy(bm, zero_bitmap);
    for(int i=0; i<NUM_BITMAP; i++) bm[i] = BITMAP_ZERO;
}

int bitmap_read(T_BITMAP(bm), int ix) {
    // Read the ix'th bit from a bitmap
    int bit_ix = ix & BITMAP_IMASK; // % BITMAP_ELEM_BITS;
    int elem_ix = ix >> BITMAP_ELOG; // (ix - bit_ix) / BITMAP_ELEM_BITS;
    // printf("READ ix=%d bit_ix=%d elem_ix=%d\n", ix, bit_ix, elem_ix);
    return (bm[elem_ix] & ( BITMAP_ONE << bit_ix)) != BITMAP_ZERO;
}

void bitmap_set(T_BITMAP(bm), int ix) {
    // Set the ix'th bit in a bitmap
    int bit_ix = ix & BITMAP_IMASK; // % BITMAP_ELEM_BITS;
    int elem_ix = ix >> BITMAP_ELOG; // (ix - bit_ix) / BITMAP_ELEM_BITS;
    // printf("SET ix=%d bit_ix=%d elem_ix=%d\n", ix, bit_ix, elem_ix);
    bm[elem_ix] |= ( BITMAP_ONE << bit_ix);
}

static inline int bitmap_elem_bitcount(T_BITMAP_ELEM v) {
    // Return the number of 1s in a bitmap element
    #if __x86_64__ || __amd64__	/* assembler code on these architectures */
    register T_BITMAP_ELEM res;
    __asm__( "popcnt %[w], %[t]"
         :[t] "=rm" (res)
         :[w] "rm"  (v)
    );
    return (int)res;
    #else
    #error Wrong CPU architecture
    /* Sample implementation for a 64 bit bitmap */
    v=(v&0x5555555555555555ul)+((v>>1)&0x5555555555555555ul);
    v=(v&0x3333333333333333ul)+((v>>2)&0x3333333333333333ul);
    v=(v&0x0F0F0F0F0F0F0F0Ful)+((v>>4)&0x0F0F0F0F0F0F0F0Ful);
    v=(v&0x00FF00FF00FF00FFul)+((v>>8)&0x00FF00FF00FF00FFul);
    v=(v&0x0000FFFF0000FFFFul)+((v>>16)&0x0000FFFF0000FFFFul);
    return (int)((v&0xFFFF)+(v>>32));
    #endif
}

int bitmap_bitcount(T_BITMAP(bm)) {
    // Return the number of 1s in a bitmap
    int c = 0;
    for(int i=0; i<NUM_BITMAP; i++) c+= bitmap_elem_bitcount(bm[i]);
    return c;
}

void bitmap_print(T_BITMAP(bm), int max) {
    printf("<");
    for(int i=0; i<max; i++) printf("%d", bitmap_read (bm, i) ? 1 : 0);
    printf(">");
}

struct ray_record* rs_allocate_ray(void) {
    // Ensure a new ray slot is available and return its address
    // Use vec_zero() and rs_zero_bitmap() to initialize
    if(RS_STORE_RANGE == RS_STORE_SIZE) {
        // running out of store
        _rs_extend_store();
    }
    struct ray_record *new_rec = &RS_STORE[RS_STORE_RANGE];
    RS_STORE_RANGE++;
    new_rec->used = U_USED;
    return new_rec;
}

void rs_garbage_collection(void) {
    // Drop rays marked as negative
    // Fill in holes
    T_RAYIX current = 0;
    while(1) {
        assert(RS_STORE_RANGE > 0, "Uh-oh, no rays left?");
        if(current >= RS_STORE_SIZE) { break; }
        if(current >= RS_STORE_RANGE) { break; }
        if(RS_STORE[RS_STORE_RANGE-1].used == U_NEG) { RS_STORE_RANGE--; continue; }
        if(RS_STORE[current].used != U_NEG) { current++; continue; }
        memcpy(&RS_STORE[current], &RS_STORE[RS_STORE_RANGE-1], sizeof(struct ray_record));
        current++;
        RS_STORE_RANGE--;
    }
}
