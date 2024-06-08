
// Keep slicing a cone by axioms until we get all rays

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <time.h>
#include <unistd.h>
#include <string.h>
#include <math.h>
#include <sys/time.h>
#include <pthread.h>

// #define FULL_FACE_CHECK // whether to check new rays against axioms they were derived from (slower if enabled)
// #define CHECK_BITMAPS // whether to keep checking bitmaps against dot products after each step
// #define DUMP_DATA // whether to dump data after each step. Use axioms.c as reference

#define ALGEBRAIC_TEST // If defined, use algebraic test
// #define COMBINATORIAL_TEST // If defined, use combinatorial test. DO NOT USE BOTH!

// Type for a value in a matrix/vector
#define T_ELEM long long int
// long long int  format: %lld
// double  format: %lf
#define ABS(f) llabs(f)

// Type for ray indices
#define T_RAYIX size_t

#if AXIOMS_FILE == 4
#include "data/axioms4.c"
#elif AXIOMS_FILE == 5
#include "data/axioms5.c"
#elif AXIOMS_FILE == 6
#include "data/axioms6.c"
#endif

// Length of a vector
#define VECLEN VARS
#define T_VEC(a) T_ELEM a[VECLEN]
// Loop through variables / elements in a row vector
#define VEC_LOOP(i) for(int i=0; i<VECLEN; i++)
// Loop through all axioms
#define AXIOM_LOOP(i) for(int i=0; i<AXIOMS; i++)

// Simplify vectors if a number is above
#define SIMPLIFY_ABOVE 1024*1024

#define NUM_THREADS 7

#include "util.c"
#include "ray_store.c"
#include "slicer_solver.c"
#define SO_EARLYSTOP
#include "slicer_solver.c"

int axioms_used[AXIOMS]; // Which axioms have already been used (bool, 0|1; sometimes index)
int num_axioms_used = 0;
// Global variables for check_pairs(), common to all threads
T_RAYIX cp_all_pairs;
int cp_axiom_ix;
T_RAYIX cp_old_number_of_rays;


T_RAYIX new_axiom_ray_pairs(int axiom_ix) {
    // Return how many pos-neg ray pairs we would need to investigate if we added this axiom
    T_RAYIX pos_count = 0, neg_count = 0;

    for(T_RAYIX i=0; i<RS_STORE_RANGE; i++) {
        // Note: due to the garbage collector, there are no unused rays
        T_ELEM d = dot_opt(rs_get_ray(i)->coords, axioms[axiom_ix]);
        if(d > 0) { // positive side
            pos_count++;
        }
        else if(d < 0) { // negative side
            neg_count++;
        }
    }
    return pos_count*neg_count;
}


void dump_data(void) {
    // Print all relevant data
    // WARNING Call after garbage collection
    printf("\nDATA_DUMP_STARTS\n");
    printf("%s\n", LABEL);
    printf("vars=%d total_axioms=%d\n", VARS, AXIOMS);
    printf("AXIOMS_APPLIED\n");
    printf("num_axioms_used=%d\n", num_axioms_used);
    AXIOM_LOOP(a) {
        if(!axioms_used[a]) continue;
        printf("axiom_applied:%d\n", a);
        // print_vec(axioms[a]);
    }
    printf("RAYS\n");
    printf("num_rays=%zu\n", RS_STORE_RANGE);
    rs_dump();
    printf("DATA_DUMP_ENDS\n\n");
    fflush(stdout);
}


void check_bitmaps(void) {
    // Check all bitmaps against dot products
    // WARNING call after garbage collection
    for(T_RAYIX r=0; r<RS_STORE_RANGE; r++) {
        struct ray_record *ray = rs_get_ray(r);
        AXIOM_LOOP(a) {
            if(!axioms_used[a]) {
                assert(bitmap_read(ray->faces, a) == 0, "check_bitmaps: unused bit");
            }
            else {
                T_ELEM d = dot(ray->coords, axioms[a]);
                assert(d >= 0, "check_bitmaps: inside");
                assert(bitmap_read(ray->faces, a) == (d == 0), "check_bitmaps: correlates with bitmap");
            }
        }
    }
    printf("check_bitmaps OK\n"); fflush(stdout); // DEBUG
}


T_RAYIX mark_rays(int axiom_ix) {
    // Mark each ray according to whether it is on the pos or neg side or on the face of the axiom
    struct ray_record* ray;
    T_RAYIX pos_count = 0, neg_count = 0, zero_count = 0;

    for(T_RAYIX i=0; i<RS_STORE_RANGE; i++) {
        // Note: due to the garbage collector, there are no unused rays
        ray = rs_get_ray(i);

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
            zero_count++;
            ray->used = U_USED;
        }
    } // end loop over stored rays

    printf("positive_rays=%zu negative_rays=%zu zero_rays=%zu ray_pairs=%zu\n", pos_count, neg_count, zero_count, pos_count*neg_count);
    fflush(stdout); // DEBUG
    return pos_count*neg_count;
}


void *check_pairs(void *my_thread_num) {
    // Check each pos-neg ray pair
    size_t thread_num = (size_t)my_thread_num;
    int axiom_ix = cp_axiom_ix;

    struct ray_record* ray_pos;
    struct ray_record* ray_neg;
    T_BITMAP(face_bm);
    T_RAYIX new_rays = 0;
    T_RAYIX pairs_checked = 0;
    struct timeval prev_time, current_time;
    struct ray_record* ray;

    gettimeofday(&prev_time, NULL);
    double prev_elapsed_time = 0;
    
    for(T_RAYIX ray_i=0; ray_i<cp_old_number_of_rays; ray_i++) {
        #if NUM_THREADS > 1
            if((ray_i % NUM_THREADS) != thread_num) continue;
        #endif
        ray_pos = rs_get_ray(ray_i);
        if(ray_pos->used != U_POS) continue;
        
        for(T_RAYIX ray_j=0; ray_j<cp_old_number_of_rays; ray_j++) {
            ray_neg = rs_get_ray(ray_j);
            if(ray_neg->used != U_NEG) continue;

            printf("Checking pair... ray_i=%zu ray_j=%zu old_number_of_rays=%zu\n", ray_i, ray_j, cp_old_number_of_rays ); fflush(stdout); // DEBUG
            
            // Timestamp logs
            if((pairs_checked % 50000) == 0) {
                gettimeofday(&current_time, NULL);
                double elapsed = (current_time.tv_sec - prev_time.tv_sec) + (current_time.tv_usec - prev_time.tv_usec) / 1000. / 1000.;
                if(elapsed > prev_elapsed_time + 10) {
                    prev_elapsed_time = elapsed;
                    printf("Axiom #%d (%dth %.2f%%) - %zu pairs checked in %.1lf s, %.6f ms/pair (%.2f%%) thread=%zu\n",
                        axiom_ix,
                        num_axioms_used-1,
                        ((float)(num_axioms_used-1))/((float)AXIOMS)*100.,
                        pairs_checked, 
                        elapsed, 
                        elapsed*1000./(float)pairs_checked,
                        ((float)(pairs_checked*NUM_THREADS))/((float)cp_all_pairs)*100.,
                        thread_num
                    );
                    fflush(stdout);
                }
            }
            pairs_checked++;

            // Get the axioms/faces both rays are on 
            // (restricted to the axioms already used as only those bits are set in the bitmap)
            for(int i=0; i<NUM_BITMAP; i++) face_bm[i] = (ray_pos->faces[i] & ray_neg->faces[i]);

            printf("Ray pos: %zu ", ray_i); // DEBUG
            print_vec(ray_pos->coords); // DEBUG
            printf("\nRay neg: %zu ", ray_j); // DEBUG
            print_vec(ray_neg->coords); // DEBUG
            printf("\nRay pos: %3zu ", ray_i); // DEBUG
            bitmap_print(ray_pos->faces, AXIOMS); // DEBUG
            printf("\nRay neg: %3zu ", ray_j); // DEBUG
            bitmap_print(ray_neg->faces, AXIOMS); // DEBUG
            printf("\nIntersection "); // DEBUG
            bitmap_print(face_bm, AXIOMS); // DEBUG
            printf("\n"); // DEBUG
            
            // Ensure there are at least VARS-2 faces both rays are on; otherwise they can't be
            // on the same 2D facet.
            if(bitmap_bitcount(face_bm) < VARS - 2) {
                printf("Intersection too short\n"); fflush(stdout); // DEBUG
                continue;
            }
            
            #ifdef COMBINATORIAL_TEST
                // Combinatorial test
                // Next, ensure other there is no other ray that is on all the common faces.
                printf("Combinatorial test\n"); // DEBUG
                int good = 1;
                for(T_RAYIX ray_k=0; ray_k<cp_old_number_of_rays; ray_k++) {
                    printf("Combinatorial: checking ray_k=%zu\n", ray_k); // DEBUG
                    if(ray_k == ray_i || ray_k == ray_j) {
                        printf("Skipping\n"); // DEBUG
                        continue;
                    }
                    int good_ray = 0;
                    ray = rs_get_ray(ray_k);
                    for(int i=0; i<NUM_BITMAP; i++) {
                        if((face_bm[i] & ~ray->faces[i]) != 0) { 
                            good_ray = 1;
                            printf("Ray_k:   %3zu ", ray_k); bitmap_print(ray->faces, AXIOMS); // DEBUG
                            printf("\nRay %zu differs at the intersection (good)\n", ray_k); fflush(stdout); // DEBUG
                            // We already know this ray is not on all faces in the intersection, so we don't need to keep checking it.
                            break;
                        }
                    }
                    if(!good_ray) {
                        printf("Ray_k:   %3zu ", ray_k); bitmap_print(ray->faces, AXIOMS); // DEBUG
                        printf("\nRay %zu shares intersection (bad)\n", ray_k); fflush(stdout); // DEBUG
                        // We found a ray that is all faces in the intersection. We don't need to check further rays.
                        good = 0;
                        break;
                    }
                }            
                if(!good) continue;
            #endif
            
            #ifdef ALGEBRAIC_TEST
                // Algebraic test
                // Solve the common axioms + the new one; see if we get a ray
                printf("Algebraic test\n"); // DEBUG
                so_init_matrix(thread_num);
                AXIOM_LOOP(a) {
                    if(bitmap_read(face_bm, a)) {
                        so_add_to_matrix(thread_num, axioms[a]);
                        printf("Adding axiom #%d to solver ", a); print_vec(axioms[a]); printf("\n"); // DEBUG
                    }
                }
                so_add_to_matrix(thread_num, axioms[axiom_ix]);
                printf("Adding the new axiom #%d to solver ", axiom_ix); print_vec(axioms[axiom_ix]); printf("\n"); // DEBUG
                int f = so_solve_early(thread_num);
                if(f != 1) {
                    printf("X: No good solution (%d)\n", f); fflush(stdout); // DEBUG
                    continue;
                }
            #endif
            
            // --- Create new ray ---
            // We have a good case; construct the new ray
            printf("Setting up new ray #%zu...\n", RS_STORE_RANGE); fflush(stdout); // DEBUG
            
            new_rays++;
            ray = rs_allocate_ray();
            
            // Calculate the coordinates
            #ifdef COMBINATORIAL_TEST
                // new_ray := alpha*ray_pos + beta*ray_neg
                // where alpha, beta > 0
                //   and dot(new_ray, new_axiom) == 0
                //       alpha*dot(ray_pos,new_axiom) + beta*dot(ray_neg,new_axiom) == 0
                //    so:
                T_ELEM alpha = 0 - dot_opt(ray_neg->coords, axioms[axiom_ix]);
                T_ELEM beta = dot_opt(ray_pos->coords, axioms[axiom_ix]);
                VEC_LOOP(i) ray->coords[i] = alpha*ray_pos->coords[i] + beta*ray_neg->coords[i];
                simplify(ray->coords);
                printf("Ray new (from combinatorial): #%zu ", RS_STORE_RANGE-1); // DEBUG
                print_vec(ray->coords); printf("\n"); // DEBUG
            #endif      
            #ifdef ALGEBRAIC_TEST
                // In case of the algebraic test, the new ray (without sign) is in solution_coll[thread_num].
                vec_cpy(ray->coords, solution_coll[thread_num]);                
            #endif      
            
            // Store which faces the new ray is on.
            // We only do this for the used axioms/faces, plus the new face, which is already marked used.
            // When the combinatorial test is enabled, this applies to the result from that test only.
            bitmap_zero(ray->faces);
            int neg_faces = 0, pos_faces = 0;
            AXIOM_LOOP(a) {
                if(!axioms_used[a]) continue;
                if(a == axiom_ix || bitmap_read(face_bm, a)) {
                    // We know the ray is on these faces
                    #ifdef FULL_FACE_CHECK
                        T_ELEM d = dot_opt(ray->coords, axioms[a]);
                        printf("Checking against known face %d, dot=%lld\n", a, d); fflush(stdout); // DEBUG
                        if(d != 0) {
                            printf("Axiom %d: ", a); print_vec(axioms[a]); printf("\n"); // DEBUG
                            assert(0, "Full face check, known");
                        }
                    #endif
                    bitmap_set(ray->faces, a);
                }
                else {
                    T_ELEM d = dot_opt(ray->coords, axioms[a]);
                    printf("Checking against other face %d, dot=%lld\n", a, d); fflush(stdout); // DEBUG
                    if(d < 0){ 
                        neg_faces++; 
                    }else if(d > 0){ 
                        pos_faces++; 
                    }else {
                        bitmap_set(ray->faces, a);
                    }
                }
            }
            printf("Face check for other faces: neg_faces=%d pos_faces=%d\n", neg_faces, pos_faces); // DEBUG
            
            #ifdef COMBINATORIAL_TEST
                // This condition should be true as from the combinatorial test the solution should have the right sign.
                assert(neg_faces == 0, "Other face check (neg)");                
            #endif
            #ifdef ALGEBRAIC_TEST
                // In this case, we don't know the direction of the solution
                if(neg_faces > 0) {
                    assert(pos_faces == 0, "Other face check");
                    vec_scale(ray->coords, -1);
                } else {
                    assert(neg_faces == 0, "Other face check (neg)");
                }
            #endif

            printf("Set up new ray. new_rays=%zu RS_STORE_RANGE=%zu pairs_checked=%zu\n", new_rays, RS_STORE_RANGE, pairs_checked); fflush(stdout); // DEBUG
            printf("Bitmap for new ray #%zu: ", RS_STORE_RANGE-1); bitmap_print(ray->faces, AXIOMS); printf("\n"); fflush(stdout); // DEBUG

        } // for ray_j ends
    } // for ray_i ends
    return NULL;
}


void apply_axiom(int axiom_ix) {
    // Apply the new axiom/face axiom_ix to the known rays

    printf("applying_axiom #%d (%dth %.2f%%) prev_total_rays=%zu\n",
        axiom_ix, 
        num_axioms_used,
        ((float)num_axioms_used)/((float)AXIOMS)*100.,
        RS_STORE_RANGE
    );
    print_vec(axioms[axiom_ix]); printf("\n"); fflush(stdout); // DEBUG

    axioms_used[axiom_ix] = 1;
    num_axioms_used++;
    
    // Set up global variables common to all threads
    cp_all_pairs = mark_rays(axiom_ix);
    cp_axiom_ix = axiom_ix;
    cp_old_number_of_rays = RS_STORE_RANGE;
    
    do_threaded(check_pairs);
    
    printf("Garbage collection...\n"); fflush(stdout); // DEBUG
    rs_garbage_collection();

    printf("Setting up new face...\n"); fflush(stdout); // DEBUG
    // Also fill in the bitmaps for the new axiom
    for(T_RAYIX i=0; i<RS_STORE_RANGE; i++) { // after garbage collection, these are the used rays
        struct ray_record *ray = rs_get_ray(i);
        if(!bitmap_read(ray->faces, axiom_ix)) {
            if(dot_opt(ray->coords, axioms[axiom_ix]) == 0) { 
                bitmap_set(ray->faces, axiom_ix);
            }
        }
    }
    
    #ifdef CHECK_BITMAPS
    check_bitmaps();
    #endif
    
    #ifdef DUMP_DATA
    dump_data();
    #endif

    printf("Adding axiom done. pairs_checked=%zu total_rays=%zu\n\n", cp_all_pairs, RS_STORE_RANGE);
    fflush(stdout);
}


int main(void) {
    printf("Slicer starting LABEL=%s VARS=%d AXIOMS=%d\n", LABEL, VARS, AXIOMS);
    util_init();
    so_init();
    
    rs_init(AXIOMS); // total number of faces
    
    // First we search for VARS independent axioms
    printf("Finding initial independent axioms...\n");

    AXIOM_LOOP(a) axioms_used[a] = 0;
    int next_axiom = 0;
    while(1) {
        axioms_used[next_axiom] = num_axioms_used+1; // use a 1-based index to mark them
        printf("add #%d ", next_axiom); // DEBUG
        num_axioms_used++;
        next_axiom++;
        
        // Try them out with the solver to see if they are independent
        so_init_matrix(0);
        AXIOM_LOOP(a) if(axioms_used[a]) so_add_to_matrix(0, axioms[a]);
        assert(so_rows_coll[0] == num_axioms_used, "Axiom init num");
        int f = so_solve(0);
        printf("-> freedoms=%d\n", f); // DEBUG
        assert(f >= VARS - num_axioms_used, "Axiom init freedoms");
        if(f == VARS - num_axioms_used) {
            // They are independent
            if(num_axioms_used == VARS) break;
        }
        else {
            // Not independent
            num_axioms_used--;
            printf("removing %d\n", next_axiom-1); // DEBUG
            axioms_used[next_axiom-1] = 0;
        }
    }
    
    assert(num_axioms_used == VARS, "num_axioms_used");
    printf("Initial axioms chosen: ");
    AXIOM_LOOP(a) if(axioms_used[a]) printf("%d, ", a);
    printf("\n");
    
    AXIOM_LOOP(a) printf("%d;", axioms_used[a]); printf("\n"); // DEBUG
    
    // Calculate the initial rays
    // Take VARS-1 axioms, solve them, and ensure the resulting ray is inside the remaining axiom
    printf("INITIAL RAYS\n"); fflush(stdout);
    VEC_LOOP(var_ix) {
        
        int miss_axiom = -1;
        so_init_matrix(0);
        AXIOM_LOOP(a) {
            if(axioms_used[a] == var_ix+1) { // Remember we used indices+1, not bool
                miss_axiom = a;
            } else if(axioms_used[a] != 0) {
                so_add_to_matrix(0, axioms[a]);
            }
        }
        printf("Ray #%d to miss face %d, added %zu axioms\n", var_ix, miss_axiom, so_rows_coll[0]); // DEBUG
        assert(miss_axiom != -1, "Init axiom cannot find miss");
        assert(so_rows_coll[0] == VARS - 1, "Init axiom subset rows");
        int f = so_solve_early(0);
        if(f != 1) {
            printf("freedom=%d", f); // DEBUG
            assert(0, "Init axiom subset freedom");
        }
        
        // If the ray is not inside the remaining axiom, flip it
        T_ELEM d = dot_opt(solution_coll[0], axioms[miss_axiom]);
        assert(d != 0, "Init axioms ray indep");
        if(d < 0) vec_scale(solution_coll[0], -1);
        
        // Add the initial ray
        struct ray_record *ray = rs_allocate_ray();
        vec_cpy(ray->coords, solution_coll[0]);

        // Create the bitmap
        // We only fill in the bits for the axioms/faces used
        bitmap_zero(ray->faces);
        AXIOM_LOOP (a) {
            if(!axioms_used[a]) continue;
            T_ELEM d = dot_opt(axioms[a], ray->coords);
            if(a == miss_axiom) {
                assert(d > 0, "Init ray facecheck");
            } else {
                assert(d == 0, "Init ray facecheck");
            }
            if(d == 0) {
                // printf("Set bitmap ix %d\n", a); 
                bitmap_set(ray->faces, a);
            }
        }

        printf("Initialized ray vector:"); // DEBUG
        print_vec(ray->coords); // DEBUG
        printf(" bitmap: "); // DEBUG
        bitmap_print (ray->faces, AXIOMS); // DEBUG
        printf("\n"); // DEBUG
        
    } // end VAR_LOOP(var_ix)

    printf("ADDING MORE AXIOMS\n"); fflush(stdout);
    
    while(1) {
        // Choose the next axiom that has the least ray pairs
        printf("Choosing which axiom/face to use next...\n");
        T_RAYIX min_pairs;
        int min_pairs_axiom = -1;
        AXIOM_LOOP(a) {
            if(axioms_used[a]) continue;
            T_RAYIX pairs = new_axiom_ray_pairs(a);
            printf("  Axiom #%d would result in %zu ray pairs\n", a, pairs); // DEBUG
            if(min_pairs_axiom == -1 || min_pairs > pairs) {
                min_pairs_axiom = a;
                min_pairs = pairs;
            }
        }
        
        printf("Chose axiom #%d to add next\n", min_pairs_axiom);
        apply_axiom(min_pairs_axiom);
        
        if(num_axioms_used == AXIOMS) break;
    }

    AXIOM_LOOP(a) assert(axioms_used[a] != 0, "axioms remain");
    
    #ifdef COMBINATORIAL_TEST
        printf("Used combinatorial test\n");
    #endif
    #ifdef ALGEBRAIC_TEST
        printf("Used algebraic test\n");
    #endif
    printf("Threads=%d TOTAL_RAYS=%zu\n", NUM_THREADS, RS_STORE_RANGE); fflush(stdout);
    
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
