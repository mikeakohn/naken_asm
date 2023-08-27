/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
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
  for (i = 0; i < var_stack_ptr; i++) printf("%d) %lx <-\n", i, var_stack[i].value_int); \
}

int EvalExpression::eval_expression_go(
  AsmContext *asm_context,
  Var &var,
  Operator *last_operator)
{
  char token[TOKENLEN];
  int token_type;
  Var var_stack[3];
  int var_stack_ptr = 1;
  Operator oper;
  int last_token_was_op = -1;

#ifdef DEBUG
printf("Enter eval_expression_go,  var=%d/%f/%d\n", var_get_int32(var), var_get_float(var), var_get_type(var));
#endif

  memcpy(&oper, last_operator, sizeof(Operator));
  var_stack[0] = var;

  while (true)
  {
#ifdef DEBUG
printf("eval_expression> going to grab a token\n");
#endif
    token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
printf("eval_expression> token=%s   var_stack_ptr=%d\n", token, var_stack_ptr);
#endif

    // Issue 15: Return an error if a stack is full with noe operator.
    if (var_stack_ptr == 3 && oper.operation == OPER_UNSET)
    {
      return -1;
    }

    if (token_type == TOKEN_QUOTED)
    {
      if (token[0] == '\\')
      {
        int e = tokens_escape_char(asm_context, (unsigned char *)token);
        if (e == 0) { return -1; }
        if (token[e + 1] != 0)
        {
          print_error(asm_context, "Quoted literal too long.");
          return -1;
        }
        snprintf(token, sizeof(token), "%d", token[e]);
      }
        else
      {
        if (token[1] != 0)
        {
          print_error(asm_context, "Quoted literal too long.");
          return -1;
        }
        snprintf(token, sizeof(token), "%d", token[0]);
      }

      token_type = TOKEN_NUMBER;
    }

    // Open and close parenthesis
    if (IS_TOKEN(token,'('))
    {
      if (last_token_was_op == 0 && oper.operation != OPER_UNSET)
      {
        operate(
          var_stack[var_stack_ptr - 2],
          var_stack[var_stack_ptr - 1],
          last_operator);

        var_stack_ptr--;
        oper.operation = OPER_UNSET;

        var = var_stack[var_stack_ptr-1];
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      if (oper.operation == OPER_UNSET && var_stack_ptr == 2)
      {
        // This is probably the x(r12) case.. so this is actually okay
        var = var_stack[var_stack_ptr - 1];
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      Var paren_var;
      Operator paren_operator;

      paren_var.clear();

      if (eval_expression_go(asm_context, paren_var, &paren_operator) != 0)
      {
        return -1;
      }

      last_token_was_op = 0;

#ifdef DEBUG
printf("Paren got back %d/%f/%d\n", var_get_int32(&paren_var), var_get_float(&paren_var), var_get_type(&paren_var));
#endif

      var_stack[var_stack_ptr++] = paren_var;

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (!(token[1] == 0 && token[0] == ')'))
      {
        print_error(asm_context, "No matching ')'");
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
        print_error_unexp(asm_context, token);
        return -1;
      }

      var_stack[var_stack_ptr++].set_int(atoll(token));
    }
      else
    if (token_type == TOKEN_FLOAT)
    {
      if (var_stack_ptr == 3)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      var_stack[var_stack_ptr++].set_float(atof(token));
    }
      else
    if (token_type == TOKEN_SYMBOL)
    {
      last_token_was_op = 1;

      Operator operator_prev = oper;

      //memcpy(&operator_prev, &oper, sizeof(Operator));

      if (get_operator(token, &oper) == -1)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      // Issue 15: 2015-July-21 mkohn - If operator is ~ then reverse
      // the next number.
      if (oper.operation == OPER_NOT)
      {
        int64_t num;

        if (parse_unary(asm_context, &num, OPER_NOT) != 0) { return -1; }

        if (var_stack_ptr == 3)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        var_stack[var_stack_ptr++].set_int(num);

        oper = operator_prev;
        //memcpy(&oper, &operator_prev, sizeof(Operator));

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
printf("TOKEN %s: precedence %d %d\n", token, last_operator->precedence, oper.precedence);
#endif

      if (last_operator->precedence == PREC_UNSET)
      {
        memcpy(last_operator, &oper, sizeof(Operator));
      }
        else
      if (last_operator->precedence > oper.precedence)
      {
        if (eval_expression_go(asm_context, var_stack[var_stack_ptr - 1], &oper) == -1)
        {
          return -1;
        }
      }
        else
      {
        operate(var_stack[var_stack_ptr - 2], var_stack[var_stack_ptr - 1], last_operator);
        var_stack_ptr--;
        memcpy(last_operator, &oper, sizeof(Operator));
      }
    }
      else
    {
      if (asm_context->pass != 1)
      {
        print_error_unexp(asm_context, token);
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
    operate(var_stack[var_stack_ptr-2], var_stack[var_stack_ptr-1], last_operator);
    var_stack_ptr--;
  }

  var = var_stack[var_stack_ptr-1];

  return 0;
}

int EvalExpression::get_operator(char *token, Operator *oper)
{
  if (token[1] == 0)
  {
    if (token[0] == '~')
    {
      oper->precedence = PREC_NOT;
      oper->operation = OPER_NOT;
    }
      else
    if (token[0] == '*')
    {
      oper->precedence = PREC_MUL;
      oper->operation = OPER_MUL;
    }
      else
    if (token[0] == '/')
    {
      oper->precedence = PREC_MUL;
      oper->operation = OPER_DIV;
    }
      else
    if (token[0] == '%')
    {
      oper->precedence = PREC_MUL;
      oper->operation = OPER_MOD;
    }
      else
    if (token[0] == '+')
    {
      oper->precedence = PREC_ADD;
      oper->operation = OPER_PLUS;
    }
      else
    if (token[0] == '-')
    {
      oper->precedence = PREC_ADD;
      oper->operation = OPER_MINUS;
    }
      else
    if (token[0] == '&')
    {
      oper->precedence = PREC_AND;
      oper->operation = OPER_AND;
    }
      else
    if (token[0] == '^')
    {
      oper->precedence = PREC_XOR;
      oper->operation = OPER_XOR;
    }
      else
    if (token[0] == '|')
    {
      oper->precedence = PREC_OR;
      oper->operation = OPER_OR;
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
      oper->precedence = PREC_SHIFT;
      oper->operation = OPER_LEFT_SHIFT;
    }
      else
    if (token[0] == '>' && token[1] == '>')
    {
      oper->precedence = PREC_SHIFT;
      oper->operation = OPER_RIGHT_SHIFT;
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

int EvalExpression::operate(Var &var_d, Var &var_s, Operator *oper)
{
#ifdef DEBUG
printf(">>> OPERATING ON %d/%f/%d (%d) %d/%f/%d\n",
  var_get_int32(var_d), var_get_float(var_d), var_get_type(var_d),
  oper->operation,
  var_get_int32(var_s), var_get_float(var_s), var_get_type(var_s));
#endif

  switch (oper->operation)
  {
    case OPER_NOT:
      return var_d.logical_not(var_d);
    case OPER_MUL:
      return var_d.mul(var_d, var_s);
    case OPER_DIV:
      return var_d.div(var_d, var_s);
    case OPER_MOD:
      return var_d.mod(var_d, var_s);
    case OPER_PLUS:
      return var_d.add(var_d, var_s);
    case OPER_MINUS:
      return var_d.sub(var_d, var_s);
    case OPER_LEFT_SHIFT:
      return var_d.shift_left(var_d, var_s);
    case OPER_RIGHT_SHIFT:
      return var_d.shift_right(var_d, var_s);
    case OPER_AND:
      return var_d.logical_and(var_d, var_s);
    case OPER_XOR:
      return var_d.logical_xor(var_d, var_s);
    case OPER_OR:
      return var_d.logical_or(var_d, var_s);
    default:
      printf("Internal Error: WTF, bad operator %d\n", oper->operation);
      return 0;
  }
}

int EvalExpression::parse_unary(
  AsmContext *asm_context,
  int64_t *num,
  int operation)
{
  char token[TOKENLEN];
  int token_type;
  int64_t temp;
  Var var;

  var.set_int(0);

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
    if (eval_expression_ex(asm_context, var) != 0) { return -1; }

    if (var.get_type() != VAR_INT)
    {
      print_error(asm_context, "Non-integer number in expression");
      return -1;
    }

    temp = var.get_int64();

    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (IS_NOT_TOKEN(token,')'))
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }
    else
  {
    print_error_unexp(asm_context, token);
    return -1;
  }

  if (operation == OPER_NOT) { *num = ~temp; }
  else if (operation == OPER_MINUS) { *num = -temp; }
  else { print_error_internal(NULL, __FILE__, __LINE__); return -1; }

  return 0;
}

int eval_expression_ex(AsmContext *asm_context, Var &var)
{
  EvalExpression::Operator oper;

  var.set_int(0);

  return EvalExpression::eval_expression_go(asm_context, var, &oper);
}

int eval_expression(AsmContext *asm_context, int *num)
{
  Var var;

  int ret = eval_expression_ex(asm_context, var);
  *num = var.get_int32();

  return ret;
}

