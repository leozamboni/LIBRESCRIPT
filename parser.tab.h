/* A Bison parser, made by GNU Bison 3.8.2.  */

/* Bison interface for Yacc-like parsers in C

   Copyright (C) 1984, 1989-1990, 2000-2015, 2018-2021 Free Software Foundation,
   Inc.

   This program is free software: you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation, either version 3 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <https://www.gnu.org/licenses/>.  */

/* As a special exception, you may create a larger work that contains
   part or all of the Bison parser skeleton and distribute that work
   under terms of your choice, so long as that work isn't itself a
   parser generator using the skeleton or a modified version thereof
   as a parser skeleton.  Alternatively, if you modify or redistribute
   the parser skeleton itself, you may (at your option) remove this
   special exception, which will cause the skeleton and the resulting
   Bison output files to be licensed under the GNU General Public
   License without this special exception.

   This special exception was added by the Free Software Foundation in
   version 2.2 of Bison.  */

/* DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
   especially those whose name start with YY_ or yy_.  They are
   private implementation details that can be changed or removed.  */

#ifndef YY_YY_PARSER_TAB_H_INCLUDED
# define YY_YY_PARSER_TAB_H_INCLUDED
/* Debug traces.  */
#ifndef YYDEBUG
# define YYDEBUG 1
#endif
#if YYDEBUG
extern int yydebug;
#endif
/* "%code requires" blocks.  */
#line 9 "parser.y"

    #include "symtab.h"

#line 53 "parser.tab.h"

/* Token kinds.  */
#ifndef YYTOKENTYPE
# define YYTOKENTYPE
  enum yytokentype
  {
    YYEMPTY = -2,
    YYEOF = 0,                     /* "end of file"  */
    YYerror = 256,                 /* error  */
    YYUNDEF = 257,                 /* "invalid token"  */
    LET = 258,                     /* LET  */
    IF = 259,                      /* IF  */
    TRUE = 260,                    /* TRUE  */
    FALSE = 261,                   /* FALSE  */
    ID = 262,                      /* ID  */
    NUMBER = 263,                  /* NUMBER  */
    INT8 = 264,                    /* INT8  */
    INT16 = 265,                   /* INT16  */
    INT32 = 266,                   /* INT32  */
    INT64 = 267,                   /* INT64  */
    UINT8 = 268,                   /* UINT8  */
    UINT16 = 269,                  /* UINT16  */
    UINT32 = 270,                  /* UINT32  */
    UINT64 = 271,                  /* UINT64  */
    SIZE = 272,                    /* SIZE  */
    CHAR = 273,                    /* CHAR  */
    STRING = 274,                  /* STRING  */
    BOOL = 275,                    /* BOOL  */
    FLOAT = 276,                   /* FLOAT  */
    DOUBLE = 277,                  /* DOUBLE  */
    EQUAL = 278,                   /* EQUAL  */
    AND = 279,                     /* AND  */
    OR = 280,                      /* OR  */
    NOT = 281,                     /* NOT  */
    NOTEQUAL = 282,                /* NOTEQUAL  */
    GRATER = 283,                  /* GRATER  */
    LESS = 284,                    /* LESS  */
    GRATEREQUAL = 285,             /* GRATEREQUAL  */
    LESSEQUAL = 286                /* LESSEQUAL  */
  };
  typedef enum yytokentype yytoken_kind_t;
#endif

/* Value type.  */
#if ! defined YYSTYPE && ! defined YYSTYPE_IS_DECLARED
union YYSTYPE
{
#line 13 "parser.y"

    int num;
    char *str;
    _Bool bl;

#line 107 "parser.tab.h"

};
typedef union YYSTYPE YYSTYPE;
# define YYSTYPE_IS_TRIVIAL 1
# define YYSTYPE_IS_DECLARED 1
#endif


extern YYSTYPE yylval;


int yyparse (void);


#endif /* !YY_YY_PARSER_TAB_H_INCLUDED  */
