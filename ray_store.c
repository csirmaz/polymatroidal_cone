
// Module: stores a variable number of rays

// type for bitmaps
#define T_BITMAP_ELEM uint64_t
// number of variables in a bitmap
#define NUM_BITMAP 5
#define T_BITMAP(a) T_BITMAP_ELEM a[NUM_BITMAP]
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
T_BITMAP(zero_bitmap) = {0,0,0,0,0}; // see NUM_BITMAP

// Possible values for ray_record.used:
#define U_USED 1 // or "zero" - on the face
#define U_POS 2
#define U_NEG 3

#define RS_BITMAP_ONE ((T_BITMAP_ELEM)1)

void rs_assert_bitmap_size(int size) {
    // Check that the bitmap is at least this size
    printf("bitmap_size=%ld required_bm_size=%d\n", sizeof(T_BITMAP_ELEM)*NUM_BITMAP*8, size);
    assert(sizeof(T_BITMAP_ELEM)*NUM_BITMAP*8 >= size, "bitmap size");
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

void rs_bitmap_cpy(T_BITMAP(dest), T_BITMAP(src)) {
    // Copy one bitmap over another
    memcpy(dest, src, sizeof(T_BITMAP_ELEM)*NUM_BITMAP);
}

void rs_bitmap_zero(T_BITMAP(bm)) {
    // Set bitmap to zero
    rs_bitmap_cpy(bm, zero_bitmap);
}

int rs_bitmap_read(T_BITMAP(bm), int ix) {
    // Read a bit from a bitmap
    int bit_ix = ix % (sizeof(T_BITMAP_ELEM)*8);
    int elem_ix = (ix - bit_ix) / (sizeof(T_BITMAP_ELEM)*8);
    return (bm[elem_ix] & (RS_BITMAP_ONE << bit_ix)) != 0;
}

int rs_bitmap_read2(T_BITMAP(bm), int elem_ix, int bit_ix) {
    return (bm[elem_ix] & (RS_BITMAP_ONE << bit_ix)) != 0;
}

void rs_bitmap_set(T_BITMAP(bm), int ix) {
    // Set a bit on the bitmap
    int bit_ix = ix % (sizeof(T_BITMAP_ELEM)*8);
    int elem_ix = (ix - bit_ix) / (sizeof(T_BITMAP_ELEM)*8);
    // printf("ix=%d bit_ix=%d elem_ix=%d\n", ix, bit_ix, elem_ix);
    bm[elem_ix] += (RS_BITMAP_ONE << bit_ix);
}

int rs_bitmap_set2(T_BITMAP(bm), int elem_ix, int bit_ix) {
    bm[elem_ix] += (RS_BITMAP_ONE << bit_ix);
}

void rs_print_bitmap(T_BITMAP(bm), int max) {
    printf("<");
    for(int i=0; i<max; i++) printf("%d", rs_bitmap_read(bm, i) ? 1 : 0);
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
