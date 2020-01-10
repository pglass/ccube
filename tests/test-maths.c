#include <stdio.h>
#include "../maths.h"
#include "util.h"


TEST(test_new_point)
    Point* p = new_point(3, 2, 1);
    ASSERT_POINT(p, 3, 2, 1);
    free_point(p);
END_TEST()


TEST(test_print_point)
    // redirect stdout into buf
    char buf[255] = "";
    int saved_fd = redirect_stdout(buf);

    Point p = {4, 5, 6};
    print_point(&p);

    // restore stdout
    unredirect_stdout(saved_fd);

    ASSERT_STRING_EQUAL(buf, "Point(4, 5, 6)\n", "Did not print the right thing");
END_TEST()


TEST(test_iadd_point)
    Point p = {3, 2, 1};
    Point q = {1, 3, 5};
    iadd_point(&p, &q);
    ASSERT_POINT(&p, 4, 5, 6);
END_TEST()


TEST(test_isub_point)
    Point p = {9, 9, 9};
    Point q = {3, 7, 5};
    isub_point(&p, &q);
    ASSERT_POINT(&p, 6, 2, 4);
END_TEST()


TEST(test_icross_point)
    Point p = {4, 6, 7};
    Point q = {3, 1, 4};
    icross_point(&p, &q);
    ASSERT_POINT(&p, 17, 5, -14);
END_TEST()


TEST(test_add_point)
    Point p = {3, 2, 1};
    Point q = {1, 3, 5};
    Point* sum = add_point(&p, &q);
    ASSERT_POINT(sum, 4, 5, 6);
    free_point(sum);
END_TEST()


TEST(test_sub_point)
    Point p = {9, 9, 9};
    Point q = {3, 7, 5};
    Point* diff = sub_point(&p, &q);
    ASSERT_POINT(diff, 6, 2, 4);
    free_point(diff);
END_TEST()


TEST(test_dot_point)
    Point p = {1, 2, 3};
    Point q = {4, 5, 6};
    int product = dot_point(&p, &q);
    ASSERT_INT_EQUAL(product, 32, "Dot product is not correct");
END_TEST()


TEST(test_cross_point)
    Point p = {4, 6, 7};
    Point q = {3, 1, 4};
    Point* product = cross_point(&p, &q);
    ASSERT_POINT(product, 17, 5, -14);
    free_point(product);
END_TEST()


TEST(test_eq_point)
    Point a = {4, 6, 7};
    Point b = {4, 6, 7};
    Point c = {3, 4, 5};
    ASSERT_BOOL(eq_point(&a, &a), "Points were not equal");
    ASSERT_BOOL(eq_point(&a, &b), "Points were not equal");
    ASSERT_BOOL(!eq_point(&a, &c), "Points were equal");
END_TEST()


TEST(test_count_point)
    Point a = {4, 4, 4};
    Point b = {1, 4, 1};
    Point c = {5, 6, 7};
    ASSERT_INT_EQUAL(count_point(&a, 4), 3, "Incorrect count");
    ASSERT_INT_EQUAL(count_point(&a, 3), 0, "Incorrect count");
    ASSERT_INT_EQUAL(count_point(&b, 1), 2, "Incorrect count");
    ASSERT_INT_EQUAL(count_point(&b, 4), 1, "Incorrect count");
    ASSERT_INT_EQUAL(count_point(&c, 5), 1, "Incorrect count");
    ASSERT_INT_EQUAL(count_point(&c, 6), 1, "Incorrect count");
    ASSERT_INT_EQUAL(count_point(&c, 7), 1, "Incorrect count");
END_TEST()


TEST(test_new_matrix)
    Matrix* m = new_matrix(1, 2, 3, 4, 5, 6, 7, 8, 9);
    ASSERT_MATRIX(m, 1, 2, 3, 4, 5, 6, 7, 8, 9);
    free_matrix(m);
END_TEST()


TEST(test_print_matrix)
    Matrix m = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    char buf[255] = "";
    int restore_fd = redirect_stdout(buf);
    print_matrix(&m);
    unredirect_stdout(restore_fd);
    ASSERT_STRING_EQUAL(buf, "[9, 8, 7,\n 6, 5, 4,\n 3, 2, 1]\n",
                        "Did not print out the right matrix");
END_TEST()


TEST(test_eq_matrix)
    Matrix a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix c = {2, 3, 4, 5, 6, 7, 8, 9, 10};
    Matrix d = {1, 2, 3, 4, 5, 0, 7, 8, 9};
    ASSERT_BOOL(eq_matrix(&a, &a), "Matrices were not equal");
    ASSERT_BOOL(eq_matrix(&a, &b), "Matrices were not equal");
    ASSERT_BOOL(!eq_matrix(&a, &c), "Matrices were equal");
    ASSERT_BOOL(!eq_matrix(&a, &d), "Matrices were equal");
END_TEST()


TEST(test_iadd_matrix)
    Matrix a = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    Matrix b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    iadd_matrix(&a, &b);
    ASSERT_MATRIX(&a, 2, 3, 4, 5, 6, 7, 8, 9, 10);
END_TEST()


TEST(test_isub_matrix)
    Matrix a = {1, 1, 1, 1, 1, 1, 1, 1, 1};
    Matrix b = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    isub_matrix(&a, &b);
    ASSERT_MATRIX(&a, 0, -1, -2, -3, -4, -5, -6, -7, -8);
END_TEST()


TEST(test_imul_matrix)
    Matrix a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Matrix b = {9, 8, 7, 6, 5, 4, 3, 2, 1};
    imul_matrix(&a, &b);
    ASSERT_MATRIX(&a, 30, 24, 18, 84, 69, 54, 138, 114, 90);
END_TEST()


TEST(test_imul_matrix_point)
    Matrix a = {1, 2, 3, 4, 5, 6, 7, 8, 9};
    Point p = {1, 2, 3};
    imul_matrix_point(&a, &p);
    ASSERT_POINT(&p, 14, 32, 50);
END_TEST()


int main(int argc, char *argv[]) {
    int errors = 0;

    printf("============== test point ===============\n");
    errors += test_print_point();
    errors += test_new_point();
    errors += test_iadd_point();
    errors += test_icross_point();
    errors += test_sub_point();
    errors += test_add_point();
    errors += test_sub_point();
    errors += test_dot_point();
    errors += test_cross_point();
    errors += test_eq_point();
    errors += test_count_point();

    printf("============== test matrix ===============\n");
    errors += test_new_matrix();
    errors += test_eq_matrix();
    errors += test_print_matrix();
    errors += test_iadd_matrix();
    errors += test_isub_matrix();
    errors += test_imul_matrix();
    errors += test_imul_matrix_point();

    if (errors != 0) {
        printf("FAILED: %d maths tests failed\n", errors);
    }
}
