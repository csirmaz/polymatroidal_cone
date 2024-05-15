import get_axioms as M

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

def explain_axioms(tight):
    M.SET_N = 4
    M.TIGHT = tight
    mods = M.get_axioms()
    for exp in mods:
        print(M.display_expression(exp))


test_dimensions()
test_set_to_bitmap()
test_set_to_set()
test_set_len()
test_set_map()
# explain_axioms(False)
# explain_axioms(True)

print("get_axioms tests OK")