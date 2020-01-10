#ifndef CCUBE_MATHS_H
#define CCUBE_MATHS_H

#include <stdbool.h>

#define COPY_POINT(p) {(p).x, (p).y, (p).z}

typedef struct _point {
    int x, y, z;
} Point;

typedef struct _matrix {
    int vals[9];
} Matrix;

Point* new_point(int x, int y, int z);
void free_point(Point* p);
void print_point(Point* p);
void iadd_point(Point* a, Point* b);
void isub_point(Point* a, Point* b);
void icross_point(Point* a, Point* b);
Point* add_point(Point* a, Point* b);
Point* sub_point(Point* a, Point* b);
int dot_point(Point* a, Point* b);
Point* cross_point(Point* a, Point* b);
bool eq_point(Point* a, Point* b);
int count_point(Point* p, int val);
int get_point_component(Point* p, char axis);

Matrix* new_matrix(int a0, int a1, int a2,
                   int a3, int a4, int a5,
                   int a6, int a7, int a8);
void free_matrix(Matrix* m);
void print_matrix(Matrix* m);
bool eq_matrix(Matrix* a, Matrix *b);

void iadd_matrix(Matrix* a, Matrix* b);
void isub_matrix(Matrix* a, Matrix* b);
void imul_matrix(Matrix* a, Matrix* b);
// p = M * p
void imul_matrix_point(Matrix* m, Point* p);

#endif
