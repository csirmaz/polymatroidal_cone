
// Keep slicing a cone by axioms until we get all rays

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define T_ELEM long long int
// long long int  format: %lld
// double  format: %lf
#define ABS(f) llabs(f)

#include "axioms.c"

// Length of a vector
#define VECLEN VARS
#define T_VEC(a) T_ELEM a[VECLEN]
// Loop through variables / elements in a row vector
#define VEC_LOOP(i) for(int i=0; i<VECLEN; i++)
// Loop through all axioms
#define ROW_LOOP(i) for(int i=0; i<AXIOMS; i++)

// Simplify vectors if a number is above
#define SIMPLIFY_ABOVE 1024*1024

#include "util.c"
#include "ray_store.c"
#include "slicer_solver.c"

// Axioms extended with identities
T_ELEM ext_axioms[AXIOMS+VARS][VARS];

void remove_neg_rays() {
    // Set rays marked as negative as unused (free)
    for(int i=0; i<RS_STORE_USED; i++)
        if(RS_STORE[i].used == U_NEG)
            RS_STORE[i].used = U_FREE;
}

void apply_axiom(int axiom_ix) {
    // Apply an axiom to the known rays
    printf("applying_axiom=%d\n", axiom_ix);
    // (1) Check if the ray is on pos, neg side or on the face of the axiom
    struct ray_record* ray;
    struct ray_record* ray_pos;
    struct ray_record* ray_neg;
    int pos_count = 0, neg_count = 0;
    for(int i=0; i<RS_STORE_USED; i++) {
        if(RS_STORE[i].used == U_FREE) continue;
        ray = &RS_STORE[i];

        T_ELEM d = dot_opt(ray->coords, axioms[axiom_ix]);
        if(d > 0) { // positive side
            pos_count++;
            ray->used = U_POS;
        }
        else if(d < 0) { // negative side
            neg_count++;
            ray->used = U_NEG;
        }
        else {
            // We don't reset U_POS to U_USED so make sure these are marked
            ray->used = U_USED;
        }
    } // end loop over stored rays
    printf("positive_rays=%d negative_rays=%d ray_pairs=%d\n", pos_count, neg_count, pos_count*neg_count);
    
    // (2) For each pos-neg ray pair
    T_BITMAP(face_bm);
    int new_rays = 0;
    for(int ray_i=0; ray_i<RS_STORE_USED; ray_i++) {
        if(RS_STORE[ray_i].used != U_POS) continue;
        ray_pos = &RS_STORE[ray_i];
        for(int ray_j=0; ray_j<RS_STORE_USED; ray_j++) {
            if(RS_STORE[ray_j].used != U_NEG) continue;
            ray_neg = &RS_STORE[ray_j];
            
            // Get the axioms/faces both are on
            for(int i=0; i<NUM_BITMAP; i++) face_bm[i] = (ray_pos->faces[i] & ray_neg->faces[i]);
            
            // Check that these axioms + the new one solve to a new ray
            
            // LATER Check if there are any 0 columns
            // TODO create axiom bitmaps if this is frequent enough
            
            // Solve
            T_VEC(solution);
            
            // New ray
            new_rays++;
            ray = rs_allocate_ray();
            vec_cpy(ray->coords, solution); // store the coordinates
            
            // Store which faces the new ray is on
            // The new ray is on all faces that ray_post and ray_neg were on
            rs_bitmap_cpy(ray->faces, face_bm);
            // The new ray is also on the new face
            rs_bitmap_set(ray->faces, axiom_ix);
            // Check the rest of the faces
            for(int i=0; i<AXIOMS+VARS; i++) {
                if(!rs_bitmap_read(ray->faces, i))
                    if(dot_opt(solution, axioms[i]) == 0)
                        rs_bitmap_set(ray->faces, i);
            }
            
        } // for ray_j ends
    } // for ray_i ends
    printf("new_rays=%d total_rays=%u\n", new_rays, RS_STORE_USED);
    
    remove_neg_rays();
}


int main(void) {
    printf("Slicer starting\n");
    util_init();
    
    // Initialize ext_axioms by adding an identity matrix
    VEC_LOOP(j) VEC_LOOP(i) ext_axioms[j][i] = (i==j ? 1 : 0);
    ROW_LOOP(j) VEC_LOOP(i) ext_axioms[VARS+j][i] = axioms[j][i];
    
    rs_assert_bitmap_size(VARS+AXIOMS); // total number of faces
    
    // Add the rays we know from the identity matrix
    VEC_LOOP(i) {
        struct ray_record *ray = rs_allocate_ray();
        vec_zero(ray->coords);
        ray->coords[i] = 1;
        rs_bitmap_zero(ray->faces);
        VEC_LOOP(j) if(i != j) rs_bitmap_set(ray->faces, j);
    }
    
    // TODO when can we forget about the identity axioms/faces?
    
    
}




/*
-
-    int x[4]
-    -> x can be treated as a pointer, === &x[0]
-    -> &x[i] to get address of element
-    -> *x === x[0]
-    
-    int *a[10] === 10 int* pointers
-    inf (*a)[10] === pointer to int[10]
-    -> (*a)[i] to get element
-    
*/
