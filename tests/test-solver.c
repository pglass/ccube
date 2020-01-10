#include "../solver.h"
#include "util.h"

TEST(test_solver)
#define CASES 5
    const char* cubes[CASES] = {
        "DLURRDFFUBBLDDRBRBLDLRBFRUULFBDDUFBRBBRFUDFLUDLUULFLFR",
        "GGBYOBWBBBOYRGYOYOGWROWYWGWRBRGYBGOOGBBYOYORWWRRGRWRYW",
        "BYOYYRGOWRROWGOYWGBBGOROBWGWORBBWRWYRGYBGYWOGBROYGBWYR",
        "YWYYGWWGYBBYRRBRGWOOOYWRWRBOBYROWRGOBGRWOGWBBGBGOYYGRO",
        "ROORRYOWBWWGBYGRRBYBGGGGWWOYYBRBOWBYRWOGBYORYBOWYOGRGW"
    };
    for (int i = 0; i < CASES; i++) {
        Cube* c = new_cube(cubes[i]);
        Solver* solver = new_solver(c);
        solve_cube(solver);
        ASSERT_BOOL(is_cube_solved(c), "Cube is not solved!");
        free_solver(solver);
        free_cube(c);
    }
END_TEST()

TEST(test_get_moves)
    const char* cube_str = "DLURRDFFUBBLDDRBRBLDLRBFRUULFBDDUFBRBBRFUDFLUDLUULFLFR";
    Cube* c = new_cube(cube_str);
    Solver* solver = new_solver(c);
    solve_cube(solver);
    ASSERT_BOOL(is_cube_solved(c), "Cube is not solved!");

    char* moves = get_moves(solver);
    int n_moves = count_moves(moves);
    ASSERT_INT_EQUAL(n_moves, 222, "Incorrect number of moves");

    free_solver(solver);
    free_cube(c);

    // Use the moves to resolve the cube
    c = new_cube(cube_str);
    apply_move_sequence(c, moves);
    ASSERT_BOOL(is_cube_solved(c), "Output moves were incorrect - could not resolve cube");

    free_cube(c);
    free(moves);
END_TEST()


int main(int argc, char *argv[]) {
    int errors = 0;

    printf("============== test solver ===============\n");
    errors += test_solver();
    errors += test_get_moves();
    if (errors != 0) {
        printf("FAILED: %d maths tests failed\n", errors);
    }
}
