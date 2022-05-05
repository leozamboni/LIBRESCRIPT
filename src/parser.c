#include "header/parser.h"


void parse_variable_declaration(Lex_t **_lex)
{
    Token_t *tk = _start;
    _parse(int_); 
    _parse(id_); 
    if (tk->type == dot_) {
         _parse(dot_);
        while(tk->type != smug_) {
            _parse(id_);
            if (tk->type == dot_) _parse(dot_); 
        }
    }
    _parse(smug_);
    _parse(number_);
    _parse(semicolon_);
}

void parser(Lex_t **lex)
{
    parse_variable_declaration(&(*lex));
}