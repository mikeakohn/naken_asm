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

#include "asm/lc3.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/lc3.h"

#define MAX_OPERANDS 3

enum
{
  OPERAND_REG,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_lc3(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  token++;

  if (token[0] != 0 && token[1] == 0)
  {
    if (*token < '0' || *token > '7') { return -1; }

    return (*token) - '0';
  }

  return -1;
}

int parse_instruction_lc3(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int offset, value;
  uint16_t opcode;
  int found = 0;
  int nzp = 0;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  if (instr_case[0] == 'b' && instr_case[1] == 'r')
  {
    n = 2;
    while (instr_case[n] != 0)
    {
      if (instr_case[n] == 'n' || instr_case[n] == 'N')
      {
        nzp |= 4;
      }
        else
      if (instr_case[n] == 'z' || instr_case[n] == 'Z')
      {
        nzp |= 2;
      }
        else
      if (instr_case[n] == 'p' || instr_case[n] == 'P')
      {
        nzp |= 1;
      }
        else
      {
        nzp = 0;
        break;
      }
    }

    if (nzp != 0) { instr_case[2] = 0; }
  }

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

    num = get_register_lc3(token);

    if (num != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_REG;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          num = 0;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = num;
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          num = 0;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_ADDRESS;
      operands[operand_count].value = num;
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == MAX_OPERANDS)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  n = 0;
  while (table_lc3[n].instr != NULL)
  {
    if (strcmp(table_lc3[n].instr, instr_case) == 0)
    {
      found = 1;

      switch (table_lc3[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin16(asm_context, table_lc3[n].opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_R_R_R:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_REG)
          {
            opcode = table_lc3[n].opcode |
                     (operands[0].value << 9) |
                     (operands[1].value << 6) |
                      operands[2].value;
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_R_R_IMM5:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (asm_context->pass == 2)
            {
              value = operands[2].value;
            }
              else
            {
              value = 0;
            }

            if (value < -16 || value > 15)
            {
              print_error_range("Constant", -16, 15, asm_context);
              return -1;
            }

            opcode = table_lc3[n].opcode |
                     (operands[0].value << 9) |
                     (operands[1].value << 6) |
                     (value & 0x1f);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_BR:
        {
          if (operand_count == 1 &&
              operands[0].type == OPERAND_ADDRESS)
          {
            if (asm_context->pass == 2)
            {
              offset = operands[0].value - ((asm_context->address / 2) + 1);
            }
              else
            {
              offset = 0;
            }

            if (offset < -256 || offset > 255)
            {
              print_error_range("Offset", -256, 255, asm_context);
              return -1;
            }

            if (nzp == 0) { nzp = 7; }

            opcode = table_lc3[n].opcode |
                     (nzp << 9) |
                     (offset & 0x1ff);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_BASER:
        {
          if (operand_count == 1 &&
              operands[0].type == OPERAND_REG)
          {
            opcode = table_lc3[n].opcode | (operands[0].value << 6);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_OFFSET11:
        {
          if (operand_count == 1 &&
              operands[0].type == OPERAND_ADDRESS)
          {
            if (asm_context->pass == 2)
            {
              offset = operands[0].value - ((asm_context->address / 2) + 1);
            }
              else
            {
              offset = 0;
            }

            if (offset < -1024 || offset > 1023)
            {
              print_error_range("Offset", -1024, 1023, asm_context);
              return -1;
            }

            opcode = table_lc3[n].opcode | (offset & 0x7ff);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_R_OFFSET9:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (asm_context->pass == 2)
            {
              offset = operands[1].value - ((asm_context->address / 2) + 1);
            }
              else
            {
              offset = 0;
            }

            if (offset < -256 || offset > 255)
            {
              print_error_range("Offset", -256, 255, asm_context);
              return -1;
            }

            opcode = table_lc3[n].opcode |
                     (operands[0].value << 9) |
                     (offset & 0x1ff);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_R_R_OFFSET6:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (operands[2].value < -32 || operands[2].value > 31)
            {
              print_error_range("Offset", -32, 31, asm_context);
              return -1;
            }

            opcode = table_lc3[n].opcode |
                     (operands[0].value << 9) |
                     (operands[1].value << 6) |
                     (operands[2].value & 0x3f);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_R_R:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            opcode = table_lc3[n].opcode |
                     (operands[0].value << 9) |
                     (operands[1].value << 6);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_VECTOR:
        {
          if (operand_count == 1 &&
              operands[0].type == OPERAND_ADDRESS)
          {
            if (operands[0].value < 0 || operands[0].value > 255)
            {
              print_error_range("Vector", 0, 255, asm_context);
              return -1;
            }

            opcode = table_lc3[n].opcode | operands[0].value;
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
          break;
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
    return -1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

