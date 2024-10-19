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

int EvalExpression::run(AsmContext *asm_context, Var &answer, bool is_paren)
{
  char token[TOKENLEN];
  int token_type;
  VarStack var_stack;
  OperStack oper_stack;
  int count = 0;

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      tokens_push(asm_context, token, token_type);
      break;
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
      // This is probably the x(r12) case.. so this is actually okay.
      if (need_symbol(count))
      {
        tokens_push(asm_context, token, token_type);
        break;
      }

      Var var;
      if (run(asm_context, var, true) != 0) { return -1; }
      var_stack.push(var);
      count++;
    }
      else
    if (IS_TOKEN(token, ')'))
    {
      if (is_paren == false)
      {
        // This is probably the end of some instruction syntax such as
        // Z80 "and (ix+5)".
        tokens_push(asm_context, token, token_type);
        break;
      }

      break;
    }
      else
    if (IS_TOKEN(token, ',') ||
        IS_TOKEN(token, ']') ||
        IS_TOKEN(token, '[') ||
        IS_TOKEN(token, '.') ||
        token_type == TOKEN_EOF)
    {
      if (is_paren == true)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      // End of expression
      tokens_push(asm_context, token, token_type);
      break;
    }
      else
    if (token_type == TOKEN_NUMBER)
    {
      // 0: empty
      // 1: num
      // 2:   oper
      // 3: num
      // 4:   oper
      // 5: (num)

      if (need_symbol(count) || var_stack.size() == 3)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      var_stack.push_int(token);
      count++;
    }
      else
    if (token_type == TOKEN_FLOAT)
    {
      if (need_symbol(count) || var_stack.size() == 3)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      var_stack.push_float(token);
      count++;
    }
      else
    if (token_type == TOKEN_SYMBOL)
    {
      if (need_number(count))
      {
        Var var;

        if (IS_TOKEN(token, '+') && count == 0)
        {
          // If the expression starts with +.
          // Was needed for Z80 "and (ix+5)".
          Operator oper;
          oper.set_operator("+");
          var_stack.push_int((uint64_t)0);
          oper_stack.push(oper);
          count += 2;
        }
          else
        if (IS_TOKEN(token, '-'))
        {
          // Needed for: 6 + -5.
          parse_unary_new(asm_context, var);
          var.negative();
          var_stack.push(var);
          count++;
        }
          else
        if (IS_TOKEN(token, '~'))
        {
          // Needed for: ~0xfe.
          parse_unary_new(asm_context, var);
          var.complement();
          var_stack.push(var);
          count++;
        }
          else
        {
          print_error_unexp(asm_context, token);
          return -1;
        }
      }
        else
      {
        if (var_stack.is_empty() || need_symbol(count) == false)
        {
          printf("Error: Unexpected operator '%s' at %s:%d\n",
            token,
            asm_context->tokens.filename,
            asm_context->tokens.line);
          return -1;
        }

        Operator oper;

        if (oper.set_operator(token) == false)
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        oper_stack.push(oper);
        count++;
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

    if (var_stack.size() == 3)
    {
      if (oper_stack.size() != 2)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      if (execute_stack(var_stack, oper_stack) != 0) { return  -1; }
      count -= 2;
    }
  }

  if (var_stack.is_empty()) { return -1; }

  while (var_stack.size() > 1 && oper_stack.is_empty() == false)
  {
    if (execute_stack(var_stack, oper_stack) != 0) { return  -1; }
  }

  answer = var_stack.pop();

  return 0;
}

int EvalExpression::execute_stack(VarStack &var_stack, OperStack &oper_stack)
{
  Operator oper;
  Var d;
  Var s;

  if (oper_stack.get_precedence_index() == 0)
  {
    oper = oper_stack.pop_first();

    d = var_stack.pop_first();
    s = var_stack.pop_first();

    if (oper.execute(d, s) != 0) { return -1; }
    var_stack.push_front(d);
  }
    else
  {
    oper = oper_stack.pop();

    s = var_stack.pop();
    d = var_stack.pop();

    if (oper.execute(d, s) != 0) { return -1; }
    var_stack.push(d);
  }

  return 0;
}

int EvalExpression::parse_unary_new(AsmContext *asm_context, Var &answer)
{
  char token[TOKENLEN];
  int token_type;

  answer.clear();

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (token_type == TOKEN_NUMBER)
  {
    answer.set_int(token);
  }
    else
  if (token_type == TOKEN_FLOAT)
  {
    answer.set_float(token);
  }
    else
  if (IS_TOKEN(token, '('))
  {
    if (run(asm_context, answer, true) != 0) { return -1; }
  }
    else
  if (IS_TOKEN(token, '~'))
  {
    if (parse_unary_new(asm_context, answer) != 0) { return -1; }
    answer.complement();
  }
    else
  if (IS_TOKEN(token, '-'))
  {
    if (parse_unary_new(asm_context, answer) != 0) { return -1; }
    answer.negative();
  }
    else
  {
    print_error_unexp(asm_context, token);
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

int eval_expression(AsmContext *asm_context, Var &answer)
{
  answer.clear();

  return EvalExpression::run(asm_context, answer, false);
}

int eval_expression(AsmContext *asm_context, int *num)
{
  Var answer;

  int ret = eval_expression(asm_context, answer);
  *num = answer.get_int32();

  return ret;
}

