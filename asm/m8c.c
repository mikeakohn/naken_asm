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
#include <ctype.h>

#include "asm/common.h"
#include "asm/m8c.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/m8c.h"

struct _operand
{
  uint8_t type;
  int value;
};

int add_operand(
  struct _asm_context *asm_context,
  struct _operand *operand,
  uint8_t opcode)
{
  switch (operand->type)
  {
    case OP_NONE:
    case OP_A:
    case OP_X:
    case OP_F:
    case OP_SP:
      return 0;
    case OP_EXPR:
    case OP_INDEX_EXPR:
    case OP_INDEX_X_EXPR:
      add_bin8(asm_context, opcode, IS_OPCODE);
      return 0;
    case OP_INDEX_EXPR_INC:
    case OP_REG_INDEX_EXPR:
    case OP_REG_INDEX_X_EXPR:
      break;
  }

  return -1;
}

int ignore_expression(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOF || token_type == TOKEN_EOL) { break; }

    if (IS_TOKEN(token, ']') || IS_TOKEN(token, ','))
    {
      break;
    }
  }

  tokens_push(asm_context, token, token_type);

  return 0;
}

int parse_instruction_m8c(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[2];
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int found = 0;
  int n, num;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while(1)
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

    if (strcasecmp(token, "A") == 0)
    {
      operands[operand_count].type = OP_A;
    }
      else
    if (strcasecmp(token, "X") == 0)
    {
      operands[operand_count].type = OP_X;
    }
      else
    if (strcasecmp(token, "F") == 0)
    {
      operands[operand_count].type = OP_F;
    }
      else
    if (strcasecmp(token, "SP") == 0)
    {
      operands[operand_count].type = OP_SP;
    }
      else
    if (IS_TOKEN(token, '['))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, 'X'))
      {
        if (expect_token(asm_context, '+') == -1) { return -1; }

        if (asm_context->pass == 1)
        {
          ignore_expression(asm_context);
        }
          else
        {
          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OP_INDEX_X_EXPR;
          operands[operand_count].value = num;
        }

        if (expect_token(asm_context, ']') == -1) { return -1; }
      }
        else
      if (IS_TOKEN(token, '['))
      {
        if (asm_context->pass == 1)
        {
          ignore_expression(asm_context);
        }
          else
        {
          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OP_INDEX_EXPR_INC;
          operands[operand_count].value = num;
        }

        if (expect_token(asm_context, ']') == -1) { return -1; }
        if (expect_token(asm_context, '+') == -1) { return -1; }
        if (expect_token(asm_context, '+') == -1) { return -1; }
        if (expect_token(asm_context, ']') == -1) { return -1; }
      }
        else
      {
        if (asm_context->pass == 1)
        {
          ignore_expression(asm_context);
        }
          else
        {
          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OP_INDEX_EXPR;
          operands[operand_count].value = num;
        }

        if (expect_token(asm_context, ']') == -1) { return -1; }
      }
    }
      else
    {
      if (asm_context->pass == 1)
      {
        ignore_expression(asm_context);
      }
        else
      {
        if (eval_expression(asm_context, &num) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].type = OP_INDEX_EXPR;
        operands[operand_count].value = num;
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  n = 0;

  while (table_m8c[n].instr != NULL)
  {
    if (strcmp(table_m8c[n].instr, instr_case) == 0)
    {
      found = 1;
    }

    if (table_m8c[n].operand_0 == operands[0].type &&
        table_m8c[n].operand_1 == operands[1].type)
    {
      uint32_t address = asm_context->address;
      const uint8_t opcode = table_m8c[n].opcode;

      add_bin8(asm_context, opcode, IS_OPCODE);

      if (table_m8c[n].operand_0 == OP_NONE)
      {
        return 1;
      }

      if (add_operand(asm_context, &operands[0], opcode) != 0)
      {
        return -1;
      }

      if (add_operand(asm_context, &operands[1], opcode) != 0)
      {
        return -1;
      }

      if (asm_context->address - address != table_m8c[n].byte_count)
      {
        print_error_internal(asm_context, __FILE__, __LINE__);
        exit(1);
      }

      return table_m8c[n].byte_count;
    }

    n++;
  }

  if (found == 1)
  {
    print_error_illegal_operands(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

