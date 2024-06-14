#define LABEL "SET_N=4 TIGHT=True"
#define SET_N 4
#define AXIOMS 24
#define REAL_AXIOMS 24 // Without "identity" axioms
#define HAS_IDENTITY_AXIOMS 0
#define VARS 11
T_ELEM axioms[AXIOMS][VARS] = {
{1,1,-1,0,0,0,0,0,0,0,0} /* Axiom0 Group0 f({0}) + f({1}) >= f({0, 1})*/,
{0,0,0,-1,1,1,-1,0,0,0,0} /* Axiom1 Group1 f({0, 2}) + f({1, 2}) >= f({2}) + f({0, 1, 2, 3})*/,
{0,0,0,0,0,0,-1,-1,1,1,0} /* Axiom2 Group1 f({0, 3}) + f({1, 3}) >= f({0, 1, 2, 3}) + f({3})*/,
{0,0,0,0,0,0,1,0,0,0,-1} /* Axiom3 Group2 f({0, 1, 2, 3}) >= f({2, 3})*/,
{1,0,0,1,-1,0,0,0,0,0,0} /* Axiom4 Group0 f({0}) + f({2}) >= f({0, 2})*/,
{0,-1,1,0,0,1,-1,0,0,0,0} /* Axiom5 Group1 f({0, 1}) + f({1, 2}) >= f({1}) + f({0, 1, 2, 3})*/,
{0,0,0,0,0,0,-1,-1,1,0,1} /* Axiom6 Group1 f({0, 3}) + f({2, 3}) >= f({0, 1, 2, 3}) + f({3})*/,
{0,0,0,0,0,0,1,0,0,-1,0} /* Axiom7 Group2 f({0, 1, 2, 3}) >= f({1, 3})*/,
{0,1,0,1,0,-1,0,0,0,0,0} /* Axiom8 Group0 f({1}) + f({2}) >= f({1, 2})*/,
{-1,0,1,0,1,0,-1,0,0,0,0} /* Axiom9 Group1 f({0, 1}) + f({0, 2}) >= f({0}) + f({0, 1, 2, 3})*/,
{0,0,0,0,0,0,-1,-1,0,1,1} /* Axiom10 Group1 f({1, 3}) + f({2, 3}) >= f({0, 1, 2, 3}) + f({3})*/,
{0,0,0,0,0,0,1,0,-1,0,0} /* Axiom11 Group2 f({0, 1, 2, 3}) >= f({0, 3})*/,
{1,0,0,0,0,0,0,1,-1,0,0} /* Axiom12 Group0 f({0}) + f({3}) >= f({0, 3})*/,
{0,-1,1,0,0,0,-1,0,0,1,0} /* Axiom13 Group1 f({0, 1}) + f({1, 3}) >= f({1}) + f({0, 1, 2, 3})*/,
{0,0,0,-1,1,0,-1,0,0,0,1} /* Axiom14 Group1 f({0, 2}) + f({2, 3}) >= f({2}) + f({0, 1, 2, 3})*/,
{0,0,0,0,0,-1,1,0,0,0,0} /* Axiom15 Group2 f({0, 1, 2, 3}) >= f({1, 2})*/,
{0,1,0,0,0,0,0,1,0,-1,0} /* Axiom16 Group0 f({1}) + f({3}) >= f({1, 3})*/,
{-1,0,1,0,0,0,-1,0,1,0,0} /* Axiom17 Group1 f({0, 1}) + f({0, 3}) >= f({0}) + f({0, 1, 2, 3})*/,
{0,0,0,-1,0,1,-1,0,0,0,1} /* Axiom18 Group1 f({1, 2}) + f({2, 3}) >= f({2}) + f({0, 1, 2, 3})*/,
{0,0,0,0,-1,0,1,0,0,0,0} /* Axiom19 Group2 f({0, 1, 2, 3}) >= f({0, 2})*/,
{0,0,0,1,0,0,0,1,0,0,-1} /* Axiom20 Group0 f({2}) + f({3}) >= f({2, 3})*/,
{-1,0,0,0,1,0,-1,0,1,0,0} /* Axiom21 Group1 f({0, 2}) + f({0, 3}) >= f({0}) + f({0, 1, 2, 3})*/,
{0,-1,0,0,0,1,-1,0,0,1,0} /* Axiom22 Group1 f({1, 2}) + f({1, 3}) >= f({1}) + f({0, 1, 2, 3})*/,
{0,0,-1,0,0,0,1,0,0,0,0} /* Axiom23 Group2 f({0, 1, 2, 3}) >= f({0, 1})*/
};
char* human_readable_axioms[AXIOMS] = {
"Axiom0 Group0 f({0}) + f({1}) >= f({0, 1})",
"Axiom1 Group1 f({0, 2}) + f({1, 2}) >= f({2}) + f({0, 1, 2, 3})",
"Axiom2 Group1 f({0, 3}) + f({1, 3}) >= f({0, 1, 2, 3}) + f({3})",
"Axiom3 Group2 f({0, 1, 2, 3}) >= f({2, 3})",
"Axiom4 Group0 f({0}) + f({2}) >= f({0, 2})",
"Axiom5 Group1 f({0, 1}) + f({1, 2}) >= f({1}) + f({0, 1, 2, 3})",
"Axiom6 Group1 f({0, 3}) + f({2, 3}) >= f({0, 1, 2, 3}) + f({3})",
"Axiom7 Group2 f({0, 1, 2, 3}) >= f({1, 3})",
"Axiom8 Group0 f({1}) + f({2}) >= f({1, 2})",
"Axiom9 Group1 f({0, 1}) + f({0, 2}) >= f({0}) + f({0, 1, 2, 3})",
"Axiom10 Group1 f({1, 3}) + f({2, 3}) >= f({0, 1, 2, 3}) + f({3})",
"Axiom11 Group2 f({0, 1, 2, 3}) >= f({0, 3})",
"Axiom12 Group0 f({0}) + f({3}) >= f({0, 3})",
"Axiom13 Group1 f({0, 1}) + f({1, 3}) >= f({1}) + f({0, 1, 2, 3})",
"Axiom14 Group1 f({0, 2}) + f({2, 3}) >= f({2}) + f({0, 1, 2, 3})",
"Axiom15 Group2 f({0, 1, 2, 3}) >= f({1, 2})",
"Axiom16 Group0 f({1}) + f({3}) >= f({1, 3})",
"Axiom17 Group1 f({0, 1}) + f({0, 3}) >= f({0}) + f({0, 1, 2, 3})",
"Axiom18 Group1 f({1, 2}) + f({2, 3}) >= f({2}) + f({0, 1, 2, 3})",
"Axiom19 Group2 f({0, 1, 2, 3}) >= f({0, 2})",
"Axiom20 Group0 f({2}) + f({3}) >= f({2, 3})",
"Axiom21 Group1 f({0, 2}) + f({0, 3}) >= f({0}) + f({0, 1, 2, 3})",
"Axiom22 Group1 f({1, 2}) + f({1, 3}) >= f({1}) + f({0, 1, 2, 3})",
"Axiom23 Group2 f({0, 1, 2, 3}) >= f({0, 1})"
};
/* VARIABLES / SETS
 Variable0 is for set {0}
 Variable1 is for set {1}
 Variable2 is for set {0, 1}
 Variable3 is for set {2}
 Variable4 is for set {0, 2}
 Variable5 is for set {1, 2}
 Variable6 is for set {0, 1, 2, 3}
 Variable7 is for set {3}
 Variable8 is for set {0, 3}
 Variable9 is for set {1, 3}
 Variable10 is for set {2, 3}
*/
/* AXIOM GROUPS
 Group0 size=6 {0, 4, 8, 12, 16, 20}
 Group1 size=12 {1, 2, 5, 6, 9, 10, 13, 14, 17, 18, 21, 22}
 Group2 size=6 {3, 7, 11, 15, 19, 23}
*/
