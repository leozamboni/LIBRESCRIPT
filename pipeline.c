/*
 * TODO:
 * lista de variaveis com respectivos tipos para verificao de atribuicao e saida de dados. 
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <stdint.h>

#define NTKS (sizeof(look_table)/sizeof(TkTable_t))

enum tokens_enum {
    ID = 1,
    SEMICOLON,
    DEF,
    LEFT_CURL,
    RIGHT_CURL,
    DECORATOR,
    TRUE,
    FALSE,
    /* Types */
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
    /* Operators */
    ASSIGNMENT_OP,
    QUOTATION_OP,
    /* Number */
    SUM_OP,
    SUB_OP,
    MULT_OP,
    DIV_OP,
    /* Logic */
    OR_OP,
    GREATER_OP,
    LESS_OP,
    EQUAL_OP,
    /* Functions */
    PRINTF_F,
};

typedef struct { size_t tk_id; char *tk_str; } TkTable_t;

static TkTable_t look_table[] = {
    { SEMICOLON, "!" }, 
    { DEF, "def" },
    { LEFT_CURL, "\"" }, 
    { RIGHT_CURL, "\"" }, 
    { DECORATOR, "@" },
    { TRUE, "tru" },
    { FALSE, "fake" },
    { VOID_T, "vud" },
    { INT8_T, "ak_46" },
    { INT16_T, "ak_47" },
    { INT32_T, "ak_47s" },
    { INT64_T, "rpk" },
    { UINT8_T, "UwU_ak_46" },
    { UINT16_T, "UwU_ak_47" },
    { UINT16_T, "i_love_int" },
    { UINT32_T, "UwU_ak_47s" },
    { UINT64_T, "UwU_rpk" },
    { CHAR_T, "m4" },
    { STRING_T, "m4a1_colt" },
    { BOOL_T, "m46" },
    { FLOAT_T, "i_hate_this" },
    { DOUBLE_T, "idfc" },
    { ASSIGNMENT_OP, ":3" },
    { QUOTATION_OP, "\'" },
    { SUM_OP, "+" },
    { SUB_OP, "-" },
    { MULT_OP, "*" },
    { DIV_OP, "/" },
    { OR_OP, "or" },
    { GREATER_OP, ">" },
    { LESS_OP, "<" },
    { EQUAL_OP, "=" },
    { PRINTF_F, "show_this" },
};

typedef struct QueueNode Node_t;

struct QueueNode {
  	char *tk_str;
    size_t tk_id; 
    size_t tk_line;
  	Node_t *n;
};

typedef struct Queue Queue_t;

struct Queue {
  	Node_t *out;
  	Node_t *inp;
};

Queue_t *create(void) 
{
	Queue_t *q = (Queue_t *) malloc(sizeof(Queue_t));
  	q->out = q->inp = NULL;
  	return q;
}

void push(Queue_t *q, char *tk_str, size_t tk_id, size_t tk_l) 
{
  	Node_t *tk = (Node_t *) malloc(sizeof(Node_t));
    tk->tk_str = (char *) malloc(strlen(tk_str)*sizeof(char));
    strcpy(tk->tk_str,tk_str);
    tk->tk_id = tk_id;
    tk->tk_line = tk_l;
  	tk->n = NULL;
  	if (!q->out) q->out = tk;
  	else q->inp->n = tk;
  	q->inp = tk;
}

void output(Node_t *out) 
{
  	if (!out) return;
  	printf("%s",out->tk_str);
  	output(out->n);
}

size_t get_tk_id(char *tk_str)
{
    TkTable_t tb;

    for (size_t i = 0; i < NTKS; ++i)
    {
        tb = look_table[i];
        if (strcmp(tb.tk_str,tk_str)==0)
        {
            return tb.tk_id;
        }
    }
    return ID;
}

_Bool lex(FILE *f, Queue_t *tk)
{
    _Bool quot = 0;
    size_t t = 0, i = t, tk_l = 1;
    char c, tk_str_aux[126], tk_str[126];

    while((c = getc(f)) != EOF) 
	{ 
        tk_str_aux[i++]=c;
		if(c == ' ' || c == '\n' || c == '\'' || c == '!' || c == '@')
		{ 
            if ((c == ' ' && (!quot)) || (i > 1 && c == '\'') || c == '!') 
            {
                i--;
            }
            if (c == '\'') 
            {
                quot = !quot;
            }
            if (c == '\n') 
            {
                tk_l++;
            }

            tk_str_aux[i]='\0';
            strcpy(tk_str,tk_str_aux);

            t = get_tk_id(tk_str);
            if (!t) return 1;

            if (tk_str[0] != '\0' && c != '\n')
            {
                push(tk,tk_str,t,tk_l);
            }
            if (tk_str[0] != '\'' && c == '\'') 
            {
                push(tk,look_table[21].tk_str,QUOTATION_OP,tk_l);
            } else if (c == '!') {
                push(tk,look_table[0].tk_str,SEMICOLON,tk_l);
            }
            
            i = 0;
		} 
	} 
    return 0;
}

/* 
 * Parser 
 */

_Bool _ak_46(Node_t *out, Queue_t *ast)
{
    push(ast, "int8_t ", out->tk_id, out->tk_line);
    out = out->n;

    if ((out) && out->tk_id == ID)
    {
        push(ast, out->tk_str, out->tk_id, out->tk_line);
        out = out->n;

        if ((out) && out->tk_id == ASSIGNMENT_OP)
        {
            push(ast, "=", ASSIGNMENT_OP, out->tk_line);
            out = out->n;
            // TODO: check var existence
            if ((out) && out->tk_id == ID) { 
                push(ast, out->tk_str, out->tk_id, out->tk_line);
                out = out->n;

                if ((out) && out->tk_id == SEMICOLON) 
                {
                    push(ast, ";", SEMICOLON, out->tk_line);
                    return 0;
                } else if ((out) && out->tk_id >= SUM_OP && out->tk_id <= DIV_OP) {
                    while (out->tk_id != SEMICOLON)
                    {
                        if (out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                        {
                            push(ast, out->tk_str, out->tk_id, out->tk_line);
                            out = out->n;
                        } else {
                            fprintf(stderr, "error '%s', expected '!' expression in line %ld", out->tk_str, out->tk_line);
                            return 1;
                        }

                        if ((out) && out->tk_id == ID)
                        {
                            push(ast, out->tk_str, out->tk_id, out->tk_line);
                            out = out->n;
                            if (!out) return 1;

                        } else if (!out) {
                            return 1;
                        } else {
                            fprintf(stderr, "error '%s', unexpected expression in line %ld", out->tk_str, out->tk_line);
                            return 1;
                        }
                    }
                    push(ast, ";", SEMICOLON, out->tk_line);
                    return 0;
                } else {
                    fprintf(stderr, "error '%s', expected '!' expression in line %ld", out->tk_str, out->tk_line);
                }
            } else {
                fprintf(stderr, "error '%s', expected var assign in line %ld", out->tk_str, out->tk_line);
            }
        } else {
            fprintf(stderr, "error '%s', expected ':3' expression in line %ld", out->tk_str, out->tk_line);
        }
    } else {
        fprintf(stderr, "error '%s', expected var name in line %ld", out->tk_str, out->tk_line);
    }
    return 1;
}

_Bool _m4a1_colt(Node_t *out, Queue_t *ast)
{
    push(ast, "char*", STRING_T, out->tk_line);
    out = out->n;
    
    if ((out) && out->tk_id == ID)
    {
        push(ast, out->tk_str, out->tk_id, out->tk_line);
        out = out->n;

        if ((out) && out->tk_id == ASSIGNMENT_OP)
        {
            push(ast, "=", ASSIGNMENT_OP, out->tk_line);
            out = out->n;

            if ((out) && out->tk_id == QUOTATION_OP)
            {
                push(ast, "\"", QUOTATION_OP, out->tk_line);
                out = out->n;

                if ((out) && out->tk_id == ID)
                {
                    while (out->tk_id != QUOTATION_OP)
                    {
                        push(ast, out->tk_str, out->tk_id, out->tk_line);
                        out = out->n;
                        if (!out) return 1;
                    }

                    if ((out) && out->tk_id == QUOTATION_OP) 
                    {
                        push(ast, "\"", QUOTATION_OP, out->tk_line);
                        out = out->n;

                        if ((out) && out->tk_id == SEMICOLON)
                        {
                            push(ast, ";", SEMICOLON, out->tk_line);
                            return 0;
                        } else {
                            fprintf(stderr, "error '%s', expected '!' expression in line %ld", out->tk_str, out->tk_line);
                        }
                    } else {
                        fprintf(stderr, "error '%s', expected ''' expression in line %ld", out->tk_str, out->tk_line);
                    }
                } else {
                    fprintf(stderr, "error '%s', expected var content in line %ld", out->tk_str, out->tk_line);
                }
            } else {
                fprintf(stderr, "error '%s', expected ''' expression in line %ld", out->tk_str, out->tk_line);
            }
        } else {
            fprintf(stderr, "error '%s', expected ':3' expression in line %ld", out->tk_str, out->tk_line);
        }
    } else {
        fprintf(stderr, "error '%s', expected var name in line %ld", out->tk_str, out->tk_line);
    }
    return 1;
}

_Bool _show_this(Node_t *out, Queue_t *ast)
{
    push(ast, "printf(", STRING_T, out->tk_line);
    out = out->n;

    if ((out) && out->tk_id == QUOTATION_OP)
    {
        push(ast, "\"", QUOTATION_OP, out->tk_line);
        out = out->n;

        if (out)
        {
            while (out->tk_id != QUOTATION_OP)
            {
                push(ast, out->tk_str, out->tk_id, out->tk_line);
                out = out->n;
                if (!out) return 1;
            }
            if (out->tk_id == QUOTATION_OP)
            {   
                push(ast, "\")", QUOTATION_OP, out->tk_line);
                out = out->n;

                if ((out) && out->tk_id == SEMICOLON)
                {
                    push(ast, ";", SEMICOLON, out->tk_line);
                    return 0;
                } else {
                    fprintf(stderr, "error '%s', expected '!' expression in line %ld", out->tk_str, out->tk_line);
                }
            } else {
                fprintf(stderr, "error '%s', expected ''' expression in line %ld", out->tk_str, out->tk_line);
            }
        } else {
            return 1;
        }   
    } else if ((out) && out->tk_id == ID) {
        push(ast, "\"%s\",", ID, out->tk_line);
        push(ast, out->tk_str, out->tk_id, out->tk_line);
        out = out->n;

        if ((out) && out->tk_id == SEMICOLON)
        {
            push(ast, ");", SEMICOLON, out->tk_line);
            return 0;
        } else {
            fprintf(stderr, "error '%s', expected '!' expression in line %ld", out->tk_str, out->tk_line);
        }
    } else {
        fprintf(stderr, "error '%s', expected ''' expression or 'm4a1_colt' type in line %ld", out->tk_str, out->tk_line);
    }
    return 1;
}

_Bool parser(Node_t *out, Queue_t *ast)
{
    _Bool r = 0;

    while (out)
    {
        switch (out->tk_id)
        {
            case INT8_T:
                r = _ak_46(out,ast);
                if (r) return 1;
                break;
            case STRING_T:
                r = _m4a1_colt(out,ast); 
                if (r) return 1;
                break;
            case DEF:
                //r = _def(out,ast);
                if (r) return 1;
                break;
            case PRINTF_F:
                r = _show_this(out,ast);
                if (r) return 1;
                break;
        }
        out = out->n;
    }
    return 0;
}

int main(void)
{
    FILE *f = fopen("file.neko","r");
    //FILE *gcc = fopen("a.c","w");

    Queue_t *tk = create();
    Queue_t *ast = create();

    //fprintf(gcc,"#include <stdio.h>\nint main(void) { ");
    if (lex(f,tk)) puts(" | lex error");
    if (parser(tk->out,ast)) puts(" | parser error");
    output(tk->out);
    puts("");
    output(ast->out);

	fclose(f);
}