
#include <stdio.h>
#include <stdlib.h>
#include "error.h"

void error(char *s) {
    fprintf(stderr, "unexpected value %s\n", s);
    exit(EXIT_FAILURE);
}