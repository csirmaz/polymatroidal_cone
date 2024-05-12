import matroid as M

def test_dimensions():
    M.SET_N = 4
    M.TIGHT = False
    assert M.dimensions() == 15
    M.TIGHT = True
    assert M.dimensions() == 11

def test_set_to_bitmap():
    M.SET_N = 4
    assert M.set_to_bitmap(set()) == 0
    assert M.set_to_bitmap({0}) == 1
    assert M.set_to_bitmap({0,1}) == 3
    assert M.set_to_bitmap({0,1,2}) == 7

def test_set_to_set():
    assert M.set_bitmap_to_set(0) == set()
    assert M.set_bitmap_to_set(1) == {0}
    assert M.set_bitmap_to_set(3) == {0, 1}
    assert M.set_bitmap_to_set(7) == {0, 1, 2}

def test_set_len():
    assert M.set_len(0) == 0
    assert M.set_len(2) == 1
    assert M.set_len(256) == 1
    assert M.set_len(5) == 2

def test_set_map():
    M.SET_N = 4
    M.TIGHT = False
    map = M.set_create_index_map()
    assert 0 not in map
    assert map[M.set_to_bitmap({0})] == 0
    assert map[M.set_to_bitmap({1})] == 1
    assert map[M.set_to_bitmap({0,1})] == 2
    assert map[M.set_to_bitmap({0,1,2,3})] == 14
    M.SET_N = 4
    M.TIGHT = True
    map = M.set_create_index_map()
    assert 0 not in map
    assert map[M.set_to_bitmap({0})] == 0
    assert map[M.set_to_bitmap({1})] == 1
    assert map[M.set_to_bitmap({0,1})] == 2
    assert map[M.set_to_bitmap({2})] == 3
    assert map[M.set_to_bitmap({0,1,2})] == 6
    assert map[M.set_to_bitmap({0,1,3})] == 6
    assert map[M.set_to_bitmap({0,2,3})] == 6
    assert map[M.set_to_bitmap({1,2,3})] == 6
    assert map[M.set_to_bitmap({0,1,2,3})] == 6

def explain_modularities(tight):
    M.SET_N = 4
    M.TIGHT = tight
    smap = M.set_create_index_map()
    rev_map = {v: k for k, v in smap.items()}
    mods = M.modularities()
    for exp in mods:
        big = []
        small = []
        print(exp, end="  \t")
        for e, v in enumerate(exp):
            if v == 0: continue
            s = rev_map[e]
            f = f"f({M.set_bitmap_to_set(s)})"
            if v == 1:
                big.append(f)
            elif v == -1:
                small.append(f)
            else:
                raise ValueError("Wrong v")
        print(f"{' + '.join(big)} >= {' + '.join(small)}")


test_dimensions()
test_set_to_bitmap()
test_set_to_set()
test_set_len()
test_set_map()
# explain_modularities(False)
explain_modularities(True)
