/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/tokens.h"

#define PRINT_STACK() \
{ \
  int i; \
  for (i = 0; i < num_stack_ptr; i++) printf("%d) %d <-\n", i, num_stack[i]); \
}

struct _operator
{
  int operation;
  int precedence;
};

static int get_operator(char *token, struct _operator *operator)
{
  if (token[1] == 0)
  {
    if (token[0] == '~')
    {
      operator->precedence = PREC_NOT;
      operator->operation = OPER_NOT;
    }
      else
    if (token[0] == '*')
    {
      operator->precedence = PREC_MUL;
      operator->operation = OPER_MUL;
    }
      else
    if (token[0] == '/')
    {
      operator->precedence = PREC_MUL;
      operator->operation = OPER_DIV;
    }
      else
    if (token[0] == '%')
    {
      operator->precedence = PREC_MUL;
      operator->operation = OPER_MOD;
    }
      else
    if (token[0] == '+')
    {
      operator->precedence = PREC_ADD;
      operator->operation = OPER_PLUS;
    }
      else
    if (token[0] == '-')
    {
      operator->precedence = PREC_ADD;
      operator->operation = OPER_MINUS;
    }
      else
    if (token[0] == '&')
    {
      operator->precedence = PREC_AND;  
      operator->operation = OPER_AND;
    }
      else
    if (token[0] == '^')
    {
      operator->precedence = PREC_XOR;
      operator->operation = OPER_XOR;
    }
      else
    if (token[0] == '|')
    {
      operator->precedence = PREC_OR;
      operator->operation = OPER_OR;
    }
      else
    {
      return -1;
    }
  }
    else
  if (token[2] == 0)
  {
    if (token[0] == '<' && token[1] == '<')
    {
      operator->precedence = PREC_SHIFT;
      operator->operation = OPER_LEFT_SHIFT;
    }
      else
    if (token[0] == '>' && token[1] == '>')
    {
      operator->precedence = PREC_SHIFT;
      operator->operation = OPER_RIGHT_SHIFT;
    }
      else
    {
      return -1;
    }
  }
    else
  {
    return -1;
  }

  return 0;
}

static int operate(int a, int b, struct _operator *operator)
{
#ifdef DEBUG
printf(">>> OPERATING ON %d (%d) %d\n", a, operator->operation, b);
#endif

  switch(operator->operation)
  {
    case OPER_NOT:
      return ~a;
    case OPER_MUL:
      return a * b;
    case OPER_DIV:
      return a / b;
    case OPER_MOD:
      return a % b;
    case OPER_PLUS:
      return a + b;
    case OPER_MINUS:
      return a - b;
    case OPER_LEFT_SHIFT:
      return a << b;
    case OPER_RIGHT_SHIFT:
      return a >> b;
    case OPER_AND:
      return a & b;
    case OPER_XOR:
      return a ^ b;
    case OPER_OR:
      return a | b;
    default:
      printf("Internal Error: WTF, bad operator %d\n", operator->operation);
      return 0;
  }
}

static int parse_unary(struct _asm_context *asm_context, int *num, int operation)
{
  char token[TOKENLEN];
  int token_type;
  int temp;

  token_type = tokens_get(asm_context, token, TOKENLEN);

//printf("parse_unary: %s token_type=%d(%d)\n", token, token_type, TOKEN_NUMBER);

  if (IS_TOKEN(token,'-'))
  {
    if (parse_unary(asm_context, &temp, OPER_MINUS) == -1) { return -1; }
  }
    else
  if (IS_TOKEN(token,'~'))
  {
    if (parse_unary(asm_context, &temp, OPER_NOT) != 0) { return -1; }
  }
    else
  if (token_type == TOKEN_NUMBER)
  {
    temp = atoll(token);
  }
    else
  if (IS_TOKEN(token, '('))
  {
    if (eval_expression(asm_context, &temp) != 0) { return -1; }

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (IS_NOT_TOKEN(token,')'))
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }
    else
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if (operation == OPER_NOT) { *num = ~temp; }
  else if (operation == OPER_MINUS) { *num = -temp; }
  else { print_error_internal(NULL, __FILE__, __LINE__); return -1; }

  return 0;
}

static int eval_expression_go(struct _asm_context *asm_context, int *num, struct _operator *last_operator)
{
  char token[TOKENLEN];
  int token_type;
  int num_stack[3];
  int num_stack_ptr = 1;
  struct _operator operator;
  int last_token_was_op = -1;

#ifdef DEBUG
printf("Enter eval_expression_go,  num=%d\n", *num);
#endif

  memcpy(&operator, last_operator, sizeof(struct _operator));
  num_stack[0] = *num;

  while(1)
  {
#ifdef DEBUG
printf("eval_expression> going to grab a token\n");
#endif
    token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
printf("eval_expression> token=%s   num_stack_ptr=%d\n", token, num_stack_ptr);
#endif
//printf("num_stack_ptr=%d operator=%d\n", num_stack_ptr, operator.operation);

    // Issue 15: Return an error if a stack is full with no operator.
    if (num_stack_ptr == 3 && operator.operation == OPER_UNSET)
    {
      return -1;
    }

    if (token_type == TOKEN_QUOTED)
    {
      if (token[0] == '\\')
      {
        int e = tokens_escape_char(asm_context, (unsigned char *)token);
        if (e == 0) return -1;
        if (token[e+1] != 0)
        {
          print_error("Quoted literal too long.", asm_context);
          return -1;
        }
        sprintf(token, "%d", token[e]);
      }
        else
      {
        if (token[1]!=0)
        {
          print_error("Quoted literal too long.", asm_context);
          return -1;
        }
        sprintf(token, "%d", token[0]);
      }

      token_type = TOKEN_NUMBER;
    }

    // Open and close parenthesis
    if (IS_TOKEN(token,'('))
    {
      if (last_token_was_op == 0 && operator.operation != OPER_UNSET)
      {
        num_stack[num_stack_ptr-2] = operate(num_stack[num_stack_ptr-2], num_stack[num_stack_ptr-1], last_operator);
        num_stack_ptr--;
        operator.operation = OPER_UNSET;

        *num = num_stack[num_stack_ptr-1];
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      if (operator.operation == OPER_UNSET && num_stack_ptr == 2)
      {
        // This is probably the x(r12) case.. so this is actually okay
        *num = num_stack[num_stack_ptr-1];
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      int paren_num = 0;
      struct _operator paren_operator;
      paren_operator.precedence = PREC_UNSET;
      paren_operator.operation = OPER_UNSET;

      if (eval_expression_go(asm_context, &paren_num, &paren_operator) != 0)
      {
        return -1;
      }

      last_token_was_op = 0;

#ifdef DEBUG
printf("Paren got back %d\n", paren_num);
#endif
      num_stack[num_stack_ptr++] = paren_num;

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (!(token[1] == 0 && token[0] == ')'))
      {
        print_error("No matching ')'", asm_context);
        return -1;
      }
      continue;
    }

    if (IS_TOKEN(token,')'))
    {
      tokens_push(asm_context, token, token_type);
      break;
    }

    // End of expression
    if (IS_TOKEN(token,',') || IS_TOKEN(token,']') || token_type == TOKEN_EOF ||
        IS_TOKEN(token,'.'))
    {
      tokens_push(asm_context, token, token_type);
      break;
    }

    if (token_type == TOKEN_EOL)
    {
      //asm_context->tokens.line++;
      tokens_push(asm_context, token, token_type);
      break;
    }

    // Read number
    if (token_type == TOKEN_NUMBER)
    {
      last_token_was_op = 0;

      if (num_stack_ptr == 3)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      num_stack[num_stack_ptr++] = atoll(token);
#ifdef DEBUG
printf("pushed\n");
PRINT_STACK()
#endif
    }
      else
    if (token_type == TOKEN_SYMBOL)
    {
      last_token_was_op = 1;

      struct _operator operator_prev;
      memcpy(&operator_prev, &operator, sizeof(struct _operator));

      if (get_operator(token, &operator) == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      // Issue 15: 2015-July-21 mkohn - If operator is ~ then reverse
      // the next number.
      if (operator.operation == OPER_NOT)
      {
        int num;

        if (parse_unary(asm_context, &num, OPER_NOT) != 0) { return -1; }

        if (num_stack_ptr == 3)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        num_stack[num_stack_ptr++] = num;
        memcpy(&operator, &operator_prev, sizeof(struct _operator));

        last_token_was_op = 0;

        continue;
      }

      // Stack pointer probably shouldn't be less than 2
      if (num_stack_ptr == 0)
      {
        printf("Error: Unexpected operator '%s' at %s:%d\n", token, asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

#ifdef DEBUG
printf("OPERATOR '%s': precedence last=%d this=%d\n", token, last_operator->precedence, operator.precedence);
#endif

      if (last_operator->precedence == PREC_UNSET)
      {
        memcpy(last_operator, &operator, sizeof(struct _operator));
      }
        else
      if (last_operator->precedence > operator.precedence)
      {
        // The older operation has LESS precedence
        if (eval_expression_go(asm_context, &num_stack[num_stack_ptr-1], &operator) == -1)
        {
          return -1;
        }
      }
        else
      {
        num_stack[num_stack_ptr-2] = operate(num_stack[num_stack_ptr-2], num_stack[num_stack_ptr-1], last_operator);
        num_stack_ptr--;
        memcpy(last_operator, &operator, sizeof(struct _operator));
      }
    }
      else
    {
      if (asm_context->pass != 1)
      {
        print_error_unexp(token, asm_context);
      }
      return -1;
    }
  }

#ifdef DEBUG
printf("going to leave %d\n", last_operator->operation);
PRINT_STACK()
#endif

  if (last_operator->operation != OPER_UNSET)
  {
    num_stack[num_stack_ptr-2] = operate(num_stack[num_stack_ptr-2], num_stack[num_stack_ptr-1], last_operator);
    num_stack_ptr--;
  }

  *num = num_stack[num_stack_ptr-1];

  return 0;
}

int eval_expression(struct _asm_context *asm_context, int *num)
{
  struct _operator operator;

  *num = 0;
  operator.precedence = PREC_UNSET;
  operator.operation = OPER_UNSET;
  return eval_expression_go(asm_context, num, &operator);
}

