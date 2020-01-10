.PHONY: run run-random-cubes test

CC=clang
CFLAGS=-O2

maths.o: maths.h maths.c
	$(CC) $(CFLAGS) -c $^

cube.o: cube.h cube.c
	$(CC) $(CFLAGS) -c $^

solver.o: solver.h solver.c
	$(CC) $(CFLAGS) -c $^

main: maths.o main.c
	$(CC) $(CFLAGS) -o $@ $^

tests/util.o: tests/util.h tests/util.c
	$(CC) $(CFLAGS) -c $^
	mv util.o tests/

test-maths: maths.o tests/util.o tests/test-maths.c
	$(CC) $(CFLAGS) -o $@ $^

test-cube: maths.o cube.o tests/util.o tests/test-cube.c
	$(CC) $(CFLAGS) -o $@ $^

test-solver: maths.o cube.o solver.o tests/util.o tests/test-solver.c
	$(CC) $(CFLAGS) -o $@ $^

test: test-maths test-cube test-solver
	@./test-maths
	@./test-cube
	@./test-solver

solve-random-cubes: maths.o cube.o solver.o solve-random-cubes.c
	$(CC) $(CFLAGS) -o $@ $^

run-random-cubes: solve-random-cubes
	@./solve-random-cubes

run: main
	@./main

clean:
	rm -f *.o *.gch main
	rm -f tests/*.o test-maths test-cube test-solver
	rm -f solve-random-cubes
