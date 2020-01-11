Overview
--------

This is a Rubik's cube solver, ported from my Python version at https://github.com/pglass/cube.

It follows a fixed, layer-by-layer algorithm which does not optimize for moves or speed.

WARNING: This manages memory in an ad-hoc way. You should assume it contains issues/vulnerabilities like buffer overflows, memory leaks, and so on.


Benchmark
---------

I've added a "solve random cubes" script here and in the Python version. The
script repeatedly creates an unsolved cube and times how long it takes to solve.

Below is a rough comparison of the Python version (with CPython 3.6 and PyPy3) 
and this C version, on a 2019 MacBook Air with a 1.6 GHz Intel Core i5 cpu.

|            | Avg Time per solve | Memory usage  |
| ---------- | ------------------ | ------------- |
| Python 3.6 | 0.070s             | 6 MB          |
| PyPy 3     | 0.013s             | 47 MB         |
| C          | 0.0001s            | 340KB         |


### Python 3.6

Below shows an average time of about 0.070s per solve.

```
$ python solve_random_cubes.py
1: 1 successes (100.000% passing) avg_moves=257.000 avg_opt_moves=211.000 avg_time=0.061s
100: 100 successes (100.000% passing) avg_moves=254.460 avg_opt_moves=197.460 avg_time=0.065s
200: 200 successes (100.000% passing) avg_moves=252.160 avg_opt_moves=194.480 avg_time=0.066s
300: 300 successes (100.000% passing) avg_moves=253.097 avg_opt_moves=194.217 avg_time=0.067s
400: 400 successes (100.000% passing) avg_moves=251.775 avg_opt_moves=192.545 avg_time=0.068s
500: 500 successes (100.000% passing) avg_moves=252.200 avg_opt_moves=193.076 avg_time=0.070s
600: 600 successes (100.000% passing) avg_moves=251.532 avg_opt_moves=192.598 avg_time=0.070s
700: 700 successes (100.000% passing) avg_moves=251.733 avg_opt_moves=192.476 avg_time=0.071s
800: 800 successes (100.000% passing) avg_moves=251.075 avg_opt_moves=191.935 avg_time=0.071s
900: 900 successes (100.000% passing) avg_moves=252.142 avg_opt_moves=192.929 avg_time=0.072s
1000: 1000 successes (100.000% passing) avg_moves=252.091 avg_opt_moves=192.785 avg_time=0.073s
```

### PyPy3

PyPy is much faster, about 0.013s per solve.


```
$ python solve_random_cubes.py pypy3 solve_random_cubes.py
1: 1 successes (100.000% passing) avg_moves=248.000 avg_opt_moves=196.000 avg_time=0.068s
100: 100 successes (100.000% passing) avg_moves=255.470 avg_opt_moves=195.990 avg_time=0.015s
200: 200 successes (100.000% passing) avg_moves=252.185 avg_opt_moves=192.345 avg_time=0.014s
300: 300 successes (100.000% passing) avg_moves=251.990 avg_opt_moves=192.583 avg_time=0.013s
400: 400 successes (100.000% passing) avg_moves=252.468 avg_opt_moves=192.698 avg_time=0.013s
500: 500 successes (100.000% passing) avg_moves=251.246 avg_opt_moves=191.346 avg_time=0.013s
600: 600 successes (100.000% passing) avg_moves=251.623 avg_opt_moves=191.583 avg_time=0.013s
700: 700 successes (100.000% passing) avg_moves=251.623 avg_opt_moves=191.534 avg_time=0.013s
800: 800 successes (100.000% passing) avg_moves=251.898 avg_opt_moves=191.945 avg_time=0.012s
900: 900 successes (100.000% passing) avg_moves=252.126 avg_opt_moves=192.108 avg_time=0.013s
1000: 1000 successes (100.000% passing) avg_moves=252.416 avg_opt_moves=192.372 avg_time=0.013s
```

### C

Below shows the C version, about 0.0001s per solve.

```
$ make run-random-cubes
clang -O2 -c maths.h maths.c
clang -O2 -c cube.h cube.c
clang -O2 -c solver.h solver.c
clang -O2 -o solve-random-cubes maths.o cube.o solver.o solve-random-cubes.c
1 solves: avg_moves=239.000 avg_time=0.00011500s
100 solves: avg_moves=259.980 avg_time=0.00010970s
200 solves: avg_moves=258.450 avg_time=0.00010410s
300 solves: avg_moves=258.687 avg_time=0.00010110s
400 solves: avg_moves=260.170 avg_time=0.00009954s
500 solves: avg_moves=260.740 avg_time=0.00009863s
600 solves: avg_moves=260.805 avg_time=0.00009726s
700 solves: avg_moves=261.526 avg_time=0.00009644s
800 solves: avg_moves=261.494 avg_time=0.00009572s
900 solves: avg_moves=261.513 avg_time=0.00009520s
1000 solves: avg_moves=261.618 avg_time=0.00009479s
```
