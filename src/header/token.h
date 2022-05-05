#ifndef TOKEN_H
#define TOKEN_H


enum token_type {
	int_,
	id_,
	char_,
	void_,
	float_,
	string_,
	number_,
	plus_,
	minus_,
	slash_,
	asterisk_,
	smug_,
	semicolon_,
	dot_,
};

typedef enum token_type TokenType_t;

typedef struct token Token_t;

struct token {
	char *key;
	TokenType_t type;
};

Token_t * create_token(char *, TokenType_t);


#endif
