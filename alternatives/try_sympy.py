
from sympy import Rationals, Matrix, linsolve, EmptySet

A = Matrix([
    [3, 2],
    [1, 2]
])
b = Matrix([[1], [0]])

has_sol = linsolve((A, b))

A = Matrix([
    [0, 2],
    [0, 2]
])
b = Matrix([[1], [1]])

under_det = linsolve((A, b)) # {(tau0, 1/2)}

A = Matrix([
    [0, 2],
    [0, 3]
])
b = Matrix([[1], [1]])

no_sol = linsolve((A, b)) # EmptySet
assert isinstance(no_sol, EmptySet)
print(type(no_sol))

