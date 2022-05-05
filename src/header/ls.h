#ifndef LS_H
#define LS_H

#include "lex.h"
#include "parser.h"
#include <stdio.h>

typedef struct ls LS_t;

struct ls {
	Lex_t *lex;	
};


void ls();

#endif
