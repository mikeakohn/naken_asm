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
#include <string.h>

#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/Operator.h"
#include "common/tokens.h"

int EvalExpression::run(
  AsmContext *asm_context,
  Var &var,
  Operator &last_oper)
{
  char token[TOKENLEN];
  int token_type;
  VarStack var_stack;
  Operator oper;
  int last_token_was_op = -1;

  oper = last_oper;
  var_stack.push(var);

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

#ifdef DEBUG
printf("eval_expression> token=%s   var_stack_ptr=%d\n", token, var_stack_ptr);
#endif

    // Issue 15: Return an error if a stack is full with no operator.
    if (var_stack.size() == 3 && oper.is_unset())
    {
      return -1;
    }

    if (token_type == TOKEN_QUOTED)
    {
      if (get_quoted_literal(asm_context, token, sizeof(token)) != 0)
      {
        return -1;
      }

      token_type = TOKEN_NUMBER;
    }

    // Open and close parenthesis
    if (IS_TOKEN(token, '('))
    {
      if (last_token_was_op == 0 && oper.is_set())
      {
        last_oper.execute(
          var_stack.get_second(),
          var_stack.get_last());

        var_stack.pop();
        oper.reset();

        var = var_stack.pop();

        tokens_push(asm_context, token, token_type);
        return 0;
      }

      if (oper.is_unset() && var_stack.size() == 2)
      {
        // This is probably the x(r12) case.. so this is actually okay.
        var = var_stack.pop();
        tokens_push(asm_context, token, token_type);
        return 0;
      }

      Var paren_var;
      Operator paren_operator;

      paren_var.clear();

      if (run(asm_context, paren_var, paren_operator) != 0)
      {
        return -1;
      }

      last_token_was_op = 0;

#ifdef DEBUG
printf("Paren got back %d/%f/%d\n", var_get_int32(&paren_var), var_get_float(&paren_var), var_get_type(&paren_var));
#endif

      var_stack.push(paren_var);

      token_type = tokens_get(asm_context, token, TOKENLEN);

      //if (!(token[0] == ')' && token[1] == 0))
      if (IS_NOT_TOKEN(token, ')'))
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
    if (IS_TOKEN(token, ',') ||
        IS_TOKEN(token, ']') ||
        IS_TOKEN(token, '.') ||
        token_type == TOKEN_EOF)
    {
      tokens_push(asm_context, token, token_type);
      break;
    }

    if (token_type == TOKEN_EOL)
    {
      tokens_push(asm_context, token, token_type);
      break;
    }

    //bool do_minus = false;

#if 0
    if (oper.is_set() && IS_TOKEN(token, '-'))
    {
      do_minus = true;
      token_type = tokens_get(asm_context, token, TOKENLEN);
    }
#endif

    // Read number.
    if (token_type == TOKEN_NUMBER)
    {
      last_token_was_op = 0;

      if (var_stack.size() == 3)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      var_stack.push_int(token);
    }
      else
    if (token_type == TOKEN_FLOAT)
    {
      if (var_stack.size() == 3)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      var_stack.push_float(token);
    }
      else
    if (token_type == TOKEN_SYMBOL)
    {
      last_token_was_op = 1;

      Operator operator_prev = oper;

      if (oper.set_operator(token) == false)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      // Issue 15: 2015-July-21 mkohn - If operator is ~ then reverse
      // the next number.
      if (oper.operation == Operator::OPER_NOT)
      {
        int64_t num;

        if (parse_unary(asm_context, &num, Operator::OPER_NOT) != 0)
        {
          return -1;
        }

        if (var_stack.size() == 3)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        var_stack.push_int(num);

        oper = operator_prev;
        last_token_was_op = 0;

        continue;
      }

      if (var_stack.is_empty())
      {
        printf("Error: Unexpected operator '%s' at %s:%d\n",
          token,
          asm_context->tokens.filename,
          asm_context->tokens.line);
        return -1;
      }

      if (last_oper.is_unset())
      {
        last_oper = oper;
      }
        else
      if (last_oper.precedence > oper.precedence)
      {
        Var var = var_stack.pop();

        if (run(asm_context, var, oper) == -1)
        {
          return -1;
        }

        var_stack.push(var);
      }
        else
      {
        last_oper.execute(
          var_stack.get_second(),
          var_stack.get_last());

        var_stack.pop();
        last_oper = oper;
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
printf("going to leave  operation=%d\n", last_oper.operation);
PRINT_STACK()
#endif

  if (last_oper.is_set())
  {
    last_oper.execute(
      var_stack.get_second(),
      var_stack.get_last());

    var_stack.pop();
  }

  var = var_stack.pop();

  return 0;
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

  //var.clear();

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token, '-'))
  {
    if (parse_unary(asm_context, &temp, Operator::OPER_MINUS) == -1)
    {
      return -1;
    }
  }
    else
  if (IS_TOKEN(token, '~'))
  {
    if (parse_unary(asm_context, &temp, Operator::OPER_NOT) != 0)
    {
      return -1;
    }
  }
    else
  if (token_type == TOKEN_NUMBER)
  {
    temp = atoll(token);
  }
    else
  if (IS_TOKEN(token, '('))
  {
    if (eval_expression(asm_context, var) != 0) { return -1; }

    if (var.get_type() != VAR_INT)
    {
      print_error(asm_context, "Non-integer number in expression");
      return -1;
    }

    temp = var.get_int64();

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_NOT_TOKEN(token, ')'))
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

  switch (operation)
  {
    case Operator::OPER_NOT:
      *num = ~temp;
      break;
    case Operator::OPER_MINUS:
      *num = -temp;
      break;
    default:
      print_error_internal(NULL, __FILE__, __LINE__);
      return -1;
  }

  return 0;
}

int EvalExpression::get_quoted_literal(
  AsmContext *asm_context,
  char *token,
  int length)
{
  if (token[0] == '\\')
  {
    int e = tokens_escape_char(asm_context, (uint8_t *)token);
    if (e == 0) { return -1; }

    if (token[e + 1] != 0)
    {
      print_error(asm_context, "Quoted literal too long.");
      return -1;
    }

    snprintf(token, length, "%d", token[e]);
  }
    else
  {
    if (token[1] != 0)
    {
      print_error(asm_context, "Quoted literal too long.");
      return -1;
    }

    snprintf(token, length, "%d", token[0]);
  }

  return 0;
}

int eval_expression(AsmContext *asm_context, Var &var)
{
  Operator oper;

  var.clear();

  return EvalExpression::run(asm_context, var, oper);
}

int eval_expression(AsmContext *asm_context, int *num)
{
  Var var;

  int ret = eval_expression(asm_context, var);
  *num = var.get_int32();

  return ret;
}

