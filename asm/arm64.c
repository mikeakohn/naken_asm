/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "asm/arm64.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/arm64.h"

#define MAX_OPERANDS 4

enum
{
  OPERAND_REG_32,
  OPERAND_REG_64,
  OPERAND_REG_VECTOR,
  OPERAND_REG_VECTOR_ELEMENT,
  OPERAND_REG_SCALAR,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_OPTION,
  OPERAND_AT,
};

enum
{
  SIZE_8B,
  SIZE_16B,
  SIZE_4H,
  SIZE_8H,
  SIZE_2S,
  SIZE_4S,
  SIZE_1D,
  SIZE_2D,
  SIZE_B,
  SIZE_H,
  SIZE_S,
  SIZE_D,
};

enum
{
  OPTION_UXTB,  // 000
  OPTION_UXTH,  // 001
  OPTION_UXTW,  // 010
  OPTION_UXTX,  // 011
  OPTION_SXTB,  // 100
  OPTION_SXTH,  // 101
  OPTION_SXTW,  // 110
  OPTION_SXTX,  // 111
  OPTION_LSL,
  OPTION_LSR,
  OPTION_ASR,
};

struct _operand
{
  int value;
  int type;
  int attribute;
};

static int get_vector_number(char **p, int *num)
{
  int value = 0;
  char *s = *p;

  while (*s != 0)
  {
    if (*s < '0' || *s > '9') { break; }

    value = (value * 10) + (*s - '0');

    s++;
  }

  if (s == *p) { return -1; }

  *num = value;
  *p = s;

  return 0;
}

static int get_vector_size(char **p, char *token, int length)
{
  int ptr = 0;
  char *s = *p;

  for (ptr = 0; ptr < length - 2; ptr++)
  {
    if (s[ptr] == 0) { break; }
    if (s[ptr] < '0' || s[ptr] > '9') { break; }

    token[ptr] = s[ptr];
  }

  token[ptr] = s[ptr];
  token[ptr + 1] = 0;

  *p = s + ptr + 1;

  return 0;
}

static int get_register_arm64(
  struct _asm_context *asm_context,
  struct _operand *operand,
  char *s)
{
  char token[TOKENLEN];
  int token_type;
  int num, size = 0, type;

  if (strcasecmp(token, "wzr") == 0)
  {
    operand->type = OPERAND_REG_32;
    operand->value = 31;
    return 0;
  }
    else
  if (strcasecmp(token, "xzr") == 0)
  {
    operand->type = OPERAND_REG_64;
    operand->value = 31;
    return 0;
  }
    else
  if (strcasecmp(token, "wsp") == 0)
  {
    operand->type = OPERAND_REG_32;
    operand->value = 31;
    return 0;
  }
    else
  if (strcasecmp(token, "sp") == 0)
  {
    operand->type = OPERAND_REG_64;
    operand->value = 31;
    return 0;
  }

  if (s[0] == 'w' || s[0] == 'W')
  {
    type = OPERAND_REG_32;
  }
    else
  if (s[0] == 'x' || s[0] == 'X')
  {
    type = OPERAND_REG_64;
  }
    else
  if (s[0] == 'v' || s[0] == 'V')
  {
    type = OPERAND_REG_VECTOR;
  }
    else
  if (s[0] == 'b' || s[0] == 'B')
  {
    type = OPERAND_REG_SCALAR;
    size = 0;
  }
    else
  if (s[0] == 'h' || s[0] == 'H')
  {
    type = OPERAND_REG_SCALAR;
    size = 1;
  }
    else
  if (s[0] == 's' || s[0] == 'S')
  {
    type = OPERAND_REG_SCALAR;
    size = 2;
  }
    else
  if (s[0] == 'd' || s[0] == 'D')
  {
    type = OPERAND_REG_SCALAR;
    size = 3;
  }
    else
  {
    return -1;
  }

  if (type != OPERAND_REG_VECTOR)
  {
    num = get_reg_number(s + 1, 31);
    if (num < 0 || num > 31) { return -1; }
    operand->type = type;
    operand->value = num;
    operand->attribute = size;
    return 0;
  }

  char *orig = s;
  s++;

  if (get_vector_number(&s, &num) == -1)
  {
    print_error_unexp(orig, asm_context);
    return -2;
  }

  if (*s != '.')
  {
    print_error_unexp(orig, asm_context);
    return -2;
  }

  s++;

  if (get_vector_size(&s, token, TOKENLEN) == -1)
  {
    print_error_unexp(orig, asm_context);
    return -2;
  }

  if (strcasecmp(token, "8b") == 0)
  {
    size = SIZE_8B;
  }
    else
  if (strcasecmp(token, "16b") == 0)
  {
    size = SIZE_16B;
  }
    else
  if (strcasecmp(token, "4h") == 0)
  {
    size = SIZE_4H;
  }
    else
  if (strcasecmp(token, "8h") == 0)
  {
    size = SIZE_8H;
  }
    else
  if (strcasecmp(token, "2s") == 0)
  {
    size = SIZE_2S;
  }
    else
  if (strcasecmp(token, "4s") == 0)
  {
    size = SIZE_4S;
  }
    else
  if (strcasecmp(token, "1d") == 0)
  {
    size = SIZE_1D;
  }
    else
  if (strcasecmp(token, "2d") == 0)
  {
    size = SIZE_2D;
  }
    else
  if (strcasecmp(token, "b") == 0)
  {
    size = SIZE_B;
  }
    else
  if (strcasecmp(token, "h") == 0)
  {
    size = SIZE_H;
  }
    else
  if (strcasecmp(token, "s") == 0)
  {
    size = SIZE_S;
  }
    else
  if (strcasecmp(token, "d") == 0)
  {
    size = SIZE_D;
  }
    else
  {
    print_error_unexp(token, asm_context);
    return -2;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token, '['))
  {
    tokens_push(asm_context, token, token_type);

    if (size > SIZE_2D)
    {
      print_error_unexp(token, asm_context);
      return -2;
    }

    operand->type = type;
    operand->value = num;
    operand->attribute = size;
    return 0;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  int element = atoi(token);

  switch (size)
  {
    case SIZE_8B:
    case SIZE_16B:
    case SIZE_B:
      if (element < 1 || element > 16)
      {
        print_error_range("Element", 1, 16, asm_context);
        return -2;
      }
      size = SIZE_B;
      break;
    case SIZE_4H:
    case SIZE_8H:
    case SIZE_H:
      if (element < 1 || element > 8)
      {
        print_error_range("Element", 1, 8, asm_context);
        return -2;
      }
      size = SIZE_H;
      break;
    case SIZE_2S:
    case SIZE_4S:
    case SIZE_S:
      if (element < 1 || element > 4)
      {
        print_error_range("Element", 1, 4, asm_context);
        return -2;
      }
      size = SIZE_S;
      break;
    case SIZE_1D:
    case SIZE_2D:
    case SIZE_D:
      if (element < 1 || element > 2)
      {
        print_error_range("Element", 1, 2, asm_context);
        return -2;
      }
      size = SIZE_D;
      break;
    default:
      break;
  };

  if (expect_token(asm_context, ']') == -1) { return -2; }

  operand->type = OPERAND_REG_VECTOR_ELEMENT;
  operand->value = num;
  operand->attribute = size;

  return 0;
}

static int get_shift_value(struct _asm_context *asm_context)
{
  int num;

  if (expect_token(asm_context, '#') == -1) { return -2; }

  if (asm_context->pass == 1)
  {
    ignore_operand(asm_context);
    return 0;
  }

  if (eval_expression(asm_context, &num) != 0)
  {
    return -2;
  }

  return num;
}

static int get_option(
  struct _asm_context *asm_context,
  struct _operand *operand,
  char *token)
{
  if (strcasecmp(token, "uxtb") == 0)
  {
    return OPTION_UXTB;
  }
    else
  if (strcasecmp(token, "uxth") == 0)
  {
    return OPTION_UXTH;
  }
    else
  if (strcasecmp(token, "uxtw") == 0)
  {
    return OPTION_UXTW;
  }
    else
  if (strcasecmp(token, "uxtx") == 0)
  {
    return OPTION_UXTX;
  }
    else
  if (strcasecmp(token, "sxtb") == 0)
  {
    return OPTION_SXTB;
  }
    else
  if (strcasecmp(token, "sxth") == 0)
  {
    return OPTION_SXTH;
  }
    else
  if (strcasecmp(token, "sxtw") == 0)
  {
    return OPTION_SXTW;
  }
    else
  if (strcasecmp(token, "sxtx") == 0)
  {
    return OPTION_SXTX;
  }
    else
  if (strcasecmp(token, "lsl") == 0)
  {
    int num = get_shift_value(asm_context);

    if (num == -2) { return -2; }

    operand->value = num;

    return OPTION_LSL;
  }
    else
  if (strcasecmp(token, "lsr") == 0)
  {
    int num = get_shift_value(asm_context);

    if (num == -2) { return -2; }

    operand->value = num;

    return OPTION_LSL;
  }
    else
  if (strcasecmp(token, "asr") == 0)
  {
    int num = get_shift_value(asm_context);

    if (num == -2) { return -2; }

    operand->value = num;

    return OPTION_ASR;
  }

  return -1;
}

static int get_at(const char *token)
{
  int n;

  if (token[0] != 's' && token[0] != 'S') { return -1; }
  if (token[1] != '1') { return -1; }

  for (n = 0; table_arm64_at_op[n].name != NULL; n++)
  {
    if (strcasecmp(token, table_arm64_at_op[n].name) == 0)
    {
      return table_arm64_at_op[n].value;
    }
  }

  return -1;
}

int parse_instruction_arm64(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int offset;
  uint32_t opcode;
  int found = 0;

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

    num = get_register_arm64(asm_context, &operands[operand_count], token);

    if (num == -2) { return -1; }

    if (num != -1)
    {
    }
      else
    if ((num = get_option(asm_context, &operands[operand_count], token)) != -1)
    {
      if (num == -2) { return -1; }

      operands[operand_count].type = OPERAND_OPTION;
      operands[operand_count].attribute = num;
    }
      else
    if ((num = get_at(token)) != -1)
    {
      operands[operand_count].type = OPERAND_AT;
      operands[operand_count].value = num;
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

  for (n = 0; table_arm64[n].instr != NULL; n++)
  {
    if (strcmp(table_arm64[n].instr, instr_case) == 0)
    {
      found = 1;

      int type = table_arm64[n].type;

      switch (type)
      {
        case OP_NONE:
        {
          add_bin32(asm_context, table_arm64[n].opcode, IS_OPCODE);
          return 4;
        }
        case OP_MATH_R_R_R_OPTION:
        {
          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            break;
          }

          if (operands[0].type != operands[1].type ||
              operands[0].type != operands[2].type)
          {
            break;
          }

          int size = operands[0].type == OPERAND_REG_64 ? 1: 0;

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (operands[2].value << 16) |
                 ((size & 1) << 31);

          if (operand_count == 3)
          {
            // If rd or rn is 31 and option is LSL #0, then LSL #0 can
            // be omitted.

            if (operands[0].value == 31 || operands[1].value == 31)
            {
              opcode |= size == 0 ? (2 << 13) : (3 << 13);
              add_bin32(asm_context, opcode, IS_OPCODE);
              return 4;
            }

            break;
          }

          if (operand_count == 4)
          {
            if (operands[3].type != OPERAND_OPTION) { break; }
            if (operands[3].attribute > OPTION_LSL) { break; }

            if (operands[3].attribute == OPTION_LSL)
            {
              if (operands[3].value < 0 || operands[3].value > 7)
              {
                print_error_range("Shift", 0, 7, asm_context);
                return -2;
              }

              opcode |= operands[3].value << 10;
              opcode |= size == 0 ? (2 << 13) : (3 << 13);
            }
              else
            {
              opcode |= operands[3].attribute << 13;
            }

            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }

          break;
        }
        case OP_MATH_R_R_IMM_SHIFT:
        {
          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            break;
          }

          if (operands[0].type != operands[1].type)
          {
            break;
          }

          int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
          int shift, value;

          // Immediate should be between 0x000-0xfff or 0x001-0xfff000.
          if (operands[2].value >= 0 && operands[2].value <= 0xfff)
          {
            value = operands[2].value;
            shift = 0;
          }
            else
          if (operands[2].value >= 0x1000 && operands[2].value <= 0xfff000)
          {
            value = operands[2].value >> 12;
            shift = 1;
          }
            else
          {
            print_error("Immediate out of range (0x000-0xfff or (0x001000-0xfff000)", asm_context);
            return -1;
          }

          if (operand_count == 4)
          {
            if (operands[3].type != OPERAND_OPTION ||
                operands[3].attribute != OPTION_LSL ||
                shift != 0)
            {
              break;
            }

            if (operands[3].value == 0)
            {
              shift = 0;
            }
              else
            if (operands[3].value == 12)
            {
              shift = 1;
            }
              else
            {
              print_error("Shift value must be 0 or 12", asm_context);
              return -1;
            }
          }
            else
          if (operand_count != 3)
          {
            break;
          }

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (value << 10) |
                 ((shift & 3) << 22) |
                  (size << 31);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_MATH_R_R_R_SHIFT:
        {
          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            break;
          }

          if (operands[0].type != operands[1].type ||
              operands[0].type != operands[2].type)
          {
            break;
          }

          int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
          int shift = 0, value = 0;

          if (operand_count == 4)
          {
            if (operands[3].attribute < OPTION_LSL ||
                operands[3].attribute > OPTION_ASR)
            {
              break;
            }

            shift = operands[3].attribute - OPTION_LSL;
            value = operands[3].value;

            if (value < 0 || value > 0x3f)
            {
              print_error_range("Shift", 0, 0x3f, asm_context);
              return -1;
            }
          }
            else
          if (operand_count != 3)
          {
            break;
          }

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (operands[2].value << 16) |
                  (value << 10) |
                 ((shift & 3) << 22) |
                  (size << 31);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_MATH_R_R_IMM6_IMM4:
        {
          if (operands[0].type != OPERAND_REG_64 ||
              operands[1].type != OPERAND_REG_64)
          {
            break;
          }

          int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
          if (size != 1) { break; }

          int imm6 = operands[2].value;
          int imm4 = operands[3].value;

          if (check_range(asm_context, "immediate4", imm4, 0, 15) != 0)
          {
            return -1;
          }

          if (check_range(asm_context, "immediate6", imm4, 0, 15) != 0)
          {
            return -1;
          }

          if ((imm6 % 16) != 0)
          {
            print_error("Immedate6 is not a multiple of 16", asm_context);
            return -1;
          }

          imm6 = imm6 / 16;

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (imm6 << 16) |
                  (imm4 << 10);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_REG_RELATIVE:
        {
          if (operands[0].type != OPERAND_REG_64 ||
              operands[1].type != OPERAND_ADDRESS)
          {
            break;
          }

          offset = operands[1].value - (asm_context->address + 4);

          if (check_range(asm_context, "offset", offset, -(1 << 20), (1 << 20) - 1) != 0)
          {
            return -1;
          }

          offset &= (1 << 21) - 1;

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                 ((offset & 0x3) << 29) |
                 ((offset >> 2) << 5);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_REG_PAGE_RELATIVE:
        {
          if (operands[0].type != OPERAND_REG_64 ||
              operands[1].type != OPERAND_ADDRESS)
          {
            break;
          }

          uint64_t base = asm_context->address & (~0xfffULL);

          if ((operands[1].value & 0xfff) != 0)
          {
            print_error("Address is not on page boundary.", asm_context);
            return -1;
          }

          int64_t offset = operands[1].value - base;

#if 0
          //if (check_range(asm_context, "offset", offset, -(1LL << 32), (1LL << 32) - 1) != 0)
          if (check_range(asm_context, "offset", offset, -(1 << 30), (1 << 30) - 1) != 0)
          {
            return -1;
          }
#endif

          offset = offset >> 12;
          offset &= (1 << 21) - 1;

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                 ((offset & 0x3) << 29) |
                 ((offset >> 2) << 5);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VECTOR_D_V:
        {
          if (operands[0].type != OPERAND_REG_SCALAR ||
              operands[1].type != OPERAND_REG_VECTOR ||
              operands[0].attribute != 3 ||
              operands[1].attribute != SIZE_2D)
          {
            break;
          }

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VECTOR_V_V_TO_SCALAR:
        {
          if (operands[0].type != OPERAND_REG_VECTOR ||
              operands[1].type != OPERAND_REG_VECTOR ||
              operands[0].attribute != 3 ||
              operands[1].attribute != SIZE_2D)
          {
            break;
          }

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_MATH_R_R_IMMR_S:
        {
          if (operands[0].type != operands[1].type) { break; }
          if (operands[2].type != OPERAND_NUMBER) { break; }

          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            break;
          }

          int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
          int imm = operands[2].value;
          int max = size == 0 ? 0xfff : 0x1fff;

          if (check_range(asm_context, "immediate", imm, 0, max) != 0)
          {
            return -1;
          }

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (size << 31);

          opcode |= (imm & 0x3f) << 16;
          opcode |= ((imm >> 6) & 0x3f) << 10;

          if (size == 1) { opcode |= ((imm >> 12) & 0x1) << 22; }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_MATH_R_R_IMMR:
        {
          if (operands[0].type != operands[1].type) { break; }
          if (operands[2].type != OPERAND_NUMBER) { break; }

          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            break;
          }

          int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
          int imm = operands[2].value;
          int max = size == 0 ? 31 : 63;

          if (check_range(asm_context, "immediate", imm, 0, max) != 0)
          {
            return -1;
          }

          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (size << 31);

          opcode |= (imm & 0x3f) << 16;

          if (size == 1) { opcode |= ((imm >> 6) & 0x1) << 22; }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_AT:
        {
          if (operands[0].type != OPERAND_AT) { break; }
          if (operands[1].type != OPERAND_REG_64) { break; }

          opcode = table_arm64[n].opcode | operands[1].value;
          opcode |= ((operands[0].value >> 4) & 0x7) << 16;
          opcode |= ((operands[0].value >> 8) & 0x1) << 8;
          opcode |= (operands[0].value & 0x7) << 5;

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RELATIVE19:
        {
          if (operands[0].type != OPERAND_ADDRESS) { break; }

          offset = operands[0].value - (asm_context->address + 4);

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          if (check_range(asm_context, "offset", offset, -(1 << 20), (1 << 20) - 1) != 0)
          {
            return -1;
          }

          offset = offset / 4;
          offset &= (1 << 19) - 1;

          opcode = table_arm64[n].opcode | (offset << 5);

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case OP_RELATIVE26:
        {
          if (operands[0].type != OPERAND_ADDRESS) { break; }

          offset = operands[0].value - (asm_context->address + 4);

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          if (check_range(asm_context, "offset", offset, -(1 << 27), (1 << 27) - 1) != 0)
          {
            return -1;
          }

          offset = offset / 4;
          offset &= (1 << 26) - 1;

          opcode = table_arm64[n].opcode | offset;

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        default:
        {
          break;
        }
      }

      // The instructions below should be simpler multiple register
      // instructions.
      if (table_arm64[n].operand_count != operand_count) { continue; }

      int size = 0;
      int scalar_size = 0;
      int vector_size = 0;
      uint8_t is_scalar = 0;
      uint8_t is_vector = 0;

      switch (table_arm64[n].reg_type)
      {
        case 'w':
          if (operands[0].type != OPERAND_REG_32) { continue; }
          break;
        case 'x':
          if (operands[0].type != OPERAND_REG_64) { continue; }
          break;
        case 'b':
          if (operands[0].type != OPERAND_REG_64 &&
              operands[0].type != OPERAND_REG_32)
          {
            continue;
          }
          size = operands[0].type == OPERAND_REG_64 ? 1 : 0;
          break;
        case 'v':
          if (operands[0].type != OPERAND_REG_VECTOR) { continue; }
          vector_size = operands[0].attribute;
          is_vector = 1;
          break;
        case 'd':
          if (operands[0].type != OPERAND_REG_SCALAR) { continue; }
          if (operands[0].attribute != 3) { continue; }
          scalar_size = 3;
          is_scalar = 1;
          break;
        default:
          continue;
      }

      if (table_arm64[n].operand_count >= 2)
      {
        if (operands[0].type != operands[1].type) { continue; }
        if (is_vector == 1 && operands[0].attribute != operands[1].attribute)
        {
          continue;
        }
      }

      if (table_arm64[n].operand_count >= 3)
      {
        if (operands[0].type != operands[2].type) { continue; }
        if (is_vector == 1 && operands[0].attribute != operands[2].attribute)
        {
          continue;
        }
      }

      switch (type)
      {
        case OP_SCALAR_D_D:
        case OP_SCALAR_D_D_D:
        case OP_VECTOR_V_V:
        case OP_VECTOR_V_V_V:
        case OP_MATH_R_R_R:
        case OP_REG_REG_CRYPT:
        {
          opcode = table_arm64[n].opcode |
                   operands[0].value |
                  (operands[1].value << 5) |
                  (size << 31);

          // Rm field.
          if (((table_arm64[n].mask >> 16) & 0x1f) == 0)
          {
            opcode |= operands[2].value << 16;
          }

          // Ra field.
          if (((table_arm64[n].mask >> 10) & 0x1f) == 0)
          {
            opcode |= operands[3].value << 10;
          }

          if (is_scalar != 0)
          {
            if (((table_arm64[n].mask >> 22) & 0x3) == 0x0)
            {
              opcode |= scalar_size << 22;
            }
          }
            else
          if (type == OP_REG_REG_CRYPT)
          {
            if (vector_size != SIZE_16B)
            {
              print_error("Invalid vector size", asm_context);
              return -1;
            }
          }
            else
          if (is_vector != 0)
          {
            // Size field.
            if (((table_arm64[n].mask >> 22) & 0x3) == 0x3)
            {
              if (vector_size > 0x1)
              {
                print_error("Invalid vector size", asm_context);
                return -1;
              }
            }

            // Q field.
            if (((table_arm64[n].mask >> 30) & 0x1) == 0x1)
            {
              if ((vector_size & 0x1) != 0)
              {
                print_error("Invalid vector size", asm_context);
                return -1;
              }
            }

            if (vector_size > SIZE_2D)
            {
              print_error("Unknown vector size", asm_context);
              return -1;
            }

            opcode |= (vector_size & 0x1) << 30;
            opcode |= (vector_size >> 1) << 22;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        default:
        {
          print_error_internal(asm_context, __FILE__, __LINE__);
          break;
        }
      }
    }
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

