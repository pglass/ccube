#include <stdio.h>
#include "../cube.h"
#include "util.h"

const char* DEBUG_CUBE_STR =
    "    012\n"
    "    345\n"
    "    678\n"
    "9ab cde fgh ijk\n"
    "lmn opq rst uvw\n"
    "xyz ABC DEF GHI\n"
    "    JKL\n"
    "    MNO\n"
    "    PQR\n";

const char* SOLVED_CUBE_STR =
    "    UUU\n"
    "    UUU\n"
    "    UUU\n"
    "LLL FFF RRR BBB\n"
    "LLL FFF RRR BBB\n"
    "LLL FFF RRR BBB\n"
    "    DDD\n"
    "    DDD\n"
    "    DDD\n";


TEST(test_new_corner_piece)
    Piece* p = new_piece(1, -1, -1, 'a', 'b', 'c');
    ASSERT_POINT(&p->pos, 1, -1, -1);
    ASSERT_COLORS(p->colors, 'a', 'b', 'c');
    ASSERT_INT_EQUAL(p->type, CORNER, "Piece type is wrong");
    free_piece(p);
END_TEST()


TEST(test_new_edge_piece)
    Piece* p = new_piece(1, 0, -1, 'a', 0, 'c');
    ASSERT_POINT(&p->pos, 1, 0, -1);
    ASSERT_COLORS(p->colors, 'a', 0, 'c');
    ASSERT_INT_EQUAL(p->type, EDGE, "Piece type is wrong");
    free_piece(p);
END_TEST()


TEST(test_new_face_piece)
    Piece* p = new_piece(0, 0, 1, 0, 0, 'c');
    ASSERT_POINT(&p->pos, 0, 0, 1);
    ASSERT_COLORS(p->colors, 0, 0, 'c');
    ASSERT_INT_EQUAL(p->type, FACE, "Piece type is wrong");
    free_piece(p);
END_TEST()


TEST(test_rotate_piece_xy_cc)
    // Piece points along the +x axis
    Piece *p = new_piece(1, 0, 0, 'a', 0, 0);
    // Rotate 90 degrees counter-clockwise in the XY plane
    rotate_piece(p, &ROT_XY_CC);
    // Ends up pointing vertically in the +y axis
    ASSERT_POINT(&p->pos, 0, 1, 0);
    ASSERT_COLORS(p->colors, 0, 'a', 0);
    free_piece(p);
END_TEST()

TEST(test_rotate_piece_xz_cw)
    // Piece points along y = x
    Piece *p = new_piece(1, 1, 0, 'a', 'b', 0);
    // Rotate 90 degrees counter-clockwise in the XY plane
    rotate_piece(p, &ROT_XZ_CW);
    // Ends up pointing along y = z
    ASSERT_POINT(&p->pos, 0, 1, 1);
    ASSERT_COLORS(p->colors, 0, 'b', 'a');
    free_piece(p);
END_TEST()


TEST(test_new_cube)
    //     012
    //     345
    //     678
    // 9ab cde fgh ijk
    // lmn opq rst uvw
    // xyz ABC DEF GHI
    //     JKL
    //     MNO
    //     PQR
    Cube* c = new_cube(DEBUG_CUBE_STR);
    for (int i = 0; i < 6; i++) {
        Piece* p = c->faces[i];
        ASSERT_INT_EQUAL(p->type, FACE, "Piece should be a face");
    }
    for (int i = 0; i < 12; i++) {
        Piece* p = c->edges[i];
        ASSERT_INT_EQUAL(p->type, EDGE, "Piece should be an edge");
    }
    for (int i = 0; i < 8; i++) {
        Piece* p = c->corners[i];
        ASSERT_INT_EQUAL(p->type, CORNER, "Piece should be a corner");
    }
    free_cube(c);
END_TEST()


TEST(test_get_cube_face)
    Cube* c = new_cube(DEBUG_CUBE_STR);
    // The are the sticker colors of each face, in the
    // particular order they are found by get_cube_face.
    const char *front = "pqodBeCcA";
    const char *left  = "maynlb9zx";
    const char *right = "sgErtfhDF";
    const char *up    = "453718260";
    const char *down  = "NOMKQLRJP";
    const char *back  = "vuwjHiGkI";

    Piece* face[9];

    get_cube_face(c, &FRONT, face);
    for (int i = 0; i < 9; i++) {
        ASSERT_CHAR_EQUAL(face[i]->colors.z, front[i], "Front face has wrong color");
    }
    get_cube_face(c, &BACK, face);
    for (int i = 0; i < 9; i++) {
        ASSERT_CHAR_EQUAL(face[i]->colors.z, back[i], "Back face has wrong color");
    }
    get_cube_face(c, &LEFT, face);
    for (int i = 0; i < 9; i++) {
        ASSERT_CHAR_EQUAL(face[i]->colors.x, left[i], "Left face has wrong color");
    }
    get_cube_face(c, &RIGHT, face);
    for (int i = 0; i < 9; i++) {
        ASSERT_CHAR_EQUAL(face[i]->colors.x, right[i], "Right face has wrong color");
    }
    get_cube_face(c, &UP, face);
    for (int i = 0; i < 9; i++) {
        ASSERT_CHAR_EQUAL(face[i]->colors.y, up[i], "Up face has wrong color");
    }
    get_cube_face(c, &DOWN, face);
    for (int i = 0; i < 9; i++) {
        ASSERT_CHAR_EQUAL(face[i]->colors.y, down[i], "Down face has wrong color");
    }
    free_cube(c);
END_TEST()

TEST(test_get_cube_slice)
    Cube* c = new_cube(DEBUG_CUBE_STR);

    Piece* slice[8];
    // select all pieces with x = 0
    get_cube_slice(c, 'x', slice);
    ASSERT_POINT(&slice[0]->pos, 0, 1, 0);
    ASSERT_POINT(&slice[1]->pos, 0, -1, 0);
    ASSERT_POINT(&slice[2]->pos, 0, 0, 1);
    ASSERT_POINT(&slice[3]->pos, 0, 0, -1);
    ASSERT_POINT(&slice[4]->pos, 0, 1, 1);
    ASSERT_POINT(&slice[5]->pos, 0, 1, -1);
    ASSERT_POINT(&slice[6]->pos, 0, -1, 1);
    ASSERT_POINT(&slice[7]->pos, 0, -1, -1);

    // select all pieces with y = 0
    get_cube_slice(c, 'y', slice);
    ASSERT_POINT(&slice[0]->pos, 1, 0, 0);
    ASSERT_POINT(&slice[1]->pos, -1, 0, 0);
    ASSERT_POINT(&slice[2]->pos, 0, 0, 1);
    ASSERT_POINT(&slice[3]->pos, 0, 0, -1);
    ASSERT_POINT(&slice[4]->pos, 1, 0, 1);
    ASSERT_POINT(&slice[5]->pos, 1, 0, -1);
    ASSERT_POINT(&slice[6]->pos, -1, 0, 1);
    ASSERT_POINT(&slice[7]->pos, -1, 0, -1);

    // select all pieces with z = 0
    get_cube_slice(c, 'z', slice);
    ASSERT_POINT(&slice[0]->pos, 1, 0, 0);
    ASSERT_POINT(&slice[1]->pos, -1, 0, 0);
    ASSERT_POINT(&slice[2]->pos, 0, 1, 0);
    ASSERT_POINT(&slice[3]->pos, 0, -1, 0);
    ASSERT_POINT(&slice[4]->pos, 1, 1, 0);
    ASSERT_POINT(&slice[5]->pos, 1, -1, 0);
    ASSERT_POINT(&slice[6]->pos, -1, 1, 0);
    ASSERT_POINT(&slice[7]->pos, -1, -1, 0);
    free_cube(c);
END_TEST()


TEST(test_is_cube_solved)
    Cube* c = new_cube(SOLVED_CUBE_STR);
    ASSERT_BOOL(is_cube_solved(c), "Cube should be solved");
    free_cube(c);

    c = new_cube(DEBUG_CUBE_STR);
    ASSERT_BOOL(!is_cube_solved(c), "Cube should not be solved");
    free_cube(c);
END_TEST()


TEST(test_cube_to_string)
    Cube* c = new_cube(DEBUG_CUBE_STR);
    char buf[CUBE_STR_SIZE];
    cube_to_string(c, buf);
    ASSERT_STRING_EQUAL(buf, DEBUG_CUBE_STR, "Cube string was incorrect");
    free_cube(c);
END_TEST()


TEST(test_rotate_cube_face)
    Cube* c = new_cube(DEBUG_CUBE_STR);
    rotate_cube_face(c, &FRONT, &ROT_XY_CC);
    ASSERT_CUBE(c,
        "    012\n"
        "    345\n"
        "    frD\n"
        "9a8 eqC Lgh ijk\n"
        "lm7 dpB Kst uvw\n"
        "xy6 coA JEF GHI\n"
        "    bnz\n"
        "    MNO\n"
        "    PQR\n");
    free_cube(c);
END_TEST()

#define TEST_ROT(func, expected)        \
TEST(test_##func)                       \
    Cube* c = new_cube(DEBUG_CUBE_STR); \
    func(c);                            \
    ASSERT_CUBE(c, expected);           \
    free_cube(c);                       \
END_TEST()

TEST_ROT(rot_L,
         "    I12\n"
         "    w45\n"
         "    k78\n"
         "xl9 0de fgh ijP\n"
         "yma 3pq rst uvM\n"
         "znb 6BC DEF GHJ\n"
         "    cKL\n"
         "    oNO\n"
         "    AQR\n")

TEST_ROT(rot_Li,
         "    c12\n"
         "    o45\n"
         "    A78\n"
         "bnz Jde fgh ij6\n"
         "amy Mpq rst uv3\n"
         "9lx PBC DEF GH0\n"
         "    IKL\n"
         "    wNO\n"
         "    kQR\n")

TEST_ROT(rot_R,
         "    01e\n"
         "    34q\n"
         "    67C\n"
         "9ab cdL Drf 8jk\n"
         "lmn opO Esg 5vw\n"
         "xyz ABR Fth 2HI\n"
         "    JKG\n"
         "    MNu\n"
         "    PQi\n")

TEST_ROT(rot_Ri,
         "    01G\n"
         "    34u\n"
         "    67i\n"
         "9ab cd2 htF Rjk\n"
         "lmn op5 gsE Ovw\n"
         "xyz AB8 frD LHI\n"
         "    JKe\n"
         "    MNq\n"
         "    PQC\n")

TEST_ROT(rot_U,
         "    630\n"
         "    741\n"
         "    852\n"
         "cde fgh ijk 9ab\n"
         "lmn opq rst uvw\n"
         "xyz ABC DEF GHI\n"
         "    JKL\n"
         "    MNO\n"
         "    PQR\n")

TEST_ROT(rot_Ui,
         "    258\n"
         "    147\n"
         "    036\n"
         "ijk 9ab cde fgh\n"
         "lmn opq rst uvw\n"
         "xyz ABC DEF GHI\n"
         "    JKL\n"
         "    MNO\n"
         "    PQR\n")

TEST_ROT(rot_D,
         "    012\n"
         "    345\n"
         "    678\n"
         "9ab cde fgh ijk\n"
         "lmn opq rst uvw\n"
         "GHI xyz ABC DEF\n"
         "    PMJ\n"
         "    QNK\n"
         "    ROL\n")

TEST_ROT(rot_Di,
         "    012\n"
         "    345\n"
         "    678\n"
         "9ab cde fgh ijk\n"
         "lmn opq rst uvw\n"
         "ABC DEF GHI xyz\n"
         "    LOR\n"
         "    KNQ\n"
         "    JMP\n")

TEST_ROT(rot_F,
         "    012\n"
         "    345\n"
         "    znb\n"
         "9aJ Aoc 6gh ijk\n"
         "lmK Bpd 7st uvw\n"
         "xyL Cqe 8EF GHI\n"
         "    Drf\n"
         "    MNO\n"
         "    PQR\n")

TEST_ROT(rot_Fi,
         "    012\n"
         "    345\n"
         "    frD\n"
         "9a8 eqC Lgh ijk\n"
         "lm7 dpB Kst uvw\n"
         "xy6 coA JEF GHI\n"
         "    bnz\n"
         "    MNO\n"
         "    PQR\n")

TEST_ROT(rot_B,
         "    htF\n"
         "    345\n"
         "    678\n"
         "2ab cde fgR Gui\n"
         "1mn opq rsQ Hvj\n"
         "0yz ABC DEP Iwk\n"
         "    JKL\n"
         "    MNO\n"
         "    9lx\n")

TEST_ROT(rot_Bi,
         "    xl9\n"
         "    345\n"
         "    678\n"
         "Pab cde fg0 kwI\n"
         "Qmn opq rs1 jvH\n"
         "Ryz ABC DE2 iuG\n"
         "    JKL\n"
         "    MNO\n"
         "    Fth\n")

TEST_ROT(rot_M,
         "    0H2\n"
         "    3v5\n"
         "    6j8\n"
         "9ab c1e fgh iQk\n"
         "lmn o4q rst uNw\n"
         "xyz A7C DEF GKI\n"
         "    JdL\n"
         "    MpO\n"
         "    PBR\n")

TEST_ROT(rot_Mi,
         "    0d2\n"
         "    3p5\n"
         "    6B8\n"
         "9ab cKe fgh i7k\n"
         "lmn oNq rst u4w\n"
         "xyz AQC DEF G1I\n"
         "    JHL\n"
         "    MvO\n"
         "    PjR\n")

TEST_ROT(rot_E,
         "    012\n"
         "    345\n"
         "    678\n"
         "9ab cde fgh ijk\n"
         "uvw lmn opq rst\n"
         "xyz ABC DEF GHI\n"
         "    JKL\n"
         "    MNO\n"
         "    PQR\n")

TEST_ROT(rot_Ei,
         "    012\n"
         "    345\n"
         "    678\n"
         "9ab cde fgh ijk\n"
         "opq rst uvw lmn\n"
         "xyz ABC DEF GHI\n"
         "    JKL\n"
         "    MNO\n"
         "    PQR\n")

TEST_ROT(rot_S,
         "    012\n"
         "    yma\n"
         "    678\n"
         "9Mb cde f3h ijk\n"
         "lNn opq r4t uvw\n"
         "xOz ABC D5F GHI\n"
         "    JKL\n"
         "    Esg\n"
         "    PQR\n")

TEST_ROT(rot_Si,
         "    012\n"
         "    gsE\n"
         "    678\n"
         "95b cde fOh ijk\n"
         "l4n opq rNt uvw\n"
         "x3z ABC DMF GHI\n"
         "    JKL\n"
         "    amy\n"
         "    PQR\n")

TEST_ROT(rot_X,
         "    cde\n"
         "    opq\n"
         "    ABC\n"
         "bnz JKL Drf 876\n"
         "amy MNO Esg 543\n"
         "9lx PQR Fth 210\n"
         "    IHG\n"
         "    wvu\n"
         "    kji\n")

TEST_ROT(rot_Xi,
         "    IHG\n"
         "    wvu\n"
         "    kji\n"
         "xl9 012 htF RQP\n"
         "yma 345 gsE ONM\n"
         "znb 678 frD LKJ\n"
         "    cde\n"
         "    opq\n"
         "    ABC\n")

TEST_ROT(rot_Y,
         "    630\n"
         "    741\n"
         "    852\n"
         "cde fgh ijk 9ab\n"
         "opq rst uvw lmn\n"
         "ABC DEF GHI xyz\n"
         "    LOR\n"
         "    KNQ\n"
         "    JMP\n")

TEST_ROT(rot_Yi,
         "    258\n"
         "    147\n"
         "    036\n"
         "ijk 9ab cde fgh\n"
         "uvw lmn opq rst\n"
         "GHI xyz ABC DEF\n"
         "    PMJ\n"
         "    QNK\n"
         "    ROL\n")

TEST_ROT(rot_Z,
         "    xl9\n"
         "    yma\n"
         "    znb\n"
         "PMJ Aoc 630 kwI\n"
         "QNK Bpd 741 jvH\n"
         "ROL Cqe 852 iuG\n"
         "    Drf\n"
         "    Esg\n"
         "    Fth\n")

TEST_ROT(rot_Zi,
         "    htF\n"
         "    gsE\n"
         "    frD\n"
         "258 eqC LOR Gui\n"
         "147 dpB KNQ Hvj\n"
         "036 coA JMP Iwk\n"
         "    bnz\n"
         "    amy\n"
         "    9lx\n")

TEST(test_apply_move_sequence)
    Cube* c = new_cube(SOLVED_CUBE_STR);
    apply_move_sequence(c, "L U M Ri X E Xi Ri D D F F Bi");
    ASSERT_CUBE(c,
         "    DLU\n"
         "    RRD\n"
         "    FFU\n"
         "BBL DDR BRB LDL\n"
         "RBF RUU LFB DDU\n"
         "FBR BBR FUD FLU\n"
         "    DLU\n"
         "    ULF\n"
         "    LFR\n");
    free_cube(c);
END_TEST()

TEST(test_get_cube_piece)
    Cube* c = new_cube(DEBUG_CUBE_STR);
    Piece* p = get_cube_piece(c, 1, 1, 1);
    ASSERT_POINT(&p->pos, 1, 1, 1);
    ASSERT_COLORS(p->colors, 'f', '8', 'e');

    p = get_cube_piece(c, 1, 1, -1);
    ASSERT_POINT(&p->pos, 1, 1, -1);
    ASSERT_COLORS(p->colors, 'h', '2', 'i');

    p = get_cube_piece(c, 1, 0, 0);
    ASSERT_POINT(&p->pos, 1, 0, 0);
    ASSERT_COLORS(p->colors, 's', 0, 0);

    p = get_cube_piece(c, 0, 0, 0);
    ASSERT_BOOL(p == NULL, "Piece should be null");
    free_cube(c);
END_TEST()

#define TEST_CUBE_COLOR(func, expected) \
TEST(test_##func)                       \
    Cube* c = new_cube(DEBUG_CUBE_STR); \
    char color = func(c);               \
    ASSERT_CHAR_EQUAL(color, expected, "Color did not match"); \
END_TEST()

TEST_CUBE_COLOR(left_cube_color, 'm')
TEST_CUBE_COLOR(right_cube_color, 's')
TEST_CUBE_COLOR(up_cube_color, '4')
TEST_CUBE_COLOR(down_cube_color, 'N')
TEST_CUBE_COLOR(front_cube_color, 'p')
TEST_CUBE_COLOR(back_cube_color, 'v')

TEST(test_find_edge_piece)
    Cube* c = new_cube(DEBUG_CUBE_STR);
    Piece* p = find_edge_piece(c, 'o', 'n');
    ASSERT_POINT(&p->pos, -1, 0, 1);
    free_cube(c);
END_TEST()

TEST(test_find_corner_piece)
    Cube* c = new_cube(DEBUG_CUBE_STR);
    Piece* p = find_corner_piece(c, '8', 'e', 'f');
    ASSERT_POINT(&p->pos, 1, 1, 1);
    p = find_corner_piece(c, 'e', 'f', '8');
    ASSERT_POINT(&p->pos, 1, 1, 1);
    free_cube(c);
END_TEST()

int main(int argc, char* argv[]) {
    printf("============== test cube ===============\n");
    int errors = 0;
    errors += test_new_corner_piece();
    errors += test_new_edge_piece();
    errors += test_new_face_piece();
    errors += test_rotate_piece_xy_cc();
    errors += test_rotate_piece_xz_cw();
    errors += test_new_cube();
    errors += test_get_cube_face();
    errors += test_get_cube_slice();
    errors += test_is_cube_solved();
    errors += test_cube_to_string();
    errors += test_rotate_cube_face();
    errors += test_rot_L();
    errors += test_rot_Li();
    errors += test_rot_R();
    errors += test_rot_Ri();
    errors += test_rot_U();
    errors += test_rot_Ui();
    errors += test_rot_D();
    errors += test_rot_Di();
    errors += test_rot_F();
    errors += test_rot_Fi();
    errors += test_rot_B();
    errors += test_rot_Bi();
    errors += test_rot_M();
    errors += test_rot_Mi();
    errors += test_rot_E();
    errors += test_rot_Ei();
    errors += test_rot_X();
    errors += test_rot_Xi();
    errors += test_rot_Y();
    errors += test_rot_Yi();
    errors += test_rot_Z();
    errors += test_rot_Zi();
    errors += test_apply_move_sequence();
    errors += test_get_cube_piece();
    errors += test_left_cube_color();
    errors += test_right_cube_color();
    errors += test_up_cube_color();
    errors += test_down_cube_color();
    errors += test_front_cube_color();
    errors += test_back_cube_color();
    errors += test_find_edge_piece();
    errors += test_find_corner_piece();

    if (errors != 0) {
        printf("FAILED: %d cube tests failed\n", errors);
    }
}
