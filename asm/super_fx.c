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

#include "asm/super_fx.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/super_fx.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_REG,
  OPERAND_NUMBER,
  OPERAND_AT_ADDRESS,
  OPERAND_AT_REG,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_super_fx(char *token)
{
  int reg = 0;

  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  token++;

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }
    reg = (reg * 10) + (*token) - '0';

    token++;
  }

  if (reg >= 16) { return -1; }

  return reg;
}

int parse_instruction_super_fx(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int count;
  uint16_t opcode;
  int min, max;

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

    num = get_register_super_fx(token);

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
    if (IS_TOKEN(token,'('))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      num = get_register_super_fx(token);

      if (num != -1)
      {
        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_AT_REG;
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

        operands[operand_count].value = num;
        operands[operand_count].type = OPERAND_AT_ADDRESS;
      }

      if (expect_token(asm_context, ')') == -1) { return -1; }
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
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

  n = 0;
  while (table_super_fx[n].instr != NULL)
  {
    if (strcmp(table_super_fx[n].instr, instr_case) == 0)
    {
      if (table_super_fx[n].alt == 1)
      {
        add_bin8(asm_context, 0x3d, IS_OPCODE);
        count = 1;
      }
        else
      if (table_super_fx[n].alt == 2)
      {
        add_bin8(asm_context, 0x3e, IS_OPCODE);
        count = 1;
      }
        else
      if (table_super_fx[n].alt == 3)
      {
        add_bin8(asm_context, 0x3f, IS_OPCODE);
        count = 1;
      }

      switch (table_super_fx[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_super_fx[n].opcode, IS_OPCODE);

          return count + 1;
        }
        case OP_REG:
        case OP_ATREG:
        case OP_N:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (table_super_fx[n].type == OP_N)
          {
            min = 0; max = 15;

            if (table_super_fx[n].reg_mask != 0)
            {
              min = table_super_fx[n].reg_mask >> 8;
              max = table_super_fx[n].reg_mask & 0xff;
            }

            if (operands[0].value < min || operands[0].value > max)
            {
              print_error_range("Literal", min, max, asm_context);
              return -1;
            }
          }
            else
          {
            if (table_super_fx[n].type == OP_REG)
            {
              if (operands[0].type != OPERAND_REG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
            }
              else
            {
              if (operands[0].type != OPERAND_AT_REG)
              {
                print_error_illegal_operands(instr, asm_context);
                return -1;
              }
            }

            if (((1 << operands[0].value) & table_super_fx[n].reg_mask) == 0)
            {
              printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
              return -1;
            }
          }

          opcode = table_super_fx[n].opcode | operands[0].value;

          add_bin8(asm_context, opcode, IS_OPCODE);

          return count + 1;
        }
        case OP_OFFSET:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int offset = operands[0].value - (asm_context->address + 2);

          if (offset < -128 || operands[0].value > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_super_fx[n].opcode, IS_OPCODE);
          add_bin8(asm_context, offset & 0xff, IS_OPCODE);

          return count + 2;
        }
        case OP_REG_PP:
        case OP_REG_XX:
        case OP_REG_ATXX:
        case OP_REG_ATYY:
        case OP_ATXX_REG:
        case OP_ATYY_REG:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          int reg;
          int value = 0;
          int type = -1;
          int needed_type = -2;

          if (table_super_fx[n].type == OP_ATXX_REG ||
              table_super_fx[n].type == OP_ATYY_REG)
          {
            if (operands[1].type != OPERAND_REG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            reg = operands[1].value;
          }
            else
          {
            if (operands[0].type != OPERAND_REG)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }
            reg = operands[0].value;
          }

          value = operands[1].value;
          type = operands[1].type;

          switch (table_super_fx[n].type)
          {
            case OP_REG_PP:
              min = -128; max = 255;
              needed_type = OPERAND_NUMBER;
              break;
            case OP_REG_XX:
              min = -32768; max = 0xffff;
              needed_type = OPERAND_NUMBER;
              break;
            case OP_REG_ATXX:
              min = 0; max = 0xffff;
              needed_type = OPERAND_AT_ADDRESS;
              break;
            case OP_REG_ATYY:
              min = 0; max = 0x1fe;
              needed_type = OPERAND_AT_ADDRESS;
              if ((value & 1) == 1)
              {
                printf("Error: Short address must be even at %s:%d.\n", asm_context->tokens.filename, asm_context->tokens.line);
                return -1;
              }
              break;
            case OP_ATXX_REG:
              min = 0; max = 0xffff;
              needed_type = OPERAND_AT_ADDRESS;
              value = operands[0].value;
              type = operands[0].type;
              break;
            case OP_ATYY_REG:
              min = 0; max = 0x1fe;
              needed_type = OPERAND_AT_ADDRESS;
              value = operands[0].value;
              type = operands[0].type;
              if ((value & 1) == 1)
              {
                printf("Error: Short address must be even at %s:%d.\n", asm_context->tokens.filename, asm_context->tokens.line);
                return -1;
              }
              break;
            default:
              // Should never hit here.
              min = 0; max = 0;
          }

          if (type != needed_type)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << reg) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
            return -1;
          }

          if (value < min || value > max)
          {
            print_error_range("Literal", min, max, asm_context);
            return -1;
          }

          if (table_super_fx[n].type == OP_REG_ATYY ||
              table_super_fx[n].type == OP_ATYY_REG)
          {
            value = value >> 1;
          }

          opcode = table_super_fx[n].opcode | reg;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, value & 0xff, IS_OPCODE);

          if (table_super_fx[n].type == OP_REG_ATXX ||
              table_super_fx[n].type == OP_ATXX_REG ||
              table_super_fx[n].type == OP_REG_XX)
          {
            add_bin8(asm_context, (value >> 8) & 0xff, IS_OPCODE);
            count++;
          }

          return count + 2;
        }
        default:
          break;
      }
    }

    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

