
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

int_run:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -D EARLY_STOP int_solver.c -o int_solver
	./int_solver

int_debug:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -D DEBUG int_solver.c -o int_solver
	./int_solver

collect_unique:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -D EARLY_STOP int_solver.c -o int_solver
	./int_solver | python collect_stats.py

rand_rays:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -DTRY_RAND_RAYS=5 int_solver.c -o rand_rays
	./rand_rays # | python collect_stats.py

int_test:
	python get_axioms_test.py
	gcc -Wall -lm -D UNIT_TEST -D TEST0 -D DEBUG int_solver.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST1 -D DEBUG int_solver.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST2 -D DEBUG int_solver.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST3 -D DEBUG int_solver.c && ./a.out
	rm a.out
