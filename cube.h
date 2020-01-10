#ifndef CCUBE_CUBE_H
#define CCUBE_CUBE_H

#include "maths.h"

#define CUBE_STR_SIZE 97


// Axes
extern Point X_AXIS;
extern Point Y_AXIS;
extern Point Z_AXIS;

// Piece types
extern int FACE;
extern int EDGE;
extern int CORNER;

// Faces
extern Point RIGHT;
extern Point LEFT;
extern Point UP;
extern Point DOWN;
extern Point FRONT;
extern Point BACK;

// Faces
extern Point RIGHT;
extern Point LEFT;
extern Point UP;
extern Point DOWN;
extern Point FRONT;
extern Point BACK;

// Edges
extern Point RIGHT_UP;
extern Point RIGHT_DOWN;
extern Point RIGHT_FRONT;
extern Point RIGHT_BACK;
extern Point LEFT_UP;
extern Point LEFT_DOWN;
extern Point LEFT_FRONT;
extern Point LEFT_BACK;
extern Point UP_FRONT;
extern Point UP_BACK;
extern Point DOWN_FRONT;
extern Point DOWN_BACK;

// Corners
extern Point RIGHT_UP_FRONT;
extern Point RIGHT_UP_BACK;
extern Point RIGHT_DOWN_FRONT;
extern Point RIGHT_DOWN_BACK;
extern Point LEFT_UP_FRONT;
extern Point LEFT_UP_BACK;
extern Point LEFT_DOWN_FRONT;
extern Point LEFT_DOWN_BACK;

// Rotation matrices
extern Matrix ROT_XY_CW;
extern Matrix ROT_XY_CC;
extern Matrix ROT_XZ_CW;
extern Matrix ROT_XZ_CC;
extern Matrix ROT_YZ_CW;
extern Matrix ROT_YZ_CC;

typedef struct _colors {
    char x, y, z;
} Colors;

typedef struct _piece {
    Point pos;
    Colors colors;
    int type;  // FACE, EDGE, CORNER
} Piece;

typedef struct _cube {
    Piece *faces[6];
    Piece *edges[12];
    Piece *corners[8];
} Cube;

char get_color_by_axis(Colors* c, char axis);

// (x, y, z) is the point on the cube
// (cx, cy, cz) are the colors in each direction.
// Use '\0' if the piece has no color in a direction
Piece* new_piece(int x, int y, int z, char cx, char cy, char cz);
void free_piece(Piece* p);
void print_piece(Piece* p);
void rotate_piece(Piece* p, Matrix* m);

Cube* new_cube(const char* cube_str);
void free_cube(Cube* c);
void print_cube(Cube* c);
void cube_to_string(Cube *c, char buf[CUBE_STR_SIZE]);
void get_cube_face(Cube* c, Point* face, Piece* result[9]);
void get_cube_slice(Cube* c, char plane, Piece* result[8]);
bool is_cube_solved(Cube* c);
void rotate_cube_face(Cube* c, Point* face, Matrix* m);
void rotate_slice(Cube* c, char plane, Matrix* m);
Piece* get_cube_piece(Cube* c, int x, int y, int z);
Piece* find_edge_piece(Cube *c, char c1, char c2);
Piece* find_corner_piece(Cube *c, char c1, char c2, char c3);
char left_cube_color(Cube* c);
char right_cube_color(Cube* c);
char up_cube_color(Cube* c);
char down_cube_color(Cube* c);
char front_cube_color(Cube* c);
char back_cube_color(Cube* c);
void apply_move_sequence(Cube* c, const char* moves);

// Rubik's Cube Notation: http://ruwix.com/the-rubiks-cube/notation/
typedef void (*rot_func_type)(Cube*);
void rot_L(Cube* c);
void rot_Li(Cube* c);
void rot_R(Cube* c);
void rot_Ri(Cube* c);
void rot_U(Cube* c);
void rot_Ui(Cube* c);
void rot_D(Cube* c);
void rot_Di(Cube* c);
void rot_F(Cube* c);
void rot_Fi(Cube* c);
void rot_B(Cube* c);
void rot_Bi(Cube* c);
void rot_M(Cube* c);
void rot_Mi(Cube* c);
void rot_E(Cube* c);
void rot_Ei(Cube* c);
void rot_S(Cube* c);
void rot_Si(Cube* c);
void rot_X(Cube* c);
void rot_Xi(Cube* c);
void rot_Y(Cube* c);
void rot_Yi(Cube* c);
void rot_Z(Cube* c);
void rot_Zi(Cube* c);

#endif
