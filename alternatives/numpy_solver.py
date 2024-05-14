
# Add to matroid.py


def solve():
    axioms = get_axioms()
    num_axioms = len(axioms)
    variables = dimensions()
    print(f"Axioms: {num_axioms} Variables: {dimensions()}")
    axioms = np.array(axioms, dtype=np.float_)  # turn into a numpy matrix

    # An extra f(MAXIMAL)=1 expression to avoid the (0,0,0,...) solution
    extra_expression = np.array([[0 for i in range(variables)]], dtype=np.float_)
    smap = set_create_index_map()
    extra_expression[0][smap[2**SET_N-1]] = 1
    result = np.zeros(variables)
    result[0] = 1  # result = [1, 0, 0, ..., 0]

    tries = 0
    det_null = 0
    violates = 0
    success = 0
    while True:
        print(f"Tries={tries} no solution={det_null} violates={violates} success={success}")
        tries += 1
        index = np.random.choice(num_axioms, size=variables-1, replace=False)
        mat = axioms[index]  # a random subset of axioms
        mat = np.concatenate([extra_expression, mat])
        # mat: [1,1,1,...,1
        #      [axiom
        #      ...
        #      [axiom
        if np.linalg.det(mat) == 0:
            # No solution
            det_null += 1
            continue
        x = np.linalg.solve(a=mat, b=result)
        # Check the solution against all axioms
        r = np.dot(axioms, x)
        if np.min(r) < -EPSILON:
            violates += 1
            if DEBUG:
                print("VIOLATION")
                for i_, e_ in enumerate(axioms):
                    print(f"#{i_} {display_expression(e_)}", end="")
                    if i_ in index:
                        print(" (selected)", end="")
                    if r[i_] < -EPSILON:
                        print(f" Violated, result = {r[i_]}")
                    else:
                        print("")  # new line
                print(f"selected: {index}")
                print(f"solution: {display_vector(x)}")
            continue
        success += 1
        if DEBUG:
            print("SUCCESS")
            for i_, e_ in enumerate(axioms):
                print(f"#{i_} {display_expression(e_)}", end="")
                if i_ in index:
                    print(" (selected)", end="")
                print(f" Result = {r[i_]}")
            print(f"selected: {index}")
            print(f"solution: {display_vector(x)}")
