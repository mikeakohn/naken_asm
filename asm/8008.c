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

#include "asm/8008.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/8008.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_REG,
  OPERAND_M,
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
};

int parse_instruction_8008(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int matched = 0;
  int token_type;
  int num, n;
  uint8_t opcode;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

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

    if (IS_TOKEN(token,'A') || IS_TOKEN(token,'a'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 0;
    }
      else
    if (IS_TOKEN(token,'B') || IS_TOKEN(token,'b'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 1;
    }
      else
    if (IS_TOKEN(token,'C') || IS_TOKEN(token,'c'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 2;
    }
      else
    if (IS_TOKEN(token,'D') || IS_TOKEN(token,'d'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 3;
    }
      else
    if (IS_TOKEN(token,'E') || IS_TOKEN(token,'e'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 4;
    }
      else
    if (IS_TOKEN(token,'H') || IS_TOKEN(token,'h'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 5;
    }
      else
    if (IS_TOKEN(token,'L') || IS_TOKEN(token,'l'))
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].value = 6;
    }
      else
    if (IS_TOKEN(token,'M') || IS_TOKEN(token,'m'))
    {
      operands[operand_count].type = OPERAND_M;
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        ignore_operand(asm_context);
        num = 0;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_NUMBER;
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

  for (n = 0; table_8008[n].instr != NULL; n++)
  {
    if (strcmp(table_8008[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_8008[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0) { continue; }

          add_bin8(asm_context, table_8008[n].opcode, IS_OPCODE);

          return 1;
        }
        case OP_SREG:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_REG)
          {
            opcode = table_8008[n].opcode | operands[0].value;
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_DREG:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_REG)
          {
            opcode = table_8008[n].opcode | (operands[0].value << 3);
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_DREG_NOT_A:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_REG && operands[0].value != 0)
          {
            opcode = table_8008[n].opcode | (operands[0].value << 3);
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_REG_REG:
        {
          if (operand_count != 2) { continue; }
          if (operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            opcode =
              table_8008[n].opcode |
              (operands[0].value << 3) |
               operands[1].value;
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_REG_M:
        {
          if (operand_count != 2) { continue; }
          if (operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_M)
          {
            opcode = table_8008[n].opcode | (operands[0].value << 3);
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_M_REG:
        {
          if (operand_count != 2) { continue; }
          if (operands[0].type == OPERAND_M &&
              operands[1].type == OPERAND_REG)
          {
            opcode = table_8008[n].opcode | operands[1].value;
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_M:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_M)
          {
            add_bin8(asm_context, table_8008[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_ADDRESS:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "address", operands[0].value, 0, 0xffff) == -1) { return -1; }
            add_bin8(asm_context, table_8008[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value >> 8, IS_OPCODE);
            return 3;
          }

          break;
        }
        case OP_IMMEDIATE:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "immediate", operands[0].value, -32768, 0xffff) == -1) { return -1; }
            add_bin8(asm_context, table_8008[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[0].value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_M_IMMEDIATE:
        {
          if (operand_count != 2) { continue; }
          if (operands[0].type == OPERAND_M &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "immediate", operands[1].value, -32768, 0xffff) == -1) { return -1; }
            add_bin8(asm_context, table_8008[n].opcode, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG_IMMEDIATE:
        {
          if (operand_count != 2) { continue; }
          if (operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "immediate", operands[1].value, -32768, 0xffff) == -1) { return -1; }
            opcode = table_8008[n].opcode | (operands[0].value << 3);
            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, operands[1].value, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_SUB:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_NUMBER)
          {
            if (operands[0].value >= 0xc0 ||
               (operands[0].value & 0x7) != 0)
            {
              print_error("Subroutine address needs to be a multiple of 8.",
                asm_context);
            }

            opcode = table_8008[n].opcode | operands[0].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_PORT_MMM:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "port", operands[0].value, 0, 7) == -1) { return -1; }
            opcode = table_8008[n].opcode | (operands[0].value << 1);
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case OP_PORT_MMM_NOT_0:
        {
          if (operand_count != 1) { continue; }
          if (operands[0].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "port", operands[0].value, 8, 15) == -1) { return -1; }
            opcode =
              table_8008[n].opcode |
              0x10 |
            ((operands[0].value - 8) << 1);
            add_bin8(asm_context, opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        default:
          break;
      }
    }
  }

  if (matched == 1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

