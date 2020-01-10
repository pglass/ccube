#include <stdio.h>
#include <stdlib.h>
#include "maths.h"

Point* new_point(int x, int y, int z) {
    Point* p = malloc(sizeof(Point));
    p->x = x;
    p->y = y;
    p->z = z;
    return p;
}

void free_point(Point* p) {
    if (p != NULL) {
        free(p);
    }
}

void print_point(Point* p) {
    if (p != NULL) {
        printf("Point(%d, %d, %d)\n", p->x, p->y, p->z);
    } else {
        printf("Point is NULL\n");
    }
}

void iadd_point(Point* a, Point* b) {
    a->x += b->x;
    a->y += b->y;
    a->z += b->z;
}

void isub_point(Point* a, Point* b) {
    a->x -= b->x;
    a->y -= b->y;
    a->z -= b->z;
}

int dot_point(Point* a, Point* b) {
    return a->x * b->x + a->y * b->y + a->z * b->z;
}

void icross_point(Point* a, Point* b) {
    int x = a->x;
    int y = a->y;
    int z = a->z;
    a->x = y * b->z - z * b->y;
    a->y = z * b->x - x * b->z;
    a->z = x * b->y - y * b->x;
}

Point* add_point(Point* a, Point* b) {
    Point* result = new_point(a->x, a->y, a->z);
    iadd_point(result, b);
    return result;
}

Point* sub_point(Point* a, Point* b) {
    Point* result = new_point(a->x, a->y, a->z);
    isub_point(result, b);
    return result;
}

Point* cross_point(Point* a, Point* b) {
    Point* result = new_point(a->x, a->y, a->z);
    icross_point(result, b);
    return result;
}

bool eq_point(Point* a, Point* b) {
    return a->x == b->x
        && a->y == b->y
        && a->z == b->z;
}

int count_point(Point* p, int val) {
    int result = 0;
    result += (val == p->x);
    result += (val == p->y);
    result += (val == p->z);
    return result;
}

int get_point_component(Point* p, char axis) {
    if (axis == 'x') { return p->x; }
    else if (axis == 'y') { return p->y; }
    else if (axis == 'z') { return p->z; }

    printf("ERROR: Invalid axis '%c'\n", axis);
    exit(1);
}

Matrix* new_matrix(int a0, int a1, int a2,
                   int a3, int a4, int a5,
                   int a6, int a7, int a8) {
    Matrix* m = malloc(sizeof(Matrix));
    m->vals[0] = a0;
    m->vals[1] = a1;
    m->vals[2] = a2;
    m->vals[3] = a3;
    m->vals[4] = a4;
    m->vals[5] = a5;
    m->vals[6] = a6;
    m->vals[7] = a7;
    m->vals[8] = a8;
    return m;
}

void free_matrix(Matrix* m) {
    if (m != NULL) {
        free(m);
    }
}

void print_matrix(Matrix* m) {
    printf("[%d, %d, %d,\n %d, %d, %d,\n %d, %d, %d]\n",
           m->vals[0], m->vals[1], m->vals[2],
           m->vals[3], m->vals[4], m->vals[5],
           m->vals[6], m->vals[7], m->vals[8]);
}

bool eq_matrix(Matrix* a, Matrix* b) {
    return (a->vals[0] == b->vals[0]
         && a->vals[1] == b->vals[1]
         && a->vals[2] == b->vals[2]
         && a->vals[3] == b->vals[3]
         && a->vals[4] == b->vals[4]
         && a->vals[5] == b->vals[5]
         && a->vals[6] == b->vals[6]
         && a->vals[7] == b->vals[7]
         && a->vals[8] == b->vals[8]);
}

void iadd_matrix(Matrix* a, Matrix* b) {
    a->vals[0] += b->vals[0];
    a->vals[1] += b->vals[1];
    a->vals[2] += b->vals[2];
    a->vals[3] += b->vals[3];
    a->vals[4] += b->vals[4];
    a->vals[5] += b->vals[5];
    a->vals[6] += b->vals[6];
    a->vals[7] += b->vals[7];
    a->vals[8] += b->vals[8];
}

void isub_matrix(Matrix* a, Matrix* b) {
    a->vals[0] -= b->vals[0];
    a->vals[1] -= b->vals[1];
    a->vals[2] -= b->vals[2];
    a->vals[3] -= b->vals[3];
    a->vals[4] -= b->vals[4];
    a->vals[5] -= b->vals[5];
    a->vals[6] -= b->vals[6];
    a->vals[7] -= b->vals[7];
    a->vals[8] -= b->vals[8];
}

void imul_matrix(Matrix *a, Matrix* b) {
    int a0 = a->vals[0];
    int a1 = a->vals[1];
    int a2 = a->vals[2];
    int a3 = a->vals[3];
    int a4 = a->vals[4];
    int a5 = a->vals[5];
    int a6 = a->vals[6];
    int a7 = a->vals[7];
    int a8 = a->vals[8];
    a->vals[0] = a0 * b->vals[0] + a1 * b->vals[3] + a2 * b->vals[6];
    a->vals[1] = a0 * b->vals[1] + a1 * b->vals[4] + a2 * b->vals[7];
    a->vals[2] = a0 * b->vals[2] + a1 * b->vals[5] + a2 * b->vals[8];
    a->vals[3] = a3 * b->vals[0] + a4 * b->vals[3] + a5 * b->vals[6];
    a->vals[4] = a3 * b->vals[1] + a4 * b->vals[4] + a5 * b->vals[7];
    a->vals[5] = a3 * b->vals[2] + a4 * b->vals[5] + a5 * b->vals[8];
    a->vals[6] = a6 * b->vals[0] + a7 * b->vals[3] + a8 * b->vals[6];
    a->vals[7] = a6 * b->vals[1] + a7 * b->vals[4] + a8 * b->vals[7];
    a->vals[8] = a6 * b->vals[2] + a7 * b->vals[5] + a8 * b->vals[8];
}

void imul_matrix_point(Matrix* m, Point* p) {
    int x = p->x;
    int y = p->y;
    int z = p->z;
    p->x = m->vals[0] * x + m->vals[1] * y + m->vals[2] * z;
    p->y = m->vals[3] * x + m->vals[4] * y + m->vals[5] * z;
    p->z = m->vals[6] * x + m->vals[7] * y + m->vals[8] * z;
}
