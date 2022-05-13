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
#include <stdint.h>

#include "asm/sweet16.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/sweet16.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
  OPERAND_REG,
  OPERAND_AT_REG,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_sweet16(char *token)
{
  if (token[0] == 'r' || token[0] == 'R')
  {
    if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
    {
      return token[1] - '0';
    }
      else
    if (token[3] == 0 && token[1] == '1' &&
       (token[2] >= '0' && token[2] <= '5'))
    {
      return 10 + (token[2] - '0');
    }
  }

  return -1;
}

int parse_instruction_sweet16(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int found = 0;
  int n;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count >= 2)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if (IS_TOKEN(token, '@'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = get_register_sweet16(token);

      if (n == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_AT_REG;
      operands[operand_count].value = n;
    }
      else
    {
      n = get_register_sweet16(token);

      if (n == -1)
      {
        tokens_push(asm_context, token, token_type);

        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          operands[operand_count].type = OPERAND_NUMBER;
          operands[operand_count].value = 0;
        }
          else
        {
          if (eval_expression(asm_context, &n) != 0)
          {
            print_error_illegal_expression(instr, asm_context);
            return -1;
          }

          operands[operand_count].type = OPERAND_NUMBER;
          operands[operand_count].value = n;
        }
      }
        else
      {
        operands[operand_count].type = OPERAND_REG;
        operands[operand_count].value = n;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_NOT_TOKEN(token, ','))
        {
          tokens_push(asm_context, token, token_type);
        }
      }
    }

    operand_count++;
  }

  n = 0;

  while (table_sweet16[n].instr != NULL)
  {
    if (strcmp(table_sweet16[n].instr, instr_case) == 0)
    {
      found = 1;

      switch (table_sweet16[n].type)
      {
        case SWEET16_OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_sweet16[n].opcode, IS_OPCODE);

          return 1;
        }
        case SWEET16_OP_REG:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_REG)
          {
            break;
          }

          add_bin8(asm_context, table_sweet16[n].opcode | operands[0].value, IS_OPCODE);

          return 1;
        }
        case SWEET16_OP_AT_REG:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_AT_REG)
          {
            break;
          }

          add_bin8(asm_context, table_sweet16[n].opcode | operands[0].value, IS_OPCODE);

          return 1;
        }
        case SWEET16_OP_EA:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            operands[0].value = asm_context->address;
          }

          int offset = operands[0].value - (asm_context->address + 2);

          add_bin8(asm_context, table_sweet16[n].opcode, IS_OPCODE);
          add_bin8(asm_context, offset & 0xff, IS_OPCODE);
          //add_bin8(asm_context, (data >> 8) & 0xff, IS_OPCODE);

          return 2;
        }
        case SWEET16_OP_REG_VALUE:
        {
          if (operand_count != 2 ||
              operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          uint16_t data = operands[1].value;

          add_bin8(asm_context, table_sweet16[n].opcode | operands[0].value, IS_OPCODE);
          add_bin8(asm_context, data & 0xff, IS_OPCODE);
          add_bin8(asm_context, (data >> 8) & 0xff, IS_OPCODE);

          return 3;
        }
        default:
          break;
      }
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

