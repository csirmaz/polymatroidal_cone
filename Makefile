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
	./strip_debug.pl $(remove_lines) < slicer_solver.c > stripped/slicer_solver.c
	./strip_debug.pl $(remove_lines) < ray_store.c > stripped/ray_store.c
	./strip_debug.pl $(remove_lines) < util.c > stripped/util.c
	./strip_debug.pl $(remove_lines) < vars.c > stripped/vars.c
	./strip_debug.pl $(remove_lines) < slicer.c > stripped/slicer.c

slicer_run_5: c_sources
	cd stripped; gcc -Wall -lm -O3 -DAXIOMS_FILE=5 slicer.c -o slicer -pthread
	stripped/slicer

slicer_run_5_i: c_sources
	cd stripped; gcc -lm -O3 -DAXIOMS_FILE=500 slicer.c -o slicer -pthread
	stripped/slicer
	
slicer_run_5_fix_test: c_sources
	# Called by generate_order to test if fixed axioms allow selecting initial ones
	cd stripped; gcc -lm -O3 -DAXIOMS_FILE=5 -DINIT_AXIOMS_TEST -DINIT_AXIOMS_ONLY slicer.c -o slicer -pthread
	stripped/slicer

slicer_run_5_run_test: c_sources
	# Call to test the resulting order
	cd stripped; gcc -lm -O3 -DAXIOMS_FILE=5 -DINIT_AXIOMS_TEST slicer.c -o slicer -pthread
	stripped/slicer

slicer_run_6: c_sources
	cd stripped; gcc -Wall -lm -O3 -DAXIOMS_FILE=6 slicer.c -o slicer -pthread
	stripped/slicer

slicer_run_6_fix_test: c_sources
	# Called by generate_order to test if fixed axioms allow selecting initial ones
	cd stripped; gcc -lm -O3 -DAXIOMS_FILE=6 -DINIT_AXIOMS_TEST -DINIT_AXIOMS_ONLY slicer.c -o slicer -pthread
	stripped/slicer

slicer_run_6_run_test: c_sources
	# Call to test the resulting order
	cd stripped; gcc -lm -O3 -DAXIOMS_FILE=6 -DINIT_AXIOMS_TEST slicer.c -o slicer -pthread
	stripped/slicer
