.PHONY: slicer_test slicer_run_4 slicer_run_5 slicer_run_6 c_sources

# Slice a cone with axioms until we get the final one

# Adjust the below to enable debugging
remove_lines = DEBUG SO_DEBUG RS_DEBUG
# remove_lines = RS_DEBUG

data/axioms4.c: get_axioms.py
	python get_axioms.py 4 > data/axioms4.c
	
data/axioms5.c: get_axioms.py
	python get_axioms.py 5 > data/axioms5.c

data/axioms5i.c: get_axioms.py
	python get_axioms.py 5 i > data/axioms5i.c

data/axioms6.c: get_axioms.py
	python get_axioms.py 6 > data/axioms6.c

c_sources: data/axioms4.c data/axioms5.c data/axioms5i.c data/axioms6.c
	./strip_debug.pl $(remove_lines) < data/axioms4.c > data/axioms4.strp.c
	./strip_debug.pl $(remove_lines) < data/axioms5.c > data/axioms5.strp.c
	./strip_debug.pl $(remove_lines) < data/axioms5i.c > data/axioms5i.strp.c
	./strip_debug.pl $(remove_lines) < data/axioms6.c > data/axioms6.strp.c
	./strip_debug.pl $(remove_lines) < slicer_solver.c > slicer_solver.strp.c
	./strip_debug.pl $(remove_lines) < slicer_dependency_check.c > slicer_dependency_check.strp.c
	./strip_debug.pl $(remove_lines) < ray_store.c > ray_store.strp.c
	./strip_debug.pl $(remove_lines) < util.c > util.strp.c
	./strip_debug.pl $(remove_lines) < vars.c > vars.strp.c
	./strip_debug.pl $(remove_lines) < test.c > test.strp.c
	./strip_debug.pl $(remove_lines) < slicer.c > slicer.strp.c
	./strip_debug.pl $(remove_lines) < slicer_copy.c > slicer_copy.strp.c

slicer_test: c_sources
	gcc -lm test.strp.c -pthread && ./a.out
	rm a.out

slicer_run_4: c_sources
	gcc -lm -O3 -DAXIOMS_FILE=4 slicer.strp.c -o slicer -pthread
	./slicer

slicer_run_5: c_sources
	gcc -lm -O3 -DAXIOMS_FILE=5 slicer.strp.c -o slicer -pthread
	./slicer

slicer_run_5_i: c_sources
	gcc -lm -O3 -DAXIOMS_FILE=500 slicer.strp.c -o slicer -pthread
	./slicer
	
slicer_run_5_fix_test: c_sources
	# Called by generate_order to test if fixed axioms allow selecting initial ones
	gcc -lm -O3 -DAXIOMS_FILE=5 -DINIT_AXIOMS_TEST -DINIT_AXIOMS_ONLY slicer_copy.strp.c -o slicer -pthread
	./slicer

slicer_run_5_run_test: c_sources
	# Called by generate_order to test the resulting order
	gcc -lm -O3 -DAXIOMS_FILE=5 -DINIT_AXIOMS_TEST slicer_copy.strp.c -o slicer -pthread
	./slicer

slicer_run_6: c_sources
	gcc -lm -O3 -DAXIOMS_FILE=6 slicer.strp.c -o slicer -pthread
	./slicer
