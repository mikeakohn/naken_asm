/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>

#include "Operator.h"
#include "Var.h"

bool Operator::is_math_operator(const char *token)
{
  if (token[1] != 0) { return false; }

  switch (token[0])
  {
    case '*':
    case '/':
    case '%':
    case '+':
    case '-':
    case '&':
    case '^':
    case '|':
      return true;
    default:
      return false;
  }
}

bool Operator::set_operator(const char *token)
{
  // Single character token.
  if (token[1] == 0)
  {
    switch (token[0])
    {
#if 0
      case '~':
        precedence = PREC_NOT;
        operation  = OPER_NOT;
        break;
#endif
      case '*':
        precedence = PREC_MUL;
        operation  = OPER_MUL;
        break;
      case '/':
        precedence = PREC_MUL;
        operation = OPER_DIV;
        break;
      case '%':
        precedence = PREC_MUL;
        operation = OPER_MOD;
        break;
      case '+':
        precedence = PREC_ADD;
        operation = OPER_PLUS;
        break;
      case '-':
        precedence = PREC_ADD;
        operation = OPER_MINUS;
        break;
      case '&':
        precedence = PREC_AND;
        operation = OPER_AND;
        break;
      case '^':
        precedence = PREC_XOR;
        operation = OPER_XOR;
        break;
      case '|':
        precedence = PREC_OR;
        operation = OPER_OR;
        break;
      default:
        return false;
    }

    return true;
  }

  // If token is more than 2 characters, return false.
  if (token[2] != 0) { return false; }

  if (token[0] == '<' && token[1] == '<')
  {
    precedence = PREC_SHIFT;
    operation = OPER_SHIFT_L;
    return true;
  }
    else
  if (token[0] == '>' && token[1] == '>')
  {
    precedence = PREC_SHIFT;
    operation = OPER_SHIFT_R;
    return true;
  }

  return false;
}

int Operator::execute(Var &var_d, Var &var_s)
{
  switch (operation)
  {
    case OPER_NOT:     return var_d.logical_not(var_d);
    case OPER_MUL:     return var_d.mul(var_d, var_s);
    case OPER_DIV:     return var_d.div(var_d, var_s);
    case OPER_MOD:     return var_d.mod(var_d, var_s);
    case OPER_PLUS:    return var_d.add(var_d, var_s);
    case OPER_MINUS:   return var_d.sub(var_d, var_s);
    case OPER_SHIFT_L: return var_d.shift_left(var_d, var_s);
    case OPER_SHIFT_R: return var_d.shift_right(var_d, var_s);
    case OPER_AND:     return var_d.logical_and(var_d, var_s);
    case OPER_XOR:     return var_d.logical_xor(var_d, var_s);
    case OPER_OR:      return var_d.logical_or(var_d, var_s);
    default:
      printf("Internal Error: Bad operator %d?\n", operation);
      assert(false);
      return 0;
  }
}

const char *Operator::to_string()
{
  const char *s[] =
  {
    "",   // OPER_UNSET:
    "~",  // OPER_NOT:
    "*",  // OPER_MUL:
    "/",  // OPER_DIV:
    "%",  // OPER_MOD:
    "+",  // OPER_PLUS:
    "-",  // OPER_MINUS:
    "<<", // OPER_SHIFT_L:
    ">>", // OPER_SHIFT_R:
    "&",  // OPER_AND:
    "^",  // OPER_XOR:
    "|"   // OPER_OR::
  };

  return s[operation];
}

