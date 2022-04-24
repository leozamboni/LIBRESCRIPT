#include "header/token.h"
#include "header/lex.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

TokenType_t 
lex_dtype(char *stdin)
{
	if (strcmp(stdin, "int") == 0) return _int;
	return _string;
}

Token_t * 
lex_number(LS_t **ls) 
{
	char *key = calloc(1, sizeof(char));
	do {
		key = realloc(key, (strlen(key) + 2) * sizeof(char));
						strcat(key, (char[])
		{
			(*(*ls)->lex->stdin), 0
		});
	} while (*(*ls)->lex->stdin++ && isdigit(*(*ls)->lex->stdin));

	return create_token(key, _number);
}

Token_t *  
lex_alpha(LS_t **ls) 
{
	char *key = calloc(1, sizeof(char));
	do {
		key = realloc(key, (strlen(key) + 2) * sizeof(char));
		strcat(key, (char[])
		{
			(*(*ls)->lex->stdin), 0
		});
	} while (*(*ls)->lex->stdin++ && isalpha(*(*ls)->lex->stdin));

	return create_token(key, lex_dtype(key));
}

Token_t *
lex(LS_t **ls)
{
	do {
			if (isalpha(*(*ls)->lex->stdin)) return lex_alpha(&(*ls));
			if (isdigit(*(*ls)->lex->stdin)) return lex_number(&(*ls));
			switch(*(*ls)->lex->stdin)
			{
				case ' ':
				case '\t':
					break;
				case '+': return create_token(NULL, _plus); 
				case '-': return create_token(NULL, _minus); 
				case '*': return create_token(NULL, _asterisk); 
				case '/': return create_token(NULL, _slash); 
				case ';': return create_token(NULL, _semicolon); 
				case ':':
					if (*(*ls)->lex->stdin + 1 == '3') return create_token(NULL, _smug); 
			}

	}
	while (*(*ls)->lex->stdin++);

	return NULL;
}

