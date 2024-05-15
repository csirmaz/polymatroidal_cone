#define LABEL "TEST0"
#define AXIOMS 500
#define VARS 100

// All below unused

T_FACTOR axioms[AXIOMS][VARS];
char* human_readable_axioms[AXIOMS];

#define SOLVER_TARGET
int target_solved=3;
int target_freedoms=1;
int target_free_var=3;
T_FACTOR target_solution[VARS];
