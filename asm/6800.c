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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/6800.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/6800.h"

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_ADDRESS_COMMA_X,
};

int parse_instruction_6800(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int operand_type;
  int operand_value;
  int address_size = 0;
  int opcode = -1;
  int n;

  lower_copy(instr_case, instr);

  do
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      operand_type = OPERAND_NONE;
      break;
    }

    if (token_type == TOKEN_POUND)
    {
      operand_type = OPERAND_NUMBER;
      if (eval_expression(asm_context, &operand_value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        operand_value = 0xffff;
      }
    }
      else
    {
      operand_type = OPERAND_ADDRESS;
      tokens_push(asm_context, token, token_type);
      if (eval_expression(asm_context, &operand_value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        operand_value = 0xffff;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context, "x") != 0) { return -1; }
      operand_type = OPERAND_ADDRESS_COMMA_X;
    }
  } while (0);

  for (n = 0; n < 256; n++)
  {
    if (table_6800[n].instr == NULL) { continue; }

    if (strcmp(instr_case, table_6800[n].instr) == 0)
    {
      if (table_6800[n].operand_type == M6800_OP_NONE &&
          operand_type == OPERAND_NONE)
      {
        add_bin8(asm_context, n, IS_OPCODE);
        return 1;
      }
        else
      if (table_6800[n].operand_type == M6800_OP_REL_OFFSET &&
          operand_type == OPERAND_ADDRESS)
      {
        int offset = operand_value - (asm_context->address + 2);
        if (asm_context->pass != 1)
        {
          if (offset < -128 || offset > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }
        }
        add_bin8(asm_context, n, IS_OPCODE);
        add_bin8(asm_context, offset & 0xff, IS_OPCODE);

        return 1;
      }
        else
      if (table_6800[n].operand_type == M6800_OP_IMM16 &&
          operand_type == OPERAND_NUMBER)
      {
        add_bin8(asm_context, n, IS_OPCODE);
        add_bin8(asm_context, operand_value >> 8, IS_OPCODE);
        add_bin8(asm_context, operand_value & 0xff, IS_OPCODE);
        return 3;
      }
        else
      if (table_6800[n].operand_type == M6800_OP_IMM8 &&
          operand_type == OPERAND_NUMBER)
      {
        if (asm_context->pass != 1)
        {
          if (operand_value < -128 || operand_value > 255)
          {
            print_error_range("Offset", -128, 255, asm_context);
            return -1;
          }
        }
        add_bin8(asm_context, n, IS_OPCODE);
        add_bin8(asm_context, operand_value & 0xff, IS_OPCODE);
        return 2;
      }
        else
      if (table_6800[n].operand_type == M6800_OP_NN_X &&
          operand_type == OPERAND_ADDRESS_COMMA_X)
      {
        if (asm_context->pass != 1)
        {
          if (operand_value < 0 || operand_value > 255)
          {
            print_error_range("Index", 0, 255, asm_context);
            return -1;
          }
        }
        add_bin8(asm_context, n, IS_OPCODE);
        add_bin8(asm_context, operand_value & 0xff, IS_OPCODE);
        return 2;
      }
        else
      if (table_6800[n].operand_type == M6800_OP_DIR_PAGE_8 &&
          operand_type == OPERAND_ADDRESS)
      {
        if (asm_context->pass == 1)
        {
          if (address_size == 0 && (operand_value >= 0 && operand_value <= 255))
          {
            address_size = 1;
            opcode = n;
          }
        }

        if (memory_read_m(&asm_context->memory, asm_context->address) == 1)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operand_value, IS_OPCODE);
          return 2;
        }
      }
        else
      if (table_6800[n].operand_type == M6800_OP_ABSOLUTE_16 &&
          operand_type == OPERAND_ADDRESS)
      {
        if (asm_context->pass == 1)
        {
          if (address_size == 0)
          {
            address_size = 2;
            opcode=n;
          }
        }

        if (memory_read_m(&asm_context->memory, asm_context->address) == 2)
        {
          add_bin8(asm_context, n, IS_OPCODE);
          add_bin8(asm_context, operand_value >> 8, IS_OPCODE);
          add_bin8(asm_context, operand_value & 0xff, IS_OPCODE);
          return 2;
        }
      }
    }
  }

  if (opcode != -1)
  {
    add_bin8(asm_context, address_size, IS_OPCODE);
    add_bin8(asm_context, address_size, IS_OPCODE);
    if (address_size == 2)
    {
      if (operand_value < 0 || operand_value > 0xffff)
      {
        print_error_range("Address", 0, 0xffff, asm_context);
        return -1;
      }
      add_bin8(asm_context, address_size, IS_OPCODE);
      return 3;
    }
      else
    {
      return 2;
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

