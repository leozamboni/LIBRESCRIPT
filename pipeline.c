#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h> 

#define ERROR(m,t) { printf("error '%s' %s",t,m); return 1; }

/*
 * Neko tokens
 */
/* functions */
#define PRINTF "prynty" 
/* operators */ 
#define QUOTATION "'"
#define SUM "+"
#define SUB "-"
#define DIV "/"
#define ASSIGNMENT ":3"
/* types */
#define CHAR "xar"
#define INT "ynt"
/* semicolon */
#define SEMICOLON "!"

typedef struct node Node;

enum {ID=1,OP,NUM,ASSIG};

struct node {
  	char *str;
    size_t token; 
  	Node *n;
};

typedef struct queue Queue;

struct queue {
  	Node *f;
  	Node *b;
};

Queue *create_queue(void) 
{
	Queue *q = (Queue *) malloc(sizeof(Queue));
  	q->f = q->b = NULL;
  	return q;
}

void push_queue(Queue *q, char *t_str, size_t t) 
{
  	Node *new = (Node *) malloc(sizeof(Node));
    new->str = (char *) malloc(strlen(t_str)*sizeof(char));
    strcpy(new->str,t_str);
    new->token = t;
  	new->n = NULL;
  	if (!q->f) q->f = new;
  	else q->b->n = new;
  	q->b = new;
}

void output_queue(Node *f) 
{
  	if (!f) return;
  	printf("\"%s\" ",f->str);
  	output_queue(f->n);
}

void output_queue_f(Node *f, FILE *fgcc) 
{
  	if (!f) return;
  	fprintf(fgcc,"%s ",f->str);
  	output_queue_f(f->n,fgcc);  
}

_Bool is_number(char *s)
{
    for (size_t i = 0; s[i]!= '\0'; i++)
    {
        if (isdigit(s[i]) == 0)
              return 0;
    }
    return 1;
}

size_t token(char *str)
{
    if (strcmp(str,ASSIGNMENT)==0) return ASSIG;
    else if (strcmp(str,PRINTF)==0
    || strcmp(str,CHAR)==0
    || strcmp(str,INT)==0
    || strcmp(str,SEMICOLON)==0) return ID;
    else if (strcmp(str,QUOTATION)==0
    || strcmp(str,SUM)==0
    || strcmp(str,SUB)==0
    || strcmp(str,DIV)==0) return OP;
    else if (is_number(str)) return NUM;
    else return ID;
}

_Bool lex(FILE *f, Queue *tokens)
{
    
    size_t t=0;
    size_t i=0;
    char aux[126];
    char str[126];
    char c;

    while((c = getc(f)) != EOF) 
	{ 
        aux[i++]=c;
		if(c == ' ' 
        || c == '\n' 
        || c == '(' 
        || c == ')' 
        || c == '\'' 
        || c == '!')
		{ 
            if (c == ' ' || (i>1 && c == '\'') || c == '!') i--;
            aux[i]='\0';
            strcpy(str,aux);
            t = token(str);
            if (!t) return 1;
            if (c!='\n' && str[0]!='\0') push_queue(tokens,str,t);
            if (str[0]!='\'' && c == '\'') push_queue(tokens,QUOTATION,2);
            else if (c == '!') push_queue(tokens,SEMICOLON,1);
            i=0;
		} 
	} 
    return 0;
}

_Bool parser(Node *f, Queue *ast)
{
    

    Node *w = f;
    while (w)
    {
        if (w->token == ID)
        {
            if (strcmp(w->str,CHAR)==0)
            {
                push_queue(ast,"char",ID);
                if (!(w = w->n)) return 1;
                if (w->token != ID) ERROR("expected var name",w->str)
                else
                {
                    push_queue(ast,w->str,ID);
                    if (!(w = w->n)) return 1;
                    if (w->token != ASSIG) ERROR("expected :3 operator",w->str)
                    else
                    {
                        push_queue(ast,"=",ASSIG);
                        if (!(w = w->n)) return 1;
                        if (w->token != OP) ERROR("expected \' operator",w->str)
                        else
                        {
                            push_queue(ast,"\"",OP);
                            if (!(w = w->n)) return 1;
                            if (w->token != ID) ERROR("expected string",w->str)
                            else
                            {
                                while (w->token == ID)
                                {
                                    push_queue(ast,w->str,ID);
                                    if (!(w = w->n)) return 1;
                                }
                                if (w->token != OP) ERROR("expected \' operator",w->str)
                                else
                                {
                                    push_queue(ast,"\"",OP);
                                    if (!(w = w->n)) return 1;
                                    if (w->token != ID) ERROR("expected !",w->str)
                                    else {
                                        push_queue(ast,";",ID);
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        w = w->n;
    }
    return 0;
}

int main(void)
{
    FILE *f = fopen("file.ear","r");
    FILE *fgcc = fopen("a.c","w");
    Queue *tokens = create_queue();
    Queue *ast = create_queue();

    fprintf(fgcc,"int main(void) { ");
    if (lex(f,tokens)) puts("lex error");
    if (parser(tokens->f,ast)) puts(" parser error");
    //output_queue(tokens->f);
    //puts("");
    //output_queue(ast->f);
    output_queue_f(ast->f,fgcc);
    fprintf(fgcc,"}");
    fclose(fgcc);
    system("gcc -o a a.c");
    system("./a");

	fclose(f);
    return 0;
}