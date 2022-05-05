#ifndef LEX_H
#define LEX_H

#include "token.h"
#include <stdio.h>

typedef struct lex Lex_t;

struct lex
{
    size_t i;
    char *stdin;
};

Token_t *lex(Lex_t **);

#endif
