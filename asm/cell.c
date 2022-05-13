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
#include "asm/cell.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/cell.h"

#define MAX_OPERANDS 5

enum
{
  OPERAND_INVALID,
  OPERAND_REGISTER,
  OPERAND_SPR,
  OPERAND_TBR,
  OPERAND_CR,
  OPERAND_NUMBER,
  OPERAND_REGISTER_OFFSET,
};

struct _operand
{
  int value;
  int type;
  int32_t offset;
};

struct _modifiers
{
  int has_dot;
};

static int get_register_number(char *token)
{
  int num = 0;

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }

    num = (num * 10) + (*token - '0');
    token++;

    if (num > 127) { return -1; }
  }

  return num;
}

static int get_register_cell(char *token)
{
  if (token[0] != 'r') { return -1; }

  return get_register_number(token + 1);
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int n;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    do
    {
      // Check for registers
      n = get_register_cell(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      // Check if this is (reg)
      if (IS_TOKEN(token, '('))
      {
        char token[TOKENLEN];
        int token_type;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        n = get_register_cell(token);

        if (n != -1)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, ')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].offset = 0;
          operands[operand_count].type = OPERAND_REGISTER_OFFSET;
          operands[operand_count].value = n;

          break;
        }

        tokens_push(asm_context, token, token_type);
      }

      // Assume this is just a number
      operands[operand_count].type = OPERAND_NUMBER;

      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        operands[operand_count].value = 0;
      }
      else
      {
        tokens_push(asm_context, token, token_type);
        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].value = n;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token, '('))
        {
          if (operands[operand_count].value < -32768 ||
              operands[operand_count].value > 32767)
          {
            print_error_range("Offset", -32768, 32767, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          n = get_register_cell(token);
          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].offset = operands[operand_count].value;
          operands[operand_count].type = OPERAND_REGISTER_OFFSET;
          operands[operand_count].value = n;

          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, ')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }
      }

      break;
    } while (0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count == MAX_OPERANDS)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return operand_count;
}

int parse_instruction_cell(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  int matched = 0;
  uint32_t opcode;
  int32_t address;
  int32_t offset;
  //int temp;
  int n;

  memset(&operands, 0, sizeof(operands));

  lower_copy(instr_case, instr);

  operand_count = get_operands(asm_context, operands, instr, instr_case);

  if (operand_count < 0) { return -1; }

  n = 0;
  while (table_cell[n].instr != NULL)
  {
    if (strcmp(table_cell[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_cell[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin32(asm_context, table_cell[n].opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_S10_RA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER_OFFSET)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = operands[1].offset;

          if (offset < -(1 << 13) || offset >= (1 << 13))
          {
            print_error_range("Offset", -(1 << 13), (1 << 13) - 1, asm_context);
            return -1;
          }

          if ((offset & 0xf) != 0)
          {
            print_error_align(asm_context, 16);
            return -1;
          }

          offset = (offset >> 4) & 0x3ff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (operands[1].value << 7) |
                  (offset << 14);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_RA_S10:
        case OP_RT_RA_U10:
        case OP_RT_RA_U7:
        case OP_RT_RA_S6:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int value = operands[2].value;

          if (table_cell[n].type == OP_RT_RA_S10)
          {
            if (value < -(1 << 9) || value >= (1 << 9))
            {
              print_error_range("Constant", -(1 << 9), (1 << 9) - 1, asm_context);
              return -1;
            }

            value &= 0x3ff;
          }
            else
          if (table_cell[n].type == OP_RT_RA_U10)
          {
            if (value < 0 || value >= (1 << 10))
            {
              print_error_range("Constant", 0, (1 << 10) - 1, asm_context);
              return -1;
            }

            value &= 0x3ff;
          }
            else
          if (table_cell[n].type == OP_RT_RA_U7)
          {
            if (value < 0 || value >= (1 << 7))
            {
              print_error_range("Constant", 0, (1 << 7) - 1, asm_context);
              return -1;
            }

            value &= 0x7f;
          }
            else
          if (table_cell[n].type == OP_RT_RA_S6)
          {
            if (value < -64 || value >= 64)
            {
              print_error_range("Constant", -64, 63, asm_context);
              return -1;
            }

            value &= 0x7f;
          }

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (operands[1].value << 7) |
                  (value << 14);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_RA:
        case OP_RA_RB:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_cell[n].type == OP_RT_RA)
          {
            opcode = table_cell[n].opcode |
                    (operands[0].value << 0) |
                    (operands[1].value << 7);
          }
            else
          {
            opcode = table_cell[n].opcode |
                    (operands[0].value << 7) |
                    (operands[1].value << 14);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RA:
        case OP_RT:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_cell[n].type == OP_RA)
          {
            opcode = table_cell[n].opcode | (operands[0].value << 7);
          }
            else
          {
            opcode = table_cell[n].opcode | operands[0].value;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_RA_RB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (operands[1].value << 7) |
                  (operands[2].value << 14);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RA_RB_RC:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          // This is disagreeing with documentation
          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (operands[1].value << 7) |
                  (operands[2].value << 14);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_RA_RB_RC:
        {
          if (operand_count != 4)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER ||
              operands[3].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_cell[n].opcode |
                  (operands[3].value << 0) |
                  (operands[1].value << 7) |
                  (operands[2].value << 14) |
                  (operands[0].value << 21);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_ADDRESS:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          address = operands[1].value;

          if (address < 0 || address >= (1 << 18))
          {
            print_error_range("Address", 0, (1 << 18) - 1, asm_context);
            return -1;
          }

          if ((address & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          address = (address >> 2) & 0xffff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (address << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_RELATIVE:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          address = operands[1].value;

          if (address < -(1 << 17) || address >= (1 << 17))
          {
            print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context);
            return -1;
          }

          if ((address & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          address = (address >> 2) & 0xffff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (address << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_S16:
        case OP_RT_U16:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int32_t data = operands[1].value;

          if (table_cell[n].type == OP_RT_S16)
          {
            if (data < -(1 << 15) || data >= (1 << 15))
            {
              print_error_range("Immediate", -(1 << 15), (1 << 15) - 1, asm_context);
              return -1;
            }
          }
            else
          {
            if (data < -(1 << 15) || data >= (1 << 16))
            {
              print_error_range("Immediate", -(1 << 15), (1 << 16) - 1, asm_context);
              return -1;
            }
          }

          data = data & 0xffff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (data << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_U18:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int32_t data = operands[1].value;

          if (data < 0 || data >= (1 << 18))
          {
            print_error_range("Immediate", 0, (1 << 18) - 1, asm_context);
            return -1;
          }

          data = data & 0x3ffff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (data << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_S7_RA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER_OFFSET)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = operands[1].offset;

          if (offset < -(1 << 6) || offset >= (1 << 6))
          {
            print_error_range("Offset", -(1 << 6), (1 << 6), asm_context);
            return -1;
          }

#if 0
          if ((offset & 0xf) != 0)
          {
            print_error_align(asm_context, 16);
            return -1;
          }
#endif

          offset = offset & 0x7f;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (operands[1].value << 7) |
                  (offset << 14);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RA_S10:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int value = operands[1].value;

          if (value < -(1 << 13) || value >= (1 << 13))
          {
            print_error_range("Offset", -(1 << 13), (1 << 13) - 1, asm_context);
            return -1;
          }

          value &= 0x3ff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 7) |
                  (value << 14);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BRANCH_RELATIVE:
        case OP_BRANCH_ABSOLUTE:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_cell[n].type == OP_BRANCH_RELATIVE)
          {
            address = operands[0].value - asm_context->address;

            if (address < -(1 << 17) || address >= (1 << 17))
            {
              print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context);
              return -1;
            }
          }
            else
          {
            address = operands[0].value;

            if (address < 0 || address >= (1 << 18))
            {
              print_error_range("Address", 0, (1 << 18) - 1, asm_context);
              return -1;
            }
          }


          if ((address & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          address = (address >> 2) & 0xffff;

          opcode = table_cell[n].opcode | (address << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BRANCH_RELATIVE_RT:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          address = operands[1].value - asm_context->address;

          if (address < -(1 << 17) || address >= (1 << 17))
          {
            print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context);
            return -1;
          }

          if ((address & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          address = (address >> 2) & 0xffff;

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (address << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_HINT_RELATIVE_RO_RA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = operands[0].value - asm_context->address;

          if (offset < -(1 << 17) || offset >= (1 << 17))
          {
            print_error_range("Offset", -(1 << 10), (1 << 10) - 1, asm_context);
            return -1;
          }

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          offset = (offset >> 2) & 0x01ff;

          opcode = table_cell[n].opcode |
                  (((offset >> 7) & 0x3) << 14) |
                  (operands[1].value << 7) |
                  (offset & 0x7f);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_HINT_ABSOLUTE_RO_I16:
        case OP_HINT_RELATIVE_RO_I16:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, 0, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = operands[0].value - asm_context->address;

          if (offset < -(1 << 17) || offset >= (1 << 17))
          {
            print_error_range("Offset", -(1 << 10), (1 << 10) - 1, asm_context);
            return -1;
          }

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          offset = (offset >> 2) & 0x01ff;

          opcode = table_cell[n].opcode |
                  (((offset >> 7) & 0x3) << 23) |
                  (offset & 0x7f);

          if (table_cell[n].type == OP_HINT_RELATIVE_RO_I16)
          {
            address = operands[1].value - asm_context->address;

            if (address < -(1 << 17) || address >= (1 << 17))
            {
              print_error_range("Offset", -(1 << 17), (1 << 17) - 1, asm_context);
              return -1;
            }
          }
            else
          {
            address = operands[1].value;

            if (address < 0 || address >= (1 << 18))
            {
              print_error_range("Address", 0, (1 << 18) - 1, asm_context);
              return -1;
            }
          }

          if ((address & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          address = (address >> 2) & 0xffff;

          opcode |= (address << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_RA_SCALE155:
        case OP_RT_RA_SCALE173:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int scale;

          if (table_cell[n].type == OP_RT_RA_SCALE155)
          {
            scale = 155 - operands[2].value;
          }
            else
          {
            scale = 173 - operands[2].value;
          }

          if (scale < 0 || scale > 255)
          {
            print_error_range("Scale", 0, 255, asm_context);
            return -1;
          }

          opcode = table_cell[n].opcode |
                  (scale << 14) |
                  (operands[1].value << 7) |
                  (operands[0].value << 0);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_U14:
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

          if (operands[0].value < 0 || operands[0].value >= (1 << 14))
          {
            print_error_range("Offset", 0, (1 << 14) - 1, asm_context);
            return -1;
          }

          opcode = table_cell[n].opcode | operands[0].value;

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RT_SA:
        case OP_RT_CA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int32_t data = operands[1].value;

          if (data < 0 || data >= 128)
          {
            print_error_range("Immediate", 0, 127, asm_context);
            return -1;
          }

          opcode = table_cell[n].opcode |
                  (operands[0].value << 0) |
                  (data << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SA_RT:
        case OP_CA_RT:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int32_t data = operands[0].value;

          if (data < 0 || data >= 128)
          {
            print_error_range("Immediate", 0, 127, asm_context);
            return -1;
          }

          opcode = table_cell[n].opcode |
                  (operands[1].value << 0) |
                  (data << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        default:
          break;
      }
    }

    n++;
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



