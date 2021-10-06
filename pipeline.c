/*
 *  LibreScrip pipeline
 *  Copyright (c) 2021 Leonardo Zamboni
 *
 *  this program is free software: you can redistribute it and/or modify
 *  it under the terms of the gnu general public license as published by
 *  the free software foundation, either version 3 of the license, or
 *  (at your option) any later version.
 *
 *  this program is distributed in the hope that it will be useful,
 *  but without any warranty; without even the implied warranty of
 *  merchantability or fitness for a particular purpose.  see the
 *  gnu general public license for more details.
 *
 *  you should have received a copy of the gnu general public license
 *  along with this program.  if not, see <http://www.gnu.org/licenses/>.
 */
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Syntax parser errors
 */
#define exit_error(err, out)                                                  \
  fprintf (stderr, "ToT\nerror '%s' in line %ld, %s.\n", out->tk_str,         \
           out->tk_line, err);                                                \
  return 1;

#define ERROR1 "unexpected expression"
#define ERROR2 "expected ';' expression"
#define ERROR3 "expected ':3' expression"
#define ERROR4 "expected ''' expression"
#define ERROR5 "expected '\"' expression"
#define ERROR6 "expected '[' expression"
#define ERROR7 "expected ']' expression"
#define ERROR8 "strings comparison"
#define ERROR9 "expected '@end' expression"
#define ERROR10 "literal operator can't be nested"
#define ERROR11 "expected ':' expression for Shell script name"
#define ERROR12 "script not found"
#define ERROR13 "expected '*/' expression"
#define ERROR_VAR1 "unallocated variable"
#define ERROR_VAR2 "expected var name"
#define ERROR_VAR3 "incompatible var type"
#define ERROR_VAR4 "var already allocated"

#define warning(war, str, line)                                               \
  fprintf (stderr, "-_q\nwarning '%s' in line %ld, %s.\n", str, line, war);

#define NTKS (sizeof (look_table) / sizeof (TkTable_t))
#define NLIBS 2

_Bool
is_number (char *s)
{
  for (size_t i = 0; s[i] != '\0'; i++)
    {
      if (isdigit (s[i]) == 0)
        return 0;
    }
  return 1;
}

char *standard_libs[] = {
  "stdio.h",
  "stdlib.h",
};

enum tokens_enum
{
  ID = 1,
  INCLUDE,
  SCRIPTRUN,
  SHELL,
  CLANG,
  END,
  COMMENT,
  COMMENT_LEFT,
  SEMICOLON,
  DEF,
  PARENTHESES,
  RIGHT_KEY,
  LEFT_KEY,
  ASSIGNMENT,
  QUOTATION,
  _TRUE,
  _FALSE,
  IF,
  ELSE,
  ELSEIF,
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
  /* Number */
  SUM_OP,
  SUB_OP,
  MULT_OP,
  DIV_OP,
  /* Logic */
  OR_OP,
  GREATER_OP,
  ORNOT_OP,
  LESS_OP,
  EQUAL_OP,
};

typedef struct
{
  size_t tk_id;
  char *tk_str;
} TkTable_t;

static TkTable_t look_table[] = {
  { COMMENT, "/*" },      { COMMENT_LEFT, "*/" }, { SCRIPTRUN, "$" },
  { SHELL, "@shell" },    { CLANG, "@clang" },    { END, "@end" },
  { INCLUDE, "import" },  { SEMICOLON, ";" },     { DEF, "def" },
  { ASSIGNMENT, ":3" },   { QUOTATION, "\'" },    { PARENTHESES, "\"" },
  { RIGHT_KEY, "[" },     { LEFT_KEY, "]" },      { IF, "if" },
  { ELSE, "else" },       { ELSEIF, "elif" },     { _TRUE, "true" },
  { _FALSE, "false" },    { VOID_T, "void" },     { INT8_T, "int8" },
  { INT16_T, "int16" },   { INT32_T, "int32" },   { INT64_T, "int64" },
  { UINT8_T, "uint8" },   { UINT16_T, "uint16" }, { UINT16_T, "size" },
  { UINT32_T, "uint32" }, { UINT64_T, "uint64" }, { CHAR_T, "char" },
  { STRING_T, "string" }, { BOOL_T, "bool" },     { FLOAT_T, "float" },
  { DOUBLE_T, "double" }, { SUM_OP, "+" },        { SUB_OP, "-" },
  { MULT_OP, "*" },       { DIV_OP, "/" },        { OR_OP, "||" },
  { ORNOT_OP, "!=" },     { GREATER_OP, ">" },    { LESS_OP, "<" },
  { EQUAL_OP, "==" },
};

typedef struct TkListNode
{
  char *var_str;
  size_t var_id;
  struct TkListNode *n;
} TkListNode_t;

typedef struct TkList
{
  TkListNode_t *out;
  TkListNode_t *inp;
} TkList_t;

typedef TkList_t TkVar_t;
typedef TkList_t Shfp_t;
typedef TkList_t POut_t;

typedef struct TkQueueNode
{
  char *tk_str;
  size_t tk_id;
  size_t tk_line;
  struct TkQueueNode *n;
} TkNode_t;

typedef struct TkQueue
{
  TkNode_t *out;
  TkNode_t *inp;
} TkQueue_t;

TkVar_t *
create_list (void)
{
  TkVar_t *tk_var_list = (TkVar_t *)malloc (sizeof (TkVar_t));
  tk_var_list->out = tk_var_list->inp = NULL;
  return tk_var_list;
}

void
push_list (TkVar_t *tk_var_l, char *var_str, size_t var_id)
{
  TkListNode_t *tk_var = (TkListNode_t *)malloc (sizeof (TkListNode_t));
  tk_var->var_str = (char *)malloc (strlen (var_str) * sizeof (char));
  strcpy (tk_var->var_str, var_str);
  tk_var->var_id = var_id;
  tk_var->n = NULL;
  if (!tk_var_l->out)
    tk_var_l->out = tk_var;
  else
    tk_var_l->inp->n = tk_var;
  tk_var_l->inp = tk_var;
}

_Bool
output_list (TkListNode_t *out)
{
  if (!out)
    return 0;
  printf ("%s", out->var_str);
  return output_list (out->n);
}

_Bool
check_list (TkListNode_t *out, char *var_str)
{
  if (!out)
    return 0;
  if (strcmp (out->var_str, var_str) == 0)
    return 1;
  return check_list (out->n, var_str);
}

_Bool
check_var (TkListNode_t *out, char *var_str, size_t var_id)
{
  if (!out)
    return 0;
  if (strcmp (out->var_str, var_str) == 0 && out->var_id == var_id)
    return 1;
  return check_var (out->n, var_str, var_id);
}

TkQueue_t *
create (void)
{
  TkQueue_t *q = (TkQueue_t *)malloc (sizeof (TkQueue_t));
  q->out = q->inp = NULL;
  return q;
}

void
push (TkQueue_t *q, char *tk_str, size_t tk_id, size_t tk_l)
{
  TkNode_t *tk = (TkNode_t *)malloc (sizeof (TkNode_t));
  tk->tk_str = (char *)malloc (strlen (tk_str) * sizeof (char));
  strcpy (tk->tk_str, tk_str);
  tk->tk_id = tk_id;
  tk->tk_line = tk_l;
  tk->n = NULL;
  if (!q->out)
    q->out = tk;
  else
    q->inp->n = tk;
  q->inp = tk;
}

void
output (TkNode_t *out)
{
  if (!out)
    return;
  printf ("%s", out->tk_str);
  output (out->n);
}

size_t
get_tk_id (char *tk_str)
{
  TkTable_t tb;

  for (size_t i = 0; i < NTKS; ++i)
    {
      tb = look_table[i];
      if (strcmp (tb.tk_str, tk_str) == 0)
        {
          return tb.tk_id;
        }
    }
  return ID;
}

_Bool
lex (FILE *f, TkQueue_t *tk)
{
  _Bool quot = 0;
  _Bool literal = 0;
  size_t t = 0;
  size_t i = 0;
  size_t tk_l = 1;
  char *breaks = " ;\n\'\"[]+><$";
  char c;
  char tk_str_aux[126];
  char tk_str[126];

  while ((c = getc (f)) != EOF)
    {
      if ((c == '(' && !literal) || (c == ')' && !literal))
        continue;
      tk_str_aux[i++] = c;
      size_t j;
      for (j = 0; j < strlen (breaks); ++j)
        if (c == breaks[j])
          break;
      if (j != strlen (breaks))
        {
          if ((c == ' ' && !quot && !literal) || (i > 1 && c == '\'')
              || c == ';' || c == '\n' || c == '\"' || c == '+' || c == '>'
              || c == '<' || c == '$')
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

          tk_str_aux[i] = '\0';

          if ((strcmp (tk_str_aux, "@clang") == 0)
              || (strcmp (tk_str_aux, "@shell") == 0))
            {
              literal = 1;
            }
          else if (strcmp (tk_str_aux, "@end") == 0)
            {
              literal = 0;
            }

          strcpy (tk_str, tk_str_aux);
          t = get_tk_id (tk_str);

          if (literal)
            {
              push (tk, tk_str, t, tk_l);
            }
          else if (tk_str[0] != '\0' && tk_str[0] != '\n')
            {
              push (tk, tk_str, t, tk_l);
            }
          if (tk_str[0] != '\'' && c == '\'')
            {
              push (tk, "\'", QUOTATION, tk_l);
            }
          else if (c == ';' || c == '\"' || c == '+' || c == '-' || c == '>'
                   || c == '<' || c == '$')
            {
              char aux_str[1];

              aux_str[0] = c;
              aux_str[1] = '\0';
              push (tk, aux_str, get_tk_id (aux_str), tk_l);
            }

          i = 0;
        }
    }
  return 0;
}

/*
 * Parser
 */
TkNode_t *parser (TkNode_t *out, POut_t *ast, TkVar_t *var_list, Shfp_t *shfp,
                  _Bool if_cond);

_Bool
_script (TkNode_t **lex_out, POut_t *ast, Shfp_t *shfp)
{
  TkNode_t *out = *(lex_out);
#ifdef __linux__
  char *src = "./";
#elif __WIN32__
  char *src = "";
#endif
  char scrname[126];

  push_list (ast, "system(\"", out->tk_id);
  out = out->n;

  if (!(out) || !check_list (shfp->out, out->tk_str))
    {
      exit_error (ERROR12, out);
    }

  strcpy (scrname, src);
  strcat (scrname, out->tk_str);
  push_list (ast, scrname, out->tk_id);

  out = out->n;
  if ((out) && out->tk_id == SEMICOLON)
    {
#ifdef __linux__
      push_list (ast, ".sh\");", SEMICOLON);
#elif __WIN32__
      push_list (ast, ".cmd\");", SEMICOLON);
#endif
    }
  else
    {
      exit_error (ERROR2, out);
    }

  *(lex_out) = out;
  return 0;
}

_Bool
_literal (TkNode_t **lex_out, POut_t *ast, Shfp_t *shfp, size_t id)
{
  TkNode_t *out = *(lex_out);
  TkNode_t *aux;
  FILE *srcfp;
  out = out->n;

  if (id == SHELL)
    {
      if ((out) && out->tk_str[0] == ':')
        {
          char scrname[64];
#ifdef __linux__
          char *shext = ".sh";
#elif __WIN32__
          char *shext = ".cmd";
#endif
          out->tk_str++;
          out->tk_str[strlen (out->tk_str) - 1] = '\0';
          strcpy (scrname, out->tk_str);
          strcat (scrname, shext);
          push_list (shfp, out->tk_str, SHELL);
          srcfp = fopen (scrname, "w");
        }
      else
        {
          exit_error (ERROR11, out);
        }
      out = out->n;
    }

  while (out->tk_id != END)
    {
      if ((id == CLANG && out->tk_id == SHELL)
          || (id == SHELL && out->tk_id == CLANG))
        {
          exit_error (ERROR10, out);
        }

      if (id == CLANG)
        {
          push_list (ast, out->tk_str, out->tk_id);
        }
      else
        {
          fprintf (srcfp, out->tk_str);
        }
      aux = out;
      out = out->n;
      if (!out)
        break;
    }

  if (!out)
    out = aux;
  if (id == SHELL)
    fclose (srcfp);
  *(lex_out) = out;
  return 0;
}

_Bool
_import (TkNode_t **lex_out, POut_t *ast)
{
  TkNode_t *out = *(lex_out);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      for (size_t i = 0; i <= NLIBS; ++i)
        if (strcmp (standard_libs[i], out->tk_str) == 0)
          {
            out = out->n;
            if (!(out) || out->tk_id != SEMICOLON)
              {
                exit_error (ERROR2, out);
              }
            *(lex_out) = out;
            return 0;
          }
      push_list (ast, "#include <", out->tk_id);
      push_list (ast, out->tk_str, out->tk_id);
    }
  else
    {
      exit_error (ERROR1, out);
    }
  push_list (ast, ">\n", out->tk_id);

  out = out->n;
  if (!(out) || out->tk_id != SEMICOLON)
    {
      exit_error (ERROR2, out);
    }

  *(lex_out) = out;
  return 0;
}

_Bool
_conditional (TkNode_t **lex_out, POut_t *ast, TkVar_t *var_list, Shfp_t *shfp,
              size_t id, char *str)
{
  TkNode_t *out = *(lex_out);

  push_list (ast, str, out->tk_id);
  out = out->n;

  if ((out) && out->tk_id == PARENTHESES)
    {
      push_list (ast, "(", PARENTHESES);
      out = out->n;

      while (1)
        {
          if ((out) && is_number (out->tk_str))
            {
              push_list (ast, out->tk_str, out->tk_id);
              out = out->n;
            }
          else if ((out) && out->tk_id == ID)
            {
              if (strchr (out->tk_str, '.'))
                {
                  _Bool dot = 0;
                  size_t i;

                  for (i = 0; i < strlen (out->tk_str); ++i)
                    {
                      if (dot && out->tk_str[i] == '.')
                        break;
                      if (out->tk_str[i] == '.')
                        {
                          dot = 1;
                          continue;
                        }
                      if (!(isdigit (out->tk_str[i])))
                        break;
                    }
                  if (i != strlen (out->tk_str))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }
              else if (!(check_list (var_list->out, out->tk_str)))
                {
                  if (out->tk_str[0] == '-')
                    {
                      push_list (ast, "-", out->tk_id);
                      out->tk_str++;
                      if (!(is_number (out->tk_str)))
                        {
                          exit_error (ERROR_VAR1, out);
                        }
                    }
                  else
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                }
              else if (check_var (var_list->out, out->tk_str, STRING_T))
                {
                  exit_error (ERROR8, out);
                }
              push_list (ast, out->tk_str, out->tk_id);
              out = out->n;
            }
          else if (out->tk_id == _TRUE || out->tk_id == _FALSE)
            {
              push_list (ast, out->tk_id == _TRUE ? "1" : "0", out->tk_id);
              out = out->n;
            }
          else
            {
              exit_error (ERROR1, out);
            }

          if ((out) && out->tk_id >= SUM_OP && out->tk_id <= EQUAL_OP)
            {
              push_list (ast, out->tk_str, out->tk_id);
              out = out->n;
            }
          else if ((out) && out->tk_id == PARENTHESES)
            {
              break;
            }
          else
            {
              exit_error (ERROR1, out);
            }
        }

      if ((out) && out->tk_id == PARENTHESES)
        {
          push_list (ast, ")", PARENTHESES);
          out = out->n;

          if ((out) && out->tk_id == RIGHT_KEY)
            {
              TkNode_t *aux;

              push_list (ast, "{", PARENTHESES);
              aux = out;
              out = out->n;
              if (!out)
                {
                  exit_error (ERROR7, aux);
                }
              out = parser (out, ast, var_list, shfp, 1);
              if (!out)
                return 1;

              aux = out;
              out = out->n;
              if ((out) && out->tk_id == ELSEIF)
                {
                  while (out->tk_id == ELSEIF)
                    {
                      push_list (ast, "else if", out->tk_id);
                      out = out->n;

                      if ((out) && out->tk_id == PARENTHESES)
                        {
                          push_list (ast, "(", PARENTHESES);
                          out = out->n;

                          while (1)
                            {
                              if ((out) && is_number (out->tk_str))
                                {
                                  push_list (ast, out->tk_str, out->tk_id);
                                  out = out->n;
                                }
                              else if ((out) && out->tk_id == ID)
                                {
                                  if (strchr (out->tk_str, '.'))
                                    {
                                      _Bool dot = 0;
                                      size_t i;

                                      for (i = 0; i < strlen (out->tk_str);
                                           ++i)
                                        {
                                          if (dot && out->tk_str[i] == '.')
                                            break;
                                          if (out->tk_str[i] == '.')
                                            {
                                              dot = 1;
                                              continue;
                                            }
                                          if (!(isdigit (out->tk_str[i])))
                                            break;
                                        }
                                      if (i != strlen (out->tk_str))
                                        {
                                          exit_error (ERROR_VAR3, out);
                                        }
                                    }
                                  else if (!(check_list (var_list->out,
                                                         out->tk_str)))
                                    {
                                      if (out->tk_str[0] == '-')
                                        {
                                          push_list (ast, "-", out->tk_id);
                                          out->tk_str++;
                                          if (!(is_number (out->tk_str)))
                                            {
                                              exit_error (ERROR_VAR1, out);
                                            }
                                        }
                                      else
                                        {
                                          exit_error (ERROR_VAR1, out);
                                        }
                                    }
                                  else if (check_var (var_list->out,
                                                      out->tk_str, STRING_T))
                                    {
                                      exit_error (ERROR8, out);
                                    }
                                  push_list (ast, out->tk_str, out->tk_id);
                                  out = out->n;
                                }
                              else if (out->tk_id == _TRUE
                                       || out->tk_id == _FALSE)
                                {
                                  push_list (ast,
                                             out->tk_id == _TRUE ? "1" : "0",
                                             out->tk_id);
                                  out = out->n;
                                }
                              else
                                {
                                  exit_error (ERROR1, out);
                                }

                              if ((out) && out->tk_id >= SUM_OP
                                  && out->tk_id <= EQUAL_OP)
                                {
                                  push_list (ast, out->tk_str, out->tk_id);
                                  out = out->n;
                                }
                              else if ((out) && out->tk_id == PARENTHESES)
                                {
                                  break;
                                }
                              else
                                {
                                  exit_error (ERROR1, out);
                                }
                            }

                          if ((out) && out->tk_id == PARENTHESES)
                            {
                              push_list (ast, ")", PARENTHESES);
                              out = out->n;

                              if ((out) && out->tk_id == RIGHT_KEY)
                                {
                                  push_list (ast, "{", RIGHT_KEY);
                                  aux = out;
                                  out = out->n;
                                  if (!out)
                                    {
                                      exit_error (ERROR7, aux);
                                    }
                                  out = parser (out, ast, var_list, shfp, 1);
                                  if (!out)
                                    return 1;
                                }
                            }
                        }
                      else
                        {
                          exit_error (ERROR5, out);
                        }
                      aux = out;
                      out = out->n;
                    }
                }
              if ((out) && out->tk_id == ELSE)
                {
                  push_list (ast, "else ", out->tk_id);
                  out = out->n;

                  if ((out) && out->tk_id == RIGHT_KEY)
                    {
                      push_list (ast, "{", RIGHT_KEY);
                      aux = out;
                      out = out->n;
                      if (!out)
                        {
                          exit_error (ERROR7, aux);
                        }
                      out = parser (out, ast, var_list, shfp, 1);
                      if (!out)
                        return 1;
                      aux = out;
                    }
                  else
                    {
                      exit_error (ERROR6, out);
                    }
                }
              *(lex_out) = aux;
              return 0;
            }
          else
            {
              exit_error (ERROR6, out);
            }
        }
      else
        {
          exit_error (ERROR5, out);
        }
    }
  else
    {
      exit_error (ERROR5, out);
    }
  return 0;
}

_Bool
_boolean (TkNode_t **lex_out, POut_t *ast, TkVar_t *var_list, size_t id,
          char *str)
{
  TkNode_t *out = *(lex_out);

  push_list (ast, str, out->tk_id);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_list (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_list (var_list, out->tk_str, id);
      push_list (ast, out->tk_str, out->tk_id);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push_list (ast, "=", ASSIGNMENT);
          out = out->n;

          if (((out) && out->tk_id == _TRUE) || ((out) && out->tk_id == _FALSE)
              || ((out) && out->tk_id == ID))
            {
              if (!(is_number (out->tk_str)) && out->tk_id != _FALSE
                  && out->tk_id != _TRUE)
                {
                  if (!(check_list (var_list->out, out->tk_str)))
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                  else if (!(check_var (var_list->out, out->tk_str, id)))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }

              if (out->tk_id == _FALSE || out->tk_id == _TRUE
                  || out->tk_str[0] == '1' || out->tk_str[0] == '0')
                {
                  push_list (ast,
                             out->tk_id == _FALSE || out->tk_str[0] == '0'
                                 ? "0"
                                 : "1",
                             out->tk_id);
                  out = out->n;
                }
              else
                {
                  exit_error (ERROR_VAR3, out);
                }

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push_list (ast, ";", SEMICOLON);
                  *(lex_out) = out;
                  return 0;
                }
              else
                {
                  exit_error (ERROR2, out);
                }
            }
          else
            {
              exit_error (ERROR_VAR3, out);
            }
        }
      else
        {
          exit_error (ERROR3, out);
        }
    }
  else
    {
      exit_error (ERROR_VAR2, out);
    }
  return 1;
}

_Bool
_floating (TkNode_t **lex_out, POut_t *ast, TkVar_t *var_list, size_t id,
           char *str)
{
  TkNode_t *out = *(lex_out);

  push_list (ast, str, out->tk_id);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_list (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_list (var_list, out->tk_str, id);
      push_list (ast, out->tk_str, out->tk_id);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push_list (ast, "=", ASSIGNMENT);
          out = out->n;

          if ((out) && out->tk_id == ID)
            {
              if (!(is_number (out->tk_str)))
                {
                  if (strchr (out->tk_str, '.'))
                    {
                      _Bool dot = 0;
                      size_t i;

                      for (i = 0; i < strlen (out->tk_str); ++i)
                        {
                          if (dot && out->tk_str[i] == '.')
                            break;
                          if (out->tk_str[i] == '.')
                            {
                              dot = 1;
                              continue;
                            }
                          if (!(isdigit (out->tk_str[i])))
                            break;
                        }
                      if (i != strlen (out->tk_str))
                        {
                          exit_error (ERROR_VAR3, out);
                        }
                    }
                  else if (!(check_list (var_list->out, out->tk_str)))
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                  else if (!(check_var (var_list->out, out->tk_str, id)))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }
              push_list (ast, out->tk_str, out->tk_id);
              out = out->n;

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push_list (ast, ";", SEMICOLON);
                  *(lex_out) = out;
                  return 0;
                }
              else if ((out) && out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                {
                  while (out->tk_id != SEMICOLON)
                    {
                      if (out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                        {
                          push_list (ast, out->tk_str, out->tk_id);
                          out = out->n;
                        }
                      else
                        {
                          exit_error (ERROR3, out);
                        }
                      if ((out) && out->tk_id == ID)
                        {
                          if (!(is_number (out->tk_str)))
                            {
                              if (strchr (out->tk_str, '.'))
                                {
                                  _Bool dot = 0;
                                  size_t i;

                                  for (i = 0; i < strlen (out->tk_str); ++i)
                                    {
                                      if (dot && out->tk_str[i] == '.')
                                        break;
                                      if (out->tk_str[i] == '.')
                                        {
                                          dot = 1;
                                          continue;
                                        }
                                      if (!(isdigit (out->tk_str[i])))
                                        break;
                                    }
                                  if (i != strlen (out->tk_str))
                                    {
                                      exit_error (ERROR_VAR3, out);
                                    }
                                }
                              else if (!(check_list (var_list->out,
                                                     out->tk_str)))
                                {
                                  exit_error (ERROR_VAR1, out);
                                }
                              else if (!(check_var (var_list->out, out->tk_str,
                                                    id)))
                                {
                                  exit_error (ERROR_VAR3, out);
                                }
                            }
                          push_list (ast, out->tk_str, out->tk_id);
                          out = out->n;
                          if (!out)
                            return 1;
                        }
                      else if (!out)
                        {
                          return 1;
                        }
                      else
                        {
                          exit_error (ERROR1, out);
                        }
                    }
                  push_list (ast, ";", SEMICOLON);
                  *(lex_out) = out;
                  return 0;
                }
              else
                {
                  exit_error (ERROR3, out);
                }
            }
          else
            {
              exit_error (ERROR_VAR3, out);
            }
        }
      else
        {
          exit_error (ERROR4, out);
        }
    }
  else
    {
      exit_error (ERROR_VAR2, out);
    }
  return 1;
}

_Bool
_integer (TkNode_t **lex_out, POut_t *ast, TkVar_t *var_list, size_t id,
          char *str)
{
  TkNode_t *out = *(lex_out);

  push_list (ast, str, out->tk_id);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_list (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_list (var_list, out->tk_str, id);
      push_list (ast, out->tk_str, out->tk_id);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push_list (ast, "=", ASSIGNMENT);
          out = out->n;

          if ((out) && out->tk_id == ID)
            {
              if (strchr (out->tk_str, '.'))
                {
                  exit_error (ERROR1, out);
                }
              else if (!(is_number (out->tk_str)))
                {
                  if (!(check_list (var_list->out, out->tk_str)))
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                  else if (!(check_var (var_list->out, out->tk_str, id)))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }
              push_list (ast, out->tk_str, out->tk_id);
              out = out->n;

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push_list (ast, ";", SEMICOLON);
                  *(lex_out) = out;
                  return 0;
                }
              else if ((out) && out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                {
                  while (out->tk_id != SEMICOLON)
                    {
                      if (out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                        {
                          push_list (ast, out->tk_str, out->tk_id);
                          out = out->n;
                        }
                      else
                        {
                          exit_error (ERROR3, out);
                        }
                      if ((out) && out->tk_id == ID)
                        {
                          if (strchr (out->tk_str, '.'))
                            {
                              exit_error (ERROR1, out);
                            }
                          else if (!(is_number (out->tk_str)))
                            {
                              if (!(check_list (var_list->out, out->tk_str)))
                                {
                                  exit_error (ERROR_VAR1, out);
                                }
                              else if (!(check_var (var_list->out, out->tk_str,
                                                    id)))
                                {
                                  exit_error (ERROR_VAR3, out);
                                }
                            }
                          push_list (ast, out->tk_str, out->tk_id);
                          out = out->n;
                          if (!out)
                            return 1;
                        }
                      else if (!out)
                        {
                          return 1;
                        }
                      else
                        {
                          exit_error (ERROR1, out);
                        }
                    }
                  push_list (ast, ";", SEMICOLON);
                  *(lex_out) = out;
                  return 0;
                }
              else
                {
                  exit_error (ERROR3, out);
                }
            }
          else
            {
              exit_error (ERROR_VAR3, out);
            }
        }
      else
        {
          exit_error (ERROR4, out);
        }
    }
  else
    {
      exit_error (ERROR_VAR2, out);
    }
  return 1;
}

_Bool
_character (TkNode_t **lex_out, POut_t *ast, TkVar_t *var_list, size_t id,
            char *str)
{
  TkNode_t *out = *(lex_out);

  push_list (ast, str, id);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_list (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_list (var_list, out->tk_str, id);
      push_list (ast, out->tk_str, out->tk_id);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push_list (ast, "=", ASSIGNMENT);
          out = out->n;

          if ((out) && out->tk_id == QUOTATION)
            {
              push_list (ast, "\"", QUOTATION);
              out = out->n;

              if ((out) && out->tk_id == ID)
                {
                  if (id == STRING_T)
                    {
                      while (out->tk_id != QUOTATION)
                        {
                          push_list (ast, out->tk_str, out->tk_id);
                          out = out->n;
                          if (!out)
                            return 1;
                        }
                    }
                  else if (strlen (out->tk_str) > 1)
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                  else
                    {
                      push_list (ast, out->tk_str, out->tk_id);
                      out = out->n;
                      if (!out)
                        return 1;
                    }

                  if (out->tk_id == QUOTATION)
                    {
                      push_list (ast, "\"", QUOTATION);
                      out = out->n;

                      if ((out) && out->tk_id == SEMICOLON)
                        {
                          push_list (ast, ";", SEMICOLON);
                          *(lex_out) = out;
                          return 0;
                        }
                      else
                        {
                          exit_error (ERROR3, out);
                        }
                    }
                  else
                    {
                      exit_error (ERROR4, out);
                    }
                }
              else
                {
                  exit_error (ERROR_VAR3, out);
                }
            }
          else if ((out) && out->tk_id == ID)
            {
              if (is_number (out->tk_str))
                {
                  exit_error (ERROR_VAR3, out);
                }
              else if (!check_list (var_list->out, out->tk_str))
                {
                  exit_error (ERROR_VAR1, out);
                }
              else if (!(check_var (var_list->out, out->tk_str, out->tk_id)))
                {
                  exit_error (ERROR_VAR3, out);
                }

              push_list (ast, out->tk_str, out->tk_id);
              out = out->n;

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push_list (ast, ";", SEMICOLON);
                  *(lex_out) = out;
                  return 0;
                }
              else
                {
                  exit_error (ERROR3, out);
                }
            }
          else
            {
              exit_error (ERROR_VAR3, out);
            }
        }
      else
        {
          exit_error (ERROR4, out);
        }
    }
  else
    {
      exit_error (ERROR_VAR2, out);
    }
  return 1;
}

TkNode_t *
parser (TkNode_t *out, POut_t *ast, TkVar_t *var_list, Shfp_t *shfp,
        _Bool if_cond)
{
  _Bool r = 0;
  TkNode_t *aux;

  while (out)
    {
      switch (out->tk_id)
        {
        case INCLUDE:
          r = _import (&out, ast);
          aux = out;
          break;
        case SCRIPTRUN:
          r = _script (&out, ast, shfp);
          aux = out;
          break;
        case SHELL:
          r = _literal (&out, ast, shfp, SHELL);
          aux = out;
          break;
        case CLANG:
          r = _literal (&out, ast, NULL, CLANG);
          aux = out;
          break;
        case LEFT_KEY:
          if (if_cond)
            {
              push_list (ast, "}", PARENTHESES);
              return out;
            }
        case INT8_T:
          r = _integer (&out, ast, var_list, INT8_T, "int8_t\n\0");
          aux = out;
          break;
        case INT16_T:
          r = _integer (&out, ast, var_list, INT16_T, "int16_t\n\0");
          aux = out;
          break;
        case INT32_T:
          r = _integer (&out, ast, var_list, INT32_T, "int32_t\n\0");
          aux = out;
          break;
        case INT64_T:
          r = _integer (&out, ast, var_list, INT64_T, "int64_t\n\0");
          aux = out;
          break;
        case UINT8_T:
          r = _integer (&out, ast, var_list, UINT8_T, "uint8_t\n\0");
          aux = out;
          break;
        case UINT16_T:
          r = _integer (&out, ast, var_list, UINT16_T, "uint16_t\n\0");
          aux = out;
          break;
        case UINT32_T:
          r = _integer (&out, ast, var_list, UINT32_T, "uint32_t\n\0");
          aux = out;
          break;
        case UINT64_T:
          r = _integer (&out, ast, var_list, UINT64_T, "uint64_t\n\0");
          aux = out;
          break;
        case FLOAT_T:
          r = _floating (&out, ast, var_list, FLOAT_T, "float\n\0");
          aux = out;
          break;
        case DOUBLE_T:
          r = _floating (&out, ast, var_list, DOUBLE_T, "double\n\0");
          aux = out;
          break;
        case CHAR_T:
          r = _character (&out, ast, var_list, CHAR_T, "char\n\0");
          aux = out;
          break;
        case STRING_T:
          r = _character (&out, ast, var_list, STRING_T, "char *\n\0");
          aux = out;
          break;
        case BOOL_T:
          r = _boolean (&out, ast, var_list, BOOL_T, "_Bool\n\0");
          aux = out;
          break;
        case IF:
          r = _conditional (&out, ast, var_list, shfp, IF, "if\0");
          aux = out;
          break;
        case COMMENT:
          while (out->tk_id != COMMENT_LEFT)
            {
              out = out->n;
              if (!out)
                {
                  fprintf (stderr, "ToT\nerror '%s' in line %ld, %s.\n",
                           aux->tk_str, aux->tk_line, ERROR13);
                  return NULL;
                }
            }
          break;
        default:
          if (out->tk_id != ID)
            {
              fprintf (stderr, "ToT\nerror '%s' in line %ld, %s.\n",
                       aux->tk_str, aux->tk_line, ERROR1);
              return NULL;
            }
          warning ("unexpected expression", out->tk_str, out->tk_line);
          break;
        }
      if (r)
        return NULL;
      out = out->n;
    }
  if (if_cond)
    {
      fprintf (stderr, "ToT\nerror '%s' in line %ld, %s.\n", aux->tk_str,
               aux->tk_line, ERROR7);
      return NULL;
    }

  return aux;
}

void
get_source (FILE *src, TkListNode_t *out)
{
  if (!out)
    return;
  fprintf (src, "%s", out->var_str);
  get_source (src, out->n);
}

int
main (int argc, char **argv)
{
  FILE *f = fopen (argv[1], "r");
  if (!f)
    {
      fprintf (stderr, "ToT\nnothing to compile.\n");
      return 1;
    }

  FILE *src = fopen ("source.c", "w");
  if (!src)
    return 1;

  fprintf (src, "#include <stdio.h>\n"
                "#include <stdlib.h>\n\n"
                "int main(void)\n"
                "{\n");

  TkQueue_t *tk = create ();
  POut_t *ast = create_list ();
  TkVar_t *var_list = create_list ();
  Shfp_t *shfp = create_list ();

  if (lex (f, tk))
    {
      puts ("lex error");
      fclose (f);
      fclose (src);
      return 1;
    }
  if (!parser (tk->out, ast, var_list, shfp, 0))
    {
      puts ("\t^ parser error");
      fclose (f);
      fclose (src);
      remove ("source.c");
      return 1;
    }
  else
    {
      puts ("^ ^\ncompiled!");
    }
  get_source (src, ast->out);
  // output (tk->out);
  // puts ("");
  // output_list (ast->out);
  // puts ("");
  // output_list (shfp->out);

  // fflush (stderr);
  // fflush (stdout);
  fprintf (src, "return 0;\n"
                "}");

  fclose (f);
  fclose (src);
  system ("gcc -o source source.c > gcclog.txt");
  system ("source");
  remove ("source.exe");
  remove ("source.c");
  remove ("gcclog.txt");

  char shellname[64];
  TkListNode_t *outshfp = shfp->out;
  while (outshfp)
    {
#ifdef __linux__
      char *shext = ".sh";
#elif __WIN32__
      char *shext = ".cmd";
#endif
      strcpy (shellname, outshfp->var_str);
      strcat (shellname, shext);
      shellname[strlen (shellname)] = '\0';
      remove (shellname);
      outshfp = outshfp->n;
    }

  free (tk);
  free (ast);
  free (var_list);
  free (shfp);

  return 0;
}