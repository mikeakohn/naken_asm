/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/ifdef_expression.h"
#include "common/Macros.h"
#include "common/Symbols.h"
#include "common/tokens.h"

enum
{
  OPER_NONE=-1,
  OPER_EQUAL,
  OPER_GT_EQUAL,
  OPER_LT_EQUAL,
  OPER_GT,
  OPER_LT,
  OPER_OR,
  OPER_AND
};

enum
{
  PREC_OR=0,
  PREC_AND,
  PREC_EQUAL
};

typedef struct _operator
{
  int operation;
  int precedence;
} Operator;

static int get_operator(char *token, Operator *oper)
{
  if (IS_TOKEN(token,'>'))
  {
    oper->operation = OPER_GT;
    oper->precedence = PREC_EQUAL;
  }
    else
  if (IS_TOKEN(token,'<'))
  {
    oper->operation = OPER_LT;
    oper->precedence = PREC_EQUAL;
  }
    else
  if (strcmp(token,"==") == 0)
  {
    oper->operation = OPER_EQUAL;
    oper->precedence = PREC_EQUAL;
  }
    else
  if (strcmp(token,">=") == 0)
  {
    oper->operation = OPER_GT_EQUAL;
    oper->precedence = PREC_EQUAL;
  }
    else
  if (strcmp(token,"<=") == 0)
  {
    oper->operation = OPER_LT_EQUAL;
    oper->precedence = PREC_EQUAL;
  }
    else
  if (strcmp(token,"||") == 0)
  {
    oper->operation = OPER_OR;
    oper->precedence = PREC_OR;
  }
    else
  if (strcmp(token,"&&") == 0)
  {
    oper->operation = OPER_AND;
    oper->precedence = PREC_AND;
  }
    else
  {
    printf("Internal Error: Unknown equals_type '%s' %s:%d\n", token, __FILE__, __LINE__);
    return -1;
  }

  return 0;
}

static int parse_defined(AsmContext *asm_context)
{
  char token[TOKENLEN];
  //int token_type;
  int param_count; // throw away
  int ret;

  tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token,'('))
  {
    print_error_unexp(asm_context, token);
    return -1;
  }

  tokens_get(asm_context, token, TOKENLEN);

  if (macros_lookup(&asm_context->macros, token, &param_count) != NULL)
  {
    ret = 1;
  }
    else
  if (asm_context->symbols.find(token) != NULL)
  {
    ret = 1;
  }
    else
  {
    ret = 0;
  }

  tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token,')'))
  {
    print_error_unexp(asm_context, token);
    return -1;
  }

  return ret;
}

static int eval_operation(int oper, int num1, int num2)
{
#ifdef DEBUG
printf("debug> #if eval_operation()  operator=%d  num1=%d  num2=%d\n", oper, num1, num2);
#endif

  switch (oper)
  {
    case OPER_EQUAL:
      return num1 == num2 ? 1 : 0;
    case OPER_GT_EQUAL:
      return num1 >= num2 ? 1 : 0;
    case OPER_LT_EQUAL:
      return num1 <= num2 ? 1 : 0;
    case OPER_GT:
      return num1 > num2 ? 1 : 0;
    case OPER_LT:
      return num1 < num2 ? 1 : 0;
    case OPER_OR:
      return ((num1 | num2) != 0) ? 1 : 0;
    case OPER_AND:
      if (num1 != 0 && num2 != 0) { return 1; }
      return 0;
  }

  printf("Internal Error: Error in eval_operation() %s:%d\n", __FILE__, __LINE__);
  return -1;
}

static int is_num(char *value)
{
  char *s = value;

  while (*s != 0)
  {
    // If there is white space at the end of the value then remove it.
    // This is due to do adding an extra space at the end of macros.. should
    // probably change this.
    if (*s == ' ' && s != value)
    {
      while (*s == ' ') { s++; }
      return (*s == 0) ? 1 : 0;
    }

    if (*s < '0' || *s > '9') { return 0; }
    s++;
  }

  return 1;
}

static int parse_ifdef_expression(
  AsmContext *asm_context,
  int *num,
  int paren_count,
  int precedence,
  int state)
{
  char token[TOKENLEN];
  Operator oper;
  Symbols::Entry *symbols_entry;
  int token_type;
  int is_not = 0;
  int n1 = 0;
  int n;

  oper.operation = OPER_NONE;
  oper.precedence = precedence;
  n = *num;

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
printf("debug> #if: %d) %s   n=%d paren_count=%d precedence=%d state=%d\n", token_type, token, n, paren_count, precedence, state);
#endif

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      tokens_push(asm_context, token, token_type);

#if 0
      if (paren_count != 0)
      {
        print_error(asm_context, "Unbalanced parentheses.");
        return -1;
      }
#endif

      if (state != 1)
      {
        print_error(asm_context, "Unexpected end of expression.");
        return -1;
      }

      if (oper.operation != OPER_NONE)
      {
        n = eval_operation(oper.operation, n1, n);
#ifdef DEBUG
printf("debug> #if eval_operation() @EOL  n=%d precedence=%d state=%d\n", n, precedence, state);
#endif
        if (n == -1) { return -1; }
      }

      *num = n;
      return 0;
    }

    if (state != 1)
    {
      if (state == 2)
      {
        n1 = n;
      }

      if (token_type == TOKEN_SYMBOL)
      {
        if (IS_TOKEN(token,'!'))
        {
          is_not ^= 1;
          continue;
        }
          else
        if (IS_TOKEN(token,'('))
        {
          if (parse_ifdef_expression(asm_context, &n, paren_count + 1, PREC_OR, 0) == -1)
          {
            return -1;
          }
        }
          else
        if (IS_TOKEN(token,')'))
        {
          if (paren_count == 0)
          {
            print_error_unexp(asm_context, token);
            return -1;
          }

          if (state != 1)
          {
            print_error(asm_context, "Unexpected end of expression.");
            return -1;
          }

          if (oper.operation != OPER_NONE)
          {
            n = eval_operation(oper.operation, n1, n);
#ifdef DEBUG
printf("debug> #if eval_operation() @paren  n=%d\n", n);
#endif
            if (n == -1) { return -1; }
          }

          *num = n;
          return 0;
        }
      }
        else
      if (token_type == TOKEN_STRING)
      {
        int param_count;
        char *value = macros_lookup(&asm_context->macros, token, &param_count);

        if (strcasecmp(token, "defined") == 0)
        {
          n = parse_defined(asm_context);
#ifdef DEBUG
printf("debug> #if: parse_defined()=%d\n", n);
#endif
          if (n == -1) { return -1; }
        }
          else
        if (value == NULL && (symbols_entry = asm_context->symbols.find(token)) != NULL)
        {
          n = symbols_entry->address;
        }
          else
        if (value != NULL && param_count == 0 && is_num(value))
        {
          n = atoi(value);
        }
          else
        {
          print_error_unexp(asm_context, token);
          return -1;
        }
      }
        else
      if (token_type == TOKEN_NUMBER)
      {
        n = atoi(token);
      }

      if (is_not == 1)
      {
        if (n == 0) { n = 1; }
        else { n = 0; }

        is_not = 0;
      }

      state = 1;
      continue;
    }

    if (token_type == TOKEN_SYMBOL && IS_TOKEN(token, ')'))
    {
      if (paren_count == 0)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      if (oper.operation != OPER_NONE)
      {
          n = eval_operation(oper.operation, n1, n);

          if (n == -1) { return -1; }
      }

      *num = n;

      return 0;
    }

    if (token_type == TOKEN_SYMBOL || token_type == TOKEN_EQUALITY)
    {
      Operator next_operator;

      if (get_operator(token, &next_operator) == -1)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

#ifdef DEBUG
printf("debug> #if get_operator() token=%s operation=%d precedence=%d\n", token, next_operator.operation, next_operator.precedence);
#endif

      if (next_operator.precedence > precedence)
      {
        tokens_push(asm_context, token, token_type);

        if (parse_ifdef_expression(asm_context, &n, paren_count, next_operator.precedence, 1) == -1)
        {
          return -1;
        }
      }
        else
      if (next_operator.precedence < precedence)
      {
        tokens_push(asm_context, token, token_type);
        return 0;
      }
        else
      {
        state = 2;

        if (oper.operation != OPER_NONE)
        {
          n = eval_operation(oper.operation, n1, n);
#ifdef DEBUG
printf("debug> #if eval_operation() @ state 2  n=%d\n", n);
#endif
          if (n == -1) { return -1; }
        }

        oper = next_operator;
      }

      continue;
    }

    print_error_unexp(asm_context, token);
    return -1;
  }

  return -1;
}

int eval_ifdef_expression(AsmContext *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  int num = 0;

  if (parse_ifdef_expression(asm_context, &num, 0, PREC_OR, 0) == -1)
  {
    return -1;
  }

#ifdef DEBUG
printf("debug> parse_ifdef_expression() result is %d\n", num);
#endif

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(asm_context, token);
  }
    else
  {
    asm_context->tokens.line++;
  }

  return num;
}

