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
#include "common/eval_expression_ex.h"
#include "common/tokens.h"

#define VAR_COPY(var_d, var_s) \
  memcpy(var_d, var_s, sizeof(struct _var));

#define PRINT_STACK() \
{ \
  int i; \
  for (i = 0; i < var_stack_ptr; i++) printf("%d) %lx <-\n", i, var_stack[i].value_int); \
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

static int operate(struct _var *var_d, struct _var *var_s, struct _operator *operator)
{
#ifdef DEBUG
printf(">>> OPERATING ON %d/%f/%d (%d) %d/%f/%d\n",
  var_get_int32(var_d), var_get_float(var_d), var_get_type(var_d),
  operator->operation,
  var_get_int32(var_s), var_get_float(var_s), var_get_type(var_s));
#endif

  switch(operator->operation)
  {
    case OPER_NOT:
      return var_not(var_d);
    case OPER_MUL:
      return var_mul(var_d, var_s);
    case OPER_DIV:
      return var_div(var_d, var_s);
    case OPER_MOD:
      return var_mod(var_d, var_s);
    case OPER_PLUS:
      return var_add(var_d, var_s);
    case OPER_MINUS:
      return var_sub(var_d, var_s);
    case OPER_LEFT_SHIFT:
      return var_shift_left(var_d, var_s);
    case OPER_RIGHT_SHIFT:
      return var_shift_right(var_d, var_s);
    case OPER_AND:
      return var_and(var_d, var_s);
    case OPER_XOR:
      return var_xor(var_d, var_s);
    case OPER_OR:
      return var_or(var_d, var_s);
    default:
      printf("Internal Error: WTF, bad operator %d\n", operator->operation);
      return 0;
  }
}

static int parse_unary(struct _asm_context *asm_context, int64_t *num, int operation)
{
  char token[TOKENLEN];
  int token_type;
  int64_t temp;
  struct _var var;

  var_set_int(&var, 0);

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
    if (eval_expression_ex(asm_context, &var) != 0) { return -1; }
    if (var.type != VAR_INT)
    {
      print_error("Non-integer number in expression", asm_context);
      return -1;
    }

    temp = var_get_int64(&var);

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

static int eval_expression_go(struct _asm_context *asm_context, struct _var *var, struct _operator *last_operator)
{
  char token[TOKENLEN];
  int token_type;
  struct _var var_stack[3];
  int var_stack_ptr = 1;
  struct _operator operator;
  int last_token_was_op = -1;

#ifdef DEBUG
printf("Enter eval_expression_go,  var=%d/%f/%d\n", var_get_int32(var), var_get_float(var), var_get_type(var));
#endif

  memcpy(&operator, last_operator, sizeof(struct _operator));
  VAR_COPY(&var_stack[0], var);

  while(1)
  {
#ifdef DEBUG
printf("eval_expression> going to grab a token\n");
#endif
    token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
printf("eval_expression> token=%s   var_stack_ptr=%d\n", token, var_stack_ptr);
#endif

    // Issue 15: Return an error if a stack is full with noe operator.
    if (var_stack_ptr == 3 && operator.operation == OPER_UNSET)
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
        operate(&var_stack[var_stack_ptr-2], &var_stack[var_stack_ptr-1], last_operator);
        var_stack_ptr--;
        operator.operation = OPER_UNSET;

        VAR_COPY(var, &var_stack[var_stack_ptr-1]);
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      if (operator.operation == OPER_UNSET && var_stack_ptr == 2)
      {
        // This is probably the x(r12) case.. so this is actually okay
        VAR_COPY(var, &var_stack[var_stack_ptr-1]);
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      struct _var paren_var;
      struct _operator paren_operator;

      paren_operator.precedence = PREC_UNSET;
      paren_operator.operation = OPER_UNSET;
      memset(&paren_var, 0, sizeof(struct _var));

      if (eval_expression_go(asm_context, &paren_var, &paren_operator) != 0)
      {
        return -1;
      }

      last_token_was_op = 0;

#ifdef DEBUG
printf("Paren got back %d/%f/%d\n", var_get_int32(&paren_var), var_get_float(&paren_var), var_get_type(&paren_var));
#endif

      VAR_COPY(&var_stack[var_stack_ptr++], &paren_var);

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

      if (var_stack_ptr == 3)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      var_set_int(&var_stack[var_stack_ptr++], atoll(token));
    }
      else
    if (token_type == TOKEN_FLOAT)
    {
      if (var_stack_ptr == 3)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      var_set_float(&var_stack[var_stack_ptr++], atof(token));
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
        int64_t num;

        if (parse_unary(asm_context, &num, OPER_NOT) != 0) { return -1; }

        if (var_stack_ptr == 3)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        var_set_int(&var_stack[var_stack_ptr++], num);
        memcpy(&operator, &operator_prev, sizeof(struct _operator));

        last_token_was_op = 0;

        continue;
      }

      // Stack pointer probably shouldn't be less than 2
      if (var_stack_ptr == 0)
      {
        printf("Error: Unexpected operator '%s' at %s:%d\n", token, asm_context->tokens.filename, asm_context->tokens.line);
        return -1;
      }

#ifdef DEBUG
printf("TOKEN %s: precedence %d %d\n", token, last_operator->precedence, operator.precedence);
#endif

      if (last_operator->precedence == PREC_UNSET)
      {
        memcpy(last_operator, &operator, sizeof(struct _operator));
      }
        else
      if (last_operator->precedence > operator.precedence)
      {
        if (eval_expression_go(asm_context, &var_stack[var_stack_ptr-1], &operator) == -1)
        {
          return -1;
        }
      }
        else
      {
        operate(&var_stack[var_stack_ptr-2], &var_stack[var_stack_ptr-1], last_operator);
        var_stack_ptr--;
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
printf("going to leave  operation=%d\n", last_operator->operation);
PRINT_STACK()
#endif

  if (last_operator->operation != OPER_UNSET)
  {
    operate(&var_stack[var_stack_ptr-2], &var_stack[var_stack_ptr-1], last_operator);
    var_stack_ptr--;
  }

  VAR_COPY(var, &var_stack[var_stack_ptr-1]);

  return 0;
}

int eval_expression_ex(struct _asm_context *asm_context, struct _var *var)
{
  struct _operator operator;

  var_set_int(var, 0);

  operator.precedence = PREC_UNSET;
  operator.operation = OPER_UNSET;

  return eval_expression_go(asm_context, var, &operator);
}

