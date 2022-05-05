%{
    /* definitions */
    #include <stdlib.h>
    #include <stdbool.h>
    int yylex();
    void yyerror(char *s);
%}

%code requires {
    #include "symtab.h"
}

%union {
    int num;
    char *str;
    _Bool bl;
}

%token LET IF
%token<bl> TRUE FALSE
%token<str> ID
%token<num> NUMBER 
%token INT8 INT16 INT32 INT64 UINT8 UINT16 UINT32 UINT64 SIZE CHAR STRING BOOL FLOAT DOUBLE
%token EQUAL AND OR NOT NOTEQUAL GRATER LESS GRATEREQUAL LESSEQUAL

%type<num> exp;
%type<num> term;
%type<bl> cond;

/* rules */
%%

start: 
| line '\n' start
;

line: 
ID                                      { printf("%d\n", pull_symbol_table($ID));   }
| '(' LET ID ')'                        { printf("%d\n", pull_symbol_table($ID));   }
| exp 
| term
| cond
| if
| '\n'
;

exp:    
NUMBER                  { $$ = $1;      }
| exp '+' exp           { $$ = $1 + $3; }
| exp '-' exp           { $$ = $1 - $3; }
| exp '*' exp           { $$ = $1 * $3; }
| exp '/' exp           { $$ = $1 / $3; }
| '(' exp '+' exp ')'   { $$ = $2 + $4; }
| '(' exp '-' exp ')'   { $$ = $2 - $4; }
| '(' exp '*' exp ')'   { $$ = $2 * $4; }
| '(' exp '/' exp ')'   { $$ = $2 / $4; }
;

dtype:
INT8 
| INT16 
| INT32 
| INT64 
| UINT8 
| UINT16 
| UINT32 
| UINT64 
| SIZE 
| CHAR 
| STRING 
| BOOL 
| FLOAT 
| DOUBLE
;

cond:
TRUE                              {   $$ = 1;         }
| FALSE                           {   $$ = 0;         }
| ID                              {   $$ = $1;        }
| NOT ID                          {   $$ = !$2;       }
| cond EQUAL cond                 {   $$ = $1 == $3;  }
| cond AND cond                   {   $$ = $1 && $3;  }
| cond OR cond                    {   $$ = $1 && $3;  }
| cond NOTEQUAL cond              {   $$ = $1 != $3;  }
| cond GRATER cond                {   $$ = $1 > $3;   }
| cond GRATEREQUAL cond           {   $$ = $1 >= $3;  }
| cond LESS cond                  {   $$ = $1 < $3;   }
| cond LESSEQUAL cond             {   $$ = $1 <= $3;  }
| '(' cond EQUAL cond ')'         {   $$ = $2 == $4;  }
| '(' cond AND cond ')'           {   $$ = $2 && $4;  }
| '(' cond OR cond ')'            {   $$ = $2 || $4;  }
| '(' cond NOTEQUAL cond ')'      {   $$ = $2 != $4;  }
| '(' cond GRATER cond ')'        {   $$ = $2 > $4;   }
| '(' cond GRATEREQUAL cond ')'   {   $$ = $2 >= $4;  }
| '(' cond LESS cond ')'          {   $$ = $2 < $4;   }
| '(' cond LESSEQUAL cond ')'     {   $$ = $2 <= $4;  }
;

if:
IF cond 
;

term:
LET ID ':' dtype '=' exp ';'            { push_symbol_table($ID, $exp);             }
| LET ID ':' dtype ';'                  { push_symbol_table($ID, 0);                }
| LET ID ':' BOOL  '=' cond ';'         { push_symbol_table($ID, $cond);            }  
| LET ID ':' BOOL  '=' TRUE ';'         { push_symbol_table($ID, 1);                }  
| LET ID ':' BOOL  '=' FALSE ';'        { push_symbol_table($ID, 0);                }  
| '(' LET ID ':' dtype '=' exp ')'      { push_symbol_table($ID, $exp);             }     
| '(' LET ID ':' dtype ')'              { push_symbol_table($ID, 0);                }
| '(' LET ID ':' BOOL  '=' TRUE ')'     { push_symbol_table($ID, 1);                }  
| '(' LET ID ':' BOOL  '=' FALSE ')'    { push_symbol_table($ID, 0);                }  
;

%%

int 
main(void) {
    init_symbol_table();
    yyparse();
    return 0;
}

void 
yyerror(char *s) {
    fprintf(stderr, "parser: %s\n", s);
      exit(EXIT_FAILURE);
}