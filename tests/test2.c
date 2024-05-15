#define LABEL "TEST1"
#define AXIOMS 3
#define VARS 4
T_FACTOR axioms[AXIOMS][VARS] = {
    {5, 0, 0, 7},
    {1, 0,-1,-1},
    {0, 0, 1,-2}
};

#define SOLVER_TARGET
int target_solved=3;
int target_freedoms=1;
T_FACTOR target_solution[VARS]={0, 1, 0, 0};
