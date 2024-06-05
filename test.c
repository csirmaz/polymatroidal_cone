
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include "vars.c"
#include "util.c"
#include "ray_store.c"
#include "slicer_solver.c"

int main(void) {
    
    util_init();
    so_init();
    
    // Test gcd()
    assert(gcd(2,3) == 1, "gcd1");
    assert(gcd(3,7) == 1, "gcd2");
    assert(gcd(2,6) == 2, "gcd3");
    assert(gcd(6,10) == 2, "gcd4");
    
    // Test lcm()
    assert(lcm(2,3) == 6, "lcm1");
    assert(lcm(6,10) == 30, "lcm2");
    
    // Test dot()
    T_VEC(a);
    T_VEC(b);
    vec_zero(a);
    vec_zero(b);
    assert(dot(a,b) == 0, "dot1");
    assert(dot_opt(a,b) == 0, "dot1");
    a[0] = 1;
    assert(dot(a,b) == 0, "dot2");
    assert(dot_opt(a,b) == 0, "dot2");
    b[0] = 1;
    assert(dot(a,b) == 1, "dot3");
    assert(dot_opt(a,b) == 1, "dot3");
    a[1] = 2;
    b[1] = 1; // a[1,2] b[1,1]
    assert(dot(a,b) == 3, "dot4");
    assert(dot_opt(a,b) == 3, "dot4");
    
    // Test solve_one()
    vec_zero(a);
    vec_zero(b);
    a[0] = 2; a[1] = 5;
    b[0] = 3; b[1] = 7;
    solve_one(a, b, 0); // [6,15]-[6,14]=[0,1]
    assert(a[0] == 0, "subtract1");
    assert(a[1] == 1, "subtract2");
    assert(b[0] == 3, "subtract3");
    a[0] = 2; a[1] = 8;
    b[0] = 3; b[1] = 6;
    solve_one(a, b, 1); // [6,24]-[12,24]=[-6,0]
    assert(a[0] == -6, "subtract1");
    assert(a[1] == 0, "subtract2");
    assert(b[0] == 3, "subtract3");

    // Test bitmap
    T_BITMAP(bm);
    int s = sizeof(T_BITMAP_ELEM)*8;
    rs_bitmap_zero(bm);
    assert(rs_bitmap_read(bm, 0) == 0, "bm1");
    assert(rs_bitmap_read(bm, 5) == 0, "bm2");
    assert(rs_bitmap_read(bm, s+1) == 0, "bm3");
    assert(rs_bitmap_read(bm, s*2+2) == 0, "bm4");
    rs_bitmap_set(bm, 5);
    assert(bm[0] == 32, "bm5");
    assert(rs_bitmap_read(bm, 5) != 0, "bm6");
    assert(rs_bitmap_read(bm, 0) == 0, "bm7");
    rs_bitmap_set(bm, s*2+2);
    assert(rs_bitmap_read(bm, s*2+2) != 0, "bm8");
    assert(rs_bitmap_read(bm, s+1) == 0, "bm9");
    rs_bitmap_set(bm, s+1);
    assert(bm[1] == 2, "bm10");
    assert(bm[0] == 32, "bm11");
    assert(rs_bitmap_read(bm, s+1) != 0, "bm12");
    assert(rs_bitmap_read(bm, s*2+1) == 0, "bm13");
    
    rs_bitmap_zero(bm);
    for(int i=0; i<sizeof(T_BITMAP_ELEM)*8*NUM_BITMAP; i++) {
        rs_bitmap_set(bm, i);
        // rs_print_bitmap(bm, sizeof(T_BITMAP_ELEM)*8*NUM_BITMAP);
        // printf("\n");
        for(int j=0; j<sizeof(T_BITMAP_ELEM)*8*NUM_BITMAP; j++) {
            // printf("i=%d j=%d o=%d\n", i, j, rs_bitmap_read(bm, j));
            assert((rs_bitmap_read(bm, j)!=0) == (j<=i), "bm100");
        }
    }
    
    // Test ray store & garbage collector
    rs_allocate_ray(); // [1, x, x, x, x
    RS_STORE[0].coords[0] = 1;
    RS_STORE_SIZE = 5;
    assert(RS_STORE_RANGE == 1, "gc1");
    rs_garbage_collection();
    assert(RS_STORE_RANGE == 1, "gc1");

    rs_allocate_ray(); // [1, 2, x, x, x
    RS_STORE[1].coords[0] = 2;
    assert(RS_STORE_RANGE == 2, "gc1");
    rs_garbage_collection();
    assert(RS_STORE_RANGE == 2, "gc1");

    rs_allocate_ray(); // [1, 2, 3, x, x
    RS_STORE[2].coords[0] = 3;
    RS_STORE[1].used = U_NEG; // [1, -, 3, x, x
    rs_garbage_collection(); // [1, 3, x, x, x
    assert(RS_STORE_RANGE == 2, "gc1");
    assert(RS_STORE[0].coords[0] == 1, "gc10");
    assert(RS_STORE[1].coords[0] == 3, "gc11");

    rs_allocate_ray(); // [1, 3, 4, x, x
    RS_STORE[2].coords[0] = 4;
    RS_STORE[2].used = U_NEG; // [1, 3, -, x, x
    RS_STORE[0].used = U_NEG; // [-, 3, -, x, x
    rs_garbage_collection(); // [3, x, x, x, x
    assert(RS_STORE_RANGE == 1, "gc1");
    assert(RS_STORE[0].coords[0] == 3, "gc10");

    rs_allocate_ray();
    rs_allocate_ray();
    rs_allocate_ray();
    rs_allocate_ray(); // [3, u, u, u, u
    assert(RS_STORE_RANGE == 5, "gc1");
    assert(RS_STORE_SIZE == 5, "gc1"); // no extension should happen
    rs_garbage_collection();
    assert(RS_STORE_RANGE == 5, "gc1");
    
    // Test the solver
    T_ELEM m1[2][3] = {
        {0, 0, 1},
        {2, 1, 0}
    };
    so_init_matrix();
    so_add_to_matrix(m1[0]);
    so_add_to_matrix(m1[1]);
    so_solve();
}
