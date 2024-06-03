
// Keep slicing a cone by axioms until we get all rays

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>

#define FULL_FACE_CHECK // whether to check new rays against axioms they were derived from (slower if enabled)
// #define DEBUG // debug messages from this file
// #define SO_DEBUG // debug messages from the solver

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

// Axioms extended with identities
T_ELEM ext_axioms[ALL_AXIOMS][VARS];

struct timeval prev_time, current_time;


void apply_axiom(int axiom_ix) {
    // Apply an axiom to the known rays

    printf("applying_axiom=%d prev_total_rays=%d\n", axiom_ix, RS_STORE_RANGE);
    #ifdef DEBUG
        print_vec(ext_axioms[axiom_ix]); printf("\n"); fflush(stdout);
    #endif
    
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
    #ifdef DEBUG
        fflush(stdout);
    #endif
    
    // (2) For each pos-neg ray pair
    T_BITMAP(face_bm);
    T_RAYIX old_number_of_rays = RS_STORE_RANGE;
    T_RAYIX new_rays = 0;
    T_RAYIX pairs_checked = 0;
    int bm_counter;
    
    for(T_RAYIX ray_i=0; ray_i<old_number_of_rays; ray_i++) {
        if(RS_STORE[ray_i].used != U_POS) continue;
        ray_pos = &RS_STORE[ray_i];
        
        for(T_RAYIX ray_j=0; ray_j<old_number_of_rays; ray_j++) {
            if(RS_STORE[ray_j].used != U_NEG) continue;
            ray_neg = &RS_STORE[ray_j];

            #ifdef DEBUG            
                printf("Checking pair... ray_i=%d ray_j=%d old_number_of_rays=%u\n", ray_i, ray_j, old_number_of_rays); fflush(stdout);
            #endif
            
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

            // Get the axioms/faces both are on
            for(int i=0; i<NUM_BITMAP; i++) face_bm[i] = (ray_pos->faces[i] & ray_neg->faces[i]);

            #ifdef DEBUG
                printf("Ray pos: %d ", ray_i);
                print_vec(ray_pos->coords);
                printf("\nRay neg: %d ", ray_j);
                print_vec(ray_neg->coords);
                printf("\nRay pos: %d ", ray_i);
                rs_print_bitmap(ray_pos->faces, ALL_AXIOMS);
                printf("\nRay neg: %d ", ray_j);
                rs_print_bitmap(ray_neg->faces, ALL_AXIOMS);
                printf("\nIntersection ");
                rs_print_bitmap(face_bm, ALL_AXIOMS);
                printf("\n");
            #endif
            
            // Check that these axioms + the new one solve to a new ray
            
            #ifdef DEBUG            
                printf("Building the matrix...\n"); fflush(stdout);
            #endif
                
            // Solve the matrix
            so_init_matrix();
            // Add the axioms where face_bm has a bit
            #ifdef DEBUG
                printf("Adding axioms: "); fflush(stdout);
            #endif
            bm_counter = 0;
            for(int bm_elem_ix=0; bm_elem_ix<NUM_BITMAP; bm_elem_ix++) {
                for(int bm_bit_ix=0; bm_bit_ix<sizeof(T_BITMAP_ELEM)*8; bm_bit_ix++) {
                    // printf("elem=%d bit=%d c=%d\n", bm_elem_ix, bm_bit_ix, bm_counter);
                    if(rs_bitmap_read2(face_bm, bm_elem_ix, bm_bit_ix)) {
                        #ifdef DEBUG
                            printf("%d ", bm_counter);
                        #endif
                        so_add_to_matrix(ext_axioms[bm_counter]);
                    }
                    bm_counter++;
                    if(bm_counter >= ALL_AXIOMS) break;
                }
                if(bm_counter >= ALL_AXIOMS) break;
            }
            #ifdef DEBUG
                printf(" finally %d\n", axiom_ix);
            #endif
            // Add the current face
            so_add_to_matrix(ext_axioms[axiom_ix]);
            
            #ifdef DEBUG
                printf("Rows in solver: %d\nSolving...\n", so_rows); fflush(stdout);
            #endif
                
            // Skip if there are not enough rows
            if(so_rows < VARS - 1) {
                #ifdef DEBUG
                    printf("X: Not enough rows\n"); fflush(stdout);
                #endif
                continue;
            }

            // Check if there are any 0 columns
            // This makes things slower
            // if(so_has_zero_columns()) continue;
            
            if(!so_solve()) {
                #ifdef DEBUG
                    printf("X: Solver returned false\n"); fflush(stdout);
                #endif
                continue;
            }
            // Solution is in `solution`
            
            #ifdef DEBUG            
                printf("Setting up new ray...\n"); fflush(stdout);
            #endif
                
            // New ray
            new_rays++;
            ray = rs_allocate_ray();
            // So possibly we have reallocated the memory, so get the new pointer. Do it here so it wouldn't happen too often
            ray_pos = &RS_STORE[ray_i];
            vec_cpy(ray->coords, solution); // store the coordinates
            
            // Store which faces the new ray is on
            // The new ray is on all faces that ray_post and ray_neg were on
            rs_bitmap_cpy(ray->faces, face_bm);
            // The new ray is also on the new face
            rs_bitmap_set(ray->faces, axiom_ix);
            
            // Check the rest of the faces
            bm_counter = 0;
            for(int bm_elem_ix=0; bm_elem_ix<NUM_BITMAP; bm_elem_ix++) {
                for(int bm_bit_ix=0; bm_bit_ix<sizeof(T_BITMAP_ELEM)*8; bm_bit_ix++) {
                    if(bm_counter != axiom_ix && !rs_bitmap_read2(face_bm, bm_elem_ix, bm_bit_ix)) {
                        if(dot_opt(solution, ext_axioms[bm_counter]) == 0)
                            rs_bitmap_set2(ray->faces, bm_elem_ix, bm_bit_ix);
                    }
                    #ifdef FULL_FACE_CHECK
                    else {
                        assert(dot_opt(solution, ext_axioms[bm_counter]) == 0, "Full face check");
                    }
                    #endif
                    bm_counter++;
                    if(bm_counter >= ALL_AXIOMS) break;
                }
                if(bm_counter >= ALL_AXIOMS) break;
            }

            #ifdef DEBUG            
                printf("Set up new ray. new_rays=%d RS_STORE_RANGE=%u\n", new_rays, RS_STORE_RANGE); fflush(stdout);
            #endif

        } // for ray_j ends
    } // for ray_i ends
    
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
    VEC_LOOP(j) VEC_LOOP(i) ext_axioms[j][i] = (i==j ? 1 : 0);
    ROW_LOOP(j) VEC_LOOP(i) ext_axioms[VARS+j][i] = axioms[j][i];
    
    // Add the rays we know from the identity matrix
    VEC_LOOP(i) {
        struct ray_record *ray = rs_allocate_ray();
        // Create the coordinates (one 1)
        vec_zero(ray->coords);
        ray->coords[i] = 1;
        
        // Create the bitmap
        rs_bitmap_zero(ray->faces);
        for(int a=0; a<ALL_AXIOMS; a++)
            if(dot_opt(ext_axioms[a], ray->coords) == 0)
                rs_bitmap_set(ray->faces, a);
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
