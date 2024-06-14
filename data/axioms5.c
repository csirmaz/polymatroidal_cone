#define LABEL "SET_N=5 TIGHT=True"
#define SET_N 5
#define AXIOMS 80
#define REAL_AXIOMS 80 // Without "identity" axioms
#define HAS_IDENTITY_AXIOMS 0
#define VARS 26
T_ELEM axioms[AXIOMS][VARS] = {
{1,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom0 Group0 f({0}) + f({1}) >= f({0, 1})*/,
{0,0,0,-1,1,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom1 Group1 f({0, 2}) + f({1, 2}) >= f({2}) + f({0, 1, 2})*/,
{0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom2 Group1 f({0, 3}) + f({1, 3}) >= f({3}) + f({0, 1, 3})*/,
{0,0,0,0,0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom3 Group2 f({0, 2, 3}) + f({1, 2, 3}) >= f({2, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,0,0,0} /* Axiom4 Group1 f({0, 4}) + f({1, 4}) >= f({4}) + f({0, 1, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,1,1,0,0,0,0} /* Axiom5 Group2 f({0, 2, 4}) + f({1, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,-1,1,1,0} /* Axiom6 Group2 f({0, 3, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,-1} /* Axiom7 Group3 f({0, 1, 2, 3, 4}) >= f({2, 3, 4})*/,
{1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom8 Group0 f({0}) + f({2}) >= f({0, 2})*/,
{0,-1,1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom9 Group1 f({0, 1}) + f({1, 2}) >= f({1}) + f({0, 1, 2})*/,
{0,0,0,0,0,0,0,-1,1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom10 Group1 f({0, 3}) + f({2, 3}) >= f({3}) + f({0, 2, 3})*/,
{0,0,0,0,0,0,0,0,0,-1,1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom11 Group2 f({0, 1, 3}) + f({1, 2, 3}) >= f({1, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,1,-1,0,0,0,0,0} /* Axiom12 Group1 f({0, 4}) + f({2, 4}) >= f({4}) + f({0, 2, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,-1,1,0,0,1,0,0,0,0} /* Axiom13 Group2 f({0, 1, 4}) + f({1, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,-1,1,0,1} /* Axiom14 Group2 f({0, 3, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,-1,0} /* Axiom15 Group3 f({0, 1, 2, 3, 4}) >= f({1, 3, 4})*/,
{0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom16 Group0 f({1}) + f({2}) >= f({1, 2})*/,
{-1,0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom17 Group1 f({0, 1}) + f({0, 2}) >= f({0}) + f({0, 1, 2})*/,
{0,0,0,0,0,0,0,-1,0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom18 Group1 f({1, 3}) + f({2, 3}) >= f({3}) + f({1, 2, 3})*/,
{0,0,0,0,0,0,0,0,-1,0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom19 Group2 f({0, 1, 3}) + f({0, 2, 3}) >= f({0, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,0,1,0,-1,0,0,0,0} /* Axiom20 Group1 f({1, 4}) + f({2, 4}) >= f({4}) + f({1, 2, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,-1,0,1,0,1,0,0,0,0,0} /* Axiom21 Group2 f({0, 1, 4}) + f({0, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,-1,0,1,1} /* Axiom22 Group2 f({1, 3, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,-1,0,0} /* Axiom23 Group3 f({0, 1, 2, 3, 4}) >= f({0, 3, 4})*/,
{1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom24 Group0 f({0}) + f({3}) >= f({0, 3})*/,
{0,-1,1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom25 Group1 f({0, 1}) + f({1, 3}) >= f({1}) + f({0, 1, 3})*/,
{0,0,0,-1,1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom26 Group1 f({0, 2}) + f({2, 3}) >= f({2}) + f({0, 2, 3})*/,
{0,0,0,0,0,-1,1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom27 Group2 f({0, 1, 2}) + f({1, 2, 3}) >= f({1, 2}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,0,0,0,1,-1,0,0} /* Axiom28 Group1 f({0, 4}) + f({3, 4}) >= f({4}) + f({0, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,-1,1,0,0,0,0,0,1,0} /* Axiom29 Group2 f({0, 1, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,1,0,0,0,0,1} /* Axiom30 Group2 f({0, 2, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0,0} /* Axiom31 Group3 f({0, 1, 2, 3, 4}) >= f({1, 2, 4})*/,
{0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom32 Group0 f({1}) + f({3}) >= f({1, 3})*/,
{-1,0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom33 Group1 f({0, 1}) + f({0, 3}) >= f({0}) + f({0, 1, 3})*/,
{0,0,0,-1,0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom34 Group1 f({1, 2}) + f({2, 3}) >= f({2}) + f({1, 2, 3})*/,
{0,0,0,0,-1,0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom35 Group2 f({0, 1, 2}) + f({0, 2, 3}) >= f({0, 2}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,0,0,0,0,1,0,-1,0} /* Axiom36 Group1 f({1, 4}) + f({3, 4}) >= f({4}) + f({1, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,-1,0,1,0,0,0,0,1,0,0} /* Axiom37 Group2 f({0, 1, 4}) + f({0, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,0,1,0,0,0,1} /* Axiom38 Group2 f({1, 2, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,-1,0,0,0,0,0} /* Axiom39 Group3 f({0, 1, 2, 3, 4}) >= f({0, 2, 4})*/,
{0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom40 Group0 f({2}) + f({3}) >= f({2, 3})*/,
{-1,0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom41 Group1 f({0, 2}) + f({0, 3}) >= f({0}) + f({0, 2, 3})*/,
{0,-1,0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0} /* Axiom42 Group1 f({1, 2}) + f({1, 3}) >= f({1}) + f({1, 2, 3})*/,
{0,0,-1,0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom43 Group2 f({0, 1, 2}) + f({0, 1, 3}) >= f({0, 1}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,1,0,0,1,0,0,-1} /* Axiom44 Group1 f({2, 4}) + f({3, 4}) >= f({4}) + f({2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,-1,0,0,0,1,0,0,1,0,0} /* Axiom45 Group2 f({0, 2, 4}) + f({0, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,-1,0,0,0,1,0,0,1,0} /* Axiom46 Group2 f({1, 2, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0} /* Axiom47 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 4})*/,
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0} /* Axiom48 Group0 f({0}) + f({4}) >= f({0, 4})*/,
{0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0} /* Axiom49 Group1 f({0, 1}) + f({1, 4}) >= f({1}) + f({0, 1, 4})*/,
{0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0} /* Axiom50 Group1 f({0, 2}) + f({2, 4}) >= f({2}) + f({0, 2, 4})*/,
{0,0,0,0,0,-1,1,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,1,0,0,0,0} /* Axiom51 Group2 f({0, 1, 2}) + f({1, 2, 4}) >= f({1, 2}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0} /* Axiom52 Group1 f({0, 3}) + f({3, 4}) >= f({3}) + f({0, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,-1,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,1,0} /* Axiom53 Group2 f({0, 1, 3}) + f({1, 3, 4}) >= f({1, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,-1,1,0,-1,0,0,0,0,0,0,0,0,0,0,1} /* Axiom54 Group2 f({0, 2, 3}) + f({2, 3, 4}) >= f({2, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom55 Group3 f({0, 1, 2, 3, 4}) >= f({1, 2, 3})*/,
{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0} /* Axiom56 Group0 f({1}) + f({4}) >= f({1, 4})*/,
{-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0} /* Axiom57 Group1 f({0, 1}) + f({0, 4}) >= f({0}) + f({0, 1, 4})*/,
{0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0} /* Axiom58 Group1 f({1, 2}) + f({2, 4}) >= f({2}) + f({1, 2, 4})*/,
{0,0,0,0,-1,0,1,0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0,0,0,0} /* Axiom59 Group2 f({0, 1, 2}) + f({0, 2, 4}) >= f({0, 2}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0} /* Axiom60 Group1 f({1, 3}) + f({3, 4}) >= f({3}) + f({1, 3, 4})*/,
{0,0,0,0,0,0,0,0,-1,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,1,0,0} /* Axiom61 Group2 f({0, 1, 3}) + f({0, 3, 4}) >= f({0, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,-1,0,1,-1,0,0,0,0,0,0,0,0,0,0,1} /* Axiom62 Group2 f({1, 2, 3}) + f({2, 3, 4}) >= f({2, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom63 Group3 f({0, 1, 2, 3, 4}) >= f({0, 2, 3})*/,
{0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0} /* Axiom64 Group0 f({2}) + f({4}) >= f({2, 4})*/,
{-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0} /* Axiom65 Group1 f({0, 2}) + f({0, 4}) >= f({0}) + f({0, 2, 4})*/,
{0,-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0} /* Axiom66 Group1 f({1, 2}) + f({1, 4}) >= f({1}) + f({1, 2, 4})*/,
{0,0,-1,0,0,0,1,0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0} /* Axiom67 Group2 f({0, 1, 2}) + f({0, 1, 4}) >= f({0, 1}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,-1} /* Axiom68 Group1 f({2, 3}) + f({3, 4}) >= f({3}) + f({2, 3, 4})*/,
{0,0,0,0,0,0,0,0,-1,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,1,0,0} /* Axiom69 Group2 f({0, 2, 3}) + f({0, 3, 4}) >= f({0, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,-1,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,1,0} /* Axiom70 Group2 f({1, 2, 3}) + f({1, 3, 4}) >= f({1, 3}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom71 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 3})*/,
{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0} /* Axiom72 Group0 f({3}) + f({4}) >= f({3, 4})*/,
{-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0} /* Axiom73 Group1 f({0, 3}) + f({0, 4}) >= f({0}) + f({0, 3, 4})*/,
{0,-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0} /* Axiom74 Group1 f({1, 3}) + f({1, 4}) >= f({1}) + f({1, 3, 4})*/,
{0,0,-1,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0} /* Axiom75 Group2 f({0, 1, 3}) + f({0, 1, 4}) >= f({0, 1}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,-1} /* Axiom76 Group1 f({2, 3}) + f({2, 4}) >= f({2}) + f({2, 3, 4})*/,
{0,0,0,0,-1,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,1,0,0,0,0,0} /* Axiom77 Group2 f({0, 2, 3}) + f({0, 2, 4}) >= f({0, 2}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,-1,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,1,0,0,0,0} /* Axiom78 Group2 f({1, 2, 3}) + f({1, 2, 4}) >= f({1, 2}) + f({0, 1, 2, 3, 4})*/,
{0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0} /* Axiom79 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 2})*/
};
char* human_readable_axioms[AXIOMS] = {
"Axiom0 Group0 f({0}) + f({1}) >= f({0, 1})",
"Axiom1 Group1 f({0, 2}) + f({1, 2}) >= f({2}) + f({0, 1, 2})",
"Axiom2 Group1 f({0, 3}) + f({1, 3}) >= f({3}) + f({0, 1, 3})",
"Axiom3 Group2 f({0, 2, 3}) + f({1, 2, 3}) >= f({2, 3}) + f({0, 1, 2, 3, 4})",
"Axiom4 Group1 f({0, 4}) + f({1, 4}) >= f({4}) + f({0, 1, 4})",
"Axiom5 Group2 f({0, 2, 4}) + f({1, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})",
"Axiom6 Group2 f({0, 3, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})",
"Axiom7 Group3 f({0, 1, 2, 3, 4}) >= f({2, 3, 4})",
"Axiom8 Group0 f({0}) + f({2}) >= f({0, 2})",
"Axiom9 Group1 f({0, 1}) + f({1, 2}) >= f({1}) + f({0, 1, 2})",
"Axiom10 Group1 f({0, 3}) + f({2, 3}) >= f({3}) + f({0, 2, 3})",
"Axiom11 Group2 f({0, 1, 3}) + f({1, 2, 3}) >= f({1, 3}) + f({0, 1, 2, 3, 4})",
"Axiom12 Group1 f({0, 4}) + f({2, 4}) >= f({4}) + f({0, 2, 4})",
"Axiom13 Group2 f({0, 1, 4}) + f({1, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})",
"Axiom14 Group2 f({0, 3, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})",
"Axiom15 Group3 f({0, 1, 2, 3, 4}) >= f({1, 3, 4})",
"Axiom16 Group0 f({1}) + f({2}) >= f({1, 2})",
"Axiom17 Group1 f({0, 1}) + f({0, 2}) >= f({0}) + f({0, 1, 2})",
"Axiom18 Group1 f({1, 3}) + f({2, 3}) >= f({3}) + f({1, 2, 3})",
"Axiom19 Group2 f({0, 1, 3}) + f({0, 2, 3}) >= f({0, 3}) + f({0, 1, 2, 3, 4})",
"Axiom20 Group1 f({1, 4}) + f({2, 4}) >= f({4}) + f({1, 2, 4})",
"Axiom21 Group2 f({0, 1, 4}) + f({0, 2, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})",
"Axiom22 Group2 f({1, 3, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({3, 4})",
"Axiom23 Group3 f({0, 1, 2, 3, 4}) >= f({0, 3, 4})",
"Axiom24 Group0 f({0}) + f({3}) >= f({0, 3})",
"Axiom25 Group1 f({0, 1}) + f({1, 3}) >= f({1}) + f({0, 1, 3})",
"Axiom26 Group1 f({0, 2}) + f({2, 3}) >= f({2}) + f({0, 2, 3})",
"Axiom27 Group2 f({0, 1, 2}) + f({1, 2, 3}) >= f({1, 2}) + f({0, 1, 2, 3, 4})",
"Axiom28 Group1 f({0, 4}) + f({3, 4}) >= f({4}) + f({0, 3, 4})",
"Axiom29 Group2 f({0, 1, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})",
"Axiom30 Group2 f({0, 2, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})",
"Axiom31 Group3 f({0, 1, 2, 3, 4}) >= f({1, 2, 4})",
"Axiom32 Group0 f({1}) + f({3}) >= f({1, 3})",
"Axiom33 Group1 f({0, 1}) + f({0, 3}) >= f({0}) + f({0, 1, 3})",
"Axiom34 Group1 f({1, 2}) + f({2, 3}) >= f({2}) + f({1, 2, 3})",
"Axiom35 Group2 f({0, 1, 2}) + f({0, 2, 3}) >= f({0, 2}) + f({0, 1, 2, 3, 4})",
"Axiom36 Group1 f({1, 4}) + f({3, 4}) >= f({4}) + f({1, 3, 4})",
"Axiom37 Group2 f({0, 1, 4}) + f({0, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})",
"Axiom38 Group2 f({1, 2, 4}) + f({2, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({2, 4})",
"Axiom39 Group3 f({0, 1, 2, 3, 4}) >= f({0, 2, 4})",
"Axiom40 Group0 f({2}) + f({3}) >= f({2, 3})",
"Axiom41 Group1 f({0, 2}) + f({0, 3}) >= f({0}) + f({0, 2, 3})",
"Axiom42 Group1 f({1, 2}) + f({1, 3}) >= f({1}) + f({1, 2, 3})",
"Axiom43 Group2 f({0, 1, 2}) + f({0, 1, 3}) >= f({0, 1}) + f({0, 1, 2, 3, 4})",
"Axiom44 Group1 f({2, 4}) + f({3, 4}) >= f({4}) + f({2, 3, 4})",
"Axiom45 Group2 f({0, 2, 4}) + f({0, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({0, 4})",
"Axiom46 Group2 f({1, 2, 4}) + f({1, 3, 4}) >= f({0, 1, 2, 3, 4}) + f({1, 4})",
"Axiom47 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 4})",
"Axiom48 Group0 f({0}) + f({4}) >= f({0, 4})",
"Axiom49 Group1 f({0, 1}) + f({1, 4}) >= f({1}) + f({0, 1, 4})",
"Axiom50 Group1 f({0, 2}) + f({2, 4}) >= f({2}) + f({0, 2, 4})",
"Axiom51 Group2 f({0, 1, 2}) + f({1, 2, 4}) >= f({1, 2}) + f({0, 1, 2, 3, 4})",
"Axiom52 Group1 f({0, 3}) + f({3, 4}) >= f({3}) + f({0, 3, 4})",
"Axiom53 Group2 f({0, 1, 3}) + f({1, 3, 4}) >= f({1, 3}) + f({0, 1, 2, 3, 4})",
"Axiom54 Group2 f({0, 2, 3}) + f({2, 3, 4}) >= f({2, 3}) + f({0, 1, 2, 3, 4})",
"Axiom55 Group3 f({0, 1, 2, 3, 4}) >= f({1, 2, 3})",
"Axiom56 Group0 f({1}) + f({4}) >= f({1, 4})",
"Axiom57 Group1 f({0, 1}) + f({0, 4}) >= f({0}) + f({0, 1, 4})",
"Axiom58 Group1 f({1, 2}) + f({2, 4}) >= f({2}) + f({1, 2, 4})",
"Axiom59 Group2 f({0, 1, 2}) + f({0, 2, 4}) >= f({0, 2}) + f({0, 1, 2, 3, 4})",
"Axiom60 Group1 f({1, 3}) + f({3, 4}) >= f({3}) + f({1, 3, 4})",
"Axiom61 Group2 f({0, 1, 3}) + f({0, 3, 4}) >= f({0, 3}) + f({0, 1, 2, 3, 4})",
"Axiom62 Group2 f({1, 2, 3}) + f({2, 3, 4}) >= f({2, 3}) + f({0, 1, 2, 3, 4})",
"Axiom63 Group3 f({0, 1, 2, 3, 4}) >= f({0, 2, 3})",
"Axiom64 Group0 f({2}) + f({4}) >= f({2, 4})",
"Axiom65 Group1 f({0, 2}) + f({0, 4}) >= f({0}) + f({0, 2, 4})",
"Axiom66 Group1 f({1, 2}) + f({1, 4}) >= f({1}) + f({1, 2, 4})",
"Axiom67 Group2 f({0, 1, 2}) + f({0, 1, 4}) >= f({0, 1}) + f({0, 1, 2, 3, 4})",
"Axiom68 Group1 f({2, 3}) + f({3, 4}) >= f({3}) + f({2, 3, 4})",
"Axiom69 Group2 f({0, 2, 3}) + f({0, 3, 4}) >= f({0, 3}) + f({0, 1, 2, 3, 4})",
"Axiom70 Group2 f({1, 2, 3}) + f({1, 3, 4}) >= f({1, 3}) + f({0, 1, 2, 3, 4})",
"Axiom71 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 3})",
"Axiom72 Group0 f({3}) + f({4}) >= f({3, 4})",
"Axiom73 Group1 f({0, 3}) + f({0, 4}) >= f({0}) + f({0, 3, 4})",
"Axiom74 Group1 f({1, 3}) + f({1, 4}) >= f({1}) + f({1, 3, 4})",
"Axiom75 Group2 f({0, 1, 3}) + f({0, 1, 4}) >= f({0, 1}) + f({0, 1, 2, 3, 4})",
"Axiom76 Group1 f({2, 3}) + f({2, 4}) >= f({2}) + f({2, 3, 4})",
"Axiom77 Group2 f({0, 2, 3}) + f({0, 2, 4}) >= f({0, 2}) + f({0, 1, 2, 3, 4})",
"Axiom78 Group2 f({1, 2, 3}) + f({1, 2, 4}) >= f({1, 2}) + f({0, 1, 2, 3, 4})",
"Axiom79 Group3 f({0, 1, 2, 3, 4}) >= f({0, 1, 2})"
};
/* VARIABLES / SETS
 Variable0 is for set {0}
 Variable1 is for set {1}
 Variable2 is for set {0, 1}
 Variable3 is for set {2}
 Variable4 is for set {0, 2}
 Variable5 is for set {1, 2}
 Variable6 is for set {0, 1, 2}
 Variable7 is for set {3}
 Variable8 is for set {0, 3}
 Variable9 is for set {1, 3}
 Variable10 is for set {0, 1, 3}
 Variable11 is for set {2, 3}
 Variable12 is for set {0, 2, 3}
 Variable13 is for set {1, 2, 3}
 Variable14 is for set {0, 1, 2, 3, 4}
 Variable15 is for set {4}
 Variable16 is for set {0, 4}
 Variable17 is for set {1, 4}
 Variable18 is for set {0, 1, 4}
 Variable19 is for set {2, 4}
 Variable20 is for set {0, 2, 4}
 Variable21 is for set {1, 2, 4}
 Variable22 is for set {3, 4}
 Variable23 is for set {0, 3, 4}
 Variable24 is for set {1, 3, 4}
 Variable25 is for set {2, 3, 4}
*/
/* AXIOM GROUPS
 Group0 size=10 {0, 32, 64, 8, 40, 72, 48, 16, 24, 56}
 Group1 size=30 {1, 2, 4, 9, 10, 12, 17, 18, 20, 25, 26, 28, 33, 34, 36, 41, 42, 44, 49, 50, 52, 57, 58, 60, 65, 66, 68, 73, 74, 76}
 Group2 size=30 {3, 5, 6, 11, 13, 14, 19, 21, 22, 27, 29, 30, 35, 37, 38, 43, 45, 46, 51, 53, 54, 59, 61, 62, 67, 69, 70, 75, 77, 78}
 Group3 size=10 {7, 39, 71, 15, 47, 79, 23, 55, 63, 31}
*/
