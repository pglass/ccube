#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cube.h"

#define ITERATE_PIECES(c, code)         \
    for (int i = 0; i < 6; i++) {       \
        Piece* p = c->faces[i];         \
        code;                           \
    }                                   \
    for (int i = 0; i < 12; i++) {      \
        Piece* p = c->edges[i];         \
        code;                           \
    }                                   \
    for (int i = 0; i < 8; i++) {       \
        Piece* p = c->corners[i];       \
        code;                           \
    }


Point X_AXIS = {1, 0, 0};
Point Y_AXIS = {0, 1, 0};
Point Z_AXIS = {0, 0, 1};

int FACE = 1;
int EDGE = 2;
int CORNER = 3;

Point RIGHT = { 1,  0,  0};
Point LEFT  = {-1,  0,  0};
Point UP    = { 0,  1,  0};
Point DOWN  = { 0, -1,  0};
Point FRONT = { 0,  0,  1};
Point BACK  = { 0,  0, -1};

Point RIGHT_UP    = { 1,  1,  0};
Point RIGHT_DOWN  = { 1, -1,  0};
Point RIGHT_FRONT = { 1,  0,  1};
Point RIGHT_BACK  = { 1,  0, -1};
Point LEFT_UP     = {-1,  1,  0};
Point LEFT_DOWN   = {-1, -1,  0};
Point LEFT_FRONT  = {-1,  0,  1};
Point LEFT_BACK   = {-1,  0, -1};
Point UP_FRONT    = { 0,  1,  1};
Point UP_BACK     = { 0,  1, -1};
Point DOWN_FRONT  = { 0, -1,  1};
Point DOWN_BACK   = { 0, -1, -1};

Point RIGHT_UP_FRONT   = { 1,  1,  1};
Point RIGHT_UP_BACK    = { 1,  1, -1};
Point RIGHT_DOWN_FRONT = { 1, -1,  1};
Point RIGHT_DOWN_BACK  = { 1, -1, -1};
Point LEFT_UP_FRONT    = {-1,  1,  1};
Point LEFT_UP_BACK     = {-1,  1, -1};
Point LEFT_DOWN_FRONT  = {-1, -1,  1};
Point LEFT_DOWN_BACK   = {-1, -1, -1};

// 90 degree rotations in the XY plane.
// CW is clockwise, CC is counter-clockwise.
Matrix ROT_XY_CW = {0, 1, 0,
                    -1, 0, 0,
                    0, 0, 1};
Matrix ROT_XY_CC = {0, -1, 0,
                    1, 0, 0,
                    0, 0, 1};

// 90 degree rotations in the XZ plane
// This is around the y-axis when pointing toward you.
Matrix ROT_XZ_CW = {0, 0, -1,
                    0, 1, 0,
                    1, 0, 0};
Matrix ROT_XZ_CC = {0, 0, 1,
                    0, 1, 0,
                    -1, 0, 0};

// 90 degree rotations in the YZ plane
// This is around the x-axis when pointing toward you.
Matrix ROT_YZ_CW = {1, 0, 0,
                    0, 0, 1,
                    0, -1, 0};
Matrix ROT_YZ_CC = {1, 0, 0,
                    0, 0, -1,
                    0, 1, 0};

char get_color_by_axis(Colors* c, char axis) {
    if (axis == 'x') { return c->x; }
    else if (axis == 'y') { return c->y; }
    else if (axis == 'z') { return c->z; }
    return 0;
}

Piece* new_piece(int x, int y, int z, char cx, char cy, char cz) {
    Piece* p = malloc(sizeof(Piece));
    memset(p, 0, sizeof(Piece));
    p->pos.x = x;
    p->pos.y = y;
    p->pos.z = z;
    p->colors.x = cx;
    p->colors.y = cy;
    p->colors.z = cz;

    // Count non-null faces on the piece to determine piece type.
    // 3 faces -> CORNER, 2 faces -> EDGE, 1 face -> FACE
    p->type = 0;
    p->type += (cx != '\0');
    p->type += (cy != '\0');
    p->type += (cz != '\0');
    if (p->type == 0) {
        free(p);
        printf("ERROR: Piece must have 1, 2, or 3 colors");
        return NULL;
    }
    return p;
}



void free_piece(Piece* p) {
    if (p != NULL) {
        free(p);
    }
}

void print_piece(Piece* p) {
    printf("Piece(%d, %c%c%c, (%d, %d, %d))\n",
           p->type, p->colors.x, p->colors.y, p->colors.z,
           p->pos.x, p->pos.y, p->pos.z);
}

void rotate_piece(Piece* p, Matrix* m) {
    Point before = {p->pos.x, p->pos.y, p->pos.z};
    // "p->pos = m * p->pos"
    imul_matrix_point(m, &p->pos);

    // we need to swap the positions of two things in self.colors
    // so colors appear on the correct faces. rot gives us the axes
    // to swap between.

    // "rot = p->pos - before"
    Point rot = {p->pos.x, p->pos.y, p->pos.z};
    isub_point(&rot, &before);

    int zero_count = count_point(&rot, 0);

    if (zero_count == 3) {
        return;  // no change occurred
    } else if (zero_count == 2) {
        // "rot += m * rot"
        Point tmp = {rot.x, rot.y, rot.z};
        imul_matrix_point(m, &tmp);
        iadd_point(&rot, &tmp);
        zero_count = count_point(&rot, 0);
    }

    if (zero_count != 1) {
        printf("There is a bug in the rotate_piece method!\n");
        print_piece(p);
        exit(1);
    }
    // now swap the colors to match the rotated coordinates
    char *a, *b;
    if (rot.x != 0 && rot.y != 0) {
        a = &p->colors.x;
        b = &p->colors.y;
    } else if (rot.x != 0 && rot.z != 0) {
        a = &p->colors.x;
        b = &p->colors.z;
    } else if (rot.y != 0 && rot.z != 0) {
        a = &p->colors.y;
        b = &p->colors.z;
    } else {
        printf("There is a bug in the rotate_piece method! (2)\n");
        print_piece(p);
        exit(1);
    }
    char tmp = *a;
    *a = *b;
    *b = tmp;
}


Cube* new_cube(const char* cube_str) {
    // Ignore whitespace from the input
    char stickers[54];
    int k = 0;
    for (int i = 0; i < strlen(cube_str); ++i) {
        char c = cube_str[i];
        if (!isspace(c) && k < 54) {
            stickers[k++] = c;
        }
    }
    if (k != 54) {
        printf("ERROR: Cube requires a string of 54 sticker colors\n");
        return NULL;
    }

    Cube* c = malloc(sizeof(Cube));
    c->faces[0] = new_piece(RIGHT.x, RIGHT.y, RIGHT.z, stickers[28], 0, 0);
    c->faces[1] = new_piece(LEFT.x, LEFT.y, LEFT.z,    stickers[22], 0, 0);
    c->faces[2] = new_piece(UP.x, UP.y, UP.z,          0, stickers[4],  0);
    c->faces[3] = new_piece(DOWN.x, DOWN.y, DOWN.z,    0, stickers[49], 0);
    c->faces[4] = new_piece(FRONT.x, FRONT.y, FRONT.z, 0, 0, stickers[25]);
    c->faces[5] = new_piece(BACK.x, BACK.y, BACK.z,    0, 0, stickers[31]);

    c->edges[0] = new_piece(RIGHT_UP.x, RIGHT_UP.y, RIGHT_UP.z,
                            stickers[16], stickers[5], 0);
    c->edges[1] = new_piece(RIGHT_DOWN.x, RIGHT_DOWN.y, RIGHT_DOWN.z,
                            stickers[40], stickers[50], 0);
    c->edges[2] = new_piece(RIGHT_FRONT.x, RIGHT_FRONT.y, RIGHT_FRONT.z,
                            stickers[27], 0, stickers[26]);
    c->edges[3] = new_piece(RIGHT_BACK.x, RIGHT_BACK.y, RIGHT_BACK.z,
                            stickers[29], 0, stickers[30]);
    c->edges[4] = new_piece(LEFT_UP.x, LEFT_UP.y, LEFT_UP.z,
                            stickers[10], stickers[3], 0);
    c->edges[5] = new_piece(LEFT_DOWN.x, LEFT_DOWN.y, LEFT_DOWN.z,
                            stickers[34], stickers[48], 0);
    c->edges[6] = new_piece(LEFT_FRONT.x, LEFT_FRONT.y, LEFT_FRONT.z,
                            stickers[23], 0, stickers[24]);
    c->edges[7] = new_piece(LEFT_BACK.x, LEFT_BACK.y, LEFT_BACK.z,
                            stickers[21], 0, stickers[32]);
    c->edges[8] = new_piece(UP_FRONT.x, UP_FRONT.y, UP_FRONT.z,
                            0, stickers[7], stickers[13]);
    c->edges[9] = new_piece(UP_BACK.x, UP_BACK.y, UP_BACK.z,
                            0, stickers[1], stickers[19]);
    c->edges[10] = new_piece(DOWN_FRONT.x, DOWN_FRONT.y, DOWN_FRONT.z,
                            0, stickers[46], stickers[37]);
    c->edges[11] = new_piece(DOWN_BACK.x, DOWN_BACK.y, DOWN_BACK.z,
                            0, stickers[52], stickers[43]);

    c->corners[0] = new_piece(RIGHT_UP_FRONT.x, RIGHT_UP_FRONT.y, RIGHT_UP_FRONT.z,
                              stickers[15], stickers[8], stickers[14]);
    c->corners[1] = new_piece(RIGHT_UP_BACK.x, RIGHT_UP_BACK.y, RIGHT_UP_BACK.z,
                              stickers[17], stickers[2], stickers[18]);
    c->corners[2] = new_piece(RIGHT_DOWN_FRONT.x, RIGHT_DOWN_FRONT.y, RIGHT_DOWN_FRONT.z,
                              stickers[39], stickers[47], stickers[38]);
    c->corners[3] = new_piece(RIGHT_DOWN_BACK.x, RIGHT_DOWN_BACK.y, RIGHT_DOWN_BACK.z,
                              stickers[41], stickers[53], stickers[42]);
    c->corners[4] = new_piece(LEFT_UP_FRONT.x, LEFT_UP_FRONT.y, LEFT_UP_FRONT.z,
                              stickers[11], stickers[6], stickers[12]);
    c->corners[5] = new_piece(LEFT_UP_BACK.x, LEFT_UP_BACK.y, LEFT_UP_BACK.z,
                              stickers[9], stickers[0], stickers[20]);
    c->corners[6] = new_piece(LEFT_DOWN_FRONT.x, LEFT_DOWN_FRONT.y, LEFT_DOWN_FRONT.z,
                              stickers[35], stickers[45], stickers[36]);
    c->corners[7] = new_piece(LEFT_DOWN_BACK.x, LEFT_DOWN_BACK.y, LEFT_DOWN_BACK.z,
                              stickers[33], stickers[51], stickers[44]);
    return c;
}

int _compare_pieces(const void* _a, const void* _b,
                    char axis, int axis_dir,
                    char axis2, int axis2_dir) {
    Piece* a = *(Piece**) _a;
    Piece* b = *(Piece**) _b;

    int a1 = get_point_component(&a->pos, axis);
    int b1 = get_point_component(&b->pos, axis);
    if (a1 < b1) {
        return -1 * axis_dir;
    } else if (a1 > b1) {
        return 1 * axis_dir;
    }

    int a2 = get_point_component(&a->pos, axis2);
    int b2 = get_point_component(&b->pos, axis2);
    if (a2 < b2) {
        return -1 * axis2_dir;
    } else if (a2 > b2) {
        return 1 * axis2_dir;
    }
    return 0;
}

int _compare_pieces_right_face(const void* a, const void* b) {
    return _compare_pieces(a, b, 'y', -1, 'z', -1);
}

int _compare_pieces_left_face(const void* a, const void* b) {
    return _compare_pieces(a, b, 'y', -1, 'z', 1);
}

int _compare_pieces_up_face(const void* a, const void* b) {
    return _compare_pieces(a, b, 'z', 1, 'x', 1);
}

int _compare_pieces_down_face(const void* a, const void* b) {
    return _compare_pieces(a, b, 'z', -1, 'x', 1);
}

int _compare_pieces_front_face(const void* a, const void* b) {
    return _compare_pieces(a, b, 'y', -1, 'x', 1);
}

int _compare_pieces_back_face(const void* a, const void* b) {
    return _compare_pieces(a, b, 'y', -1, 'x', -1);
}

//           0  1  2
//           3  4  5
//           6  7  8
//  9 10 11 12 13 14 15 16 17 18 19 20
// 21 22 23 24 25 26 27 28 29 30 31 32
// 33 34 35 36 37 38 39 40 41 42 43 44
//          45 46 47
//          48 49 50
//          51 52 53
void cube_to_string(Cube* c, char buf[CUBE_STR_SIZE]) {
    if (CUBE_STR_SIZE != 97) {
        printf("ERROR: invalid CUBE_STR_SIZE for cube_to_string\n");
        exit(1);
    }
    memset(buf, ' ', 96);
    buf[96] = 0;
    buf[7] = buf[15] = buf[23] = buf[39] = buf[55]
           = buf[71] = buf[79] = buf[87] = buf[95] = '\n';

    Piece* face[9];

    // Up face
    get_cube_face(c, &UP, face);
    qsort(face, 9, sizeof(Piece*), _compare_pieces_up_face);
    buf[4] = face[0]->colors.y;
    buf[5] = face[1]->colors.y;
    buf[6] = face[2]->colors.y;
    buf[12] = face[3]->colors.y;
    buf[13] = face[4]->colors.y;
    buf[14] = face[5]->colors.y;
    buf[20] = face[6]->colors.y;
    buf[21] = face[7]->colors.y;
    buf[22] = face[8]->colors.y;

    // Left face
    get_cube_face(c, &LEFT, face);
    qsort(face, 9, sizeof(Piece*), _compare_pieces_left_face);
    buf[24] = face[0]->colors.x;
    buf[25] = face[1]->colors.x;
    buf[26] = face[2]->colors.x;
    buf[40] = face[3]->colors.x;
    buf[41] = face[4]->colors.x;
    buf[42] = face[5]->colors.x;
    buf[56] = face[6]->colors.x;
    buf[57] = face[7]->colors.x;
    buf[58] = face[8]->colors.x;

    // Front face
    get_cube_face(c, &FRONT, face);
    qsort(face, 9, sizeof(Piece*), _compare_pieces_front_face);
    buf[28] = face[0]->colors.z;
    buf[29] = face[1]->colors.z;
    buf[30] = face[2]->colors.z;
    buf[44] = face[3]->colors.z;
    buf[45] = face[4]->colors.z;
    buf[46] = face[5]->colors.z;
    buf[60] = face[6]->colors.z;
    buf[61] = face[7]->colors.z;
    buf[62] = face[8]->colors.z;

    // Right face
    get_cube_face(c, &RIGHT, face);
    qsort(face, 9, sizeof(Piece*), _compare_pieces_right_face);
    buf[32] = face[0]->colors.x;
    buf[33] = face[1]->colors.x;
    buf[34] = face[2]->colors.x;
    buf[48] = face[3]->colors.x;
    buf[49] = face[4]->colors.x;
    buf[50] = face[5]->colors.x;
    buf[64] = face[6]->colors.x;
    buf[65] = face[7]->colors.x;
    buf[66] = face[8]->colors.x;

    // Back face
    get_cube_face(c, &BACK, face);
    qsort(face, 9, sizeof(Piece*), _compare_pieces_back_face);
    buf[36] = face[0]->colors.z;
    buf[37] = face[1]->colors.z;
    buf[38] = face[2]->colors.z;
    buf[52] = face[3]->colors.z;
    buf[53] = face[4]->colors.z;
    buf[54] = face[5]->colors.z;
    buf[68] = face[6]->colors.z;
    buf[69] = face[7]->colors.z;
    buf[70] = face[8]->colors.z;

    // Down face
    get_cube_face(c, &DOWN, face);
    qsort(face, 9, sizeof(Piece*), _compare_pieces_down_face);
    buf[76] = face[0]->colors.y;
    buf[77] = face[1]->colors.y;
    buf[78] = face[2]->colors.y;
    buf[84] = face[3]->colors.y;
    buf[85] = face[4]->colors.y;
    buf[86] = face[5]->colors.y;
    buf[92] = face[6]->colors.y;
    buf[93] = face[7]->colors.y;
    buf[94] = face[8]->colors.y;
}


void free_cube(Cube* c) {
    if (c == NULL) {
        return;
    }
    ITERATE_PIECES(c, free_piece(p))
    free(c);
}

void print_cube(Cube* c) {
    char buf[CUBE_STR_SIZE];
    cube_to_string(c, buf);
    printf("%s", buf);
}

void get_cube_face(Cube* c, Point* face, Piece* result[9]) {
    int k = 0;
    ITERATE_PIECES(c,
        if (dot_point(&p->pos, face) > 0) {
            result[k++] = p;
        }
    )
    // make sure we got all the pieces on the face
    if (k != 9) {
        printf("There's a bug in get_cube_face!\n");
        exit(1);
    }
}

void get_cube_slice(Cube* c, char axis, Piece* result[8]) {
    // Select all pieces on the plane "axis = 0"
    int k = 0;
    ITERATE_PIECES(c,
        if (get_point_component(&p->pos, axis) == 0) {
            result[k++] = p;
        }
    )
    if (k != 8) {
        printf("There's a bug in get_cube_face!\n");
        exit(1);
    }
}



bool _is_face_solved(Piece *face[9], char axis) {
    char color = get_color_by_axis(&face[0]->colors, axis);
    for (int i = 1; i < 9; i++) {
        if (color != get_color_by_axis(&face[i]->colors, axis)) {
            return false;
        }
    }
    return true;
}

bool is_cube_solved(Cube *c) {
    Piece *face[9];
    get_cube_face(c, &FRONT, face);
    if (!_is_face_solved(face, 'z')) { return false; }
    get_cube_face(c, &BACK, face);
    if (!_is_face_solved(face, 'z')) { return false; }

    get_cube_face(c, &LEFT, face);
    if (!_is_face_solved(face, 'x')) { return false; }
    get_cube_face(c, &RIGHT, face);
    if (!_is_face_solved(face, 'x')) { return false; }

    get_cube_face(c, &UP, face);
    if (!_is_face_solved(face, 'y')) { return false; }
    get_cube_face(c, &DOWN, face);
    if (!_is_face_solved(face, 'y')) { return false; }

    return true;
}

void rotate_cube_face(Cube* c, Point* face, Matrix* m) {
    Piece* pieces[9];
    get_cube_face(c, face, pieces);
    for (int i = 0; i < 9; i++) {
        rotate_piece(pieces[i], m);
    }
}

void rotate_slice(Cube* c, char plane, Matrix* m) {
    Piece* pieces[8];
    get_cube_slice(c, plane, pieces);
    for (int i = 0; i < 8; i++) {
        rotate_piece(pieces[i], m);
    }
}

void _rotate_cube(Cube* c, Matrix* m) {
    ITERATE_PIECES(c,
        rotate_piece(p, m)
    )
}

Piece* get_cube_piece(Cube* c, int x, int y, int z) {
    ITERATE_PIECES(c,
        if (p->pos.x == x && p->pos.y == y && p->pos.z == z) {
            return p;
        }
    )
    return NULL;
}

Piece* find_edge_piece(Cube* c, char c1, char c2) {
    for (int i = 0; i < 12; i++) {
        Piece* p = c->edges[i];
        if (p->colors.x == c1
                && (p->colors.y == c2 || p->colors.z == c2)) {
            return p;
        } else if (p->colors.y == c1
                && (p->colors.x == c2 || p->colors.z == c2)) {
            return p;
        } else if (p->colors.z == c1
                && (p->colors.x == c2 || p->colors.y == c2)) {
            return p;
        }
    }
    return NULL;
}

Piece* find_corner_piece(Cube* c, char c1, char c2, char c3) {
    for (int i = 0; i < 8; i++) {
        Piece* p = c->corners[i];
        if ((p->colors.x == c1 && p->colors.y == c2 && p->colors.z == c3)
            || (p->colors.x == c1 && p->colors.y == c3 && p->colors.z == c2)
            || (p->colors.x == c2 && p->colors.y == c1 && p->colors.z == c3)
            || (p->colors.x == c2 && p->colors.y == c3 && p->colors.z == c1)
            || (p->colors.x == c3 && p->colors.y == c1 && p->colors.z == c2)
            || (p->colors.x == c3 && p->colors.y == c2 && p->colors.z == c1)) {
            return p;
        }
    }
    return NULL;
}

char left_cube_color(Cube* c) { return get_cube_piece(c, -1, 0, 0)->colors.x; }
char right_cube_color(Cube* c) { return get_cube_piece(c, 1, 0, 0)->colors.x; }
char up_cube_color(Cube* c) { return get_cube_piece(c, 0, 1, 0)->colors.y; }
char down_cube_color(Cube* c) { return get_cube_piece(c, 0, -1, 0)->colors.y; }
char front_cube_color(Cube* c) { return get_cube_piece(c, 0, 0, 1)->colors.z; }
char back_cube_color(Cube* c) { return get_cube_piece(c, 0, 0, -1)->colors.z; }

void apply_move(Cube* c, char move[2]) {
    if (move[1] != 0 && move[1] != 'i') {
        printf("ERROR: Invalid move %c%c\n", move[0], move[1]);
        exit(1);
    }

    rot_func_type func;
    bool inverse = move[1] == 'i';
    switch(move[0]) {
        case 'L': func = inverse ? rot_Li : rot_L; break;
        case 'R': func = inverse ? rot_Ri : rot_R; break;
        case 'U': func = inverse ? rot_Ui : rot_U; break;
        case 'D': func = inverse ? rot_Di : rot_D; break;
        case 'F': func = inverse ? rot_Fi : rot_F; break;
        case 'B': func = inverse ? rot_Bi : rot_B; break;
        case 'M': func = inverse ? rot_Mi : rot_M; break;
        case 'E': func = inverse ? rot_Ei : rot_E; break;
        case 'S': func = inverse ? rot_Si : rot_S; break;
        case 'X': func = inverse ? rot_Xi : rot_X; break;
        case 'Y': func = inverse ? rot_Yi : rot_Y; break;
        case 'Z': func = inverse ? rot_Zi : rot_Z; break;
        default:
            printf("ERROR: Invalid move %c%c\n", move[0], move[1]);
            exit(1);
            break;
    }
    func(c);
}

void apply_move_sequence(Cube* cube, const char* moves) {
    // printf("apply_move_sequence: %s\n", moves);
    int n = strlen(moves);
    for (int i = 0; i < n; i++) {
        char c = moves[i];
        if (c == 0) {
            return;
        } else if (isspace(c)) {
            continue;
        }

        char c2 = i+1 < n ? moves[i+1] : ' ';
        if ('A' <= c && c <= 'Z') {
            if (c2 == 'i') {
                char move[2] = {c, c2};
                apply_move(cube, move);
                i++;
            } else {
                char move[2] = {c, 0};
                apply_move(cube, move);
            }
        } else {
            printf("ERROR: Invalid character in move sequence: %c\n", c);
            exit(1);
        }
    }
}

void rot_L(Cube* c)  { rotate_cube_face(c, &LEFT, &ROT_YZ_CC); }
void rot_Li(Cube* c) { rotate_cube_face(c, &LEFT, &ROT_YZ_CW); }
void rot_R(Cube* c)  { rotate_cube_face(c, &RIGHT, &ROT_YZ_CW); }
void rot_Ri(Cube* c) { rotate_cube_face(c, &RIGHT, &ROT_YZ_CC); }
void rot_U(Cube* c)  { rotate_cube_face(c, &UP, &ROT_XZ_CW); }
void rot_Ui(Cube* c) { rotate_cube_face(c, &UP, &ROT_XZ_CC); }
void rot_D(Cube* c)  { rotate_cube_face(c, &DOWN, &ROT_XZ_CC); }
void rot_Di(Cube* c) { rotate_cube_face(c, &DOWN, &ROT_XZ_CW); }
void rot_F(Cube* c)  { rotate_cube_face(c, &FRONT, &ROT_XY_CW); }
void rot_Fi(Cube* c) { rotate_cube_face(c, &FRONT, &ROT_XY_CC); }
void rot_B(Cube* c)  { rotate_cube_face(c, &BACK, &ROT_XY_CC); }
void rot_Bi(Cube* c) { rotate_cube_face(c, &BACK, &ROT_XY_CW); }
void rot_M(Cube* c)  { rotate_slice(c, 'x', &ROT_YZ_CC); }
void rot_Mi(Cube* c) { rotate_slice(c, 'x', &ROT_YZ_CW); }
void rot_E(Cube* c)  { rotate_slice(c, 'y', &ROT_XZ_CC); }
void rot_Ei(Cube* c) { rotate_slice(c, 'y', &ROT_XZ_CW); }
void rot_S(Cube* c)  { rotate_slice(c, 'z', &ROT_XY_CW); }
void rot_Si(Cube* c) { rotate_slice(c, 'z', &ROT_XY_CC); }
void rot_X(Cube* c)  { _rotate_cube(c, &ROT_YZ_CW); }
void rot_Xi(Cube* c) { _rotate_cube(c, &ROT_YZ_CC); }
void rot_Y(Cube* c)  { _rotate_cube(c, &ROT_XZ_CW); }
void rot_Yi(Cube* c) { _rotate_cube(c, &ROT_XZ_CC); }
void rot_Z(Cube* c)  { _rotate_cube(c, &ROT_XY_CW); }
void rot_Zi(Cube* c) { _rotate_cube(c, &ROT_XY_CC); }
