#define LABEL "SET_N=5 TIGHT=True"
#define SET_N 5
#define AXIOMS 106
#define REAL_AXIOMS 80 // Without "identity" axioms
#define HAS_IDENTITY_AXIOMS 1
#define VARS 26
T_ELEM axioms[AXIOMS][VARS] = {
{1,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom0 10() - g0 f(0) + f(1) >= f(0,1)"*/,
{0,0,0,-1,1,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom1 10(2) - g1 f(0,2) + f(1,2) >= f(2) + f(0,1,2)"*/,
{0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom2 10(3) - g1 f(0,3) + f(1,3) >= f(3) + f(0,1,3)"*/,
{0,0,0,0,0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom3 10(2,3) - g2 f(0,2,3) + f(1,2,3) >= f(2,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,1,-1,0,0,0,0,0,0,0} /* "Axiom4 10(4) - g1 f(0,4) + f(1,4) >= f(4) + f(0,1,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,1,1,0,0,0,0} /* "Axiom5 10(2,4) - g2 f(0,2,4) + f(1,2,4) >= f(0,1,2,3,4) + f(2,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,-1,1,1,0} /* "Axiom6 10(3,4) - g2 f(0,3,4) + f(1,3,4) >= f(0,1,2,3,4) + f(3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,-1} /* "Axiom7 10(2,3,4) - g3 f(0,1,2,3,4) >= f(2,3,4)"*/,
{1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom8 20() - g0 f(0) + f(2) >= f(0,2)"*/,
{0,-1,1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom9 20(1) - g1 f(0,1) + f(1,2) >= f(1) + f(0,1,2)"*/,
{0,0,0,0,0,0,0,-1,1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom10 20(3) - g1 f(0,3) + f(2,3) >= f(3) + f(0,2,3)"*/,
{0,0,0,0,0,0,0,0,0,-1,1,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom11 20(1,3) - g2 f(0,1,3) + f(1,2,3) >= f(1,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,1,-1,0,0,0,0,0} /* "Axiom12 20(4) - g1 f(0,4) + f(2,4) >= f(4) + f(0,2,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,-1,1,0,0,1,0,0,0,0} /* "Axiom13 20(1,4) - g2 f(0,1,4) + f(1,2,4) >= f(0,1,2,3,4) + f(1,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,-1,1,0,1} /* "Axiom14 20(3,4) - g2 f(0,3,4) + f(2,3,4) >= f(0,1,2,3,4) + f(3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,-1,0} /* "Axiom15 20(1,3,4) - g3 f(0,1,2,3,4) >= f(1,3,4)"*/,
{0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom16 21() - g0 f(1) + f(2) >= f(1,2)"*/,
{-1,0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom17 21(0) - g1 f(0,1) + f(0,2) >= f(0) + f(0,1,2)"*/,
{0,0,0,0,0,0,0,-1,0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom18 21(3) - g1 f(1,3) + f(2,3) >= f(3) + f(1,2,3)"*/,
{0,0,0,0,0,0,0,0,-1,0,1,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom19 21(0,3) - g2 f(0,1,3) + f(0,2,3) >= f(0,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,0,1,0,-1,0,0,0,0} /* "Axiom20 21(4) - g1 f(1,4) + f(2,4) >= f(4) + f(1,2,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,-1,0,1,0,1,0,0,0,0,0} /* "Axiom21 21(0,4) - g2 f(0,1,4) + f(0,2,4) >= f(0,1,2,3,4) + f(0,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,0,-1,0,1,1} /* "Axiom22 21(3,4) - g2 f(1,3,4) + f(2,3,4) >= f(0,1,2,3,4) + f(3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,-1,0,0} /* "Axiom23 21(0,3,4) - g3 f(0,1,2,3,4) >= f(0,3,4)"*/,
{1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom24 30() - g0 f(0) + f(3) >= f(0,3)"*/,
{0,-1,1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom25 30(1) - g1 f(0,1) + f(1,3) >= f(1) + f(0,1,3)"*/,
{0,0,0,-1,1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom26 30(2) - g1 f(0,2) + f(2,3) >= f(2) + f(0,2,3)"*/,
{0,0,0,0,0,-1,1,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom27 30(1,2) - g2 f(0,1,2) + f(1,2,3) >= f(1,2) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,0,0,0,1,-1,0,0} /* "Axiom28 30(4) - g1 f(0,4) + f(3,4) >= f(4) + f(0,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,-1,1,0,0,0,0,0,1,0} /* "Axiom29 30(1,4) - g2 f(0,1,4) + f(1,3,4) >= f(0,1,2,3,4) + f(1,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,1,0,0,0,0,1} /* "Axiom30 30(2,4) - g2 f(0,2,4) + f(2,3,4) >= f(0,1,2,3,4) + f(2,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0,0} /* "Axiom31 30(1,2,4) - g3 f(0,1,2,3,4) >= f(1,2,4)"*/,
{0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom32 31() - g0 f(1) + f(3) >= f(1,3)"*/,
{-1,0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom33 31(0) - g1 f(0,1) + f(0,3) >= f(0) + f(0,1,3)"*/,
{0,0,0,-1,0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom34 31(2) - g1 f(1,2) + f(2,3) >= f(2) + f(1,2,3)"*/,
{0,0,0,0,-1,0,1,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom35 31(0,2) - g2 f(0,1,2) + f(0,2,3) >= f(0,2) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,0,0,0,0,1,0,-1,0} /* "Axiom36 31(4) - g1 f(1,4) + f(3,4) >= f(4) + f(1,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,-1,0,1,0,0,0,0,1,0,0} /* "Axiom37 31(0,4) - g2 f(0,1,4) + f(0,3,4) >= f(0,1,2,3,4) + f(0,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,0,-1,0,1,0,0,0,1} /* "Axiom38 31(2,4) - g2 f(1,2,4) + f(2,3,4) >= f(0,1,2,3,4) + f(2,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,-1,0,0,0,0,0} /* "Axiom39 31(0,2,4) - g3 f(0,1,2,3,4) >= f(0,2,4)"*/,
{0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom40 32() - g0 f(2) + f(3) >= f(2,3)"*/,
{-1,0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom41 32(0) - g1 f(0,2) + f(0,3) >= f(0) + f(0,2,3)"*/,
{0,-1,0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom42 32(1) - g1 f(1,2) + f(1,3) >= f(1) + f(1,2,3)"*/,
{0,0,-1,0,0,0,1,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom43 32(0,1) - g2 f(0,1,2) + f(0,1,3) >= f(0,1) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,0,1,0,0,1,0,0,-1} /* "Axiom44 32(4) - g1 f(2,4) + f(3,4) >= f(4) + f(2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,-1,0,0,0,1,0,0,1,0,0} /* "Axiom45 32(0,4) - g2 f(0,2,4) + f(0,3,4) >= f(0,1,2,3,4) + f(0,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,-1,0,0,-1,0,0,0,1,0,0,1,0} /* "Axiom46 32(1,4) - g2 f(1,2,4) + f(1,3,4) >= f(0,1,2,3,4) + f(1,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0,0} /* "Axiom47 32(0,1,4) - g3 f(0,1,2,3,4) >= f(0,1,4)"*/,
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0,0,0} /* "Axiom48 40() - g0 f(0) + f(4) >= f(0,4)"*/,
{0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,0} /* "Axiom49 40(1) - g1 f(0,1) + f(1,4) >= f(1) + f(0,1,4)"*/,
{0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0,0,0,0} /* "Axiom50 40(2) - g1 f(0,2) + f(2,4) >= f(2) + f(0,2,4)"*/,
{0,0,0,0,0,-1,1,0,0,0,0,0,0,0,-1,0,0,0,0,0,0,1,0,0,0,0} /* "Axiom51 40(1,2) - g2 f(0,1,2) + f(1,2,4) >= f(1,2) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,-1,0,0} /* "Axiom52 40(3) - g1 f(0,3) + f(3,4) >= f(3) + f(0,3,4)"*/,
{0,0,0,0,0,0,0,0,0,-1,1,0,0,0,-1,0,0,0,0,0,0,0,0,0,1,0} /* "Axiom53 40(1,3) - g2 f(0,1,3) + f(1,3,4) >= f(1,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,-1,1,0,-1,0,0,0,0,0,0,0,0,0,0,1} /* "Axiom54 40(2,3) - g2 f(0,2,3) + f(2,3,4) >= f(2,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,0,-1,1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom55 40(1,2,3) - g3 f(0,1,2,3,4) >= f(1,2,3)"*/,
{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0,0} /* "Axiom56 41() - g0 f(1) + f(4) >= f(1,4)"*/,
{-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,0,0} /* "Axiom57 41(0) - g1 f(0,1) + f(0,4) >= f(0) + f(0,1,4)"*/,
{0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0,0,0,0} /* "Axiom58 41(2) - g1 f(1,2) + f(2,4) >= f(2) + f(1,2,4)"*/,
{0,0,0,0,-1,0,1,0,0,0,0,0,0,0,-1,0,0,0,0,0,1,0,0,0,0,0} /* "Axiom59 41(0,2) - g2 f(0,1,2) + f(0,2,4) >= f(0,2) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0,0,1,0,-1,0} /* "Axiom60 41(3) - g1 f(1,3) + f(3,4) >= f(3) + f(1,3,4)"*/,
{0,0,0,0,0,0,0,0,-1,0,1,0,0,0,-1,0,0,0,0,0,0,0,0,1,0,0} /* "Axiom61 41(0,3) - g2 f(0,1,3) + f(0,3,4) >= f(0,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,-1,0,1,-1,0,0,0,0,0,0,0,0,0,0,1} /* "Axiom62 41(2,3) - g2 f(1,2,3) + f(2,3,4) >= f(2,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,0,0,-1,0,1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom63 41(0,2,3) - g3 f(0,1,2,3,4) >= f(0,2,3)"*/,
{0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0,0} /* "Axiom64 42() - g0 f(2) + f(4) >= f(2,4)"*/,
{-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0,0} /* "Axiom65 42(0) - g1 f(0,2) + f(0,4) >= f(0) + f(0,2,4)"*/,
{0,-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,0} /* "Axiom66 42(1) - g1 f(1,2) + f(1,4) >= f(1) + f(1,2,4)"*/,
{0,0,-1,0,0,0,1,0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0} /* "Axiom67 42(0,1) - g2 f(0,1,2) + f(0,1,4) >= f(0,1) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,1,0,0,-1} /* "Axiom68 42(3) - g1 f(2,3) + f(3,4) >= f(3) + f(2,3,4)"*/,
{0,0,0,0,0,0,0,0,-1,0,0,0,1,0,-1,0,0,0,0,0,0,0,0,1,0,0} /* "Axiom69 42(0,3) - g2 f(0,2,3) + f(0,3,4) >= f(0,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,-1,0,0,0,1,-1,0,0,0,0,0,0,0,0,0,1,0} /* "Axiom70 42(1,3) - g2 f(1,2,3) + f(1,3,4) >= f(1,3) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,0,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom71 42(0,1,3) - g3 f(0,1,2,3,4) >= f(0,1,3)"*/,
{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0,0} /* "Axiom72 43() - g0 f(3) + f(4) >= f(3,4)"*/,
{-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0,0} /* "Axiom73 43(0) - g1 f(0,3) + f(0,4) >= f(0) + f(0,3,4)"*/,
{0,-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,-1,0} /* "Axiom74 43(1) - g1 f(1,3) + f(1,4) >= f(1) + f(1,3,4)"*/,
{0,0,-1,0,0,0,0,0,0,0,1,0,0,0,-1,0,0,0,1,0,0,0,0,0,0,0} /* "Axiom75 43(0,1) - g2 f(0,1,3) + f(0,1,4) >= f(0,1) + f(0,1,2,3,4)"*/,
{0,0,0,-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,1,0,0,0,0,0,-1} /* "Axiom76 43(2) - g1 f(2,3) + f(2,4) >= f(2) + f(2,3,4)"*/,
{0,0,0,0,-1,0,0,0,0,0,0,0,1,0,-1,0,0,0,0,0,1,0,0,0,0,0} /* "Axiom77 43(0,2) - g2 f(0,2,3) + f(0,2,4) >= f(0,2) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,-1,0,0,0,0,0,0,0,1,-1,0,0,0,0,0,0,1,0,0,0,0} /* "Axiom78 43(1,2) - g2 f(1,2,3) + f(1,2,4) >= f(1,2) + f(0,1,2,3,4)"*/,
{0,0,0,0,0,0,-1,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0} /* "Axiom79 43(0,1,2) - g3 f(0,1,2,3,4) >= f(0,1,2)"*/,
{1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1,0} /* identity */,
{0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,1} /* identity */
};
char* human_readable_axioms[AXIOMS] = {
"Axiom0 10() - g0 f(0) + f(1) >= f(0,1)",
"Axiom1 10(2) - g1 f(0,2) + f(1,2) >= f(2) + f(0,1,2)",
"Axiom2 10(3) - g1 f(0,3) + f(1,3) >= f(3) + f(0,1,3)",
"Axiom3 10(2,3) - g2 f(0,2,3) + f(1,2,3) >= f(2,3) + f(0,1,2,3,4)",
"Axiom4 10(4) - g1 f(0,4) + f(1,4) >= f(4) + f(0,1,4)",
"Axiom5 10(2,4) - g2 f(0,2,4) + f(1,2,4) >= f(0,1,2,3,4) + f(2,4)",
"Axiom6 10(3,4) - g2 f(0,3,4) + f(1,3,4) >= f(0,1,2,3,4) + f(3,4)",
"Axiom7 10(2,3,4) - g3 f(0,1,2,3,4) >= f(2,3,4)",
"Axiom8 20() - g0 f(0) + f(2) >= f(0,2)",
"Axiom9 20(1) - g1 f(0,1) + f(1,2) >= f(1) + f(0,1,2)",
"Axiom10 20(3) - g1 f(0,3) + f(2,3) >= f(3) + f(0,2,3)",
"Axiom11 20(1,3) - g2 f(0,1,3) + f(1,2,3) >= f(1,3) + f(0,1,2,3,4)",
"Axiom12 20(4) - g1 f(0,4) + f(2,4) >= f(4) + f(0,2,4)",
"Axiom13 20(1,4) - g2 f(0,1,4) + f(1,2,4) >= f(0,1,2,3,4) + f(1,4)",
"Axiom14 20(3,4) - g2 f(0,3,4) + f(2,3,4) >= f(0,1,2,3,4) + f(3,4)",
"Axiom15 20(1,3,4) - g3 f(0,1,2,3,4) >= f(1,3,4)",
"Axiom16 21() - g0 f(1) + f(2) >= f(1,2)",
"Axiom17 21(0) - g1 f(0,1) + f(0,2) >= f(0) + f(0,1,2)",
"Axiom18 21(3) - g1 f(1,3) + f(2,3) >= f(3) + f(1,2,3)",
"Axiom19 21(0,3) - g2 f(0,1,3) + f(0,2,3) >= f(0,3) + f(0,1,2,3,4)",
"Axiom20 21(4) - g1 f(1,4) + f(2,4) >= f(4) + f(1,2,4)",
"Axiom21 21(0,4) - g2 f(0,1,4) + f(0,2,4) >= f(0,1,2,3,4) + f(0,4)",
"Axiom22 21(3,4) - g2 f(1,3,4) + f(2,3,4) >= f(0,1,2,3,4) + f(3,4)",
"Axiom23 21(0,3,4) - g3 f(0,1,2,3,4) >= f(0,3,4)",
"Axiom24 30() - g0 f(0) + f(3) >= f(0,3)",
"Axiom25 30(1) - g1 f(0,1) + f(1,3) >= f(1) + f(0,1,3)",
"Axiom26 30(2) - g1 f(0,2) + f(2,3) >= f(2) + f(0,2,3)",
"Axiom27 30(1,2) - g2 f(0,1,2) + f(1,2,3) >= f(1,2) + f(0,1,2,3,4)",
"Axiom28 30(4) - g1 f(0,4) + f(3,4) >= f(4) + f(0,3,4)",
"Axiom29 30(1,4) - g2 f(0,1,4) + f(1,3,4) >= f(0,1,2,3,4) + f(1,4)",
"Axiom30 30(2,4) - g2 f(0,2,4) + f(2,3,4) >= f(0,1,2,3,4) + f(2,4)",
"Axiom31 30(1,2,4) - g3 f(0,1,2,3,4) >= f(1,2,4)",
"Axiom32 31() - g0 f(1) + f(3) >= f(1,3)",
"Axiom33 31(0) - g1 f(0,1) + f(0,3) >= f(0) + f(0,1,3)",
"Axiom34 31(2) - g1 f(1,2) + f(2,3) >= f(2) + f(1,2,3)",
"Axiom35 31(0,2) - g2 f(0,1,2) + f(0,2,3) >= f(0,2) + f(0,1,2,3,4)",
"Axiom36 31(4) - g1 f(1,4) + f(3,4) >= f(4) + f(1,3,4)",
"Axiom37 31(0,4) - g2 f(0,1,4) + f(0,3,4) >= f(0,1,2,3,4) + f(0,4)",
"Axiom38 31(2,4) - g2 f(1,2,4) + f(2,3,4) >= f(0,1,2,3,4) + f(2,4)",
"Axiom39 31(0,2,4) - g3 f(0,1,2,3,4) >= f(0,2,4)",
"Axiom40 32() - g0 f(2) + f(3) >= f(2,3)",
"Axiom41 32(0) - g1 f(0,2) + f(0,3) >= f(0) + f(0,2,3)",
"Axiom42 32(1) - g1 f(1,2) + f(1,3) >= f(1) + f(1,2,3)",
"Axiom43 32(0,1) - g2 f(0,1,2) + f(0,1,3) >= f(0,1) + f(0,1,2,3,4)",
"Axiom44 32(4) - g1 f(2,4) + f(3,4) >= f(4) + f(2,3,4)",
"Axiom45 32(0,4) - g2 f(0,2,4) + f(0,3,4) >= f(0,1,2,3,4) + f(0,4)",
"Axiom46 32(1,4) - g2 f(1,2,4) + f(1,3,4) >= f(0,1,2,3,4) + f(1,4)",
"Axiom47 32(0,1,4) - g3 f(0,1,2,3,4) >= f(0,1,4)",
"Axiom48 40() - g0 f(0) + f(4) >= f(0,4)",
"Axiom49 40(1) - g1 f(0,1) + f(1,4) >= f(1) + f(0,1,4)",
"Axiom50 40(2) - g1 f(0,2) + f(2,4) >= f(2) + f(0,2,4)",
"Axiom51 40(1,2) - g2 f(0,1,2) + f(1,2,4) >= f(1,2) + f(0,1,2,3,4)",
"Axiom52 40(3) - g1 f(0,3) + f(3,4) >= f(3) + f(0,3,4)",
"Axiom53 40(1,3) - g2 f(0,1,3) + f(1,3,4) >= f(1,3) + f(0,1,2,3,4)",
"Axiom54 40(2,3) - g2 f(0,2,3) + f(2,3,4) >= f(2,3) + f(0,1,2,3,4)",
"Axiom55 40(1,2,3) - g3 f(0,1,2,3,4) >= f(1,2,3)",
"Axiom56 41() - g0 f(1) + f(4) >= f(1,4)",
"Axiom57 41(0) - g1 f(0,1) + f(0,4) >= f(0) + f(0,1,4)",
"Axiom58 41(2) - g1 f(1,2) + f(2,4) >= f(2) + f(1,2,4)",
"Axiom59 41(0,2) - g2 f(0,1,2) + f(0,2,4) >= f(0,2) + f(0,1,2,3,4)",
"Axiom60 41(3) - g1 f(1,3) + f(3,4) >= f(3) + f(1,3,4)",
"Axiom61 41(0,3) - g2 f(0,1,3) + f(0,3,4) >= f(0,3) + f(0,1,2,3,4)",
"Axiom62 41(2,3) - g2 f(1,2,3) + f(2,3,4) >= f(2,3) + f(0,1,2,3,4)",
"Axiom63 41(0,2,3) - g3 f(0,1,2,3,4) >= f(0,2,3)",
"Axiom64 42() - g0 f(2) + f(4) >= f(2,4)",
"Axiom65 42(0) - g1 f(0,2) + f(0,4) >= f(0) + f(0,2,4)",
"Axiom66 42(1) - g1 f(1,2) + f(1,4) >= f(1) + f(1,2,4)",
"Axiom67 42(0,1) - g2 f(0,1,2) + f(0,1,4) >= f(0,1) + f(0,1,2,3,4)",
"Axiom68 42(3) - g1 f(2,3) + f(3,4) >= f(3) + f(2,3,4)",
"Axiom69 42(0,3) - g2 f(0,2,3) + f(0,3,4) >= f(0,3) + f(0,1,2,3,4)",
"Axiom70 42(1,3) - g2 f(1,2,3) + f(1,3,4) >= f(1,3) + f(0,1,2,3,4)",
"Axiom71 42(0,1,3) - g3 f(0,1,2,3,4) >= f(0,1,3)",
"Axiom72 43() - g0 f(3) + f(4) >= f(3,4)",
"Axiom73 43(0) - g1 f(0,3) + f(0,4) >= f(0) + f(0,3,4)",
"Axiom74 43(1) - g1 f(1,3) + f(1,4) >= f(1) + f(1,3,4)",
"Axiom75 43(0,1) - g2 f(0,1,3) + f(0,1,4) >= f(0,1) + f(0,1,2,3,4)",
"Axiom76 43(2) - g1 f(2,3) + f(2,4) >= f(2) + f(2,3,4)",
"Axiom77 43(0,2) - g2 f(0,2,3) + f(0,2,4) >= f(0,2) + f(0,1,2,3,4)",
"Axiom78 43(1,2) - g2 f(1,2,3) + f(1,2,4) >= f(1,2) + f(0,1,2,3,4)",
"Axiom79 43(0,1,2) - g3 f(0,1,2,3,4) >= f(0,1,2)",
"Axiom80 GroupID f(0) >= ",
"Axiom81 GroupID f(1) >= ",
"Axiom82 GroupID f(0,1) >= ",
"Axiom83 GroupID f(2) >= ",
"Axiom84 GroupID f(0,2) >= ",
"Axiom85 GroupID f(1,2) >= ",
"Axiom86 GroupID f(0,1,2) >= ",
"Axiom87 GroupID f(3) >= ",
"Axiom88 GroupID f(0,3) >= ",
"Axiom89 GroupID f(1,3) >= ",
"Axiom90 GroupID f(0,1,3) >= ",
"Axiom91 GroupID f(2,3) >= ",
"Axiom92 GroupID f(0,2,3) >= ",
"Axiom93 GroupID f(1,2,3) >= ",
"Axiom94 GroupID f(0,1,2,3,4) >= ",
"Axiom95 GroupID f(4) >= ",
"Axiom96 GroupID f(0,4) >= ",
"Axiom97 GroupID f(1,4) >= ",
"Axiom98 GroupID f(0,1,4) >= ",
"Axiom99 GroupID f(2,4) >= ",
"Axiom100 GroupID f(0,2,4) >= ",
"Axiom101 GroupID f(1,2,4) >= ",
"Axiom102 GroupID f(3,4) >= ",
"Axiom103 GroupID f(0,3,4) >= ",
"Axiom104 GroupID f(1,3,4) >= ",
"Axiom105 GroupID f(2,3,4) >= "
};
/* VARIABLES / SETS
 Variable0 is for set 0
 Variable1 is for set 1
 Variable2 is for set 0,1
 Variable3 is for set 2
 Variable4 is for set 0,2
 Variable5 is for set 1,2
 Variable6 is for set 0,1,2
 Variable7 is for set 3
 Variable8 is for set 0,3
 Variable9 is for set 1,3
 Variable10 is for set 0,1,3
 Variable11 is for set 2,3
 Variable12 is for set 0,2,3
 Variable13 is for set 1,2,3
 Variable14 is for set 0,1,2,3,4
 Variable15 is for set 4
 Variable16 is for set 0,4
 Variable17 is for set 1,4
 Variable18 is for set 0,1,4
 Variable19 is for set 2,4
 Variable20 is for set 0,2,4
 Variable21 is for set 1,2,4
 Variable22 is for set 3,4
 Variable23 is for set 0,3,4
 Variable24 is for set 1,3,4
 Variable25 is for set 2,3,4
*/
/* AXIOM GROUPS
 Group0 size=10 {0, 32, 64, 8, 40, 72, 48, 16, 24, 56}
 Group1 size=30 {1, 2, 4, 9, 10, 12, 17, 18, 20, 25, 26, 28, 33, 34, 36, 41, 42, 44, 49, 50, 52, 57, 58, 60, 65, 66, 68, 73, 74, 76}
 Group2 size=30 {3, 5, 6, 11, 13, 14, 19, 21, 22, 27, 29, 30, 35, 37, 38, 43, 45, 46, 51, 53, 54, 59, 61, 62, 67, 69, 70, 75, 77, 78}
 Group3 size=10 {7, 39, 71, 15, 47, 79, 23, 55, 63, 31}
*/
