#include <assert.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cube.h"
#include "solver.h"

#define DEBUG false

#define GET_ROT_FROM_FACE(face)     \
    const char* cw_cc[2];           \
    get_rot_from_face(face, cw_cc); \
    const char* cw = cw_cc[0];      \
    const char* cc = cw_cc[1];

// Places the CW, CC rotations into moves
void get_rot_from_face(Point* face, const char* moves[2]) {
    if (eq_point(face, &RIGHT)) {
        moves[0] = "R"; moves[1] = "Ri";
    } else if (eq_point(face, &LEFT)) {
        moves[0] = "L"; moves[1] = "Li";
    } else if (eq_point(face, &UP)) {
        moves[0] = "U"; moves[1] = "Ui";
    } else if (eq_point(face, &DOWN)) {
        moves[0] = "D"; moves[1] = "Di";
    } else if (eq_point(face, &FRONT)) {
        moves[0] = "F"; moves[1] = "Fi";
    } else if (eq_point(face, &BACK)) {
        moves[0] = "B"; moves[1] = "Bi";
    } else {
        printf("ERROR: Could not get rot from face: "); print_point(face);
        exit(1);
    }
}

rot_func_type get_cc_rot_from_face(Point* face) {
    if (eq_point(face, &RIGHT)) {      return rot_Ri; }
    else if (eq_point(face, &LEFT)) {  return rot_Li; }
    else if (eq_point(face, &UP)) {    return rot_Ui; }
    else if (eq_point(face, &DOWN)) {  return rot_Di; }
    else if (eq_point(face, &FRONT)) { return rot_Fi; }
    else if (eq_point(face, &BACK)) {  return rot_Bi; }
    return NULL;
}

Solver* new_solver(Cube* c) {
    Solver* solver = malloc(sizeof(Solver));
    solver->cube = c;
    memset(solver->moves, 0, 300 * sizeof(const char*));
    solver->i_moves = 0;
    solver->left_piece = get_cube_piece(c, -1, 0, 0);
    solver->right_piece = get_cube_piece(c, 1, 0, 0);
    solver->up_piece = get_cube_piece(c, 0, 1, 0);
    solver->down_piece = get_cube_piece(c, 0, -1, 0);

    // TODO: a place to store moves
    return solver;
}

void free_solver(Solver* solver) {
    if (solver == NULL) {
        return;
    }
    // note: don't free the cube
    free(solver);
}

void solve_cube(Solver* solver) {
    if (DEBUG) { print_cube(solver->cube); }
    cross(solver);
    if (DEBUG) { printf("Cross:\n"); print_cube(solver->cube); }
    cross_corners(solver);
    if (DEBUG) { printf("Corners:\n"); print_cube(solver->cube); }
    second_layer(solver);
    if (DEBUG) { printf("Second layer:\n"); print_cube(solver->cube); }
    back_face_edges(solver);
    if (DEBUG) { printf("Back face edges:\n"); print_cube(solver->cube); }
    last_layer_corners_position(solver);
    if (DEBUG) { printf("Last layer corners -- position:\n"); print_cube(solver->cube); }
    last_layer_corners_orientation(solver);
    if (DEBUG) { printf("Last layer corners -- orientation:\n"); print_cube(solver->cube); }
    last_layer_edges(solver);
    if (DEBUG) { printf("Solved:\n"); print_cube(solver->cube); }
}

void apply_solver_moves(Solver* solver, const char* moves) {
    // printf("apply_solver_moves: '%s'\n", moves);
    apply_move_sequence(solver->cube, moves);
    solver->moves[solver->i_moves++] = moves;
}

void cross(Solver* solver) {
    if (DEBUG) { printf("cross\n"); }
    Cube* cube = solver->cube;

    char front_color = front_cube_color(cube);
    Piece* fl_piece = find_edge_piece(cube, front_color, left_cube_color(cube));
    Piece* fr_piece = find_edge_piece(cube, front_color, right_cube_color(cube));
    Piece* fu_piece = find_edge_piece(cube, front_color, up_cube_color(cube));
    Piece* fd_piece = find_edge_piece(cube, front_color, down_cube_color(cube));

    cross_left_or_right(solver, fl_piece, solver->left_piece, left_cube_color(cube), "L L", "E L Ei Li");
    cross_left_or_right(solver, fr_piece, solver->right_piece, right_cube_color(cube), "R R", "Ei R E Ri");

    apply_solver_moves(solver, "Z");
    cross_left_or_right(solver, fd_piece, solver->down_piece, left_cube_color(cube), "L L", "E L Ei Li");
    cross_left_or_right(solver, fu_piece, solver->up_piece, right_cube_color(cube), "R R", "Ei R E Ri");
    apply_solver_moves(solver, "Zi");
}

void cross_left_or_right(Solver* solver, Piece* edge_piece, Piece* face_piece,
                         char face_color, const char* move1, const char* move2) {
    // do nothing if piece is in the correct place
    if (edge_piece->pos.x == face_piece->pos.x
            && edge_piece->pos.y == face_piece->pos.y
            && edge_piece->pos.z == 1
            && edge_piece->colors.y == front_cube_color(solver->cube)) {
        return;
    }

    // ensure piece is at z = -1
    const char* undo_move[2] = {NULL, NULL};
    if (edge_piece->pos.z == 0) {
        Point pos = {0, edge_piece->pos.y, edge_piece->pos.z};

        GET_ROT_FROM_FACE(&pos);

        if (eq_point(&edge_piece->pos, &LEFT_UP)
                || eq_point(&edge_piece->pos, &RIGHT_DOWN)) {
            apply_solver_moves(solver, cw);
            undo_move[0] = cc;
        } else {
            apply_solver_moves(solver, cc);
            undo_move[0] = cw;
        }
    } else if (edge_piece->pos.z == 1) {
        Point pos = COPY_POINT(edge_piece->pos);
        pos.z = 0;
        GET_ROT_FROM_FACE(&pos);

        apply_solver_moves(solver, cc);
        apply_solver_moves(solver, cc);
        // don't set the undo move if the piece starts out in the proper position
        if (edge_piece->pos.x != face_piece->pos.x) {
            undo_move[0] = cw;
            undo_move[1] = cw;
        }
    }
    assert(edge_piece->pos.z == -1);

    // piece is at z = -1, rotate to correct face (LEFT or RIGHT)
    while (edge_piece->pos.x != face_piece->pos.x
            || edge_piece->pos.y != face_piece->pos.y) {
        apply_solver_moves(solver, "B");
    }

    // if we moved a correctly-placed piece, restore it
    // these moves are possibly empty.
    if (undo_move[0] != 0) {
        apply_solver_moves(solver, undo_move[0]);
    }
    if (undo_move[1] != 0) {
        apply_solver_moves(solver, undo_move[1]);
    }

    // the piece is on the correct face on plane z = -1, but has two orientations
    if (edge_piece->colors.x == face_color) {
        apply_solver_moves(solver, move1);
    } else {
        apply_solver_moves(solver, move2);
    }
}

void cross_corners(Solver* solver) {
    if (DEBUG) { printf("cross_corners\n"); print_cube(solver->cube); }
    Cube* cube = solver->cube;

    char front_color = front_cube_color(cube);
    char down_color = down_cube_color(cube);
    char right_color = right_cube_color(cube);
    char left_color = left_cube_color(cube);
    char up_color = up_cube_color(cube);
    Piece* fld_piece = find_corner_piece(cube, front_color, left_color, down_color);
    Piece* flu_piece = find_corner_piece(cube, front_color, left_color, up_color);
    Piece* frd_piece = find_corner_piece(cube, front_color, right_color, down_color);
    Piece* fru_piece = find_corner_piece(cube, front_color, right_color, up_color);

    place_frd_corner(solver, frd_piece, solver->right_piece, solver->down_piece, front_cube_color(cube));
    apply_solver_moves(solver, "Z");
    place_frd_corner(solver, fru_piece, solver->up_piece, solver->right_piece, front_cube_color(cube));
    apply_solver_moves(solver, "Z");
    place_frd_corner(solver, flu_piece, solver->left_piece, solver->up_piece, front_cube_color(cube));
    apply_solver_moves(solver, "Z");
    place_frd_corner(solver, fld_piece, solver->down_piece, solver->left_piece, front_cube_color(cube));
    apply_solver_moves(solver, "Z");
}

void place_frd_corner(Solver* solver, Piece* corner_piece, Piece* right_piece,
                      Piece* down_piece, char front_color) {
    // rotate to z = -1
    if (corner_piece->pos.z == 1) {
        Point pos = {0, corner_piece->pos.y, 0};
        GET_ROT_FROM_FACE(&pos);

        // ensure we do only one flip so we don't move correctly
        // placed corners out of place.
        int count = 0;
        const char* undo_move = cc;

        while (corner_piece->pos.z != -1) {
            apply_solver_moves(solver, cw);
            count++;
        }

        if (count > 1) {
            // undo previous check
            for (int i = 0; i < count; i++) {
                apply_solver_moves(solver, cc);
            }

            count = 0;
            while (corner_piece->pos.z != -1) {
                apply_solver_moves(solver, cc);
                count++;
            }
            undo_move = cw;
        }
        apply_solver_moves(solver, "B");
        for (int i = 0; i < count; i++) {
            apply_solver_moves(solver, undo_move);
        }
    }

    // rotate piece to be directly below its destination
    while (corner_piece->pos.x != right_piece->pos.x
            || corner_piece->pos.y != down_piece->pos.y) {
        apply_solver_moves(solver, "B");
    }

    // there are three possible orientations for a corner
    if (corner_piece->colors.x == front_color) {
        apply_solver_moves(solver, "B D Bi Di");
    } else if (corner_piece->colors.y == front_color) {
        apply_solver_moves(solver, "Bi Ri B R");
    } else {
        apply_solver_moves(solver, "Ri B B R Bi Bi D Bi Di");
    }
}

void second_layer(Solver* solver) {
    Cube* cube = solver->cube;
    char right_color = right_cube_color(cube);
    char left_color = left_cube_color(cube);
    char up_color = up_cube_color(cube);
    char down_color = down_cube_color(cube);
    Piece* rd_piece = find_edge_piece(cube, right_color, down_color);
    Piece* ru_piece = find_edge_piece(cube, right_color, up_color);
    Piece* ld_piece = find_edge_piece(cube, left_color, down_color);
    Piece* lu_piece = find_edge_piece(cube, left_color, up_color);

    place_middle_layer_ld_edge(solver, ld_piece, left_cube_color(cube), down_cube_color(cube));
    apply_solver_moves(solver, "Z");
    place_middle_layer_ld_edge(solver, rd_piece, left_cube_color(cube), down_cube_color(cube));
    apply_solver_moves(solver, "Z");
    place_middle_layer_ld_edge(solver, ru_piece, left_cube_color(cube), down_cube_color(cube));
    apply_solver_moves(solver, "Z");
    place_middle_layer_ld_edge(solver, lu_piece, left_cube_color(cube), down_cube_color(cube));
    apply_solver_moves(solver, "Z");
}

void place_middle_layer_ld_edge(Solver* solver, Piece* ld_piece, char left_color, char down_color) {
    // move to z == -1
    if (ld_piece->pos.z == 0) {
        int count = 0;
        while (ld_piece->pos.x != -1 || ld_piece->pos.y != -1) {
            apply_solver_moves(solver, "Z");
            count++;
        }

        apply_solver_moves(solver, "B L Bi Li Bi Di B D");
        for (int i = 0; i < count; i++) {
            apply_solver_moves(solver, "Zi");
        }
    }

    assert(ld_piece->pos.z == -1);

    if (ld_piece->colors.z == left_color) {
        // left_color is on the back face, move piece to to down face
        while (ld_piece->pos.y != -1) {
            apply_solver_moves(solver, "B");
        }
        apply_solver_moves(solver, "B L Bi Li Bi Di B D");
    } else if (ld_piece->colors.z == down_color) {
        // down_color is on the back face, move to left face
        while (ld_piece->pos.x != -1) {
            apply_solver_moves(solver, "B");
        }
        apply_solver_moves(solver, "Bi Di B D B L Bi Li");
    } else {
        printf("There's a bug in place_middle_layer_ld_edge\n");
        exit(1);
    }
}

void back_face_edges(Solver* solver) {
// States:  1     2     3     4
//         -B-   -B-   ---   ---
//         BBB   BB-   BBB   -B-
//         -B-   ---   ---   ---
#define STATE1 \
    get_cube_piece(cube, 0, 1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, -1, 0, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, 0, -1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, 1, 0, 1)->colors.z == front_cube_color(cube)
#define STATE2 \
    get_cube_piece(cube, 0, 1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, -1, 0, 1)->colors.z == front_cube_color(cube)
#define STATE3 \
    get_cube_piece(cube, -1, 0, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, 1, 0, 1)->colors.z == front_cube_color(cube)
#define STATE4 \
    get_cube_piece(cube, 0, 1, 1)->colors.z != front_cube_color(cube) && \
    get_cube_piece(cube, -1, 0, 1)->colors.z != front_cube_color(cube) && \
    get_cube_piece(cube, 0, -1, 1)->colors.z != front_cube_color(cube) && \
    get_cube_piece(cube, 1, 0, 1)->colors.z != front_cube_color(cube)

    Cube* cube = solver->cube;
    // rotate BACK to FRONT
    apply_solver_moves(solver, "X X");
    while (!(STATE1)) {
        if ((STATE4) || (STATE2)) {
            apply_solver_moves(solver, "D F R Fi Ri Di");
        } else if (STATE3) {
            apply_solver_moves(solver, "D R F Ri Fi Di");
        } else {
            apply_solver_moves(solver, "F");
        }
    }
    apply_solver_moves(solver, "Xi Xi");

#undef STATE1
#undef STATE2
#undef STATE3
#undef STATE4
}


void last_layer_corners_position(Solver* solver) {
    Cube* cube = solver->cube;
    apply_solver_moves(solver, "X X");
    // UP face:
    //  4-3
    //  ---
    //  2-1
    //
    // move1 swaps corners 1 and 2
    // move2 swaps corners 1 and 3
    const char *move1 = "Li Fi L D F Di Li F L F F";
    const char *move2 = "F Li Fi L D F Di Li F L F";

    char front_color = front_cube_color(cube);
    char down_color = down_cube_color(cube);
    char right_color = right_cube_color(cube);
    char left_color = left_cube_color(cube);
    char up_color = up_cube_color(cube);

    Piece* c1 = find_corner_piece(cube, front_color, right_color, down_color);
    Piece* c2 = find_corner_piece(cube, front_color, left_color, down_color);
    Piece* c3 = find_corner_piece(cube, front_color, right_color, up_color);
    Piece* c4 = find_corner_piece(cube, front_color, left_color, up_color);

    // place corner 4
    if (eq_point(&c4->pos, &RIGHT_DOWN_FRONT)) {
        apply_solver_moves(solver, move1);
        apply_solver_moves(solver, "Zi");
        apply_solver_moves(solver, move1);
        apply_solver_moves(solver, "Z");
    } else if (eq_point(&c4->pos, &RIGHT_UP_FRONT)) {
        apply_solver_moves(solver, "Z");
        apply_solver_moves(solver, move2);
        apply_solver_moves(solver, "Zi");
    } else if (eq_point(&c4->pos, &LEFT_DOWN_FRONT)) {
        apply_solver_moves(solver, "Zi");
        apply_solver_moves(solver, move1);
        apply_solver_moves(solver, "Z");
    }
    assert(eq_point(&c4->pos, &LEFT_UP_FRONT));

    // place corner 2
    if (eq_point(&c2->pos, &RIGHT_UP_FRONT)) {
        apply_solver_moves(solver, move2);
        apply_solver_moves(solver, move1);
    } else if (eq_point(&c2->pos, &RIGHT_DOWN_FRONT)) {
        apply_solver_moves(solver, move1);
    }
    assert(eq_point(&c2->pos, &LEFT_DOWN_FRONT));

    // place corner 3 and corner 1
    if (eq_point(&c3->pos, &RIGHT_DOWN_FRONT)) {
        apply_solver_moves(solver, move2);
    }
    assert(eq_point(&c3->pos, &RIGHT_UP_FRONT));
    assert(eq_point(&c1->pos, &RIGHT_DOWN_FRONT));

    apply_solver_moves(solver, "Xi Xi");
}


void last_layer_corners_orientation(Solver* solver) {
// States:  1        2      3      4      5      6      7      8
//           B      B             B      B        B
//         BB-      -B-B   BBB    -BB    -BB   B-B-   B-B-B   BBB
//         BBB      BBB    BBB    BBB    BBB    BBB    BBB    BBB
//         -B-B     BB-    -B-    -BB    BB-B  B-B-   B-B-B   BBB
//         B          B    B B    B               B
#define STATE1 \
    get_cube_piece(cube,  1,  1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube, -1, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.x == front_cube_color(cube)
#define STATE2 \
    get_cube_piece(cube, -1,  1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1,  1, 1)->colors.x == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.y == front_cube_color(cube)
#define STATE3 \
    get_cube_piece(cube, -1, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube, -1,  1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube,  1,  1, 1)->colors.z == front_cube_color(cube)
#define STATE4 \
    get_cube_piece(cube, -1,  1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube, -1, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1,  1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.z == front_cube_color(cube)
#define STATE5 \
    get_cube_piece(cube, -1,  1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.x == front_cube_color(cube)
#define STATE6 \
    get_cube_piece(cube,  1,  1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube, -1, -1, 1)->colors.x == front_cube_color(cube) && \
    get_cube_piece(cube, -1,  1, 1)->colors.x == front_cube_color(cube)
#define STATE7 \
    get_cube_piece(cube,  1,  1, 1)->colors.x == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.x == front_cube_color(cube) && \
    get_cube_piece(cube, -1, -1, 1)->colors.x == front_cube_color(cube) && \
    get_cube_piece(cube, -1,  1, 1)->colors.x == front_cube_color(cube)
#define STATE8 \
    get_cube_piece(cube,  1,  1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube,  1, -1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, -1, -1, 1)->colors.z == front_cube_color(cube) && \
    get_cube_piece(cube, -1,  1, 1)->colors.z == front_cube_color(cube)


    Cube* cube = solver->cube;
    apply_solver_moves(solver, "X X");
    const char* move1 = "Ri Fi R Fi Ri F F R F F";
    const char* move2 = "R F Ri F R F F Ri F F";

    while (!(STATE8)) {
        if (STATE1) {
            apply_solver_moves(solver, move1);
        } else if (STATE2) {
            apply_solver_moves(solver, move2);
        } else if (STATE3) {
            apply_solver_moves(solver, move2);
            apply_solver_moves(solver, "F F");
            apply_solver_moves(solver, move1);
        } else if (STATE4) {
            apply_solver_moves(solver, move2);
            apply_solver_moves(solver, move1);
        } else if (STATE5) {
            apply_solver_moves(solver, move1);
            apply_solver_moves(solver, "F");
            apply_solver_moves(solver, move2);
        } else if (STATE6) {
            apply_solver_moves(solver, move1);
            apply_solver_moves(solver, "Fi");
            apply_solver_moves(solver, move1);
        } else if (STATE7) {
            apply_solver_moves(solver, move1);
            apply_solver_moves(solver, "F F");
            apply_solver_moves(solver, move1);
        } else {
            apply_solver_moves(solver, "F");
        }
    }


    // rotate corners into correct locations (cube is inverted, so swap up and down colors)
    Piece* bru_corner = find_corner_piece(
        cube, front_cube_color(cube), right_cube_color(cube), up_cube_color(cube)
    );
    while (!eq_point(&bru_corner->pos, &RIGHT_UP_FRONT)) {
        apply_solver_moves(solver, "F");
    }
    apply_solver_moves(solver, "Xi Xi");

#undef STATE1
#undef STATE2
#undef STATE3
#undef STATE4
#undef STATE5
#undef STATE6
#undef STATE7
#undef STATE8
}


void last_layer_edges(Solver* solver) {
// States:
//       1              2
//      ---            ---
//      ---            ---
//      -B-            -a-
//  --- B-B ---    aaa BBB ---
//  --B -B- B--    aaB -B- B--
//  --- B-B ---    aaa B-B ---
//      -B-            -B-
//      ---            ---
//      ---            ---
//              (aB edge on any FRONT)
#define STATE1 \
    bu_edge->colors.z != front_cube_color(cube) && \
    bd_edge->colors.z != front_cube_color(cube) && \
    bl_edge->colors.z != front_cube_color(cube) && \
    br_edge->colors.z != front_cube_color(cube)
#define STATE2 \
    bu_edge->colors.z == front_cube_color(cube) || \
    bd_edge->colors.z == front_cube_color(cube) || \
    bl_edge->colors.z == front_cube_color(cube) || \
    br_edge->colors.z == front_cube_color(cube)
#define H_PATTERN1 \
    get_cube_piece(cube, -1,  0, 1)->colors.x != left_cube_color(cube) && \
    get_cube_piece(cube,  1,  0, 1)->colors.x != right_cube_color(cube) && \
    get_cube_piece(cube,  0, -1, 1)->colors.y == down_cube_color(cube) && \
    get_cube_piece(cube,  0,  1, 1)->colors.y == up_cube_color(cube)
#define H_PATTERN2 \
    get_cube_piece(cube, -1,  0, 1)->colors.x == left_cube_color(cube) && \
    get_cube_piece(cube,  1,  0, 1)->colors.x == right_cube_color(cube) && \
    get_cube_piece(cube,  0, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  0,  1, 1)->colors.y == front_cube_color(cube)
#define FISH_PATTERN \
    get_cube_piece(cube,  0, -1, 1)->colors.z == down_cube_color(cube) && \
    get_cube_piece(cube,  1,  0, 1)->colors.z == right_cube_color(cube) && \
    get_cube_piece(cube,  0, -1, 1)->colors.y == front_cube_color(cube) && \
    get_cube_piece(cube,  1,  0, 1)->colors.x == front_cube_color(cube)

    apply_solver_moves(solver, "X X");

    Cube* cube = solver->cube;
    Piece* br_edge = find_edge_piece(cube, front_cube_color(cube), right_cube_color(cube));
    Piece* bl_edge = find_edge_piece(cube, front_cube_color(cube), left_cube_color(cube));
    Piece* bu_edge = find_edge_piece(cube, front_cube_color(cube), up_cube_color(cube));
    Piece* bd_edge = find_edge_piece(cube, front_cube_color(cube), down_cube_color(cube));

    const char* cycle_move = "R R F D Ui R R Di U F R R";
    const char* h_pattern_move = "Ri S Ri Ri S S Ri Fi Fi R Si Si Ri Ri Si R Fi Fi";
    // fishis "Di Li " + h_pattern_move + " L D"
    const char* fish_move = "Di Li Ri S Ri Ri S S Ri Fi Fi R Si Si Ri Ri Si R Fi Fi L D";

    if (STATE1) {
        last_layer_edges_state1(solver, br_edge, bl_edge, bu_edge, bd_edge, cycle_move, h_pattern_move);
    }
    if (STATE2) {
        last_layer_edges_state2(solver, br_edge, bl_edge, bu_edge, bd_edge, cycle_move);
    }

    int count = 0;
    while (!is_cube_solved(cube)) {
        for (int i = 0; i < 4; ++i) {
            if (FISH_PATTERN) {
                apply_solver_moves(solver, fish_move);
                if (is_cube_solved(cube)) {
                    return;
                }
            } else {
                apply_solver_moves(solver, "Z");
            }
        }

        if (H_PATTERN1) {
            apply_solver_moves(solver, h_pattern_move);
        } else if (H_PATTERN2) {
            apply_solver_moves(solver, "Z");
            apply_solver_moves(solver, h_pattern_move);
            apply_solver_moves(solver, "Zi");
        } else {
            apply_solver_moves(solver, cycle_move);
        }
        count++;
        if (count >= 10) {
            printf("Stuck in loop - unsolvable cube:\n");
            print_cube(cube);
            exit(1);
        }
    }

    apply_solver_moves(solver, "Xi Xi");
#undef STATE1
#undef STATE2
#undef H_PATTERN1
#undef H_PATTERN2
#undef FISH_PATTERN
}

void last_layer_edges_state1(Solver* solver, Piece* br_edge, Piece* bl_edge, Piece* bu_edge, Piece* bd_edge,
                             const char* cycle_move, const char* h_move) {
#define CHECK_EDGE_LR \
    get_cube_piece(cube, -1, 0, 1)->colors.z == left_cube_color(cube)
    Cube* cube = solver->cube;
    int count = 0;
    while (!(CHECK_EDGE_LR)) {
        apply_solver_moves(solver, "F");
        count++;
    }
    assert(count < 4);
    apply_solver_moves(solver, h_move);
    for (int i = 0; i < count; i++) {
        apply_solver_moves(solver, "Fi");
    }
#undef CHECK_EDGE_LR
}


// Helper for last_layer_edges_state2
Piece* correct_edge(Cube* cube) {
    char front_color = front_cube_color(cube);
    Piece* p = get_cube_piece(cube, -1, 0, 1);
    if (p->colors.z == front_color && p->colors.x == left_cube_color(cube)) {
        return p;
    }

    p = get_cube_piece(cube, 1, 0, 1);
    if (p->colors.z == front_color && p->colors.x == right_cube_color(cube)) {
        return p;
    }

    p = get_cube_piece(cube, 0, 1, 1);
    if (p->colors.z == front_color && p->colors.y == up_cube_color(cube)) {
        return p;
    }

    p = get_cube_piece(cube, 0, 1, -1);
    if (p->colors.z == front_color && p->colors.y == down_cube_color(cube)) {
        return p;
    }
    return NULL;
}

void last_layer_edges_state2(Solver* solver, Piece* br_edge, Piece* bl_edge, Piece* bu_edge, Piece* bd_edge,
                             const char* cycle_move) {
    Cube* cube = solver->cube;
    int count = 0;
    Piece* edge;
    while (true) {
        edge = correct_edge(cube);
        if (edge != NULL) {
            break;
        }
        apply_solver_moves(solver, cycle_move);
        count++;
        if (count % 3 == 0) {
            apply_solver_moves(solver, "Z");
        }
        if (count >= 12) {
            printf("ERROR: stuck in loop - unsolvable cube\n");
            print_cube(cube);
            exit(1);
        }
    }

    while (!eq_point(&edge->pos, &LEFT_FRONT)) {
        apply_solver_moves(solver, "Z");
    }

    assert(get_cube_piece(cube, -1, 0, 1)->colors.z == front_cube_color(cube));
    assert(get_cube_piece(cube, -1, 0, 1)->colors.x == left_cube_color(cube));
}

char* get_moves(Solver* solver) {
    int n = 0;
    for (int i = 0; i < solver->i_moves; i++) {
        n += strlen(solver->moves[i]) + 1;
    }

    char* result = malloc((n+1) * sizeof(char));
    int k = 0;
    for (int i = 0; i < solver->i_moves; i++) {
        strcpy(result + k, solver->moves[i]);
        k += strlen(solver->moves[i]);
        result[k++] = ' ';
    }
    result[k-1] = 0;
    assert(k == n);
    return result;
}


void print_moves(Solver* solver) {
    for (int i = 0; i < solver->i_moves; i++) {
        printf("%s ", solver->moves[i]);
    }
    printf("\n");
}

int count_moves(char* moves) {
    int n = 0;
    for (int i = 0;;i++) {
        char c = moves[i];
        if (c == 0) {
            return n;
        } else if (isspace(c)) {
            continue;
        }

        if ('A' <= c && c <= 'Z') {
            n++;
            if (moves[i+1] == 'i') {
                i++;
            }
        } else {
            printf("ERROR: Invalid character in move sequence: %d\n", c);
            exit(1);
        }
    }
    return n;
}
