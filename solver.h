#ifndef CCUBE_SOLVER_H
#define CCUBE_SOLVER_H
#include <stdlib.h>
#include "cube.h"

typedef struct _solver {
    Cube* cube;
    // These are not single move strings
    const char* moves[300];
    int i_moves;
    Piece* left_piece;
    Piece* right_piece;
    Piece* up_piece;
    Piece* down_piece;
} Solver;

Solver* new_solver(Cube* c);
// This does not free the cube, since it was provided to us
void free_solver(Solver* solver);
void solve_cube(Solver* solver);
void apply_solver_moves(Solver* solver, const char* moves);
// Need to free this
char* get_moves(Solver* solver);
void print_moves(Solver* solver);
int count_moves(char* moves);

// Helper functions
void cross(Solver* solver);
void cross_left_or_right(Solver* solver, Piece* edge_piece, Piece* face_piece,
                         char face_color, const char* move1, const char* move2);
void cross_corners(Solver* solver);
void place_frd_corner(Solver* solver, Piece* corner_piece, Piece* right_piece,
                      Piece* down_piece, char front_color);
void second_layer(Solver* solver);
void place_middle_layer_ld_edge(Solver* solver, Piece* ld_piece, char left_color, char down_color);
void back_face_edges(Solver* solver);
void last_layer_corners_position(Solver* solver);
void last_layer_corners_orientation(Solver* solver);
void last_layer_edges(Solver* solver);

void last_layer_edges_state1(Solver* solver, Piece* br_edge, Piece* bl_edge, Piece* bu_edge, Piece* bd_edge,
                             const char* cycle_move, const char* h_move);
void last_layer_edges_state2(Solver* solver, Piece* br_edge, Piece* bl_edge, Piece* bu_edge, Piece* bd_edge,
                             const char* cycle_move);


#endif
