#include "header/token.h"
#include "header/lex.h"
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

TokenType_t 
lex_dtype(char *stdin)
{
	if (strcmp(stdin, "int8") == 0) return int_;
	return id_;
}

Token_t * 
lex_number(Lex_t **lex) 
{
	char *key = calloc(1, sizeof(char));
	do {
		key = realloc(key, (strlen(key) + 2) * sizeof(char));
						strcat(key, (char[])
		{
			(*(*lex)->stdin), 0
		});
	} while (*(*lex)->stdin++ && isdigit(*(*lex)->stdin));
	return create_token(key, number_);
}

Token_t *  
lex_alpha(Lex_t **lex) 
{
	char *key = calloc(1, sizeof(char));
	do {
		key = realloc(key, (strlen(key) + 2) * sizeof(char));
		strcat(key, (char[])
		{
			(*(*lex)->stdin), 0
		});
	} while (*(*lex)->stdin++ && isalpha(*(*lex)->stdin));
	return create_token(key, lex_dtype(key));
}

Token_t *
lex(Lex_t **lex)
{
	do {
			if (isalpha(*(*lex)->stdin)) return lex_alpha(&(*lex));
			if (isdigit(*(*lex)->stdin)) return lex_number(&(*lex));
			switch(*(*lex)->stdin)
			{
				case ' ':
				case '\t':
					break;
				case '\n':
					(*lex)->i++;
					break;
				case '+': *(*lex)->stdin++; return create_token(NULL, plus_); 
				case '-': *(*lex)->stdin++; return create_token(NULL, minus_); 
				case '*': *(*lex)->stdin++; return create_token(NULL, asterisk_); 
				case '/': *(*lex)->stdin++; return create_token(NULL, slash_); 
				case ';': *(*lex)->stdin++; return create_token(NULL, semicolon_); 
				case ',': *(*lex)->stdin++; return create_token(NULL, dot_); 
				case ':':
					if (*((*lex)->stdin += 1) == '3' && *(*lex)->stdin++) return create_token(NULL, smug_);
			}

	}
	while (*(*lex)->stdin++);

	return NULL;
}

