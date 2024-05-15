#define LABEL "TEST1"
#define AXIOMS 3
#define VARS 4
T_FACTOR axioms[AXIOMS][VARS] = {
    {5,40,-5,-5},
    {1, 8,-1,-1},
    {0,-3, 1,-2}
};

#define SOLVER_TARGET
int target_solved=2;
int target_freedoms=2;
T_FACTOR target_solution[VARS]={0, 0, 0, 0}; // unused

