#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#define MTYPE unsigned long

MTYPE*** partial_results;  // partial results
/*
 * To every point (i,j) where i>=0 j>=0 we assing the values [1*choose(i+j,i), i*choose(i+j,i), j*choose(i+j,i)]
 * 
 * partial_results[k][b] holds the sum of these value for the points (i,j) where
 * - i + j = k
 * - the i'th bit of b is set
 */

void die(char *m) {
    printf("%s\n", m);
    exit(1);
}

MTYPE choose(int a, int b) {
    if(a<0 || b<0 || a<b) return 0;
    if(a==b || b==0) return 1;
    return choose(a-1, b-1) + choose(a-1, b);
}

void get_result(
        int k, // which diagonal are we in; k=i+j
        MTYPE mask, // has those bits set that we need to have filled in in this diagonal
        MTYPE prev_sum[3] // the sum so far from previous (outside) diagonals
) {
    /* Calculate the complete sum recursively starting at the outermost/topmost diagonal (k==param_k).
     * - Loop through all possible choices of points in this diagonal (b)
     * - Using partial_results maintain a sum of the 3 values so far based on the coices made on outer diagonals
     * - Restrict choices in the next lower diagonal using a mask:
     *    - If the i'th point in the diagonal k is chosen, in diagonal k-1 we need to choose the i'th and (i-1)'th points (except for -1 or k+1).
     *    - So the new mask is the current mask except for the first bit OR the current mask except the last bit
     */
    for(MTYPE b=0; b<(1<<((MTYPE)k+1)); b++) { // all possible combinations in this diagonal
        if((b & mask) == mask) { // the required points are chosen
            MTYPE new_sum[3];
            new_sum[0] = prev_sum[0] + partial_results[k][b][0];
            new_sum[1] = prev_sum[1] + partial_results[k][b][1];
            new_sum[2] = prev_sum[2] + partial_results[k][b][2];
            
            if(k == 0) { // we reached the bottommost diagonal, so stop
                printf("Sum: %ld, %ld, %ld\n", new_sum[0], new_sum[1], new_sum[2]);
            }
            else {
                // Create the mask for the lower diagonal
                MTYPE new_mask = ((b & ((1<<((MTYPE)k))-1)) | (b>>1));
                get_result(k-1, new_mask, new_sum);
            }
        }
    }
}

// Unit tests
int test_no = 0;
void assert(int b) {
    test_no++;
    if(!b) { printf("Test %d failed\n", test_no); exit(1); }
}

void tests(void) {
    assert(choose(2,0) == 1);
    assert(choose(2,1) == 2);
    assert(choose(3,1) == 3);
    assert(choose(2,2) == 1);
    assert(choose(3,2) == 3);
    assert(choose(4,2) == 6);
    assert(choose(6,6) == 1); // Test7
}

int main(int argc, char *argv[]) {
    if(argc != 2) { die("Usage: $0 k"); }
    int param_k;
    if(sscanf(argv[1], "%d", &param_k) != 1) { die("Cannot read k"); }
    
    tests();
    
    // Allocate memory
    partial_results = (MTYPE***)malloc(param_k * sizeof(MTYPE**));
    if(partial_results == NULL) die("oom");
    for(int k=0; k<=param_k; k++) {
        partial_results[k] = (MTYPE**)malloc((1<<(k+1)) * sizeof(MTYPE*));
        if(partial_results[k] == NULL) die("oom");
        for(MTYPE b=0; b<(1<<((MTYPE)k+1)); b++) { // all possible combinations in this diagonal
            partial_results[k][b] = (MTYPE*)malloc(3 * sizeof(MTYPE));
            if(partial_results[k][b] == NULL) die("oom");
        }
    }
    
    printf("Calculate partial results\n");
    // partial_results[k] corresponds to data at (i,j) where i+j=k
    // partial_results[k][b] stores the sum of triads where out of the k+1 (i,k-i) points those are chosen where b has a 1 bit
    for(int k=0; k<=param_k; k++) {
        for(MTYPE b=0; b<(1<<((MTYPE)k+1)); b++) { // all possible combinations in this diagonal
            // printf("k=%d b=%d\n", k, b);
            MTYPE sum[3] = {0,0,0};
            for(int i=0; i<k+1; i++) { // decompose the combination
                if(b & (1<<(MTYPE)i)) {
                    MTYPE binom = choose(k,i);
                    sum[0] += binom;   // 1 * choose(i+j,i)
                    sum[1] += binom*i; // i * choose(i+j,i)
                    sum[2] += binom*(k-i); // j * choose(i+j,i)
                }
            }
            partial_results[k][b][0] = sum[0];
            partial_results[k][b][1] = sum[1];
            partial_results[k][b][2] = sum[2];
        } // end for b
    } // end for k
    
    assert(partial_results[0][0][0] == 0);
    assert(partial_results[0][0][1] == 0);
    assert(partial_results[0][0][2] == 0);
    assert(partial_results[0][1][0] == 1);
    assert(partial_results[0][1][1] == 0);
    assert(partial_results[0][1][2] == 0); // Test13

    if(param_k >= 1) {
    assert(partial_results[1][0][0] == 0);
    assert(partial_results[1][0][1] == 0);
    assert(partial_results[1][0][2] == 0);
    assert(partial_results[1][1][0] == 1);
    assert(partial_results[1][1][1] == 0);
    assert(partial_results[1][1][2] == 1);
    assert(partial_results[1][2][0] == 1);
    assert(partial_results[1][2][1] == 1);
    assert(partial_results[1][2][2] == 0);
    assert(partial_results[1][3][0] == 2);
    assert(partial_results[1][3][1] == 1);
    assert(partial_results[1][3][2] == 1);
    }
    
    printf("Now calculate results from the top\n");
    MTYPE tmp_sum[3] = {0,0,0};
    get_result(param_k, 0, tmp_sum);
    
    printf("END\n");
}
