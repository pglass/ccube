#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "cube.h"
#include "solver.h"

const char* SOLVED_CUBE_STR = "OOOOOOOOOYYYWWWGGGBBBYYYWWWGGGBBBYYYWWWGGGBBBRRRRRRRRR";
const char* MOVES[9] = {"L", "R", "U", "D", "F", "B", "M", "E", "S"};

Cube* random_cube() {
    Cube* c = new_cube(SOLVED_CUBE_STR);
    for (int i = 0; i < 200; ++i) {
        int k = rand() % 9;
        apply_move_sequence(c, MOVES[k]);
    }
    return c;
}

typedef struct timespec timespec;

// https://stackoverflow.com/a/6749766
long diff(timespec start, timespec end)
{
    long s = end.tv_sec - start.tv_sec;
    long ns = end.tv_nsec - start.tv_nsec;
    return 1000000000 * s + ns;
}

int main(int argc, char* argv[]) {
    srand(time(NULL));

    int successes = 0;
    long total_time_ns = 0;
    long total_moves = 0;
    while (true) {
        Cube* c = random_cube();
        Solver* solver = new_solver(c);

        timespec start, end;
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &start);
        solve_cube(solver);
        clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &end);
        long duration = diff(start, end);
        if (duration < 0) {
            printf("duration = %ld\n", duration);
        }
        total_time_ns += duration;

        if (!is_cube_solved(c)) {
            printf("ERROR: Cube was not solved");
            print_cube(c);
            printf("moves = %s\n", get_moves(solver));
            exit(1);
        }

        successes += 1;
        char* moves = get_moves(solver);
        // printf("moves(1) = "); print_moves(solver);
        // printf("moves(2) = %s\n", moves);
        int n_moves = count_moves(moves);
        total_moves += n_moves;

        if (successes == 1 || successes % 100 == 0) {
            double avg_moves = ((double) total_moves) / successes;
            double avg_time = (((double) total_time_ns) / successes) / 1000000000;
            printf("%d solves: avg_moves=%0.3lf avg_time=%0.8lfs\n", successes, avg_moves, avg_time);
        }

        free_solver(solver);
        free_cube(c);
        free(moves);
    }

    //Cube* c = random_cube();
    //print_cube(c);
    //c = random_cube();
    //print_cube(c);
}
