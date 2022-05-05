%{
    #include "parser.tab.h"
    #include <stdio.h>
    #include <string.h>
    char * lex_string(char *tk);
%}

%%

"int8"      {   return INT8;            }
"int16"     {   return INT16;           }
"int32"     {   return INT32;           }
"int64"     {   return INT64;           }
"uint8"     {   return  UINT8;          }
"uint16"    {   return UINT16;          }
"uint32"    {   return UINT32;          }
"uint64"    {   return UINT64;          }
"double"    {   return DOUBLE;          } 
"bool"      {   return BOOL;            }
"let"       {   return LET;             }
"true"      {   return TRUE;            }
"false"     {   return FALSE;           }
"=="        {   return EQUAL;           }
">="        {   return GRATEREQUAL;     }
"<="        {   return LESSEQUAL;       }
"!="        {   return NOTEQUAL;        }
"!"         {   return NOT;             }
"|"         {   return OR;              }
"&"         {   return AND;             }
">"         {   return GRATER;          }
"<"         {   return LESS;            }
"if"        {   return IF;              }

[0-9]+                      {   yylval.num = atoi(yytext); return NUMBER;   }
[a-zA-Z][a-zA-Z0-9_{}]*     {   yylval.str = lex_string(yytext); return ID; }
[(){}[\];:,+-/\n=]          {   return *yytext;                             }
. {}

%%

char * 
lex_string(char *tk) {
    char *str = malloc(sizeof(char) * strlen(tk));
    strcpy(str, tk);
    return str;
}

int yywrap() {}

