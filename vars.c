
#define T_ELEM long long int
// long long int  format: %lld
// double  format: %lf
#define ABS(f) llabs(f)

// Length of a vector
#define VECLEN 64

// Simplify vectors if a number is above
#define SIMPLIFY_ABOVE 1024*1024

#define T_VEC(a) T_ELEM a[VECLEN]

#define VECLOOP(i) for(int i=0; i<VECLEN; i++)
