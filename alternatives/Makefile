
# Choose a random subset of axioms in the hope of finding a ray

rand_axiom_run:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -D EARLY_STOP rand_axioms.c -o int_solver
	./int_solver

rand_axiom_debug:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -D DEBUG rand_axioms.c -o int_solver
	./int_solver

rand_axiom_collect:
	python get_axioms.py > axioms.c
	gcc -O3 -lm -D EARLY_STOP rand_axioms.c -o int_solver
	./int_solver | python collect_stats.py

rand_axiom_test:
	python get_axioms_test.py
	gcc -Wall -lm -D UNIT_TEST -D TEST0 -D DEBUG rand_axioms.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST1 -D DEBUG rand_axioms.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST2 -D DEBUG rand_axioms.c && ./a.out
	gcc -lm -D SOLVER_TEST -D TEST3 -D DEBUG rand_axioms.c && ./a.out
	rm a.out

