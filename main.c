#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define IS_TOKEN(def) (strcmp(t_str,def)==0)
#define ERROR(tkn,msg) { fprintf(stderr,"'%s' error %s",tkn,msg); return 1; }

#define IS_TOKEN_ID(id) (w->token_id == id)
#define IS_TYPE (w->token_id < ASSIGNMENT_OP && w->token_id > ID)
#define NEXT_IS_NULL (!(w = w->n))
#define PUSH_PARSERED_TOKEN(pt,pid) queue_push(ast,pt,pid);

/*
 * Neko tokens
 */
#define SEMICOLON_DEF "!"
#define LEFT_CURL_DEF "\""
#define RIGHT_CURL_DEF "\""
#define DECORATOR_DEF "@"
/* Types */
#define VOID_DEF "vud"
#define INT8_DEF "ak_46"
#define INT16_DEF "ak_47"
#define INT32_DEF "ak_47s"
#define INT64_DEF "rpk"
#define UINT8_DEF "UwU_ak_46"
#define UINT16_DEF "UwU_ak_47"
#define UINT32_DEF "UwU_ak_47s"
#define UINT64_DEF "UwU_rpk"
#define SIZE_DEF "i_love_int"
#define CHAR_DEF "m4"
#define STRING_DEF "m4a1_colt"
#define BOOL_DEF "m46"
#define FLOAT_DEF "i_hate_this"
#define DOUBLE_DEF "idfc"
/* Operators */
#define ASSIGNMENT_DEF ":3"
#define QUOTATION_DEF "'"
#define SUM_DEF "+"
#define SUB_DEF "-"
#define MULT_DEF "*"
#define DIV_DEF "/"
#define OR_DEF "|"
#define GREATER_DEF ">"
#define LESS_DEF "<"
#define EQUAL_DEF "="
/* Functions */
#define DEF_DEF "def"
#define PRINTF_DEF "show_this"

enum tokens_enum {
    SEMICOLON_ = 1,
    ID,
    LEFT_CURL,
    RIGHT_CURL,
    DECORATOR,
    /* 
     * Types
     */
    VOID_T,
    INT8_T,
    INT16_T,
    INT32_T,
    INT64_T,
    UINT8_T,
    UINT16_T,
    UINT32_T,
    UINT64_T,
    CHAR_T,
    STRING_T,
    BOOL_T,
    FLOAT_T,
    DOUBLE_T,
    /* 
     * Operators 
     */
    ASSIGNMENT_OP,
    QUOTATION_OP,
    /* Number operators */
    SUM_OP,
    SUB_OP,
    MULT_OP,
    DIV_OP,
    /* Logic operators */
    OR_OP,
    GREATER_OP,
    LESS_OP,
    EQUAL_OP,
    /* 
     * Functions 
     */
    DEF_F,
    PRINTF_F,
};

typedef struct node Node;

struct node {
  	char *token_str;
    size_t token_id; 
  	Node *n;
};

typedef struct queue Queue;

struct queue {
  	Node *f;
  	Node *b;
};

Queue *queue_create(void) 
{
	Queue *q = (Queue *) malloc(sizeof(Queue));
  	q->f = q->b = NULL;
  	return q;
}

void queue_push(Queue *q, char *t_str, size_t t_id) 
{
  	Node *new = (Node *) malloc(sizeof(Node));
    new->token_str = (char *) malloc(strlen(t_str)*sizeof(char));
    strcpy(new->token_str,t_str);
    new->token_id = t_id;
  	new->n = NULL;
  	if (!q->f) q->f = new;
  	else q->b->n = new;
  	q->b = new;
}

void queue_output(Node *f) 
{
  	if (!f) return;
  	printf("\"%s\" ",f->token_str);
  	queue_output(f->n);
}

_Bool is_number(char *s)
{
    for (size_t i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0) return 0;
    }
    return 1;
}

size_t get_token_id(char *t_str)
{
    if IS_TOKEN(SEMICOLON_DEF)      return SEMICOLON_;
    else if IS_TOKEN(LEFT_CURL)     return LEFT_CURL;
    else if IS_TOKEN(RIGHT_CURL)    return RIGHT_CURL;
    else if IS_TOKEN(DECORATOR)     return DECORATOR;
    else if IS_TOKEN(INT8_DEF)      return INT8_T;
    else if IS_TOKEN(INT16_DEF)     return INT16_T;
    else if IS_TOKEN(INT32_DEF)     return INT32_T;
    else if IS_TOKEN(INT64_DEF)     return INT64_T;
    else if IS_TOKEN(UINT8_DEF)     return UINT8_T;
    else if IS_TOKEN(UINT16_DEF)    return UINT16_T;
    else if IS_TOKEN(UINT32_DEF)    return UINT32_T;
    else if IS_TOKEN(UINT64_DEF)    return UINT64_T;
    else if IS_TOKEN(CHAR_DEF)      return CHAR_T;
    else if IS_TOKEN(STRING_DEF)    return STRING_T;
    else if IS_TOKEN(BOOL_DEF)      return BOOL_T;
    else if IS_TOKEN(FLOAT_DEF)     return FLOAT_T;
    else if IS_TOKEN(DOUBLE_DEF)    return DOUBLE_T;
    else if IS_TOKEN(ASSIGNMENT_DEF)return ASSIGNMENT_OP;
    else if IS_TOKEN(QUOTATION_DEF) return QUOTATION_OP;
    else if IS_TOKEN(SUM_DEF)       return SUM_OP;
    else if IS_TOKEN(SUB_DEF)       return SUB_OP;
    else if IS_TOKEN(MULT_DEF)      return MULT_OP;
    else if IS_TOKEN(DIV_DEF)       return DIV_OP;
    else if IS_TOKEN(OR_DEF)        return OR_OP;
    else if IS_TOKEN(GREATER_DEF)   return GREATER_OP;
    else if IS_TOKEN(LESS_DEF)      return LESS_OP;
    else if IS_TOKEN(EQUAL_DEF)     return EQUAL_OP;
    else if IS_TOKEN(DEF_DEF)       return DEF_F;
    else if IS_TOKEN(PRINTF_DEF)    return PRINTF_F;
    else return ID;
}

_Bool lex(FILE *f, Queue *tokens)
{
    size_t t = 0,i = 0;
    char c, aux[126], str[126];

    while((c = getc(f)) != EOF) 
	{ 
        aux[i++]=c;
		if(c == ' ' || c == '\n' || c == '(' || c == ')' || c == '\'' || c == '!' || c == '@')
		{ 
            if (c == ' ' || (i > 1 && c == '\'') || c == '!') i--;
            aux[i]='\0';

            strcpy(str,aux);
            t = get_token_id(str);
            if (!t) return 1;

            if (c != '\n' && str[0] != '\0')
            {
                queue_push(tokens,str,t);
            }
            if (str[0] != '\'' && c == '\'') 
            {
                queue_push(tokens,QUOTATION_DEF,QUOTATION_OP);
            } else if (c == '!') 
            {
                queue_push(tokens,SEMICOLON_DEF,SEMICOLON_);
            }
            
            i=0;
		} 
	} 
    return 0;
}

_Bool type_char_parser_tree(Node *f, Node *w, Queue *ast)
{
    PUSH_PARSERED_TOKEN("char",CHAR_T)
    if NEXT_IS_NULL return 1;
    if (!IS_TOKEN_ID(ID)) ERROR(w->token_str,"expected var name")
    else {
        PUSH_PARSERED_TOKEN(w->token_str,ID)
        if NEXT_IS_NULL return 1;
        if (!IS_TOKEN_ID(ASSIGNMENT_OP)) ERROR(w->token_str,"expected :3 operator")
        else {
            PUSH_PARSERED_TOKEN("=",ASSIGNMENT_OP)
            if NEXT_IS_NULL return 1;
            if (!IS_TOKEN_ID(QUOTATION_OP)) ERROR(w->token_str,"expected \' operator")
            else {
                PUSH_PARSERED_TOKEN("\"",QUOTATION_OP)
                if NEXT_IS_NULL return 1;
                if (!IS_TOKEN_ID(ID)) ERROR(w->token_str,"expected string")
                else {
                    while IS_TOKEN_ID(ID)
                    {
                        PUSH_PARSERED_TOKEN(w->token_str,ID)
                        if NEXT_IS_NULL return 1;
                    }
                    if (!IS_TOKEN_ID(QUOTATION_OP)) ERROR(w->token_str,"expected \' operator")
                    else {
                        PUSH_PARSERED_TOKEN("\"",QUOTATION_OP)
                        if NEXT_IS_NULL return 1;
                        if (!IS_TOKEN_ID(SEMICOLON_)) ERROR(w->token_str,"expected !")
                        else PUSH_PARSERED_TOKEN(";",SEMICOLON_)
                    }
                }
            }
        }
    }
    return 0;
}

_Bool def_parser_tree(Node *f, Node *w, Queue *ast)
{   
    if NEXT_IS_NULL return 1;
    if (!IS_TYPE) ERROR(w->token_str,"expected def type")
    else {
        PUSH_PARSERED_TOKEN("$TYPE$",w->token_id)
        if NEXT_IS_NULL return 1;
        if (!IS_TOKEN_ID(ID)) ERROR(w->token_str,"expected def name")
        else {
            PUSH_PARSERED_TOKEN(w->token_str,ID);
            PUSH_PARSERED_TOKEN("(",ID);
            if NEXT_IS_NULL return 1;
            size_t i = 0;
            char str[12];

            while IS_TOKEN_ID(DECORATOR)
            {
                if NEXT_IS_NULL return 1;
                PUSH_PARSERED_TOKEN("$TYPE$",w->token_id)
                sprintf(str,"var%ld,",i++);
                PUSH_PARSERED_TOKEN(str,ID);
                if NEXT_IS_NULL return 1;
            }
            PUSH_PARSERED_TOKEN(")",ID);
            if (!IS_TOKEN_ID(LEFT_CURL)) ERROR(w->token_str,"expected \"")
            else {
                PUSH_PARSERED_TOKEN("{",LEFT_CURL);
                if NEXT_IS_NULL return 1;

                while (w)
                {
                    switch (w->token_id)
                    {
                        case RIGHT_CURL:
                            PUSH_PARSERED_TOKEN("}",RIGHT_CURL);
                            return 0;
                        case CHAR_T:
                            r = type_char_parser_tree(f,w,ast); 
                            if (r) return 1;
                            break;
                    }
                    w = w->n;
                }
            }
        }
    }
}

_Bool parser(Node *f, Queue *ast)
{
    Node *w = f;
    _Bool r = 0;

    while (w)
    {
        switch (w->token_id)
        {
            case CHAR_T:
                r = type_char_parser_tree(f,w,ast); 
                if (r) return 1;
                break;
            case DEF:
        }
        w = w->n;
    }
    return 0;
}