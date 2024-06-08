
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

# Slice a cone with axioms until we get the final one

# Adjust the below to enable debugging
remove_lines = DEBUG SO_DEBUG RS_DEBUG
# remove_lines = RS_DEBUG

slicer_test:
	gcc -lm test.c -pthread && ./a.out
	rm a.out

slicer_run:
	python get_axioms.py > axioms.c
	./strip_debug.pl $(remove_lines) < axioms.c > axioms.strp.c
	./strip_debug.pl $(remove_lines) < slicer.c > slicer.strp.c
	./strip_debug.pl $(remove_lines) < slicer_solver.c > slicer_solver.strp.c
	./strip_debug.pl $(remove_lines) < ray_store.c > ray_store.strp.c
	./strip_debug.pl $(remove_lines) < util.c > util.strp.c
	gcc -lm -O3 slicer.strp.c -o slicer -pthread
	./slicer
