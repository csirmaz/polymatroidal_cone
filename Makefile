
run:
	python get_axioms.py > axioms.c
	gcc -O3 -lm matroid.c
