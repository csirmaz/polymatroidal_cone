
// Module: stores a variable number of rays

// type for bitmaps
#define T_BITMAP_ELEM int
// number of variables in a bitmap
#define NUM_BITMAP 10
#define T_BITMAP(a) T_BITMAP_ELEM a[NUM_BITMAP]
// memory allocation step
#define RS_ALLOC_STEP 1000

struct ray_record {
    T_VEC(coords);     // coordinates of the ray
    T_BITMAP(faces);   // which faces the ray is on
    char used;         // 0 if slot is unused; 1 if used
};

struct ray_record *RS_STORE;
unsigned int RS_STORE_SIZE = 0; // total allocated size of the store
unsigned int RS_STORE_USED = 0; // first known unused slot (may have holes below)
T_BITMAP(zero_bitmap) = {0,0,0,0,0,0,0,0,0,0};

#define U_FREE 0
#define U_USED 1
#define U_POS 2
#define U_NEG 3


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
    for(int i=0; i<RS_ALLOC_STEP; i++) RS_STORE[i+RS_STORE_SIZE].used = U_FREE;
    RS_STORE_SIZE += RS_ALLOC_STEP;
    printf("Extended ray store to ray_store_size=%u\n", RS_STORE_SIZE);
}

void rs_bitmap_cpy(T_BITMAP(dest), T_BITMAP(src)) {
    memcpy(dest, src, sizeof(T_BITMAP_ELEM)*NUM_BITMAP);
}

void rs_bitmap_zero(T_BITMAP(bm)) {
    // Set bitmap to zero
    rs_bitmap_cpy(bm, zero_bitmap);
}

int rs_bitmap_read(T_BITMAP(bm), int ix) {
    int bit_ix = ix % (sizeof(T_BITMAP_ELEM)*8);
    int elem_ix = (ix - bit_ix) / (sizeof(T_BITMAP_ELEM)*8);
    return bm[elem_ix] & (1 << bit_ix);
}

void rs_bitmap_set(T_BITMAP(bm), int ix) {
    int bit_ix = ix % (sizeof(T_BITMAP_ELEM)*8);
    int elem_ix = (ix - bit_ix) / (sizeof(T_BITMAP_ELEM)*8);
    // printf("ix=%d bit_ix=%d elem_ix=%d\n", ix, bit_ix, elem_ix);
    bm[elem_ix] += (1 << bit_ix);
}

struct ray_record* rs_allocate_ray(void) {
    // Ensure a new ray slot is available and return its address
    // Use vec_zero() and rs_zero_bitmap() to initialize
    if(RS_STORE_USED == RS_STORE_SIZE) {
        // running out of store
        // TODO do garbage collection
        _rs_extend_store();
    }
    struct ray_record *new_rec = &RS_STORE[RS_STORE_USED];
    RS_STORE_USED++;
    new_rec->used = U_USED;
    return new_rec;
}


