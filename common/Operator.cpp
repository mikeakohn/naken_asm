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

#include "Operator.h"

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
      case '~':
        precedence = PREC_NOT;
        operation  = OPER_NOT;
        break;
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
    operation = OPER_LEFT_SHIFT;
    return true;
  }
    else
  if (token[0] == '>' && token[1] == '>')
  {
    precedence = PREC_SHIFT;
    operation = OPER_RIGHT_SHIFT;
    return true;
  }

  return false;
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
    "<<", // OPER_LEFT_SHIFT:
    ">>", // OPER_RIGHT_SHIFT:
    "&",  // OPER_AND:
    "^",  // OPER_XOR:
    "|"   // OPER_OR::
  };

  return s[operation];
}

