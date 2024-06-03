
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#include "vars.c"
#include "util.c"
#include "ray_store.c"

int main(void) {
    
    util_init();
    
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
    rs_bitmap_zero(bm);
    assert(rs_bitmap_read(bm, 0) == 0, "bm1");
    assert(rs_bitmap_read(bm, 5) == 0, "bm2");
    assert(rs_bitmap_read(bm, 32+1) == 0, "bm3");
    assert(rs_bitmap_read(bm, 64+2) == 0, "bm4");
    rs_bitmap_set(bm, 5);
    assert(bm[0] == 32, "bm5");
    assert(rs_bitmap_read(bm, 5) != 0, "bm6");
    assert(rs_bitmap_read(bm, 0) == 0, "bm7");
    rs_bitmap_set(bm, 64+2);
    assert(rs_bitmap_read(bm, 64+2) != 0, "bm8");
    assert(rs_bitmap_read(bm, 32+1) == 0, "bm9");
    rs_bitmap_set(bm, 32+1);
    assert(bm[1] == 2, "bm10");
    assert(bm[0] == 32, "bm11");
    assert(rs_bitmap_read(bm, 32+1) != 0, "bm12");
    assert(rs_bitmap_read(bm, 64+1) == 0, "bm13");
}
