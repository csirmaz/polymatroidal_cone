# polymatroidal_cone
Exploring polymatroidal axioms and cone

This code is to calculate the rays of the polymatroidal cone.
The rays are known for a base set of 5, and have not yet been calculated for a base set of 6.
For an introduction to polymatroids and polymatroidal axioms, see
https://www.cs.ubbcluj.ro/~studia-m/index.php/journal/article/view/1152 .

## Files

- `alternatives/` - Code related to past experiments

- `get_axioms.py` - Generates the polymatroidal axioms. Outputs C code listing the axioms, which also includes machine-readable
descriptions of axiom properties in a comment. The `AB(CDE)` label of an axiom refers to the
`f({A,C,D,E}) + f({B,C,D,E}) >= f({A,B,C,D,E}) + f({C,D,E})` submodularity axiom.
Usage: `python(3) get_axioms.py <base set size> [i]`; use `i` if the code is to add extra, "identity" axioms for each
variable to limit calculations to the positive space. In-file options include using tight polymatroids (true).
Called by `Makefile` and `generate_order`. Outputs are in `data/axioms*.c` where the index numbers of each axiom and variables can be seen.

- `strip_debug.pl` - Removes debug lines from C code. Called by `Makefile` and creates `*.strp.c` files.

- `util.c` - C utilities related to integer vectors and threads. Included by `slicer.c`.

- `ray_store.c` - C code related to managing information about rays in memory. Included by `slicer.c`.

- `slicer_solver.c` - C code to solve a collection of linear equations without a constant using integers. Included by `slicer.c`.

- `slicer.c` - Main C code, compiled by `Makefile`. For an overview of the main functions, see below.

- `get_pair_data.py` - Processes the output of `slicer.c` and show information about each step on a single line, as well
as stats on the number of rays and ray pairs in order. Usage: `python(3) get_pair_data.py (<step number>|-1) <base set size>`.
Use a step number to filter the output to that step only; useful during reverse optimization - see below.

- `conductor`, `alter_slicer_c` - Code to automate the reverse optimization: launch the slicer, process its output using `get_pair_data.py`,
select winning axiom for the next position, alter `slicer.c` to add the fixed axiom, repeat. Not in working order.

- `choose_axioms/` - The outputs from the optimization.

    - `by_rays/` - Optimize for the number of rays at each step, n=5. Individual files show results for each position for each `vary_axiom` (see below). `full_run` shows stats for a full optimized run, including the order of the axioms.
    
    - `by_ray_pairs/` - As above but optimize for the number of ray pairs (n=5).
    
    - `by_rays_id/` - Unfinished optimization (n=5) using "identity" axioms so no real axiom would need to be skipped due to the rank of the remaining ones.
    
    - `n5_generated_order` - Stats from a full optimized run using a simplified generated order by `generate_order`, including the order itself.
    
    - `n5_manual_order_1` - Stats from a full run using a simplified manual order, including the order itself.

- `data/` - Contains generated C code describing the axioms (see `get_axioms.py`) and the order in which the axioms
should be applied, either from the reverse optimization or `generate_order`.

- `generate_order` - Code that generates a simplified order of axioms based on the optimized order.
Launches `get_axioms.py` and reads the machine-readable descriptions of axioms from its output.
Launches `slicer.c` via `Makefile` to check for the rank of the non-fixed axioms as it builds the order.
Outputs information about the generated order on stdout, and writes it to `data/fixed_axioms_test.c`.
The size of the base set can be set in the file.

- `generate_order.output` - The output of `generate_order` for n=6.


## Reverse optimization

The order in which axioms (facets) are applied to the cone greatly influences the number of rays
that occur during intermediate steps of the calculation.
The code can optimize this order in the reverse order (last axiom first).

Given N (the base set size) and a set of already optimized (fixed) axioms
which fix the last axioms to be applied,
`make slicer_run_5` can loop through all eligible axioms to decide the best one to apply
just before the fixed ones if `DO_VARY_AXIOMS` is enabled. The is the `vary_axiom`.

For a given value of the `vary_axiom`, `slicer()` in `slicer.c`
performs a single generation of the matroidal cone,
by (1) bootstrapping the calculations by selecting V linearly independent axioms (V is the number of variables);
(2) determining the resulting V rays; (3) repeatedly adding new axioms to the cone and adjusting the list of rays.
In (3) the next axiom (unless fixed or is the `vary_axiom`) is chosen to minimize the number of ray pairs.

```
  [make slicer_run_5]
  Given the fixed axioms
  Loop through the remaining axioms (not fixed) -> vary_axiom:
    [slicer()]
    Choose V linearly independent initial axioms (not fixed, not vary_axiom)
      If none found, abort
    Calculate V rays generated by the initial axioms
    Loop for the next axiom:
      If at the end, use the vary_axiom or the fixed axioms
      Otherwise choose next axiom to minimize number of ray pairs (draft optimization)
      [apply_axiom()] Apply axiom; adjust list of rays
      Potentially stop early once non-fixed axioms are exhausted
```

One iteration of the internal loop, the application of an axiom, yields an "experience",
which carries data about ray pairs investigated, resulting number of rays, etc. given a prefix sequence 
of axioms already applied.

A "step" is a step in the internal loop; its number is the sequence number of the axiom applied (including the initial axioms).

The output of `make slicer_run_5` appears in `$MATROID_LOGS/` in `*_output` files.

These files are processed individually by `get_pair_data.py` which extracts data about each "experience"
as a single line. These can be filtered to list data for a given step only, so only data relevant for the position
of the `vary_axiom` could be seen. It also prints stats.
These outputs are collected in the `choose_axioms/` directory in this repo.
