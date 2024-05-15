#define LABEL "TEST3"
#define AXIOMS 3
#define VARS 4
T_FACTOR axioms[AXIOMS][VARS] = {
    {5,40,-5,-5},
    {1, 8,-1,-1},
    {0,-3, 1,-2}
};
char* human_readable_axioms[AXIOMS] = {"#0","#1","#2"};

#define SOLVER_TARGET
int target_solved=2;
int target_freedoms=2;
int target_free_var=0; // unused
T_FACTOR target_solution[VARS]={0, 0, 0, 0}; // unused

