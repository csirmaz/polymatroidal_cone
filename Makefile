
run:
	python get_axioms.py > axioms.c
	gcc -O3 -lm solver.c -o solver
	./solver

test:
	python get_axioms_test.py
	gcc -Wall -lm -D UNIT_TEST -D TEST0 -D DEBUG solver.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST1 -D DEBUG solver.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST2 -D DEBUG solver.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST3 -D DEBUG solver.c && ./a.out
	rm a.out
