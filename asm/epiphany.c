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
#include "asm/epiphany.h"
#include "common/assembler.h"
#include "common/eval_expression.h"
#include "common/tokens.h"
#include "disasm/epiphany.h"
#include "table/epiphany.h"

enum
{
  OPERAND_NONE,
  OPERAND_REG,
  OPERAND_NEG_REG,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_INDEX_REG,
  OPERAND_INDEX_REG_IMM,
  OPERAND_INDEX_REG_REG,
};

struct _operand
{
  uint8_t type;
  uint8_t reg;
  int value;
  uint8_t reg_is_negative;
};

static int get_register(const char *token)
{
  if (token[0] == 'r' || token[1] == 'R')
  {
    const char *s = token + 1;
    int n = 0, count = 0;

    while (*s != 0)
    {
      if (*s < '0' || *s > '9') { return -1; }
      n = (n * 10) + (*s - '0');
      count++;

      // Disallow leading 0's on registers
      if (n == 0 && count >1) { return -1; }

      s++;
    }

    // This token was just r or R.
    if (count == 0) { return -1; }

    return n;
  }

  // A1-A4 = R0-R3
  if (token[0] == 'a' || token[1] == 'A')
  {
    if (token[2] == 0 && token[1] >= '1' && token[1] <= '4')
    {
      return token[1] - '1';
    }
  }

  // V1-V8 = R4-R11
  if (token[0] == 'v' || token[1] == 'V')
  {
    if (token[2] == 0 && token[1] >= '1' && token[1] <= '8')
    {
      return (token[1] - '1') + 4;
    }
  }

  if (strcasecmp(token, "sb") == 0) { return 9; }
  if (strcasecmp(token, "sl") == 0) { return 10; }
  if (strcasecmp(token, "fp") == 0) { return 11; }
  if (strcasecmp(token, "sp") == 0) { return 13; }
  if (strcasecmp(token, "lr") == 0) { return 14; }

  return -1;
}

static uint16_t get_reg_combo16(int a, int b, int c)
{
  uint16_t reg_combo;

  reg_combo = (a << 13) | (b << 10) | (c << 7);

  return reg_combo;
}

static uint32_t get_reg_combo32(int a, int b, int c)
{
  uint32_t reg_combo;

  reg_combo = ((a & 0x7) << 13) |
              ((b & 0x7) << 10) |
              ((c & 0x7) << 7) |
              ((a >> 3) << 29) |
              ((b >> 3) << 26) |
              ((c >> 3) << 23);

  return reg_combo;
}

int parse_instruction_epiphany(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  char token[TOKENLEN];
  int token_type;
  struct _operand operands[3];
  uint8_t use_32_bit_instruction = 0;
  int operand_count;
  int offset;
  uint32_t reg_combo;
  uint32_t value;
  uint32_t sub;
  uint32_t special;
  uint32_t opcode;
  int found = 0;
  int n, m;

  lower_copy(instr_case, instr);
  operand_count = 0;
  memset(operands, 0, sizeof(operands));

  // Parse operands
  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (IS_TOKEN(token, '.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token, 'l') || IS_TOKEN(token, 'L'))
      {
        memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
        use_32_bit_instruction = 1;
      }
        else
      {
        print_error_unexp(token, asm_context);
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
    }

    if (operand_count != 0)
    {
      if (IS_NOT_TOKEN(token,','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
    }

    if (operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    n = get_register(token);

    if (n != -1)
    {
      operands[operand_count].type = OPERAND_REG;
      operands[operand_count].reg = n;
    }
      else
    if (IS_TOKEN(token,'-'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = get_register(token);

      if (n == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_NEG_REG;
      operands[operand_count].reg = n;
    }
      else
    if (IS_TOKEN(token,'#'))
    {
      if (eval_expression(asm_context, &n) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
          use_32_bit_instruction = 1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = n;
    }
      else
    if (IS_TOKEN(token,'['))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = get_register(token);

      if (n == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_INDEX_REG_IMM;
      operands[operand_count].reg = n;
      operands[operand_count].value = 0;

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (IS_TOKEN(token,','))
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token,'#'))
        {
          if (eval_expression(asm_context, &n) != 0)
          {
            if (asm_context->pass == 1)
            {
              ignore_operand(asm_context);
              memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
              use_32_bit_instruction = 1;
            }
              else
            {
              print_error_illegal_expression(instr, asm_context);
              return -1;
            }
          }

          operands[operand_count].type = OPERAND_INDEX_REG_IMM;
          operands[operand_count].value = n;
        }
          else
        if (IS_TOKEN(token,'-'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          n = get_register(token);

          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OPERAND_INDEX_REG_REG;
          operands[operand_count].value = n;
          operands[operand_count].reg_is_negative = 1;
        }
          else
        {
          n = get_register(token);

          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].type = OPERAND_INDEX_REG_REG;
          operands[operand_count].value = n;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);
      }

      if (IS_NOT_TOKEN(token,']'))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &n) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
          memory_write(asm_context, asm_context->address, 1, asm_context->tokens.line);
          use_32_bit_instruction = 1;
        }
          else
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_ADDRESS;
      operands[operand_count].value = n;
    }

    operand_count++;
  }

  if (memory_read(asm_context, asm_context->address) != 0)
  {
    use_32_bit_instruction = 1;
  }

  // Get opcodes
  n = 0;
  while (table_epiphany[n].instr != NULL)
  {
    if (strcmp(table_epiphany[n].instr, instr_case) == 0)
    {
      found = 1;

      if (use_32_bit_instruction == 1 && table_epiphany[n].size == 16)
      {
        n++;
        continue;
      }

      switch (table_epiphany[n].type)
      {
        case OP_NONE:
        {
          // Should never get in here.
          break;
        }
        case OP_BRANCH_16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            //if (use_32_bit_instruction == 1) { break; }

            offset = operands[0].value - asm_context->address;

            if ((offset & 1) != 0)
            {
              print_error("Address not on an odd boundary", asm_context);
              return -1;
            }

            if (offset >= -256 && offset <= 255)
            {
              offset = offset >> 1;

              add_bin16(asm_context, table_epiphany[n].opcode | (((uint8_t)offset) << 8), IS_OPCODE);
              return 2;
            }
          }
          break;
        }
        case OP_BRANCH_32:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            offset = operands[0].value - asm_context->address;

            if ((offset & 1) != 0)
            {
              print_error("Address not on an odd boundary", asm_context);
              return -1;
            }

            if (check_range(asm_context, "Offset", offset, -(1 << 24), (1 << 24) - 1) == -1)
            {
              return -1;
            }

            offset = offset >> 1;

            add_bin32(asm_context, table_epiphany[n].opcode | (((uint32_t)(offset & 0xffffff)) << 8), IS_OPCODE);

            return 4;
          }
          break;
        }
        case OP_DISP_IMM3_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_IMM)
          {
            //if (use_32_bit_instruction == 1) { break; }
            if (operands[1].value < 0 || operands[1].value > 7) { break; }
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, 0);

            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo | (operands[1].value << 7), IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_DISP_IMM11_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_IMM)
          {
            if (check_range(asm_context, "Immediate", operands[1].value, -2047, 2047) == -1) { return -1; }

            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, 0);

            if (operands[1].value < 0)
            {
              operands[1].value = -operands[1].value;
              value = 0x01000000;
            }
              else
            {
              value = 0;
            }

            value |= ((operands[1].value & 0x7) << 7) |
                     (((operands[1].value >> 3) & 0xff) << 16);

            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_INDEX_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_REG)
          {
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }
            if (operands[1].value > 7) { break; }
            if (operands[1].reg_is_negative == 1) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, operands[1].value);
            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);

            return 2;
          }
          break;
        }
        case OP_INDEX_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_REG)
          {
            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, operands[1].value);

            sub = (operands[1].reg_is_negative == 1) ? 0x00100000 : 0;
            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | sub, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_POST_MOD_16:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_IMM && operands[1].value == 0 &&
              operands[2].type == OPERAND_REG)
          {
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }
            if (operands[2].reg > 7) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, operands[2].reg);
            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_POST_MOD_32:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_IMM &&
              operands[1].value == 0 &&
              (operands[2].type == OPERAND_REG ||
               operands[2].type == OPERAND_NEG_REG))
          {
            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, operands[2].reg);
            sub = operands[2].type == OPERAND_NEG_REG ? 0x00100000 : 0;
            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | sub, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_POST_MOD_DISP_32:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_INDEX_REG_IMM &&
              operands[1].value == 0 &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[1].value, -2047, 2047) == -1) { return -1; }

            if (operands[2].value < 0)
            {
              operands[2].value = -operands[2].value;
              value = 0x01000000;
            }
              else
            {
              value = 0;
            }

            value |= ((operands[2].value & 0x7) << 7) |
                     (((operands[2].value >> 3) & 0xff) << 16);

            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, 0);
            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_REG_IMM_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_NUMBER)
          {
            //if (use_32_bit_instruction == 1) { break; }
            if (operands[0].reg > 7) { break; }
            if (operands[1].value < 0 || operands[1].value > 0xff) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, 0, 0);
            value = operands[1].value << 5;

            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_IMM_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[1].value, -0x8000, 0xffff) == -1) { return -1; }

            reg_combo = get_reg_combo32(operands[0].reg, 0, 0);
            value = (operands[1].value & 0xff) << 5;
            value |= ((operands[1].value >> 8) & 0xff) << 20;

            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_2_IMM_16:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_NUMBER)
          {
            //if (use_32_bit_instruction == 1) { break; }
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }
            if (operands[2].value < -4 || operands[2].value > 3) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, 0);
            value = (operands[2].value & 0x7) << 7;

            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_2_IMM_32:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[2].value, -1024, 1023) == -1) { return -1; }

            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, 0);
            value = (operands[2].value & 0x7) << 7;
            value |= ((operands[2].value >> 3) & 0xff) << 16;

            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_REG_2_IMM5_16:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }
            if (check_range(asm_context, "Immediate", operands[2].value, 0, 31) == -1) { return -1; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, 0);
            value = (operands[2].value & 0x1f) << 5;

            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_2_IMM5_32:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_NUMBER)
          {
            if (check_range(asm_context, "Immediate", operands[2].value, 0, 31) == -1) { return -1; }

            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, 0);
            value = (operands[2].value & 0x1f) << 5;

            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo | value, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_REG_2_ZERO_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, 0);
            value = (operands[2].value & 0x1f) << 5;

            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_2_ZERO_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, 0);
            value = (operands[2].value & 0x1f) << 5;

            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_REG_3_16:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_REG)
          {
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }
            if (operands[2].reg > 7) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, operands[2].reg);
            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_3_32:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG &&
              operands[2].type == OPERAND_REG)
          {
            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, operands[2].reg);
            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_REG_2_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            if (operands[0].reg > 7) { break; }
            if (operands[1].reg > 7) { break; }

            reg_combo = get_reg_combo16(operands[0].reg, operands[1].reg, 0);
            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_2_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            reg_combo = get_reg_combo32(operands[0].reg, operands[1].reg, 0);
            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_REG_1_16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_REG)
          {
            if (operands[0].reg > 7) { break; }
            reg_combo = get_reg_combo16(0, operands[0].reg, 0);
            add_bin16(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_REG_1_32:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_REG)
          {
            reg_combo = get_reg_combo32(0, operands[0].reg, 0);
            add_bin32(asm_context, table_epiphany[n].opcode | reg_combo, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_NUM6_16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            if (check_range(asm_context, "Immediate", operands[0].value, 0, 63) == -1) { return -1; }
            add_bin16(asm_context, table_epiphany[n].opcode | operands[0].value << 10, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_NONE_16:
        {
          if (operand_count == 0)
          {
            add_bin16(asm_context, table_epiphany[n].opcode, IS_OPCODE);
            return 2;
          }
          break;
        }
        case OP_NONE_32:
        {
          if (operand_count == 0)
          {
            add_bin32(asm_context, table_epiphany[n].opcode, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_SPECIAL_RN_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_REG)
          {
            if (operands[1].reg > 7) { break; }
            if ((operands[0].value & 0xffffff00) != 0xf0400) { break; }

            if ((operands[0].value & 0x3) != 0)
            {
              print_error("Unknown special register", asm_context);
              return -1;
            }

            special = (operands[0].value & 0xff) / 4;

            if (special > 7) { break; }

            opcode = table_epiphany[n].opcode |
                     special << 10 |
                     operands[1].reg << 13;

            add_bin16(asm_context, opcode, IS_OPCODE);

            return 2;
          }
          break;
        }
        case OP_RD_SPECIAL_16:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_ADDRESS)
          {
            if (operands[0].reg > 7) { break; }
            if ((operands[1].value & 0xffffff00) != 0xf0400) { break; }

            if ((operands[1].value & 0x3) != 0)
            {
              print_error("Unknown special register", asm_context);
              return -1;
            }

            special = (operands[1].value & 0xff) / 4;

            if (special > 7) { break; }

            opcode = table_epiphany[n].opcode |
                     operands[0].reg << 13 |
                     special << 10;

            add_bin16(asm_context, opcode, IS_OPCODE);

            return 2;
          }
          break;
        }
        case OP_SPECIAL_RN_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_REG)
          {
            m = (operands[0].value >> 8) & 0xf;
            special = (operands[0].value & 0xff) / 4;

            if ((operands[0].value & 0x3) != 0 ||
                (operands[0].value & 0xfffff000) != 0x000f0000 ||
                 m < 4 || m > 7 || special > 63)
            {
              print_error("Unknown special register", asm_context);
              return -1;
            }

            m = m - 4;

            opcode = table_epiphany[n].opcode |
                     m << 20 |
                     (special & 0x7) << 10 |
                     (special >> 3) << 26 |
                     (operands[1].reg & 0x7) << 13 |
                     (operands[1].reg >> 3) << 29;

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }
          break;
        }
        case OP_RD_SPECIAL_32:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_ADDRESS)
          {
            m = (operands[1].value >> 8) & 0xf;
            special = (operands[1].value & 0xff) / 4;

            if ((operands[1].value & 0x3) != 0 ||
                (operands[1].value & 0xfffff000) != 0x000f0000 ||
                 m < 4 || m > 7 || special > 63)
            {
              print_error("Unknown special register", asm_context);
              return -1;
            }

            m = m - 4;

            opcode = table_epiphany[n].opcode |
                     m << 20 |
                     (special & 0x7) << 10 |
                     (special >> 3) << 26 |
                     (operands[0].reg & 0x7) << 13 |
                     (operands[0].reg >> 3) << 29;

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }
          break;
        }
        default:
          printf("Internal error %s:%d\n", __FILE__, __LINE__);
          return -1;
          break;
      }
    }

    n++;
  }

  if (found == 1)
  {
    print_error_unknown_operand_combo(instr, asm_context);
    return -1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }


  return -1;
}

