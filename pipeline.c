#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/*
 * Syntax parser errors
 */
#define exit_error(err, out)                                                  \
  fprintf (stderr, "ToT\nerror '%s' in line %d, %s.\n", out->tk_str,          \
           out->tk_line, err);                                                \
  return 1;

#define ERROR1 "unexpected expression"
#define ERROR2 "expected '!' expression"
#define ERROR3 "expected ':3' expression"
#define ERROR4 "expected ''' expression"
#define ERROR5 "expected '\"' expression"
#define ERROR6 "expected '[' expression"
#define ERROR7 "expected ']' expressiondd"
#define ERROR_VAR1 "unallocated variable"
#define ERROR_VAR2 "expected var name"
#define ERROR_VAR3 "incompatible var type"
#define ERROR_VAR4 "var already allocated"

#define warning(war, str, line)                                               \
  fprintf (stderr, "-_q\nwarning '%s' in line %d, %s.\n", str, line, war);

#define NTKS (sizeof (look_table) / sizeof (TkTable_t))

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

enum tokens_enum
{
  ID = 1,
  SEMICOLON,
  DEF,
  DECORATOR,
  PARENTHESES,
  RIGHT_KEY,
  LEFT_KEY,
  ASSIGNMENT,
  QUOTATION,
  TRUE,
  FALSE,
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
  LESS_OP,
  EQUAL_OP,
  /* Functions */
  PRINTF_F,
};

typedef struct
{
  size_t tk_id;
  char *tk_str;
} TkTable_t;

static TkTable_t look_table[] = {
  { SEMICOLON, "!" },
  { DEF, "def" },
  { DECORATOR, "@" },
  { ASSIGNMENT, ":3" },
  { QUOTATION, "\'" },
  { PARENTHESES, "\"" },
  { RIGHT_KEY, "[" },
  { LEFT_KEY, "]" },
  { IF, "se" },
  { ELSE, "senao" },
  { ELSEIF, "mas,se" },
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

typedef struct TkVarNode
{
  char *var_str;
  size_t var_id;
  struct TkVarNode *n;
} TkVarNode_t;

typedef struct TkVar
{
  TkVarNode_t *out;
  TkVarNode_t *inp;
} TkVar_t;

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
create_var_List (void)
{
  TkVar_t *tk_var_list = (TkVar_t *)malloc (sizeof (TkVar_t));
  tk_var_list->out = tk_var_list->inp = NULL;
  return tk_var_list;
}

void
push_var (TkVar_t *tk_var_l, char *var_str, size_t var_id)
{
  TkVarNode_t *tk_var = (TkVarNode_t *)malloc (sizeof (TkVarNode_t));
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
output_var (TkVarNode_t *out)
{
  if (!out)
    return 0;
  printf ("%s ", out->var_str);
  return output_var (out->n);
}

_Bool
check_var (TkVarNode_t *out, char *var_str)
{
  if (!out)
    return 0;
  if (strcmp (out->var_str, var_str) == 0)
    return 1;
  return check_var (out->n, var_str);
}

_Bool
check_var_id (TkVarNode_t *out, char *var_str, size_t var_id)
{
  if (!out)
    return 0;
  if (strcmp (out->var_str, var_str) == 0 && out->var_id == var_id)
    return 1;
  return check_var_id (out->n, var_str, var_id);
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
  printf ("%s ", out->tk_str);
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
  size_t t = 0, i = t, tk_l = 1;
  char *breaks = " !@\n\'\"[]";
  char c, tk_str_aux[126], tk_str[126];

  while ((c = getc (f)) != EOF)
    {
      if (c == '(' || c == ')')
        continue;
      tk_str_aux[i++] = c;
      size_t j;
      for (j = 0; j < strlen (breaks); ++j)
        if (c == breaks[j])
          break;
      if (j != strlen (breaks))
        {
          if ((c == ' ' && (!quot)) || (i > 1 && c == '\'') || c == '!'
              || c == '\n' || c == '\"')
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
          strcpy (tk_str, tk_str_aux);
          t = get_tk_id (tk_str);

          if (tk_str[0] != '\0' && tk_str[0] != '\n')
            {
              push (tk, tk_str, t, tk_l);
            }
          if (tk_str[0] != '\'' && c == '\'')
            {
              push (tk, "\'", QUOTATION, tk_l);
            }
          else if (c == '!')
            {
              push (tk, "!", SEMICOLON, tk_l);
            }
          else if (c == '\"')
            {
              push (tk, "\"", PARENTHESES, tk_l);
            }

          i = 0;
        }
    }
  return 0;
}

/*
 * Parser
 */
_Bool parser (TkNode_t *out, TkQueue_t *ast, TkVar_t *var_list,
              _Bool inside_conditional);

_Bool
conditional (TkNode_t **lex_out, TkQueue_t *ast, TkVar_t *var_list, size_t id,
             char *str)
{
  TkNode_t *out = *(lex_out);

  push (ast, str, out->tk_id, out->tk_line);
  out = out->n;

  if ((out) && out->tk_id == PARENTHESES)
    {
      push (ast, "(", PARENTHESES, out->tk_line);
      out = out->n;

      while (1)
        {
          if ((out) && is_number (out->tk_str))
            {
              push (ast, out->tk_str, out->tk_id, out->tk_line);
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
              else if (!(check_var (var_list->out, out->tk_str)))
                {
                  exit_error (ERROR_VAR1, out);
                }
              else if (!(check_var_id (var_list->out, out->tk_str, id)))
                {
                  exit_error (ERROR_VAR3, out);
                }
              push (ast, out->tk_str, out->tk_id, out->tk_line);
              out = out->n;
            }
          else
            {
              exit_error (ERROR1, out);
            }

          if ((out) && out->tk_id >= SUM_OP && out->tk_id <= EQUAL_OP)
            {
              push (ast, out->tk_str, out->tk_id, out->tk_line);
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
          push (ast, ")", PARENTHESES, out->tk_line);
          out = out->n;

          if ((out) && out->tk_id == RIGHT_KEY)
            {
              _Bool r = 0;

              push (ast, "{", PARENTHESES, out->tk_line);
              out = out->n;

              r = parser (out, ast, var_list, 1);
              if (r)
                return 1;

              while (out->tk_id != LEFT_KEY)
                out = out->n;
              *(lex_out) = out;
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
boolean (TkNode_t **lex_out, TkQueue_t *ast, TkVar_t *var_list, size_t id,
         char *str)
{
  TkNode_t *out = *(lex_out);

  push (ast, str, out->tk_id, out->tk_line);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_var (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_var (var_list, out->tk_str, id);
      push (ast, out->tk_str, out->tk_id, out->tk_line);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push (ast, "=", ASSIGNMENT, out->tk_line);
          out = out->n;

          if (((out) && out->tk_id == TRUE) || ((out) && out->tk_id == FALSE)
              || ((out) && out->tk_id == ID))
            {
              if (!(is_number (out->tk_str)) && out->tk_id != FALSE
                  && out->tk_id != TRUE)
                {
                  if (!(check_var (var_list->out, out->tk_str)))
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                  else if (!(check_var_id (var_list->out, out->tk_str, id)))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }

              if (out->tk_id == FALSE || out->tk_id == TRUE
                  || out->tk_str[0] == '1' || out->tk_str[0] == '0')
                {
                  push (ast,
                        out->tk_id == FALSE || out->tk_str[0] == '0' ? "0"
                                                                     : "1",
                        out->tk_id, out->tk_line);
                  out = out->n;
                }
              else
                {
                  exit_error (ERROR_VAR3, out);
                }

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push (ast, ";", SEMICOLON, out->tk_line);
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
floating (TkNode_t **lex_out, TkQueue_t *ast, TkVar_t *var_list, size_t id,
          char *str)
{
  TkNode_t *out = *(lex_out);

  push (ast, str, out->tk_id, out->tk_line);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_var (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_var (var_list, out->tk_str, id);
      push (ast, out->tk_str, out->tk_id, out->tk_line);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push (ast, "=", ASSIGNMENT, out->tk_line);
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
                  else if (!(check_var (var_list->out, out->tk_str)))
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                  else if (!(check_var_id (var_list->out, out->tk_str, id)))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }
              push (ast, out->tk_str, out->tk_id, out->tk_line);
              out = out->n;

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push (ast, ";", SEMICOLON, out->tk_line);
                  *(lex_out) = out;
                  return 0;
                }
              else if ((out) && out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                {
                  while (out->tk_id != SEMICOLON)
                    {
                      if (out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                        {
                          push (ast, out->tk_str, out->tk_id, out->tk_line);
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
                              else if (!(check_var (var_list->out,
                                                    out->tk_str)))
                                {
                                  exit_error (ERROR_VAR1, out);
                                }
                              else if (!(check_var_id (var_list->out,
                                                       out->tk_str, id)))
                                {
                                  exit_error (ERROR_VAR3, out);
                                }
                            }
                          push (ast, out->tk_str, out->tk_id, out->tk_line);
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
                  push (ast, ";", SEMICOLON, out->tk_line);
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
integer (TkNode_t **lex_out, TkQueue_t *ast, TkVar_t *var_list, size_t id,
         char *str)
{
  TkNode_t *out = *(lex_out);

  push (ast, str, out->tk_id, out->tk_line);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_var (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_var (var_list, out->tk_str, id);
      push (ast, out->tk_str, out->tk_id, out->tk_line);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push (ast, "=", ASSIGNMENT, out->tk_line);
          out = out->n;

          if ((out) && out->tk_id == ID)
            {
              if (strchr (out->tk_str, '.'))
                {
                  exit_error (ERROR1, out);
                }
              else if (!(is_number (out->tk_str)))
                {
                  if (!(check_var (var_list->out, out->tk_str)))
                    {
                      exit_error (ERROR_VAR1, out);
                    }
                  else if (!(check_var_id (var_list->out, out->tk_str, id)))
                    {
                      exit_error (ERROR_VAR3, out);
                    }
                }
              push (ast, out->tk_str, out->tk_id, out->tk_line);
              out = out->n;

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push (ast, ";", SEMICOLON, out->tk_line);
                  *(lex_out) = out;
                  return 0;
                }
              else if ((out) && out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                {
                  while (out->tk_id != SEMICOLON)
                    {
                      if (out->tk_id >= SUM_OP && out->tk_id <= DIV_OP)
                        {
                          push (ast, out->tk_str, out->tk_id, out->tk_line);
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
                              if (!(check_var (var_list->out, out->tk_str)))
                                {
                                  exit_error (ERROR_VAR1, out);
                                }
                              else if (!(check_var_id (var_list->out,
                                                       out->tk_str, id)))
                                {
                                  exit_error (ERROR_VAR3, out);
                                }
                            }
                          push (ast, out->tk_str, out->tk_id, out->tk_line);
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
                  push (ast, ";", SEMICOLON, out->tk_line);
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
character (TkNode_t **lex_out, TkQueue_t *ast, TkVar_t *var_list, size_t id,
           char *str)
{
  TkNode_t *out = *(lex_out);

  push (ast, str, id, out->tk_line);
  out = out->n;

  if ((out) && out->tk_id == ID)
    {
      if (check_var (var_list->out, out->tk_str))
        {
          exit_error (ERROR_VAR4, out);
        }

      push_var (var_list, out->tk_str, id);
      push (ast, out->tk_str, out->tk_id, out->tk_line);
      out = out->n;

      if ((out) && out->tk_id == ASSIGNMENT)
        {
          push (ast, "=", ASSIGNMENT, out->tk_line);
          out = out->n;

          if ((out) && out->tk_id == QUOTATION)
            {
              push (ast, "\"", QUOTATION, out->tk_line);
              out = out->n;

              if ((out) && out->tk_id == ID)
                {
                  if (id == STRING_T)
                    {
                      while (out->tk_id != QUOTATION)
                        {
                          push (ast, out->tk_str, out->tk_id, out->tk_line);
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
                      push (ast, out->tk_str, out->tk_id, out->tk_line);
                      out = out->n;
                      if (!out)
                        return 1;
                    }

                  if (out->tk_id == QUOTATION)
                    {
                      push (ast, "\"", QUOTATION, out->tk_line);
                      out = out->n;

                      if ((out) && out->tk_id == SEMICOLON)
                        {
                          push (ast, ";", SEMICOLON, out->tk_line);
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
              else if (!check_var (var_list->out, out->tk_str))
                {
                  exit_error (ERROR_VAR1, out);
                }
              else if (!(check_var_id (var_list->out, out->tk_str,
                                       out->tk_id)))
                {
                  exit_error (ERROR_VAR3, out);
                }

              push (ast, out->tk_str, out->tk_id, out->tk_line);
              out = out->n;

              if ((out) && out->tk_id == SEMICOLON)
                {
                  push (ast, ";", SEMICOLON, out->tk_line);
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
parser (TkNode_t *out, TkQueue_t *ast, TkVar_t *var_list,
        _Bool inside_conditional)
{
  _Bool r = 0;
  TkNode_t *aux;

  while (out)
    {
      switch (out->tk_id)
        {
        case LEFT_KEY:
          if (inside_conditional)
            {
              push (ast, "}", PARENTHESES, out->tk_line);
              return 0;
            }
        case INT8_T:
          r = integer (&out, ast, var_list, INT8_T, "int8_t ");
          aux = out;
          break;
        case INT16_T:
          r = integer (&out, ast, var_list, INT16_T, "int16_t ");
          aux = out;
          break;
        case INT32_T:
          r = integer (&out, ast, var_list, INT32_T, "int32_t ");
          aux = out;
          break;
        case INT64_T:
          r = integer (&out, ast, var_list, INT64_T, "int64_t ");
          aux = out;
          break;
        case UINT8_T:
          r = integer (&out, ast, var_list, UINT8_T, "uint8_t ");
          aux = out;
          break;
        case UINT16_T:
          r = integer (&out, ast, var_list, UINT16_T, "uint16_t ");
          aux = out;
          break;
        case UINT32_T:
          r = integer (&out, ast, var_list, UINT32_T, "uint32_t ");
          aux = out;
          break;
        case UINT64_T:
          r = integer (&out, ast, var_list, UINT64_T, "uint64_t ");
          aux = out;
          break;
        case FLOAT_T:
          r = floating (&out, ast, var_list, FLOAT_T, "float ");
          aux = out;
          break;
        case DOUBLE_T:
          r = floating (&out, ast, var_list, DOUBLE_T, "double ");
          aux = out;
          break;
        case CHAR_T:
          r = character (&out, ast, var_list, CHAR_T, "char ");
          aux = out;
          break;
        case STRING_T:
          r = character (&out, ast, var_list, STRING_T, "char * ");
          aux = out;
          break;
        case BOOL_T:
          r = boolean (&out, ast, var_list, BOOL_T, "_Bool ");
          aux = out;
          break;
        case IF:
          r = conditional (&out, ast, var_list, BOOL_T, "if ");
          aux = out;
          break;
        default:
          if (out->tk_id != ID)
            {
              exit_error (ERROR1, out);
            }
          warning ("unexpected expression", out->tk_str, out->tk_line);
          break;
        }
      if (r)
        return 1;
      out = out->n;
    }
  if (inside_conditional)
    {
      exit_error (ERROR7, aux);
    }
  return 0;
}

int
main (void)
{
  FILE *f = fopen ("file.libre", "r");
  TkQueue_t *tk = create ();
  TkQueue_t *ast = create ();
  TkVar_t *var_list = create_var_List ();

  if (lex (f, tk))
    {
      puts ("lex error");
      return 1;
    }
  if (parser (tk->out, ast, var_list, 0))
    {
      puts ("\t^ parser error");
      return 1;
    }
  output_var (var_list->out);

  output (tk->out);
  puts ("");
  output (ast->out);

  fflush (stderr);
  fflush (stdout);
  fclose (f);
  return 0;
}