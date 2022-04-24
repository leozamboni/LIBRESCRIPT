#ifndef TOKEN_H
#define TOKEN_H


enum token_type {
	_int,
	_char,
	_void,
	_float,
	_string,
	_number,
	_plus,
	_minus,
	_slash,
	_asterisk,
	_smug,
	_semicolon,
};

typedef enum token_type TokenType_t;

typedef struct token Token_t;

struct token {
	char *key;
	TokenType_t type;
};

Token_t * create_token(char *, TokenType_t);


#endif
