#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include "util.h"

int assert_string_equal(char actual[], const char *expected, const char *msg) {
    if (strcmp(actual, expected) != 0) {
        printf("\n  ERROR: %s\n    received: '%s'\n    expected: '%s'\n", msg, actual, expected);
        return 1;
    }
    return 0;
}

int assert_int_equal(int a, int b, const char *msg) {
    if (a != b) {
        printf("\n  ERROR: %s: %d != %d", msg, a, b);
        return 1;
    }
    return 0;
}

int assert_char_equal(char a, char b, const char *msg) {
    if (a != b) {
        printf("\n  ERROR: %s: %c != %c", msg, a, b);
        return 1;
    }
    return 0;
}

int assert_bool(bool val, const char* msg) {
    if (!val) {
        printf("\n  ERROR: %s: result was not true", msg);
        return 1;
    }
    return 0;
}

int redirect_stdout(char buf[]) {
    fflush(stdout);
    int stdout_save = dup(STDOUT_FILENO);
    freopen("/dev/null", "a", stdout);
    setvbuf(stdout, buf, _IOFBF, 1024);
    return stdout_save;
}

void unredirect_stdout(int restore_fd) {
    fflush(stdout);
    freopen("/dev/null", "a", stdout);
    dup2(restore_fd, STDOUT_FILENO);
    setlinebuf(stdout);
}
