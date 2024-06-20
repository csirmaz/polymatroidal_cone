
# From the output of "make slicer_run_*" get the number of rays and pairs for the processed axioms.
# Usage: python get_pair_data.py N SET_N where 
# - N is -1 to show all data or only print data for step N (all Nth steps)
# - SET_N is 4|5|6

import sys
import re
import json

OUT_STEP = int(sys.argv[1])
SET_N = int(sys.argv[2])

assert SET_N in [4,5,6]

STATE = "filebegin"
INITIAL_AXIOMS = []
AXIOM_SET = []
EXPERIENCES = []
SLICER_RUNS = 0

N_5_AXIOMS = [
"Axiom0 01() - g0 f(0) + f(1) >= f(0,1)",
"Axiom1 01(2) - g1 f(0,2) + f(1,2) >= f(2) + f(0,1,2)",
"Axiom2 01(3) - g1 f(0,3) + f(1,3) >= f(3) + f(0,1,3)",
"Axiom3 01(2,3) - g2 f(0,2,3) + f(1,2,3) >= f(2,3) + f(0,1,2,3,4)",
"Axiom4 01(4) - g1 f(0,4) + f(1,4) >= f(4) + f(0,1,4)",
"Axiom5 01(2,4) - g2 f(0,2,4) + f(1,2,4) >= f(0,1,2,3,4) + f(2,4)",
"Axiom6 01(3,4) - g2 f(0,3,4) + f(1,3,4) >= f(0,1,2,3,4) + f(3,4)",
"Axiom7 01(2,3,4) - g3 f(0,1,2,3,4) >= f(2,3,4)",
"Axiom8 02() - g0 f(0) + f(2) >= f(0,2)",
"Axiom9 02(1) - g1 f(0,1) + f(1,2) >= f(1) + f(0,1,2)",
"Axiom10 02(3) - g1 f(0,3) + f(2,3) >= f(3) + f(0,2,3)",
"Axiom11 02(1,3) - g2 f(0,1,3) + f(1,2,3) >= f(1,3) + f(0,1,2,3,4)",
"Axiom12 02(4) - g1 f(0,4) + f(2,4) >= f(4) + f(0,2,4)",
"Axiom13 02(1,4) - g2 f(0,1,4) + f(1,2,4) >= f(0,1,2,3,4) + f(1,4)",
"Axiom14 02(3,4) - g2 f(0,3,4) + f(2,3,4) >= f(0,1,2,3,4) + f(3,4)",
"Axiom15 02(1,3,4) - g3 f(0,1,2,3,4) >= f(1,3,4)",
"Axiom16 12() - g0 f(1) + f(2) >= f(1,2)",
"Axiom17 12(0) - g1 f(0,1) + f(0,2) >= f(0) + f(0,1,2)",
"Axiom18 12(3) - g1 f(1,3) + f(2,3) >= f(3) + f(1,2,3)",
"Axiom19 12(0,3) - g2 f(0,1,3) + f(0,2,3) >= f(0,3) + f(0,1,2,3,4)",
"Axiom20 12(4) - g1 f(1,4) + f(2,4) >= f(4) + f(1,2,4)",
"Axiom21 12(0,4) - g2 f(0,1,4) + f(0,2,4) >= f(0,1,2,3,4) + f(0,4)",
"Axiom22 12(3,4) - g2 f(1,3,4) + f(2,3,4) >= f(0,1,2,3,4) + f(3,4)",
"Axiom23 12(0,3,4) - g3 f(0,1,2,3,4) >= f(0,3,4)",
"Axiom24 03() - g0 f(0) + f(3) >= f(0,3)",
"Axiom25 03(1) - g1 f(0,1) + f(1,3) >= f(1) + f(0,1,3)",
"Axiom26 03(2) - g1 f(0,2) + f(2,3) >= f(2) + f(0,2,3)",
"Axiom27 03(1,2) - g2 f(0,1,2) + f(1,2,3) >= f(1,2) + f(0,1,2,3,4)",
"Axiom28 03(4) - g1 f(0,4) + f(3,4) >= f(4) + f(0,3,4)",
"Axiom29 03(1,4) - g2 f(0,1,4) + f(1,3,4) >= f(0,1,2,3,4) + f(1,4)",
"Axiom30 03(2,4) - g2 f(0,2,4) + f(2,3,4) >= f(0,1,2,3,4) + f(2,4)",
"Axiom31 03(1,2,4) - g3 f(0,1,2,3,4) >= f(1,2,4)",
"Axiom32 13() - g0 f(1) + f(3) >= f(1,3)",
"Axiom33 13(0) - g1 f(0,1) + f(0,3) >= f(0) + f(0,1,3)",
"Axiom34 13(2) - g1 f(1,2) + f(2,3) >= f(2) + f(1,2,3)",
"Axiom35 13(0,2) - g2 f(0,1,2) + f(0,2,3) >= f(0,2) + f(0,1,2,3,4)",
"Axiom36 13(4) - g1 f(1,4) + f(3,4) >= f(4) + f(1,3,4)",
"Axiom37 13(0,4) - g2 f(0,1,4) + f(0,3,4) >= f(0,1,2,3,4) + f(0,4)",
"Axiom38 13(2,4) - g2 f(1,2,4) + f(2,3,4) >= f(0,1,2,3,4) + f(2,4)",
"Axiom39 13(0,2,4) - g3 f(0,1,2,3,4) >= f(0,2,4)",
"Axiom40 23() - g0 f(2) + f(3) >= f(2,3)",
"Axiom41 23(0) - g1 f(0,2) + f(0,3) >= f(0) + f(0,2,3)",
"Axiom42 23(1) - g1 f(1,2) + f(1,3) >= f(1) + f(1,2,3)",
"Axiom43 23(0,1) - g2 f(0,1,2) + f(0,1,3) >= f(0,1) + f(0,1,2,3,4)",
"Axiom44 23(4) - g1 f(2,4) + f(3,4) >= f(4) + f(2,3,4)",
"Axiom45 23(0,4) - g2 f(0,2,4) + f(0,3,4) >= f(0,1,2,3,4) + f(0,4)",
"Axiom46 23(1,4) - g2 f(1,2,4) + f(1,3,4) >= f(0,1,2,3,4) + f(1,4)",
"Axiom47 23(0,1,4) - g3 f(0,1,2,3,4) >= f(0,1,4)",
"Axiom48 04() - g0 f(0) + f(4) >= f(0,4)",
"Axiom49 04(1) - g1 f(0,1) + f(1,4) >= f(1) + f(0,1,4)",
"Axiom50 04(2) - g1 f(0,2) + f(2,4) >= f(2) + f(0,2,4)",
"Axiom51 04(1,2) - g2 f(0,1,2) + f(1,2,4) >= f(1,2) + f(0,1,2,3,4)",
"Axiom52 04(3) - g1 f(0,3) + f(3,4) >= f(3) + f(0,3,4)",
"Axiom53 04(1,3) - g2 f(0,1,3) + f(1,3,4) >= f(1,3) + f(0,1,2,3,4)",
"Axiom54 04(2,3) - g2 f(0,2,3) + f(2,3,4) >= f(2,3) + f(0,1,2,3,4)",
"Axiom55 04(1,2,3) - g3 f(0,1,2,3,4) >= f(1,2,3)",
"Axiom56 14() - g0 f(1) + f(4) >= f(1,4)",
"Axiom57 14(0) - g1 f(0,1) + f(0,4) >= f(0) + f(0,1,4)",
"Axiom58 14(2) - g1 f(1,2) + f(2,4) >= f(2) + f(1,2,4)",
"Axiom59 14(0,2) - g2 f(0,1,2) + f(0,2,4) >= f(0,2) + f(0,1,2,3,4)",
"Axiom60 14(3) - g1 f(1,3) + f(3,4) >= f(3) + f(1,3,4)",
"Axiom61 14(0,3) - g2 f(0,1,3) + f(0,3,4) >= f(0,3) + f(0,1,2,3,4)",
"Axiom62 14(2,3) - g2 f(1,2,3) + f(2,3,4) >= f(2,3) + f(0,1,2,3,4)",
"Axiom63 14(0,2,3) - g3 f(0,1,2,3,4) >= f(0,2,3)",
"Axiom64 24() - g0 f(2) + f(4) >= f(2,4)",
"Axiom65 24(0) - g1 f(0,2) + f(0,4) >= f(0) + f(0,2,4)",
"Axiom66 24(1) - g1 f(1,2) + f(1,4) >= f(1) + f(1,2,4)",
"Axiom67 24(0,1) - g2 f(0,1,2) + f(0,1,4) >= f(0,1) + f(0,1,2,3,4)",
"Axiom68 24(3) - g1 f(2,3) + f(3,4) >= f(3) + f(2,3,4)",
"Axiom69 24(0,3) - g2 f(0,2,3) + f(0,3,4) >= f(0,3) + f(0,1,2,3,4)",
"Axiom70 24(1,3) - g2 f(1,2,3) + f(1,3,4) >= f(1,3) + f(0,1,2,3,4)",
"Axiom71 24(0,1,3) - g3 f(0,1,2,3,4) >= f(0,1,3)",
"Axiom72 34() - g0 f(3) + f(4) >= f(3,4)",
"Axiom73 34(0) - g1 f(0,3) + f(0,4) >= f(0) + f(0,3,4)",
"Axiom74 34(1) - g1 f(1,3) + f(1,4) >= f(1) + f(1,3,4)",
"Axiom75 34(0,1) - g2 f(0,1,3) + f(0,1,4) >= f(0,1) + f(0,1,2,3,4)",
"Axiom76 34(2) - g1 f(2,3) + f(2,4) >= f(2) + f(2,3,4)",
"Axiom77 34(0,2) - g2 f(0,2,3) + f(0,2,4) >= f(0,2) + f(0,1,2,3,4)",
"Axiom78 34(1,2) - g2 f(1,2,3) + f(1,2,4) >= f(1,2) + f(0,1,2,3,4)",
"Axiom79 34(0,1,2) - g3 f(0,1,2,3,4) >= f(0,1,2)"
]

N_6_AXIOMS = [
"Axiom0 01() - f(0) + f(1) >= f(0,1)",
"Axiom1 01(2) - f(0,2) + f(1,2) >= f(2) + f(0,1,2)",
"Axiom2 01(3) - f(0,3) + f(1,3) >= f(3) + f(0,1,3)",
"Axiom3 01(2,3) - f(0,2,3) + f(1,2,3) >= f(2,3) + f(0,1,2,3)",
"Axiom4 01(4) - f(0,4) + f(1,4) >= f(4) + f(0,1,4)",
"Axiom5 01(2,4) - f(0,2,4) + f(1,2,4) >= f(2,4) + f(0,1,2,4)",
"Axiom6 01(3,4) - f(0,3,4) + f(1,3,4) >= f(3,4) + f(0,1,3,4)",
"Axiom7 01(2,3,4) - f(0,2,3,4) + f(1,2,3,4) >= f(2,3,4) + f(0,1,2,3,4,5)",
"Axiom8 01(5) - f(0,5) + f(1,5) >= f(5) + f(0,1,5)",
"Axiom9 01(2,5) - f(0,2,5) + f(1,2,5) >= f(2,5) + f(0,1,2,5)",
"Axiom10 01(3,5) - f(0,3,5) + f(1,3,5) >= f(3,5) + f(0,1,3,5)",
"Axiom11 01(2,3,5) - f(0,2,3,5) + f(1,2,3,5) >= f(0,1,2,3,4,5) + f(2,3,5)",
"Axiom12 01(4,5) - f(0,4,5) + f(1,4,5) >= f(4,5) + f(0,1,4,5)",
"Axiom13 01(2,4,5) - f(0,2,4,5) + f(1,2,4,5) >= f(0,1,2,3,4,5) + f(2,4,5)",
"Axiom14 01(3,4,5) - f(0,3,4,5) + f(1,3,4,5) >= f(0,1,2,3,4,5) + f(3,4,5)",
"Axiom15 01(2,3,4,5) - f(0,1,2,3,4,5) >= f(2,3,4,5)",
"Axiom16 02() - f(0) + f(2) >= f(0,2)",
"Axiom17 02(1) - f(0,1) + f(1,2) >= f(1) + f(0,1,2)",
"Axiom18 02(3) - f(0,3) + f(2,3) >= f(3) + f(0,2,3)",
"Axiom19 02(1,3) - f(0,1,3) + f(1,2,3) >= f(1,3) + f(0,1,2,3)",
"Axiom20 02(4) - f(0,4) + f(2,4) >= f(4) + f(0,2,4)",
"Axiom21 02(1,4) - f(0,1,4) + f(1,2,4) >= f(1,4) + f(0,1,2,4)",
"Axiom22 02(3,4) - f(0,3,4) + f(2,3,4) >= f(3,4) + f(0,2,3,4)",
"Axiom23 02(1,3,4) - f(0,1,3,4) + f(1,2,3,4) >= f(1,3,4) + f(0,1,2,3,4,5)",
"Axiom24 02(5) - f(0,5) + f(2,5) >= f(5) + f(0,2,5)",
"Axiom25 02(1,5) - f(0,1,5) + f(1,2,5) >= f(1,5) + f(0,1,2,5)",
"Axiom26 02(3,5) - f(0,3,5) + f(2,3,5) >= f(3,5) + f(0,2,3,5)",
"Axiom27 02(1,3,5) - f(0,1,3,5) + f(1,2,3,5) >= f(0,1,2,3,4,5) + f(1,3,5)",
"Axiom28 02(4,5) - f(0,4,5) + f(2,4,5) >= f(4,5) + f(0,2,4,5)",
"Axiom29 02(1,4,5) - f(0,1,4,5) + f(1,2,4,5) >= f(0,1,2,3,4,5) + f(1,4,5)",
"Axiom30 02(3,4,5) - f(0,3,4,5) + f(2,3,4,5) >= f(0,1,2,3,4,5) + f(3,4,5)",
"Axiom31 02(1,3,4,5) - f(0,1,2,3,4,5) >= f(1,3,4,5)",
"Axiom32 12() - f(1) + f(2) >= f(1,2)",
"Axiom33 12(0) - f(0,1) + f(0,2) >= f(0) + f(0,1,2)",
"Axiom34 12(3) - f(1,3) + f(2,3) >= f(3) + f(1,2,3)",
"Axiom35 12(0,3) - f(0,1,3) + f(0,2,3) >= f(0,3) + f(0,1,2,3)",
"Axiom36 12(4) - f(1,4) + f(2,4) >= f(4) + f(1,2,4)",
"Axiom37 12(0,4) - f(0,1,4) + f(0,2,4) >= f(0,4) + f(0,1,2,4)",
"Axiom38 12(3,4) - f(1,3,4) + f(2,3,4) >= f(3,4) + f(1,2,3,4)",
"Axiom39 12(0,3,4) - f(0,1,3,4) + f(0,2,3,4) >= f(0,3,4) + f(0,1,2,3,4,5)",
"Axiom40 12(5) - f(1,5) + f(2,5) >= f(5) + f(1,2,5)",
"Axiom41 12(0,5) - f(0,1,5) + f(0,2,5) >= f(0,5) + f(0,1,2,5)",
"Axiom42 12(3,5) - f(1,3,5) + f(2,3,5) >= f(3,5) + f(1,2,3,5)",
"Axiom43 12(0,3,5) - f(0,1,3,5) + f(0,2,3,5) >= f(0,1,2,3,4,5) + f(0,3,5)",
"Axiom44 12(4,5) - f(1,4,5) + f(2,4,5) >= f(4,5) + f(1,2,4,5)",
"Axiom45 12(0,4,5) - f(0,1,4,5) + f(0,2,4,5) >= f(0,1,2,3,4,5) + f(0,4,5)",
"Axiom46 12(3,4,5) - f(1,3,4,5) + f(2,3,4,5) >= f(0,1,2,3,4,5) + f(3,4,5)",
"Axiom47 12(0,3,4,5) - f(0,1,2,3,4,5) >= f(0,3,4,5)",
"Axiom48 03() - f(0) + f(3) >= f(0,3)",
"Axiom49 03(1) - f(0,1) + f(1,3) >= f(1) + f(0,1,3)",
"Axiom50 03(2) - f(0,2) + f(2,3) >= f(2) + f(0,2,3)",
"Axiom51 03(1,2) - f(0,1,2) + f(1,2,3) >= f(1,2) + f(0,1,2,3)",
"Axiom52 03(4) - f(0,4) + f(3,4) >= f(4) + f(0,3,4)",
"Axiom53 03(1,4) - f(0,1,4) + f(1,3,4) >= f(1,4) + f(0,1,3,4)",
"Axiom54 03(2,4) - f(0,2,4) + f(2,3,4) >= f(2,4) + f(0,2,3,4)",
"Axiom55 03(1,2,4) - f(0,1,2,4) + f(1,2,3,4) >= f(1,2,4) + f(0,1,2,3,4,5)",
"Axiom56 03(5) - f(0,5) + f(3,5) >= f(5) + f(0,3,5)",
"Axiom57 03(1,5) - f(0,1,5) + f(1,3,5) >= f(1,5) + f(0,1,3,5)",
"Axiom58 03(2,5) - f(0,2,5) + f(2,3,5) >= f(2,5) + f(0,2,3,5)",
"Axiom59 03(1,2,5) - f(0,1,2,5) + f(1,2,3,5) >= f(0,1,2,3,4,5) + f(1,2,5)",
"Axiom60 03(4,5) - f(0,4,5) + f(3,4,5) >= f(4,5) + f(0,3,4,5)",
"Axiom61 03(1,4,5) - f(0,1,4,5) + f(1,3,4,5) >= f(0,1,2,3,4,5) + f(1,4,5)",
"Axiom62 03(2,4,5) - f(0,2,4,5) + f(2,3,4,5) >= f(0,1,2,3,4,5) + f(2,4,5)",
"Axiom63 03(1,2,4,5) - f(0,1,2,3,4,5) >= f(1,2,4,5)",
"Axiom64 13() - f(1) + f(3) >= f(1,3)",
"Axiom65 13(0) - f(0,1) + f(0,3) >= f(0) + f(0,1,3)",
"Axiom66 13(2) - f(1,2) + f(2,3) >= f(2) + f(1,2,3)",
"Axiom67 13(0,2) - f(0,1,2) + f(0,2,3) >= f(0,2) + f(0,1,2,3)",
"Axiom68 13(4) - f(1,4) + f(3,4) >= f(4) + f(1,3,4)",
"Axiom69 13(0,4) - f(0,1,4) + f(0,3,4) >= f(0,4) + f(0,1,3,4)",
"Axiom70 13(2,4) - f(1,2,4) + f(2,3,4) >= f(2,4) + f(1,2,3,4)",
"Axiom71 13(0,2,4) - f(0,1,2,4) + f(0,2,3,4) >= f(0,2,4) + f(0,1,2,3,4,5)",
"Axiom72 13(5) - f(1,5) + f(3,5) >= f(5) + f(1,3,5)",
"Axiom73 13(0,5) - f(0,1,5) + f(0,3,5) >= f(0,5) + f(0,1,3,5)",
"Axiom74 13(2,5) - f(1,2,5) + f(2,3,5) >= f(2,5) + f(1,2,3,5)",
"Axiom75 13(0,2,5) - f(0,1,2,5) + f(0,2,3,5) >= f(0,1,2,3,4,5) + f(0,2,5)",
"Axiom76 13(4,5) - f(1,4,5) + f(3,4,5) >= f(4,5) + f(1,3,4,5)",
"Axiom77 13(0,4,5) - f(0,1,4,5) + f(0,3,4,5) >= f(0,1,2,3,4,5) + f(0,4,5)",
"Axiom78 13(2,4,5) - f(1,2,4,5) + f(2,3,4,5) >= f(0,1,2,3,4,5) + f(2,4,5)",
"Axiom79 13(0,2,4,5) - f(0,1,2,3,4,5) >= f(0,2,4,5)",
"Axiom80 23() - f(2) + f(3) >= f(2,3)",
"Axiom81 23(0) - f(0,2) + f(0,3) >= f(0) + f(0,2,3)",
"Axiom82 23(1) - f(1,2) + f(1,3) >= f(1) + f(1,2,3)",
"Axiom83 23(0,1) - f(0,1,2) + f(0,1,3) >= f(0,1) + f(0,1,2,3)",
"Axiom84 23(4) - f(2,4) + f(3,4) >= f(4) + f(2,3,4)",
"Axiom85 23(0,4) - f(0,2,4) + f(0,3,4) >= f(0,4) + f(0,2,3,4)",
"Axiom86 23(1,4) - f(1,2,4) + f(1,3,4) >= f(1,4) + f(1,2,3,4)",
"Axiom87 23(0,1,4) - f(0,1,2,4) + f(0,1,3,4) >= f(0,1,4) + f(0,1,2,3,4,5)",
"Axiom88 23(5) - f(2,5) + f(3,5) >= f(5) + f(2,3,5)",
"Axiom89 23(0,5) - f(0,2,5) + f(0,3,5) >= f(0,5) + f(0,2,3,5)",
"Axiom90 23(1,5) - f(1,2,5) + f(1,3,5) >= f(1,5) + f(1,2,3,5)",
"Axiom91 23(0,1,5) - f(0,1,2,5) + f(0,1,3,5) >= f(0,1,2,3,4,5) + f(0,1,5)",
"Axiom92 23(4,5) - f(2,4,5) + f(3,4,5) >= f(4,5) + f(2,3,4,5)",
"Axiom93 23(0,4,5) - f(0,2,4,5) + f(0,3,4,5) >= f(0,1,2,3,4,5) + f(0,4,5)",
"Axiom94 23(1,4,5) - f(1,2,4,5) + f(1,3,4,5) >= f(0,1,2,3,4,5) + f(1,4,5)",
"Axiom95 23(0,1,4,5) - f(0,1,2,3,4,5) >= f(0,1,4,5)",
"Axiom96 04() - f(0) + f(4) >= f(0,4)",
"Axiom97 04(1) - f(0,1) + f(1,4) >= f(1) + f(0,1,4)",
"Axiom98 04(2) - f(0,2) + f(2,4) >= f(2) + f(0,2,4)",
"Axiom99 04(1,2) - f(0,1,2) + f(1,2,4) >= f(1,2) + f(0,1,2,4)",
"Axiom100 04(3) - f(0,3) + f(3,4) >= f(3) + f(0,3,4)",
"Axiom101 04(1,3) - f(0,1,3) + f(1,3,4) >= f(1,3) + f(0,1,3,4)",
"Axiom102 04(2,3) - f(0,2,3) + f(2,3,4) >= f(2,3) + f(0,2,3,4)",
"Axiom103 04(1,2,3) - f(0,1,2,3) + f(1,2,3,4) >= f(1,2,3) + f(0,1,2,3,4,5)",
"Axiom104 04(5) - f(0,5) + f(4,5) >= f(5) + f(0,4,5)",
"Axiom105 04(1,5) - f(0,1,5) + f(1,4,5) >= f(1,5) + f(0,1,4,5)",
"Axiom106 04(2,5) - f(0,2,5) + f(2,4,5) >= f(2,5) + f(0,2,4,5)",
"Axiom107 04(1,2,5) - f(0,1,2,5) + f(1,2,4,5) >= f(0,1,2,3,4,5) + f(1,2,5)",
"Axiom108 04(3,5) - f(0,3,5) + f(3,4,5) >= f(3,5) + f(0,3,4,5)",
"Axiom109 04(1,3,5) - f(0,1,3,5) + f(1,3,4,5) >= f(0,1,2,3,4,5) + f(1,3,5)",
"Axiom110 04(2,3,5) - f(0,2,3,5) + f(2,3,4,5) >= f(0,1,2,3,4,5) + f(2,3,5)",
"Axiom111 04(1,2,3,5) - f(0,1,2,3,4,5) >= f(1,2,3,5)",
"Axiom112 14() - f(1) + f(4) >= f(1,4)",
"Axiom113 14(0) - f(0,1) + f(0,4) >= f(0) + f(0,1,4)",
"Axiom114 14(2) - f(1,2) + f(2,4) >= f(2) + f(1,2,4)",
"Axiom115 14(0,2) - f(0,1,2) + f(0,2,4) >= f(0,2) + f(0,1,2,4)",
"Axiom116 14(3) - f(1,3) + f(3,4) >= f(3) + f(1,3,4)",
"Axiom117 14(0,3) - f(0,1,3) + f(0,3,4) >= f(0,3) + f(0,1,3,4)",
"Axiom118 14(2,3) - f(1,2,3) + f(2,3,4) >= f(2,3) + f(1,2,3,4)",
"Axiom119 14(0,2,3) - f(0,1,2,3) + f(0,2,3,4) >= f(0,2,3) + f(0,1,2,3,4,5)",
"Axiom120 14(5) - f(1,5) + f(4,5) >= f(5) + f(1,4,5)",
"Axiom121 14(0,5) - f(0,1,5) + f(0,4,5) >= f(0,5) + f(0,1,4,5)",
"Axiom122 14(2,5) - f(1,2,5) + f(2,4,5) >= f(2,5) + f(1,2,4,5)",
"Axiom123 14(0,2,5) - f(0,1,2,5) + f(0,2,4,5) >= f(0,1,2,3,4,5) + f(0,2,5)",
"Axiom124 14(3,5) - f(1,3,5) + f(3,4,5) >= f(3,5) + f(1,3,4,5)",
"Axiom125 14(0,3,5) - f(0,1,3,5) + f(0,3,4,5) >= f(0,1,2,3,4,5) + f(0,3,5)",
"Axiom126 14(2,3,5) - f(1,2,3,5) + f(2,3,4,5) >= f(0,1,2,3,4,5) + f(2,3,5)",
"Axiom127 14(0,2,3,5) - f(0,1,2,3,4,5) >= f(0,2,3,5)",
"Axiom128 24() - f(2) + f(4) >= f(2,4)",
"Axiom129 24(0) - f(0,2) + f(0,4) >= f(0) + f(0,2,4)",
"Axiom130 24(1) - f(1,2) + f(1,4) >= f(1) + f(1,2,4)",
"Axiom131 24(0,1) - f(0,1,2) + f(0,1,4) >= f(0,1) + f(0,1,2,4)",
"Axiom132 24(3) - f(2,3) + f(3,4) >= f(3) + f(2,3,4)",
"Axiom133 24(0,3) - f(0,2,3) + f(0,3,4) >= f(0,3) + f(0,2,3,4)",
"Axiom134 24(1,3) - f(1,2,3) + f(1,3,4) >= f(1,3) + f(1,2,3,4)",
"Axiom135 24(0,1,3) - f(0,1,2,3) + f(0,1,3,4) >= f(0,1,3) + f(0,1,2,3,4,5)",
"Axiom136 24(5) - f(2,5) + f(4,5) >= f(5) + f(2,4,5)",
"Axiom137 24(0,5) - f(0,2,5) + f(0,4,5) >= f(0,5) + f(0,2,4,5)",
"Axiom138 24(1,5) - f(1,2,5) + f(1,4,5) >= f(1,5) + f(1,2,4,5)",
"Axiom139 24(0,1,5) - f(0,1,2,5) + f(0,1,4,5) >= f(0,1,2,3,4,5) + f(0,1,5)",
"Axiom140 24(3,5) - f(2,3,5) + f(3,4,5) >= f(3,5) + f(2,3,4,5)",
"Axiom141 24(0,3,5) - f(0,2,3,5) + f(0,3,4,5) >= f(0,1,2,3,4,5) + f(0,3,5)",
"Axiom142 24(1,3,5) - f(1,2,3,5) + f(1,3,4,5) >= f(0,1,2,3,4,5) + f(1,3,5)",
"Axiom143 24(0,1,3,5) - f(0,1,2,3,4,5) >= f(0,1,3,5)",
"Axiom144 34() - f(3) + f(4) >= f(3,4)",
"Axiom145 34(0) - f(0,3) + f(0,4) >= f(0) + f(0,3,4)",
"Axiom146 34(1) - f(1,3) + f(1,4) >= f(1) + f(1,3,4)",
"Axiom147 34(0,1) - f(0,1,3) + f(0,1,4) >= f(0,1) + f(0,1,3,4)",
"Axiom148 34(2) - f(2,3) + f(2,4) >= f(2) + f(2,3,4)",
"Axiom149 34(0,2) - f(0,2,3) + f(0,2,4) >= f(0,2) + f(0,2,3,4)",
"Axiom150 34(1,2) - f(1,2,3) + f(1,2,4) >= f(1,2) + f(1,2,3,4)",
"Axiom151 34(0,1,2) - f(0,1,2,3) + f(0,1,2,4) >= f(0,1,2) + f(0,1,2,3,4,5)",
"Axiom152 34(5) - f(3,5) + f(4,5) >= f(5) + f(3,4,5)",
"Axiom153 34(0,5) - f(0,3,5) + f(0,4,5) >= f(0,5) + f(0,3,4,5)",
"Axiom154 34(1,5) - f(1,3,5) + f(1,4,5) >= f(1,5) + f(1,3,4,5)",
"Axiom155 34(0,1,5) - f(0,1,3,5) + f(0,1,4,5) >= f(0,1,2,3,4,5) + f(0,1,5)",
"Axiom156 34(2,5) - f(2,3,5) + f(2,4,5) >= f(2,5) + f(2,3,4,5)",
"Axiom157 34(0,2,5) - f(0,2,3,5) + f(0,2,4,5) >= f(0,1,2,3,4,5) + f(0,2,5)",
"Axiom158 34(1,2,5) - f(1,2,3,5) + f(1,2,4,5) >= f(0,1,2,3,4,5) + f(1,2,5)",
"Axiom159 34(0,1,2,5) - f(0,1,2,3,4,5) >= f(0,1,2,5)",
"Axiom160 05() - f(0) + f(5) >= f(0,5)",
"Axiom161 05(1) - f(0,1) + f(1,5) >= f(1) + f(0,1,5)",
"Axiom162 05(2) - f(0,2) + f(2,5) >= f(2) + f(0,2,5)",
"Axiom163 05(1,2) - f(0,1,2) + f(1,2,5) >= f(1,2) + f(0,1,2,5)",
"Axiom164 05(3) - f(0,3) + f(3,5) >= f(3) + f(0,3,5)",
"Axiom165 05(1,3) - f(0,1,3) + f(1,3,5) >= f(1,3) + f(0,1,3,5)",
"Axiom166 05(2,3) - f(0,2,3) + f(2,3,5) >= f(2,3) + f(0,2,3,5)",
"Axiom167 05(1,2,3) - f(0,1,2,3) + f(1,2,3,5) >= f(1,2,3) + f(0,1,2,3,4,5)",
"Axiom168 05(4) - f(0,4) + f(4,5) >= f(4) + f(0,4,5)",
"Axiom169 05(1,4) - f(0,1,4) + f(1,4,5) >= f(1,4) + f(0,1,4,5)",
"Axiom170 05(2,4) - f(0,2,4) + f(2,4,5) >= f(2,4) + f(0,2,4,5)",
"Axiom171 05(1,2,4) - f(0,1,2,4) + f(1,2,4,5) >= f(1,2,4) + f(0,1,2,3,4,5)",
"Axiom172 05(3,4) - f(0,3,4) + f(3,4,5) >= f(3,4) + f(0,3,4,5)",
"Axiom173 05(1,3,4) - f(0,1,3,4) + f(1,3,4,5) >= f(1,3,4) + f(0,1,2,3,4,5)",
"Axiom174 05(2,3,4) - f(0,2,3,4) + f(2,3,4,5) >= f(2,3,4) + f(0,1,2,3,4,5)",
"Axiom175 05(1,2,3,4) - f(0,1,2,3,4,5) >= f(1,2,3,4)",
"Axiom176 15() - f(1) + f(5) >= f(1,5)",
"Axiom177 15(0) - f(0,1) + f(0,5) >= f(0) + f(0,1,5)",
"Axiom178 15(2) - f(1,2) + f(2,5) >= f(2) + f(1,2,5)",
"Axiom179 15(0,2) - f(0,1,2) + f(0,2,5) >= f(0,2) + f(0,1,2,5)",
"Axiom180 15(3) - f(1,3) + f(3,5) >= f(3) + f(1,3,5)",
"Axiom181 15(0,3) - f(0,1,3) + f(0,3,5) >= f(0,3) + f(0,1,3,5)",
"Axiom182 15(2,3) - f(1,2,3) + f(2,3,5) >= f(2,3) + f(1,2,3,5)",
"Axiom183 15(0,2,3) - f(0,1,2,3) + f(0,2,3,5) >= f(0,2,3) + f(0,1,2,3,4,5)",
"Axiom184 15(4) - f(1,4) + f(4,5) >= f(4) + f(1,4,5)",
"Axiom185 15(0,4) - f(0,1,4) + f(0,4,5) >= f(0,4) + f(0,1,4,5)",
"Axiom186 15(2,4) - f(1,2,4) + f(2,4,5) >= f(2,4) + f(1,2,4,5)",
"Axiom187 15(0,2,4) - f(0,1,2,4) + f(0,2,4,5) >= f(0,2,4) + f(0,1,2,3,4,5)",
"Axiom188 15(3,4) - f(1,3,4) + f(3,4,5) >= f(3,4) + f(1,3,4,5)",
"Axiom189 15(0,3,4) - f(0,1,3,4) + f(0,3,4,5) >= f(0,3,4) + f(0,1,2,3,4,5)",
"Axiom190 15(2,3,4) - f(1,2,3,4) + f(2,3,4,5) >= f(2,3,4) + f(0,1,2,3,4,5)",
"Axiom191 15(0,2,3,4) - f(0,1,2,3,4,5) >= f(0,2,3,4)",
"Axiom192 25() - f(2) + f(5) >= f(2,5)",
"Axiom193 25(0) - f(0,2) + f(0,5) >= f(0) + f(0,2,5)",
"Axiom194 25(1) - f(1,2) + f(1,5) >= f(1) + f(1,2,5)",
"Axiom195 25(0,1) - f(0,1,2) + f(0,1,5) >= f(0,1) + f(0,1,2,5)",
"Axiom196 25(3) - f(2,3) + f(3,5) >= f(3) + f(2,3,5)",
"Axiom197 25(0,3) - f(0,2,3) + f(0,3,5) >= f(0,3) + f(0,2,3,5)",
"Axiom198 25(1,3) - f(1,2,3) + f(1,3,5) >= f(1,3) + f(1,2,3,5)",
"Axiom199 25(0,1,3) - f(0,1,2,3) + f(0,1,3,5) >= f(0,1,3) + f(0,1,2,3,4,5)",
"Axiom200 25(4) - f(2,4) + f(4,5) >= f(4) + f(2,4,5)",
"Axiom201 25(0,4) - f(0,2,4) + f(0,4,5) >= f(0,4) + f(0,2,4,5)",
"Axiom202 25(1,4) - f(1,2,4) + f(1,4,5) >= f(1,4) + f(1,2,4,5)",
"Axiom203 25(0,1,4) - f(0,1,2,4) + f(0,1,4,5) >= f(0,1,4) + f(0,1,2,3,4,5)",
"Axiom204 25(3,4) - f(2,3,4) + f(3,4,5) >= f(3,4) + f(2,3,4,5)",
"Axiom205 25(0,3,4) - f(0,2,3,4) + f(0,3,4,5) >= f(0,3,4) + f(0,1,2,3,4,5)",
"Axiom206 25(1,3,4) - f(1,2,3,4) + f(1,3,4,5) >= f(1,3,4) + f(0,1,2,3,4,5)",
"Axiom207 25(0,1,3,4) - f(0,1,2,3,4,5) >= f(0,1,3,4)",
"Axiom208 35() - f(3) + f(5) >= f(3,5)",
"Axiom209 35(0) - f(0,3) + f(0,5) >= f(0) + f(0,3,5)",
"Axiom210 35(1) - f(1,3) + f(1,5) >= f(1) + f(1,3,5)",
"Axiom211 35(0,1) - f(0,1,3) + f(0,1,5) >= f(0,1) + f(0,1,3,5)",
"Axiom212 35(2) - f(2,3) + f(2,5) >= f(2) + f(2,3,5)",
"Axiom213 35(0,2) - f(0,2,3) + f(0,2,5) >= f(0,2) + f(0,2,3,5)",
"Axiom214 35(1,2) - f(1,2,3) + f(1,2,5) >= f(1,2) + f(1,2,3,5)",
"Axiom215 35(0,1,2) - f(0,1,2,3) + f(0,1,2,5) >= f(0,1,2) + f(0,1,2,3,4,5)",
"Axiom216 35(4) - f(3,4) + f(4,5) >= f(4) + f(3,4,5)",
"Axiom217 35(0,4) - f(0,3,4) + f(0,4,5) >= f(0,4) + f(0,3,4,5)",
"Axiom218 35(1,4) - f(1,3,4) + f(1,4,5) >= f(1,4) + f(1,3,4,5)",
"Axiom219 35(0,1,4) - f(0,1,3,4) + f(0,1,4,5) >= f(0,1,4) + f(0,1,2,3,4,5)",
"Axiom220 35(2,4) - f(2,3,4) + f(2,4,5) >= f(2,4) + f(2,3,4,5)",
"Axiom221 35(0,2,4) - f(0,2,3,4) + f(0,2,4,5) >= f(0,2,4) + f(0,1,2,3,4,5)",
"Axiom222 35(1,2,4) - f(1,2,3,4) + f(1,2,4,5) >= f(1,2,4) + f(0,1,2,3,4,5)",
"Axiom223 35(0,1,2,4) - f(0,1,2,3,4,5) >= f(0,1,2,4)",
"Axiom224 45() - f(4) + f(5) >= f(4,5)",
"Axiom225 45(0) - f(0,4) + f(0,5) >= f(0) + f(0,4,5)",
"Axiom226 45(1) - f(1,4) + f(1,5) >= f(1) + f(1,4,5)",
"Axiom227 45(0,1) - f(0,1,4) + f(0,1,5) >= f(0,1) + f(0,1,4,5)",
"Axiom228 45(2) - f(2,4) + f(2,5) >= f(2) + f(2,4,5)",
"Axiom229 45(0,2) - f(0,2,4) + f(0,2,5) >= f(0,2) + f(0,2,4,5)",
"Axiom230 45(1,2) - f(1,2,4) + f(1,2,5) >= f(1,2) + f(1,2,4,5)",
"Axiom231 45(0,1,2) - f(0,1,2,4) + f(0,1,2,5) >= f(0,1,2) + f(0,1,2,3,4,5)",
"Axiom232 45(3) - f(3,4) + f(3,5) >= f(3) + f(3,4,5)",
"Axiom233 45(0,3) - f(0,3,4) + f(0,3,5) >= f(0,3) + f(0,3,4,5)",
"Axiom234 45(1,3) - f(1,3,4) + f(1,3,5) >= f(1,3) + f(1,3,4,5)",
"Axiom235 45(0,1,3) - f(0,1,3,4) + f(0,1,3,5) >= f(0,1,3) + f(0,1,2,3,4,5)",
"Axiom236 45(2,3) - f(2,3,4) + f(2,3,5) >= f(2,3) + f(2,3,4,5)",
"Axiom237 45(0,2,3) - f(0,2,3,4) + f(0,2,3,5) >= f(0,2,3) + f(0,1,2,3,4,5)",
"Axiom238 45(1,2,3) - f(1,2,3,4) + f(1,2,3,5) >= f(1,2,3) + f(0,1,2,3,4,5)",
"Axiom239 45(0,1,2,3) - f(0,1,2,3,4,5) >= f(0,1,2,3)"
]

if SET_N == 5: AXIOM_DESC = N_5_AXIOMS
if SET_N == 6: AXIOM_DESC = N_6_AXIOMS

for line in sys.stdin:
    # print(f"::{STATE}::{line}", end="")
    
    if STATE=="filebegin":
        if re.search(r"\bSLICER_STARTING\b", line):
            SLICER_RUNS += 1
            m = re.search(r"\bVARS=([0-9]+)", line)
            VARS=int(m.group(1))
            
            m = re.search(r"\bAXIOMS=([0-9]+)", line)
            AXIOMS=int(m.group(1))
            
            m = re.search(r"\bSET_N=([0-9]+)", line)
            FILE_SET_N=int(m.group(1))
            assert SET_N == FILE_SET_N
            
            INITIAL_AXIOMS = []
            AXIOM_SET = []
            STATE="filebegin2"
            continue

    if STATE=="filebegin2":
        m = re.search(r"(\bvary_axiom=|Last but one axiom: )(.*)$", line)
        if m:
            VARY_AXIOM = int(m.group(2))
            STATE="init"
    
    if STATE=="init":
        m = re.search(r"\binitial_axioms=(.*)$", line)
        if m:
            init_axioms = m.group(1).split(',')[:-1]
            assert len(init_axioms) == VARS
            INITIAL_AXIOMS = []
            for a in init_axioms:
                AXIOM_SET.append(int(a)) # collect for previous axioms
                INITIAL_AXIOMS.append(int(a))
            STATE="initends"
            continue
        
        if re.search(r"\bNO_INITIAL_AXIOMS\b", line):
            EXPERIENCES.append({
                'vary_axiom': VARY_AXIOM,
                'no_initial': True
            })
            STATE="filebegin"
            
        
    if STATE=="initends":
        if re.search(r"ADDING MORE AXIOMS", line):
            STATE="axiombegins"
            continue
        
    if STATE=="axiombegins":
        m = re.search(r"\bwill_apply_axiom=([0-9]+)", line)
        if m:
            NEW_AXIOM=int(m.group(1))
            m = re.search(r"\bwill_process_pairs=([0-9]+)", line)
            RAY_PAIRS = int(m.group(1))
            EXPERIENCES.append({
                'initial_axioms': [x for x in INITIAL_AXIOMS],
                'prev_axioms': [x for x in AXIOM_SET],
                'new_axiom': NEW_AXIOM,
                'vary_axiom': VARY_AXIOM,
                'ray_pairs': RAY_PAIRS
            })
            STATE="axiomheader"
            continue
        
        m = re.search(r"\btotal_elapsed_time=([0-9\.]+)", line)
        if m:
            EXPERIENCES[-1]['total_time'] = float(m.group(1))
            INITIAL_AXIOMS = []
            AXIOM_SET = []
            STATE='filebegin'
            continue
        
    if STATE=="axiomheader":
        if re.search(r"\bTOOMANYPAIRS\b", line):
            EXPERIENCES[-1]['too_many'] = 1
            INITIAL_AXIOMS = []
            AXIOM_SET = []
            STATE='filebegin'
            continue
        
        m = re.search(r"\bapplying_axiom=([0-9]+)", line)
        if m:
            assert NEW_AXIOM == int(m.group(1))
            m = re.search(r"\bprev_total_rays=([0-9]+)", line)
            PREV_RAYS = int(m.group(1))
            EXPERIENCES[-1]['prev_rays'] = PREV_RAYS
            STATE='axiombody'
            
    if STATE=='axiombody':
        m = re.search(r"\bapplied_axiom=([0-9]+)", line)
        if m:
            assert NEW_AXIOM == int(m.group(1))
            m = re.search(r"\btotal_rays=([0-9]+)", line)
            EXPERIENCES[-1]['result_rays'] = int(m.group(1))
            AXIOM_SET.append(NEW_AXIOM)
            STATE='axiombegins'
            continue

def printw(v, n):
    # print v at width n
    x = str(v)
    while len(x) < n:
        x = " "+x
    return x

max_ray_pairs = {i:None for i in range(AXIOMS+1)}
max_prev_rays = {i:None for i in range(AXIOMS+1)}
pairs_to_axiom = {i:[] for i in range(AXIOMS+1)} # number of ray pairs to axiom
rays_to_axiom = {i:[] for i in range(AXIOMS+1)} # number of prev rays to axiom

for e in EXPERIENCES:
    if e.get('no_initial'):
        print(f"! VaryAxiom={e.get('vary_axiom')} NoInitialAxiomSet")
        continue
    
    step = len(e['prev_axioms'])+1
    o = []
    o.append(f"Step={step}")
    if e.get('too_many'):
        o.append("TooManyRayPairs")
    o.append(f"PrevRays={printw(e.get('prev_rays'), 6)}")
    if max_prev_rays[step] is None or e['prev_rays'] > max_prev_rays[step]:
        max_prev_rays[step] = e['prev_rays']
    o.append(f"ResultRays={printw(e.get('result_rays'), 6)}")
    o.append(f"RayPairs={printw(e.get('ray_pairs'), 10)}")
    if max_ray_pairs[step] is None or e['ray_pairs'] > max_ray_pairs[step]:
        max_ray_pairs[step] = e['ray_pairs']
    o.append(f"NewAxiom={e.get('new_axiom')}")
    o.append(f"""PrevAxioms={','.join([str(x) for x in e['prev_axioms']] + [f'+{e["new_axiom"]}'])}""")
    o.append(f"MissingAxioms={','.join([str(x) for x in range(AXIOMS) if x not in e['prev_axioms'] and x != e['new_axiom']])}")
    if e.get('total_time'):
        o.append(f"TotalTime={e.get('total_time')}")
        
    if OUT_STEP == -1 or OUT_STEP == step:
        print(' '.join(o))

    pairs_to_axiom[step].append({'ray_pairs': e['ray_pairs'], 'new_axiom': e['new_axiom']})
    rays_to_axiom[step].append({'prev_rays': e['prev_rays'], 'new_axiom': e['new_axiom']})

if OUT_STEP != -1:
    print("")
    print("Ordered by number of ray pairs:")

    for step in range(AXIOMS+1):
        if OUT_STEP==-1 or OUT_STEP==step:
            pairs_to_axiom[step].sort(key=lambda x: x['ray_pairs'])
            for e in pairs_to_axiom[step]:
                o = []
                o.append(f"Step={step}")
                d = int(e['ray_pairs']*30/max_ray_pairs[step]+.5)
                o.append("#"*d + "."*(30-d))
                if e['ray_pairs'] == pairs_to_axiom[step][0]['ray_pairs']:
                    o.append("(min)")
                o.append(AXIOM_DESC[e['new_axiom']])
                print(' '.join(o))

    print("")
    print("Ordered by number of previous rays:")

    for step in range(AXIOMS+1):
        if OUT_STEP==-1 or OUT_STEP==step:
            rays_to_axiom[step].sort(key=lambda x: x['prev_rays'])
            for e in rays_to_axiom[step]:
                o = []
                o.append(f"Step={step}")
                d = int(e['prev_rays']*30/max_prev_rays[step]+.5)
                o.append("$"*d + "."*(30-d))
                if e['prev_rays'] == rays_to_axiom[step][0]['prev_rays']:
                    o.append("(min)")
                o.append(AXIOM_DESC[e['new_axiom']])
                print(' '.join(o))

else: # For a single run, called with -1
    
    if SLICER_RUNS != 1:
        print("")
        print("WARNING! NUMBER OF RUNS IN FILE IS NOT 1 - DATA IS LIKELY INACCURATE")
    
    print()
    print("Initial axioms:")
    for a in EXPERIENCES[0]['initial_axioms']:
        print(AXIOM_DESC[a])
    
    # List axioms in order
    print()
    print("Axioms in order:")
    for step in range(AXIOMS+1):
        if len(rays_to_axiom[step]):
            assert len(rays_to_axiom[step]) == 1
            print(f"Step={step} {AXIOM_DESC[rays_to_axiom[step][0]['new_axiom']]}")

print("")    
print(f"Runs found in file: {SLICER_RUNS}")
