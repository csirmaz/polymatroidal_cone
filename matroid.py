
import numpy as np

SET_N = 6 # number of elements in the base set
TIGHT = True  # whether to consider tight matroids only
DEBUG = False
EPSILON = 1e-14

def dimensions() -> int:
    """Return the number of variables / possible f() values"""
    d = 2**SET_N
    d -= 1  # for f(0)
    if TIGHT:
        d -= SET_N  # for f(M-i)
    return d

def set_to_bitmap(s: set) -> int:
    """Given a set, return a bitmap representing the set"""
    d = 0
    for e in range(SET_N):  # 0 <= e < SET_N
        if e in s:
            d += 2**e
    return d

def set_bitmap_to_set(s: int) -> set:
    """Given a set as a bitmap, return an actual set"""
    r = set()
    d = 0
    while s:
        if s & 1:
            r.add(d)
        s >>= 1
        d += 1
    return r

def set_len(s: int) -> int:
    """Given a set represented as a bitmap, return the number of elements"""
    d = 0
    while s:
        if s & 1:
            d += 1
        s >>= 1
    return d

def set_create_index_map():
    """Return a map (dict) mapping sets as bitmaps to an index"""
    map = {}
    next_index = 0
    maximal = None
    for s in range(2**SET_N):  # 0 <= s < 2**SET_N
        if s == 0:
            continue  # the empty set is not mapped to anything
        if TIGHT:
            # If TIGHT, all M-i and M are mapped to the same variable
            if set_len(s) >= SET_N - 1:
                if maximal is None:
                    maximal = next_index  # next available index
                else:
                    map[s] = maximal
                    continue
        map[s] = next_index
        next_index += 1
    assert next_index == dimensions()
    return map

def modularities():
    """Create linear expressions (matrix of coefficients) for the (sub)modularity axioms"""
    # f(K+i) + f(K+j) >= F(K+i+j) + f(K)
    smap = set_create_index_map()
    expressions = []
    for i in range(SET_N):  # 0 <= i < SET_N
        for j in range(i):  # 0 <= j < i
            for k in range(2 ** SET_N):  # 0 <= s < 2**SET_N
                if (k & 2**i) or (k & 2**j):
                    continue
                expression = [0 for x in range(dimensions())]  # initialize to [0, 0, ...]
                expression[smap[k + 2**i]] += 1
                expression[smap[k + 2**j]] += 1
                if k > 0:  # f(0) is always 0 so not represented
                    expression[smap[k]] -= 1
                expression[smap[k + 2**j + 2**i]] -= 1
                expressions.append(expression)
    assert len(expressions) == SET_N*(SET_N-1)*(2**(SET_N-3))
    return expressions

def monotonicity():
    """Create linear expressions (matrix of coefficients) for the monotonicity axioms"""
    # f(M) >= F(M-i)
    if TIGHT:
        return []
    smap = set_create_index_map()
    expressions = []
    for i in range(SET_N):  # 0 <= i < SET_N
        expression = [0 for x in range(dimensions())]  # initialize to [0, 0, ...]
        expression[smap[2**SET_N - 1]] += 1
        expression[smap[2**SET_N - 1 - 2**i]] -= 1
        expressions.append(expression)
    assert len(expressions) == SET_N
    return expressions

def get_axioms():
    """Return linear expressions (matrix of coefficients) for all axioms"""
    return modularities() + monotonicity()

def solve():
    axioms = get_axioms()
    num_axioms = len(axioms)
    variables = dimensions()
    axioms = np.array(axioms, dtype=np.float_)  # turn into a numpy matrix

    # An extra SUM f(x) = 1 expression to avoid the (0,0,0,...) solution
    extra_expression = np.array([[1 for i in range(variables)]], dtype=np.float_)
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


def display_vector(v) -> str:
    """Create human-readable string from a vector of coefficients"""
    smap = set_create_index_map()
    rev_map = {v: k for k, v in smap.items()}
    r = []
    for e, v in enumerate(v):
        if abs(v) < EPSILON: continue  # do not list 0s
        s = rev_map[e]
        f = f"f({set_bitmap_to_set(s)})={v}"
        r.append(f)
    return ", ".join(r)

def display_expression(exp) -> str:
    """Create human-readable string from an expression"""
    smap = set_create_index_map()
    rev_map = {v: k for k, v in smap.items()}
    big = []
    small = []
    for e, v in enumerate(exp):
        if v == 0: continue
        s = rev_map[e]
        f = f"f({set_bitmap_to_set(s)})"
        if v == 1:
            big.append(f)
        elif v == -1:
            small.append(f)
        else:
            raise ValueError("Wrong v")
    return f"{' + '.join(big)} >= {' + '.join(small)}"

solve()
