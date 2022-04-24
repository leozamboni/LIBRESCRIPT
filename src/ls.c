#include "header/lex.h"
#include "header/token.h"
#include <stdio.h>

void
ls() 
{
	Token_t *tk = lex("int a :3 12;");
	printf("%u\n", tk->type);
}
