
// Keep slicing a cone by axioms until we get all rays

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define FULL_FACE_CHECK // whether to check new rays against axioms they were derived from (slower if enabled)

// Type for a value in a matrix/vector
#define T_ELEM long long int
// long long int  format: %lld
// double  format: %lf
#define ABS(f) llabs(f)

// Type for ray indices
#define T_RAYIX size_t

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
#define SO_EARLYSTOP
#include "slicer_solver.c"

// Axioms extended with identities
T_ELEM ext_axioms[ALL_AXIOMS][VARS];
char axioms_used[ALL_AXIOMS]; // Which axioms have already been used (bool, 0|1)

struct timeval prev_time, current_time;

// TODO no additional axioms


void apply_axiom(int axiom_ix) {
    // Apply an axiom to the known rays

    printf("applying_axiom=%d prev_total_rays=%zu\n", axiom_ix, RS_STORE_RANGE);
    print_vec(ext_axioms[axiom_ix]); printf("\n"); fflush(stdout); // DEBUG
    
    // (1) Check if the ray is on pos, neg side or on the face of the axiom
    struct ray_record* ray;
    struct ray_record* ray_pos;
    struct ray_record* ray_neg;
    T_RAYIX pos_count = 0, neg_count = 0, zero_count = 0;

    for(T_RAYIX i=0; i<RS_STORE_RANGE; i++) {
        // Note: due to the garbage collector, there are no unused rays
        ray = &RS_STORE[i];

        T_ELEM d = dot_opt(ray->coords, ext_axioms[axiom_ix]);
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
            zero_count++;
            ray->used = U_USED;
        }
    } // end loop over stored rays

    printf("positive_rays=%zu negative_rays=%zu zero_rays=%zu ray_pairs=%zu\n", pos_count, neg_count, zero_count, pos_count*neg_count);
    fflush(stdout); // DEBUG
    
    // (2) For each pos-neg ray pair
    T_BITMAP(face_bm);
    T_RAYIX old_number_of_rays = RS_STORE_RANGE;
    T_RAYIX new_rays = 0;
    T_RAYIX pairs_checked = 0;
    
    for(T_RAYIX ray_i=0; ray_i<old_number_of_rays; ray_i++) {
        if(RS_STORE[ray_i].used != U_POS) continue;
        ray_pos = &RS_STORE[ray_i];
        
        for(T_RAYIX ray_j=0; ray_j<old_number_of_rays; ray_j++) {
            if(RS_STORE[ray_j].used != U_NEG) continue;
            ray_neg = &RS_STORE[ray_j];

            printf("Checking pair... ray_i=%zu ray_j=%zu old_number_of_rays=%zu\n", ray_i, ray_j, old_number_of_rays); // DEBUG
            fflush(stdout); // DEBUG
            
            if((pairs_checked % 500000) == 0) {
                gettimeofday(&current_time, NULL);
                double elapsed = (current_time.tv_sec - prev_time.tv_sec) + (current_time.tv_usec - prev_time.tv_usec) / 1000. / 1000.;
                gettimeofday(&prev_time, NULL);
                printf("%zu pairs checked in %lf s, %.6f ms/pair (%.2f%%)\n", 
                    pairs_checked, 
                    elapsed, 
                    elapsed*1000./(float)pairs_checked,
                    ((float)pairs_checked)/((float)(pos_count*neg_count))*100.
                );
                fflush(stdout);
            }
            pairs_checked++;

            // Get the axioms/faces both are on, restricted to the axioms already used
            // Check that these axioms + the new one solve to a new ray

            for(int i=0; i<NUM_BITMAP; i++) face_bm[i] = (ray_pos->faces[i] & ray_neg->faces[i]);

            printf("Ray pos: %zu ", ray_i); // DEBUG
            print_vec(ray_pos->coords); // DEBUG
            printf("\nRay neg: %zu ", ray_j); // DEBUG
            print_vec(ray_neg->coords); // DEBUG
            printf("\nRay pos: %zu ", ray_i); // DEBUG
            rs_print_bitmap(ray_pos->faces, ALL_AXIOMS); // DEBUG
            printf("\nRay neg: %zu ", ray_j); // DEBUG
            rs_print_bitmap(ray_neg->faces, ALL_AXIOMS); // DEBUG
            printf("\nIntersection "); // DEBUG
            rs_print_bitmap(face_bm, ALL_AXIOMS); // DEBUG
            printf("\n"); // DEBUG

            printf("Building the matrix...\n"); fflush(stdout); // DEBUG
                
            so_init_matrix();
            for(int a=0; a<ALL_AXIOMS; a++) {
                if(!axioms_used[a]) continue;
                if(!rs_bitmap_read(face_bm, a)) continue;
                so_add_to_matrix(ext_axioms[a]);
            }
            // Add the current face
            so_add_to_matrix(ext_axioms[axiom_ix]);
            
            printf("Rows in solver: %d vars=%d\n", so_rows, VARS); fflush(stdout); // DEBUG
                
            // Skip if there are not enough rows
            if(so_rows < VARS - 1) {
                printf("X: Not enough rows, VARS=%d\n", VARS); fflush(stdout); // DEBUG
                continue;
            }

            // Check if there are any 0 columns
            // This makes things slower
            // if(so_has_zero_columns()) continue;
            
            if(so_solve_early() != 1) {
                printf("X: No good solution\n"); fflush(stdout); // DEBUG
                continue;
            }
            // Solution is in `solution`
            
            printf("Setting up new ray...\n"); fflush(stdout); // DEBUG
                
            // New ray
            new_rays++;
            ray = rs_allocate_ray();
            // So possibly we have reallocated the memory, so get the new pointer. Do it here so it wouldn't happen too often
            ray_pos = &RS_STORE[ray_i];
            vec_cpy(ray->coords, solution); // store the coordinates
            
            // Store which faces the new ray is on
            rs_bitmap_zero(ray->faces);
            for(int a=0; a<ALL_AXIOMS; a++) {
                if(a == axiom_ix || (axioms_used[a] && rs_bitmap_read(face_bm, a))) {
                    // We know the ray is on these faces
                    #ifdef FULL_FACE_CHECK
                        T_ELEM d = dot_opt(solution, ext_axioms[a]);
                        printf("Checking against known face %d, dot=%lld\n", a, d); fflush(stdout); // DEBUG
                        if(d != 0) {
                            printf("Axiom %d: ", a); print_vec(ext_axioms[a]); printf("\n"); // DEBUG
                            assert(0, "Full face check, known");
                        }
                    #endif
                    rs_bitmap_set(ray->faces, a);
                }
                else {
                    T_ELEM d = dot_opt(solution, ext_axioms[a]);
                    printf("Checking against other face %d, dot=%lld\n", a, d); fflush(stdout); // DEBUG
                    #ifdef FULL_FACE_CHECK
                        if(axioms_used[a] && d < 0) {
                            printf("Axiom %d: ", a); print_vec(ext_axioms[a]); printf("\n"); // DEBUG
                            assert(0, "Full face check, inside");
                        }
                    #endif
                    if(d == 0) rs_bitmap_set(ray->faces, a);
                }
            }

            printf("Set up new ray. new_rays=%zu RS_STORE_RANGE=%zu\n", new_rays, RS_STORE_RANGE); fflush(stdout); // DEBUG
            printf("Bitmap: "); rs_print_bitmap(ray->faces, ALL_AXIOMS); printf("\n"); // DEBUG

        } // for ray_j ends
    } // for ray_i ends
    
    axioms_used[axiom_ix] = 1;
    printf("Garbage collection...\n");
    rs_garbage_collection();
    printf("new_rays=%zu total_rays=%zu pairs_checked=%zu\n\n", new_rays, RS_STORE_RANGE, pairs_checked);
    fflush(stdout);
}


int main(void) {
    printf("Slicer starting LABEL=%s VARS=%d AXIOMS=%d ALL_AXIOMS=%d\n", LABEL, VARS, AXIOMS, ALL_AXIOMS);
    util_init();
    so_init();
    
    rs_assert_bitmap_size(ALL_AXIOMS); // total number of faces
    
    // Initialize ext_axioms by adding an identity matrix
    VEC_LOOP(j) {
        VEC_LOOP(i) ext_axioms[j][i] = (i==j ? 1 : 0);
        axioms_used[j] = 1;
    }
    ROW_LOOP(j) {
        VEC_LOOP(i) ext_axioms[VARS+j][i] = axioms[j][i];
        axioms_used[VARS+j] = 0;
    }
    
    for(int a=0; a<ALL_AXIOMS; a++) {
        printf("Axiom %d:", a);
        print_vec(ext_axioms[a]);
        printf("\n");
    }
    
    // Add the rays we know from the identity matrix
    VEC_LOOP(i) {
        struct ray_record *ray = rs_allocate_ray();
        // Create the coordinates (one 1)
        vec_zero(ray->coords);
        ray->coords[i] = 1;
        
        // Create the bitmap
        rs_bitmap_zero(ray->faces);
        for(int a=0; a<ALL_AXIOMS; a++) {
            if(dot_opt(ext_axioms[a], ray->coords) == 0) {
                // printf("Set bitmap %d\n", a); 
                rs_bitmap_set(ray->faces, a);
            }
        }

        printf("Initialized ray %d vector:", i);
        print_vec(ray->coords);
        printf(" bitmap: ");
        rs_print_bitmap(ray->faces, ALL_AXIOMS);
        printf("\n");
    }
    
    // TODO when can we forget about the identity axioms/faces?
    
    ROW_LOOP(a)
        apply_axiom(VARS+a);
    
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
