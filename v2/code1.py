
"""
Staircases are represented as vectors of counts (minus one) of elements selected in each column
That is,
[0] is
 .  .  .
 .  .  .
 *  .  .
 
[2,0] is
 *  .  .
 *  .  .
 *  *  .

D_n is the diagonal set {(i,j): i+j<=n}
"""

def add_row(staircase):
    """Add a full row to a staircase"""
    return [x + 1 for x in (staircase + [-1])]


def add_n_null(staircase, n):
    """Add an extra element in the n+1'th column"""
    assert len(staircase) == n
    return staircase + [0]


def next_set(prev_set, n):
    """Generate S_{n+1} from prev_set == S_n"""
    new_set = set()
    for staircase in prev_set:
        if len(staircase) < n:
            new_set.add(staircase)
        else:  # len(staircase) == n
            if not pnp_reducible_with_nplus(staircase):
                new_set.add(staircase)
            new_set.add(add_row(staircase))
            s_star = add_n_null(staircase, n)
            if ..... :
                new_set.add(s_star)
    return new_set


