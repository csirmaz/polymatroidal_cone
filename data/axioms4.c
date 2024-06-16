#define LABEL "SET_N=4 TIGHT=True"
#define SET_N 4
#define AXIOMS 24
#define REAL_AXIOMS 24 // Without "identity" axioms
#define HAS_IDENTITY_AXIOMS 0
#define VARS 11
T_ELEM axioms[AXIOMS][VARS] = {
{1,1,-1,0,0,0,0,0,0,0,0} /* "Axiom0 01() - f(0) + f(1) >= f(0,1)"*/,
{0,0,0,-1,1,1,-1,0,0,0,0} /* "Axiom1 01(2) - f(0,2) + f(1,2) >= f(2) + f(0,1,2,3)"*/,
{0,0,0,0,0,0,-1,-1,1,1,0} /* "Axiom2 01(3) - f(0,3) + f(1,3) >= f(0,1,2,3) + f(3)"*/,
{0,0,0,0,0,0,1,0,0,0,-1} /* "Axiom3 01(2,3) - f(0,1,2,3) >= f(2,3)"*/,
{1,0,0,1,-1,0,0,0,0,0,0} /* "Axiom4 02() - f(0) + f(2) >= f(0,2)"*/,
{0,-1,1,0,0,1,-1,0,0,0,0} /* "Axiom5 02(1) - f(0,1) + f(1,2) >= f(1) + f(0,1,2,3)"*/,
{0,0,0,0,0,0,-1,-1,1,0,1} /* "Axiom6 02(3) - f(0,3) + f(2,3) >= f(0,1,2,3) + f(3)"*/,
{0,0,0,0,0,0,1,0,0,-1,0} /* "Axiom7 02(1,3) - f(0,1,2,3) >= f(1,3)"*/,
{0,1,0,1,0,-1,0,0,0,0,0} /* "Axiom8 12() - f(1) + f(2) >= f(1,2)"*/,
{-1,0,1,0,1,0,-1,0,0,0,0} /* "Axiom9 12(0) - f(0,1) + f(0,2) >= f(0) + f(0,1,2,3)"*/,
{0,0,0,0,0,0,-1,-1,0,1,1} /* "Axiom10 12(3) - f(1,3) + f(2,3) >= f(0,1,2,3) + f(3)"*/,
{0,0,0,0,0,0,1,0,-1,0,0} /* "Axiom11 12(0,3) - f(0,1,2,3) >= f(0,3)"*/,
{1,0,0,0,0,0,0,1,-1,0,0} /* "Axiom12 03() - f(0) + f(3) >= f(0,3)"*/,
{0,-1,1,0,0,0,-1,0,0,1,0} /* "Axiom13 03(1) - f(0,1) + f(1,3) >= f(1) + f(0,1,2,3)"*/,
{0,0,0,-1,1,0,-1,0,0,0,1} /* "Axiom14 03(2) - f(0,2) + f(2,3) >= f(2) + f(0,1,2,3)"*/,
{0,0,0,0,0,-1,1,0,0,0,0} /* "Axiom15 03(1,2) - f(0,1,2,3) >= f(1,2)"*/,
{0,1,0,0,0,0,0,1,0,-1,0} /* "Axiom16 13() - f(1) + f(3) >= f(1,3)"*/,
{-1,0,1,0,0,0,-1,0,1,0,0} /* "Axiom17 13(0) - f(0,1) + f(0,3) >= f(0) + f(0,1,2,3)"*/,
{0,0,0,-1,0,1,-1,0,0,0,1} /* "Axiom18 13(2) - f(1,2) + f(2,3) >= f(2) + f(0,1,2,3)"*/,
{0,0,0,0,-1,0,1,0,0,0,0} /* "Axiom19 13(0,2) - f(0,1,2,3) >= f(0,2)"*/,
{0,0,0,1,0,0,0,1,0,0,-1} /* "Axiom20 23() - f(2) + f(3) >= f(2,3)"*/,
{-1,0,0,0,1,0,-1,0,1,0,0} /* "Axiom21 23(0) - f(0,2) + f(0,3) >= f(0) + f(0,1,2,3)"*/,
{0,-1,0,0,0,1,-1,0,0,1,0} /* "Axiom22 23(1) - f(1,2) + f(1,3) >= f(1) + f(0,1,2,3)"*/,
{0,0,-1,0,0,0,1,0,0,0,0} /* "Axiom23 23(0,1) - f(0,1,2,3) >= f(0,1)"*/
};
char* human_readable_axioms[AXIOMS] = {
"Axiom0 01() - f(0) + f(1) >= f(0,1)",
"Axiom1 01(2) - f(0,2) + f(1,2) >= f(2) + f(0,1,2,3)",
"Axiom2 01(3) - f(0,3) + f(1,3) >= f(0,1,2,3) + f(3)",
"Axiom3 01(2,3) - f(0,1,2,3) >= f(2,3)",
"Axiom4 02() - f(0) + f(2) >= f(0,2)",
"Axiom5 02(1) - f(0,1) + f(1,2) >= f(1) + f(0,1,2,3)",
"Axiom6 02(3) - f(0,3) + f(2,3) >= f(0,1,2,3) + f(3)",
"Axiom7 02(1,3) - f(0,1,2,3) >= f(1,3)",
"Axiom8 12() - f(1) + f(2) >= f(1,2)",
"Axiom9 12(0) - f(0,1) + f(0,2) >= f(0) + f(0,1,2,3)",
"Axiom10 12(3) - f(1,3) + f(2,3) >= f(0,1,2,3) + f(3)",
"Axiom11 12(0,3) - f(0,1,2,3) >= f(0,3)",
"Axiom12 03() - f(0) + f(3) >= f(0,3)",
"Axiom13 03(1) - f(0,1) + f(1,3) >= f(1) + f(0,1,2,3)",
"Axiom14 03(2) - f(0,2) + f(2,3) >= f(2) + f(0,1,2,3)",
"Axiom15 03(1,2) - f(0,1,2,3) >= f(1,2)",
"Axiom16 13() - f(1) + f(3) >= f(1,3)",
"Axiom17 13(0) - f(0,1) + f(0,3) >= f(0) + f(0,1,2,3)",
"Axiom18 13(2) - f(1,2) + f(2,3) >= f(2) + f(0,1,2,3)",
"Axiom19 13(0,2) - f(0,1,2,3) >= f(0,2)",
"Axiom20 23() - f(2) + f(3) >= f(2,3)",
"Axiom21 23(0) - f(0,2) + f(0,3) >= f(0) + f(0,1,2,3)",
"Axiom22 23(1) - f(1,2) + f(1,3) >= f(1) + f(0,1,2,3)",
"Axiom23 23(0,1) - f(0,1,2,3) >= f(0,1)"
};
/* VARIABLES / SETS
 Variable0 is for set 0
 Variable1 is for set 1
 Variable2 is for set 0,1
 Variable3 is for set 2
 Variable4 is for set 0,2
 Variable5 is for set 1,2
 Variable6 is for set 0,1,2,3
 Variable7 is for set 3
 Variable8 is for set 0,3
 Variable9 is for set 1,3
 Variable10 is for set 2,3
*/
/* AXIOMS DATA
axiom_data;len;24
axiom_data;ix;0;label;01();elem1;0;elem2;1;subset;
axiom_data;ix;1;label;01(2);elem1;0;elem2;1;subset;2
axiom_data;ix;2;label;01(3);elem1;0;elem2;1;subset;3
axiom_data;ix;3;label;01(2,3);elem1;0;elem2;1;subset;23
axiom_data;ix;4;label;02();elem1;0;elem2;2;subset;
axiom_data;ix;5;label;02(1);elem1;0;elem2;2;subset;1
axiom_data;ix;6;label;02(3);elem1;0;elem2;2;subset;3
axiom_data;ix;7;label;02(1,3);elem1;0;elem2;2;subset;13
axiom_data;ix;8;label;12();elem1;1;elem2;2;subset;
axiom_data;ix;9;label;12(0);elem1;1;elem2;2;subset;0
axiom_data;ix;10;label;12(3);elem1;1;elem2;2;subset;3
axiom_data;ix;11;label;12(0,3);elem1;1;elem2;2;subset;03
axiom_data;ix;12;label;03();elem1;0;elem2;3;subset;
axiom_data;ix;13;label;03(1);elem1;0;elem2;3;subset;1
axiom_data;ix;14;label;03(2);elem1;0;elem2;3;subset;2
axiom_data;ix;15;label;03(1,2);elem1;0;elem2;3;subset;12
axiom_data;ix;16;label;13();elem1;1;elem2;3;subset;
axiom_data;ix;17;label;13(0);elem1;1;elem2;3;subset;0
axiom_data;ix;18;label;13(2);elem1;1;elem2;3;subset;2
axiom_data;ix;19;label;13(0,2);elem1;1;elem2;3;subset;02
axiom_data;ix;20;label;23();elem1;2;elem2;3;subset;
axiom_data;ix;21;label;23(0);elem1;2;elem2;3;subset;0
axiom_data;ix;22;label;23(1);elem1;2;elem2;3;subset;1
axiom_data;ix;23;label;23(0,1);elem1;2;elem2;3;subset;01
*/
