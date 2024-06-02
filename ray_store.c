
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

void rs_assert_bitmap_size(int size) {
    // Check that the bitmap is at least this size
    printf("bitmap size %ld required size %d\n", sizeof(T_BITMAP_ELEM)*NUM_BITMAP*8, size);
    assert(sizeof(T_BITMAP_ELEM)*NUM_BITMAP*8 >= size, "bitmap size");
}

void rs_extend_store(void) {
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
    for(int i=0; i<RS_ALLOC_STEP; i++) RS_STORE[i+RS_STORE_SIZE].used = 0;
    RS_STORE_SIZE += RS_ALLOC_STEP;
    printf("Extended ray store to %ud entries\n", RS_STORE_SIZE);
}

struct ray_record* rs_allocate_ray(T_VEC(v)) {
    // Ensure a new ray slot is available and return its address
    // Use vec_zero() and ... to initialize
    if(RS_STORE_USED == RS_STORE_SIZE) {
        // running out of store
        // TODO do garbage collection
        rs_extend_store();
    }
    struct ray_record *new_rec = &RS_STORE[RS_STORE_USED];
    RS_STORE_USED++;

    new_rec->used = 1;
    return new_rec;
}
