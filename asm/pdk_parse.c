/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include "asm/pdk_parse.h"
#include "asm/common.h"
#include "common/eval_expression.h"

int pdk_parse(struct _asm_context *asm_context, struct _operand *operands)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int num;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    if (IS_TOKEN(token, 'a') || IS_TOKEN(token, 'A'))
    {
      operands[operand_count].type = OPERAND_A;
    }
      else
    if (IS_TOKEN(token, '['))
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_ADDRESS;

      if (expect_token(asm_context, ']') == -1) { return -1; }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '.'))
      {
        if (eval_expression(asm_context, &num) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].bit = num;
        operands[operand_count].type = OPERAND_ADDRESS_BIT_OFFSET;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_IMMEDIATE;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, '.'))
      {
        if (eval_expression(asm_context, &num) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].bit = num;
        operands[operand_count].type = OPERAND_BIT_OFFSET;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return operand_count;
}

