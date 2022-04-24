#include "header/token.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

Token_t *
create_token(char *key, TokenType_t type)
{
	Token_t *token = malloc(sizeof(Token_t));
	if (key)
	{
		token->key = malloc(strlen(key) * sizeof(char));
		strcpy(token->key, key);
	}
	token->type = type;
	return token;
}
