#ifndef LS_H
#define LS_H

#include "lex.h"
#include <stdio.h>

typedef struct lex Lex_t;

struct lex {
	size_t i;
	char *stdin;
};



typedef struct ls LS_t;

struct ls {
	Lex_t *lex;	
};


void ls();

#endif
