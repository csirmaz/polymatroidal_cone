
# Create C code defining polymatroidal axioms
# Optionally sort axioms into groups according to symmetries
# Usage: python get_axioms.py (4|5|6)

import sys

SET_N = int(sys.argv[1]) # number of elements in the base set
TIGHT = True  # whether to consider tight matroids only
MAKE_GROUPS = True # Whether to create groups of equivalent axioms when permuting the base set

def dimensions() -> int:
    """Return the number of variables / possible f() values"""
    d = 2**SET_N
    d -= 1  # for f(0)
    if TIGHT:
        d -= SET_N  # for f(M-i)
    return d


def set_to_bitmap(s: set) -> int:
    """Given a set containing numbers, return a bitmap representing the set"""
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


SET_MAP = None
REV_MAP = None
def set_create_index_map(force=False):
    """Return a map (dict) mapping sets as bitmaps to an index"""
    global SET_MAP, REV_MAP
    if (SET_MAP is not None) and (not force):
        return SET_MAP
    map = {} # bitmap to index
    rev_map = {} # index to bitmap
    next_index = 0
    maximal = None  # later, the index of "maximal" sets in tight polymatroids
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
                    # This is a many-to-one relationship but we keep the full set
                    rev_map[maximal] = s
                    continue
        map[s] = next_index
        rev_map[next_index] = s
        next_index += 1
    assert next_index == dimensions()
    if TIGHT:
        assert rev_map[maximal] == 2**SET_N-1
    SET_MAP = map
    REV_MAP = rev_map
    return map


def set_create_rev_map():
    """Create the reverse map mapping an index to a bitmap"""
    # Note: for tight matroids the index of the maximal set can in theory map to multiple bitmaps but we keep the full set
    if REV_MAP is None:
        set_create_index_map()
    return REV_MAP


def permutations():
    """Generate all permutations of 0,1,...,SET_N"""
    permutations = []
    order = [-1 for i in range(SET_N)]
    order_ix = 0
    while True:
        # print(f'P {order} ix={order_ix}')

        first = -1
        for i in range(SET_N):
            found = False
            for ix in range(order_ix):
                if order[ix] == i:
                    found = True
                    break
            if order[order_ix] >= i:
                found = True
            if found:
                continue
            first = i
            break

        if first == -1:
            # print('backstep')
            order[order_ix] = -1
            order_ix -= 1
            if order_ix < 0:
                break
            continue

        order[order_ix] = first
        
        order_ix += 1
        if order_ix >= SET_N:
            # print(f'P* {order}')
            permutations.append(list(order))
            order_ix -= 1
            
    f = 1
    for i in range(SET_N):
            f *= i + 1
    assert len(permutations) == f
    return permutations


def set_apply_permutation(s, permutation):
    """Apply a permutation to a set"""
    out = set([permutation[e] for e in s])
    assert len(out) == len(s)
    return out


def set_bitmap_apply_permutation(s, permutation):
    """Apply a permutation to a set as a bitmap"""
    return set_to_bitmap(set_apply_permutation(set_bitmap_to_set(s), permutation))


def set_index_apply_permutation(i, permutation):
    """Apply a permutation to a set as an index"""
    smap = set_create_index_map()
    rmap = set_create_rev_map()
    return smap[set_bitmap_apply_permutation(rmap[i], permutation)]


def expression_apply_permutation(vec, permutation):
    """Apply a base set permutation to a vector indexed by subsets"""
    out = [None for i in vec]
    for ix, v in enumerate(vec):
        new_ix = set_index_apply_permutation(ix, permutation)
        if out[new_ix] is not None:
            raise ValueError("index clash")
        out[new_ix] = v
    return out


def same_expression(vec1, vec2):
    """Returns whether two vectors are the same"""
    if len(vec1) != len(vec2):
        raise ValueError("length error")
    for i, v1 in enumerate(vec1):
        if vec2[i] != v1:
            return False
    return True


def expression2str(vec):
    """String representation of a vector"""
    return ",".join([str(i) for i in vec])


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


def get_axiom_groups(axioms):
    """Try to rotate each axiom by each permutation and record which other axiom we get. Return a list of sets of equivalent axioms by index"""
    groups = {i: set() for i in range(len(axioms))} # Maps an axiom (by index) to what other axioms we get by rotating
    rev_axiom_map = {expression2str(axiom): i for i, axiom in enumerate(axioms)} # Map axiom to axiom index
    
    for permutation in permutations():
        # print(f'P={permutation}')
        for ix, axiom in enumerate(axioms):
            # print(f'#{ix} {display_expression(axiom)}')
            new_axiom = expression_apply_permutation(vec=axiom, permutation=permutation)
            # print(f'N {display_expression(new_axiom)}')
            new_ix = rev_axiom_map[expression2str(new_axiom)]
            groups[ix].add(new_ix)

    # Cross-check the groups
    for ix, group in groups.items():
        assert ix in group
        for gix in group:
            assert ix in groups[gix]
    
    # Get unique groups
    unique_groups = []
    seen = set()
    for ix, group in groups.items():
        if ix in seen:
            continue
        unique_groups.append(group)
        seen.add(ix)
        for gix in group:
            seen.add(gix)
    
    return unique_groups


def axiom_to_group(axioms, unique_groups):
    """Construct a map from axiom index to group index"""
    map = {}
    for axiom_ix in range(len(axioms)):
        for group_ix, group in enumerate(unique_groups):
            if axiom_ix in group:
                map[axiom_ix] = group_ix
    return map


def print_c_code():
    """Print C code defining the axioms and other constants, and dump json data"""
    
    axioms = get_axioms()
    if MAKE_GROUPS:
        groups = get_axiom_groups(axioms)
        group_map = axiom_to_group(axioms, groups)
    else:
        group_map = {i: '?' for i in range(len(axioms))}
    
    print(f'#define LABEL "SET_N={SET_N} TIGHT={TIGHT}"')
    print(f'#define SET_N {SET_N}')
    print(f'#define AXIOMS {len(axioms)}')
    print(f'#define VARS {len(axioms[0])}')
    out = []
    for i, e in enumerate(axioms):
        out.append('{'+','.join([str(x) for x in e]) + '} /* ' + f'Axiom{i} Group{group_map[i]} ' + display_expression(e) + '*/')
    print('T_ELEM axioms[AXIOMS][VARS] = {')
    print(',\n'.join(out))
    print('};')
    
    # Also store the readable axioms
    out = []
    for i, e in enumerate(axioms):
        out.append(f'"Axiom{i} Group{group_map[i]} {display_expression(e)}"')
    print('char* human_readable_axioms[AXIOMS] = {')
    print(',\n'.join(out))
    print('};')
    
    # And which variable is which set
    print('/* VARIABLES / SETS');
    rev_map = set_create_rev_map()
    for v in range(len(axioms[0])):
        print(f" Variable{v} is for set {set_bitmap_to_set(rev_map[v])}")
    print('*/')
    
    # Display information about axiom groups
    if MAKE_GROUPS:
        print('/* AXIOM GROUPS');
        for i, group in enumerate(groups):
            print(f" Group{i} size={len(group)} {group}")
        print('*/')
        

def display_vector(v) -> str:
    """Create human-readable string from a vector of coefficients"""
    rev_map = set_create_rev_map()
    r = []
    for e, v in enumerate(v):
        if abs(v) < EPSILON: continue  # do not list 0s
        s = rev_map[e]
        f = f"f({set_bitmap_to_set(s)})={v}"
        r.append(f)
    return ", ".join(r)


def display_expression(exp) -> str:
    """Create human-readable string from an expression"""
    rev_map = set_create_rev_map()
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


if __name__ == "__main__":
    print_c_code()
