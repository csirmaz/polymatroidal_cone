 
// Here we try to find subsets of the axioms that have >1 freedoms.
// Include this file in slicer.c and launch dependency_check() after all the init functions.

#define DC_RND_MASK 511

// The minimum count of 1s in a bitmap to consider
#if AXIOMS_FILE == 5
#define DC_COUNT_LIMIT 64
#else // AXIOMS_FILE == 6
#define DC_COUNT_LIMIT 210
#endif

// The max number of bitmaps to collect
#define DC_MAX_BITMAPS 100

// Pick a random number for an axiom not in the bitmap
int dc_rnd_new_axiom(T_BITMAP(bm)) {
    int r;
    while(1) {
        r = (rand() & DC_RND_MASK);
        if(r < AXIOMS && !bitmap_read(bm, r)) return r;
    }
}

void dependency_check(void) {
    T_BITMAP(dc_bitmap_masks[DC_MAX_BITMAPS]);
    int dc_bitmap_mask_num = 0;
    int dc_duplicates_found = 0;

    printf("Dependency check starts\n");
    assert(DC_RND_MASK > AXIOMS, "DC_RND_MASK");
    srand(time(NULL) + getpid());
    
    T_BITMAP(selected_axioms);
    
    while(1) { // loop of experiments
        printf("// New experiment\n"); // DEBUG
        int num_axioms = 0;
        int fails = 0;
        bitmap_zero(selected_axioms);        
        while(1) { // loop of axioms
            int new_axiom = dc_rnd_new_axiom(selected_axioms);
            printf("//  Adding #%d to ", new_axiom); bitmap_print(selected_axioms, AXIOMS); printf("\n"); fflush(stdout); // DEBUG
            so_init_matrix(0);
            AXIOM_LOOP(a) {
                if(bitmap_read(selected_axioms, a)) so_add_to_matrix(0, axioms[a]);
            }
            so_add_to_matrix(0, axioms[new_axiom]);
            printf("//  In matrix: %zu\n", so_rows_coll[0]); // DEBUG
            int f = so_solve(0);
            printf("//  Freedoms=%d\n", f); // DEBUG
            if(f <= 1) {
                fails++;
                if(fails > (AXIOMS-num_axioms)*(AXIOMS-num_axioms)) {
                    // the bitmap has a collection with freedom 2
                    printf("// "); bitmap_print(selected_axioms, AXIOMS); printf(" (%d)\n", num_axioms); fflush(stdout);
                    break;
                }
            }
            else {
                bitmap_set(selected_axioms, new_axiom);
                num_axioms++;
                fails = 0;
            }
        } // end loop of axioms
        
        if(num_axioms >= DC_COUNT_LIMIT) {
            // Check if we already have this
            int found = 0;
            for(int i=0; i<dc_bitmap_mask_num; i++) {
                if(bitmap_eq(selected_axioms, dc_bitmap_masks[i])) { found = 1; break; }
            }
            if(found) {
                printf("//Found this mask before\n");
                dc_duplicates_found++;
                continue;
            }
            printf("//Saving the bitmask #%d (duplicates=%d)\n", dc_bitmap_mask_num, dc_duplicates_found);
            bitmap_cpy(dc_bitmap_masks[dc_bitmap_mask_num], selected_axioms);
            dc_bitmap_mask_num++;
            
            if(dc_bitmap_mask_num >= DC_MAX_BITMAPS) {
                printf("//Reached %d masks\n", DC_MAX_BITMAPS);
                break;
            }
            
            if(dc_duplicates_found >= dc_bitmap_mask_num*2) {
                printf("//Too many duplicates\n");
                break;
            }
        }
        
    } // end loop of experiments
    
    // Print the output
    printf("//==== RESULTS FOR N=%d ====\n", SET_N);
    printf("#define SPARSE_MASK_NUM %d\n", dc_bitmap_mask_num);
    printf("T_BITMAP(sparse_masks[%d]) = {\n", dc_bitmap_mask_num);
    for(int i=0; i<dc_bitmap_mask_num; i++) {
        printf("/*"); bitmap_print(dc_bitmap_masks[i], AXIOMS); printf("*/ {");
        for(int j=0; j<NUM_BITMAP; j++) {
            if(j>0) printf(",");
            printf("%luUL", dc_bitmap_masks[i][j]);
        }
        printf("}");
        if(i<dc_bitmap_mask_num-1) printf(",");
        printf("\n");
    }
    printf("};\n\n");
    fflush(stdout);    
}
