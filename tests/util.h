#ifndef CCUBE_TESTS_UTIL_H
#define CCUBE_TESTS_UTIL_H
#include <stdio.h>
#include <stdbool.h>

#define TEST(name) int name() { \
    printf(#name" ... ");         \
    int errors = 0;

#define END_TEST() \
    if (errors == 0) {  \
        printf("ok\n"); \
        return 0;       \
    }                   \
    printf("\n");       \
    return 1;           \
}

#define ASSERT(stmt) errors += stmt
#define ASSERT_INT_EQUAL(...) ASSERT(assert_int_equal(__VA_ARGS__))
#define ASSERT_CHAR_EQUAL(...) ASSERT(assert_char_equal(__VA_ARGS__))
#define ASSERT_STRING_EQUAL(...) ASSERT(assert_string_equal(__VA_ARGS__))
#define ASSERT_BOOL(...) ASSERT(assert_bool(__VA_ARGS__))

#define ASSERT_POINT(p, _x, _y, _z) \
    ASSERT_INT_EQUAL((p)->x, (_x), "Point.x not equal"); \
    ASSERT_INT_EQUAL((p)->y, (_y), "Point.y not equal"); \
    ASSERT_INT_EQUAL((p)->z, (_z), "Point.z not equal");

#define ASSERT_COLORS(c, _x, _y, _z) \
    ASSERT_CHAR_EQUAL((c).x, (_x), "Color.x not equal"); \
    ASSERT_CHAR_EQUAL((c).y, (_y), "Color.y not equal"); \
    ASSERT_CHAR_EQUAL((c).z, (_z), "Color.z not equal");

#define ASSERT_MATRIX(m, a0, a1, a2, a3, a4, a5, a6, a7, a8) \
    ASSERT_INT_EQUAL((m)->vals[0], a0, "Matrix.0 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[1], a1, "Matrix.1 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[2], a2, "Matrix.2 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[3], a3, "Matrix.3 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[4], a4, "Matrix.4 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[5], a5, "Matrix.5 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[6], a6, "Matrix.6 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[7], a7, "Matrix.7 not equal"); \
    ASSERT_INT_EQUAL((m)->vals[8], a8, "Matrix.8 not equal");

#define ASSERT_CUBE(c, expected) \
    char buf[CUBE_STR_SIZE];     \
    cube_to_string(c, buf);      \
    ASSERT_STRING_EQUAL(buf, expected, "Cube string was incorrect");


int assert_int_equal(int a, int b, const char *msg);
int assert_char_equal(char a, char b, const char *msg);
int assert_string_equal(char *actual, const char *expected, const char *msg);
int assert_bool(bool val, const char* msg);
int redirect_stdout(char buf[]);
void unredirect_stdout(int restore_fd);

#endif
