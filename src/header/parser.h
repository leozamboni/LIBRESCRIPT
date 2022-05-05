#ifndef PARSER_H
#define PARSER_H

#include "token.h"
#include "lex.h"

#define _error fprintf(stderr, "unexpected value in line %d\n", (*_lex)->i)
#define _start lex(&(*_lex))
#define _parse(expected) \
    (tk->type != expected && _error || *(*_lex)->stdin && (tk = lex(&(*_lex))))

void parser(Lex_t **);

#endif