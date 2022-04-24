#include "header/token.h"
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
lex_number(char *stdin) 
{
	char *key = calloc(1, sizeof(char));
	do {
		key = realloc(key, (strlen(key) + 2) * sizeof(char));
		strcat(key, (char[])
		{
			(*stdin), 0
		});
	} while (*stdin++ && isdigit(*stdin));

	return create_token(key, _number);
}

Token_t *  
lex_alpha(char *stdin) 
{
	char *key = calloc(1, sizeof(char));
	do {
		key = realloc(key, (strlen(key) + 2) * sizeof(char));
		strcat(key, (char[])
		{
			(*stdin), 0
		});
	} while (*stdin++ && isalpha(*stdin));

	return create_token(key, lex_dtype(key));
}

Token_t *
lex(char *stdin)
{
	do {
			if (isalpha(*stdin)) return lex_alpha(stdin);
			if (isdigit(*stdin)) return lex_number(stdin);
			switch(*stdin)
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
					if (*stdin + 1 == '3') return create_token(NULL, _smug); 
			}

	}
	while (*stdin++);

	return NULL;
}

