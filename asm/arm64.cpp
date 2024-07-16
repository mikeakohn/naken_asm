/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
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
  OPERAND_REG_IMM_OFFSET,
  OPERAND_REG_REG_OFFSET,
  OPERAND_REG_V,
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
  OPTION_ROR,
};

enum
{
  INDEX_NONE = 0,
  INDEX_PRE,
  INDEX_POST
};

struct _operand
{
  int value;
  int offset_imm;
  uint8_t offset_reg;
  uint8_t offset_shift;
  uint8_t type;
  uint8_t attribute;
  uint8_t element;
  uint8_t index_type;
  // This is used for ld/st instructions where an option can be added.
  uint8_t option;
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
  AsmContext *asm_context,
  struct _operand *operand,
  char *s)
{
  char token[TOKENLEN];
  int token_type;
  int num, size = 0, type;

  if (strcasecmp(s, "wzr") == 0)
  {
    operand->type = OPERAND_REG_32;
    operand->value = 31;
    return 0;
  }
    else
  if (strcasecmp(s, "xzr") == 0)
  {
    operand->type = OPERAND_REG_64;
    operand->value = 31;
    return 0;
  }
    else
  if (strcasecmp(s, "wsp") == 0)
  {
    operand->type = OPERAND_REG_32;
    operand->value = 31;
    return 0;
  }
    else
  if (strcasecmp(s, "sp") == 0)
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
  if (s[0] == 'q' || s[0] == 'Q')
  {
    type = OPERAND_REG_SCALAR;
    size = 4;
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

  const char *orig = s;
  s++;

  if (get_vector_number(&s, &num) == -1)
  {
    print_error_unexp(asm_context, orig);
    return -2;
  }

  if (*s != '.')
  {
    operand->type = OPERAND_REG_V;
    operand->value = num;
    return 0;
  }

  s++;

  if (get_vector_size(&s, token, TOKENLEN) == -1)
  {
    print_error_unexp(asm_context, orig);
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
    print_error_unexp(asm_context, token);
    return -2;
  }

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token, '['))
  {
    tokens_push(asm_context, token, token_type);

    if (size > SIZE_2D)
    {
      print_error_unexp(asm_context, token);
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
      if (element < 0 || element > 15)
      {
        print_error_range(asm_context, "Element", 0, 15);
        return -2;
      }
      size = SIZE_B;
      break;
    case SIZE_4H:
    case SIZE_8H:
    case SIZE_H:
      if (element < 0 || element > 7)
      {
        print_error_range(asm_context, "Element", 0, 7);
        return -2;
      }
      size = SIZE_H;
      break;
    case SIZE_2S:
    case SIZE_4S:
    case SIZE_S:
      if (element < 0 || element > 3)
      {
        print_error_range(asm_context, "Element", 0, 3);
        return -2;
      }
      size = SIZE_S;
      break;
    case SIZE_1D:
    case SIZE_2D:
    case SIZE_D:
      if (element < 1 || element > 2)
      {
        print_error_range(asm_context, "Element", 0, 1);
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
  operand->element = element;

  return 0;
}

static int get_register_arm64(char *s)
{
  if (s[0] != 'x') { return -1; }

  int num = get_reg_number(s + 1, 31);
  if (num < 0 || num > 31) { return -1; }

  return num;
}

static int get_register_arm32(char *s)
{
  if (s[0] != 'w') { return -1; }

  int num = get_reg_number(s + 1, 31);
  if (num < 0 || num > 31) { return -1; }

  return num;
}

static int get_shift_value(AsmContext *asm_context)
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
  AsmContext *asm_context,
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

    return OPTION_LSR;
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

static int op_3_reg_option(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[0].type != operands[1].type ||
      operands[0].type != operands[2].type)
  {
    return -2;
  }

  int size = operands[0].type == OPERAND_REG_64 ? 1: 0;

  opcode |= operands[0].value |
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

    return -2;
  }

  if (operand_count == 4)
  {
    if (operands[3].type != OPERAND_OPTION) { return -2; }
    if (operands[3].attribute > OPTION_LSL) { return -2; }

    if (operands[3].attribute == OPTION_LSL)
    {
      if (operands[3].value < 0 || operands[3].value > 7)
      {
        print_error_range(asm_context, "Shift", 0, 7);
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

  return -2;
}

static int op_3_reg_shift(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[0].type != operands[1].type ||
      operands[0].type != operands[2].type)
  {
    return -2;
  }

  int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
  int shift = 0, value = 0;

  if (operand_count == 4)
  {
    if (operands[3].attribute < OPTION_LSL ||
        operands[3].attribute > OPTION_ROR)
    {
      return -2;
    }

    shift = operands[3].attribute - OPTION_LSL;
    value = operands[3].value;

    if (value < 0 || value > 0x3f)
    {
      print_error_range(asm_context, "Shift", 0, 0x3f);
      return -1;
    }
  }
    else
  if (operand_count != 3)
  {
    return -2;
  }

  opcode |= operands[0].value |
           (operands[1].value << 5) |
           (operands[2].value << 16) |
           (value << 10) |
          ((shift & 3) << 22) |
           (size << 31);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_add_sub_imm(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[0].type != operands[1].type)
  {
    return -2;
  }

  if (operands[2].type != OPERAND_NUMBER)
  {
    return -2;
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
    print_error(asm_context, "Immediate out of range (0x000-0xfff or (0x001000-0xfff000)");
    return -1;
  }

  if (operand_count == 4)
  {
    if (operands[3].type != OPERAND_OPTION ||
        operands[3].attribute != OPTION_LSL ||
        shift != 0)
    {
      return -2;
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
      print_error(asm_context, "Shift value must be 0 or 12");
      return -1;
    }
  }
    else
  if (operand_count != 3)
  {
    return -2;
  }

  opcode |= operands[0].value |
           (operands[1].value << 5) |
           (value << 10) |
          ((shift & 3) << 22) |
           (size << 31);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_2_reg_imm6_imm4(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_64 ||
      operands[1].type != OPERAND_REG_64)
  {
    return -2;
  }

  int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
  if (size != 1) { return -2; }

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
    print_error(asm_context, "Immediate6 is not a multiple of 16");
    return -1;
  }

  imm6 = imm6 / 16;

  opcode |= operands[0].value |
           (operands[1].value << 5) |
           (imm6 << 16) |
           (imm4 << 10);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_move(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_32 && operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[1].type != OPERAND_REG_32 && operands[1].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[0].type != operands[1].type)
  {
    return -2;
  }

  int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;

  opcode |= (size << 31) | operands[0].value | (operands[1].value << 16);
  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_reg_relative(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_64 ||
      operands[1].type != OPERAND_ADDRESS)
  {
    return -2;
  }

  //int offset = operands[1].value - (asm_context->address + 4);
  int offset = operands[1].value - asm_context->address;

  if (check_range(asm_context, "offset", offset, -(1 << 20), (1 << 20) - 1) != 0)
  {
    return -1;
  }

  offset &= (1 << 21) - 1;

  opcode |= operands[0].value |
           ((offset & 0x3) << 29) |
           ((offset >> 2) << 5);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_reg_page_relative(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_64 ||
      operands[1].type != OPERAND_ADDRESS)
  {
    return -2;
  }

  uint64_t base = asm_context->address & (~0xfffULL);

  if ((operands[1].value & 0xfff) != 0)
  {
    print_error(asm_context, "Address is not on page boundary.");
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

  opcode |= operands[0].value |
          ((offset & 0x3) << 29) |
          ((offset >> 2) << 5);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_d_v(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_SCALAR ||
      operands[1].type != OPERAND_REG_VECTOR ||
      operands[0].attribute != 3 ||
      operands[1].attribute != SIZE_2D)
  {
    return -2;
  }

  opcode |= operands[0].value | (operands[1].value << 5);
  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_v_v_to_scalar(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != OPERAND_REG_SCALAR ||
      operands[1].type != OPERAND_REG_VECTOR)
  {
    return -2;
  }

  // addv doesn't allow D, but there are similar instructions that might.
  if ((operands[0].attribute / 2) == 3 || operands[1].attribute == 3)
  {
    return -2;
  }

  // addv will want these to be the same size, but similar instructions
  // might not.
  if ((operands[0].attribute / 2) == operands[1].attribute)
  {
    return -2;
  }

  int size = operands[0].attribute;
  int q = operands[1].attribute & 1;

  opcode |= (q << 30) |
            (size << 22) |
             operands[0].value |
            (operands[1].value << 5);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_logical_imm(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[0].type != operands[1].type) { return -2; }
  if (operands[2].type != OPERAND_NUMBER) { return -2; }

  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;

  // Figure out how many 1's on the right side.
  uint64_t value = operands[2].value;

  char pattern[64] = { 0 };
  int position = 0;

  for (int n = 0; n < 64; n++)
  {
    if ((position & 1) == 0)
    {
      if ((value & 1) == 1) { position++; }

      pattern[position]++;
    }
      else
    {
      if ((value & 1) == 0) { position++; }

      pattern[position]++;
    }

    value = value >> 1;
  }

#if 0
for (int i = 0; i <= position; i++)
{
printf("%d\n", pattern[i]);
}
#endif

  // N imms  element size
  // 0 1 1 1 1 0 x   2 bits
  // 0 1 1 1 0 x x   4 bits
  // 0 1 1 0 x x x   8 bits
  // 0 1 0 x x x x   16 bits
  // 0 0 x x x x x   32 bits
  // 1 x x x x x x   64 bits

  int immr;
  int imms;
  int n = 0;

  // FIXME: This works for single blocks of 1's and not repeating patterns.
  if (size == 0)
  {
    immr = (32 - pattern[0]) & 0x1f;
    imms = pattern[1] - 1;
  }
    else
  {
    immr = (64 - pattern[0]) & 0x3f;
    imms = pattern[1] - 1;
  }

#if 0
  int max = size == 0 ? 0xfff : 0x1fff;
  if (check_range(asm_context, "immediate", imm, 0, max) != 0)
  {
    return -1;
  }
#endif

  opcode |= (size << 31) |
            (n << 22) |
            (immr << 16) |
            (imms << 10) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_ld_st_imm_p(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr,
  char reg_type)
{
  if (operand_count != 2)
  {
    return -2;
  }

  if (operands[1].type != OPERAND_REG_IMM_OFFSET) { return -2; }

  if (reg_type == 'w' && operands[0].type != OPERAND_REG_32)
  {
    return -2;
  }

  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64 &&
      operands[0].type != OPERAND_REG_SCALAR)
  {
    return -2;
  }

  if (operands[1].index_type == INDEX_NONE) { return -2; }

  int offset = operands[1].offset_imm;
  int v = 0;
  int size = 0;
  int opc = 0;

  if (operands[0].type == OPERAND_REG_SCALAR)
  {
    v = 1;
    opcode &= 0x3fffffff;

    size = operands[0].attribute;
    if (size == 4) { opc = 2; }
    size = size & 0x3;
  }

  if (check_range(asm_context, "offset", offset, -256, 255) != 0)
  {
    return -1;
  }

  int p = operands[1].index_type == INDEX_PRE ? 3 : 1;

  if (operands[0].type == OPERAND_REG_64) { size = 1; }

  opcode |= (size << 30) |
            (v << 26) |
            (opc << 22) |
           ((offset & 0x1ff) << 12) |
            (p << 10) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_ld_st_imm(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr,
  char reg_type)
{
  if (operand_count != 2)
  {
    return -2;
  }

  if (operands[1].type != OPERAND_REG_IMM_OFFSET) { return -2; }

  if (reg_type == 'w' && operands[0].type != OPERAND_REG_32)
  {
    return -2;
  }

  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64 &&
      operands[0].type != OPERAND_REG_SCALAR)
  {
    return -2;
  }

  if (operands[1].index_type != INDEX_NONE) { return -2; }

  int offset = operands[1].offset_imm;
  int v = 0;
  int size = 0;
  int opc = 0;
  int shift = 0;

  if (operands[0].type == OPERAND_REG_SCALAR)
  {
    v = 1;
    opcode &= 0x3fffffff;

    size = operands[0].attribute;
    if (size == 4) { opc = 2; }
    size = size & 0x3;
    shift = size;
  }
    else
  {
    shift = opcode >> 30;
  }

  if (check_range(asm_context, "offset", offset, 0, 0xfff << shift) != 0)
  {
    return -1;
  }

  if ((offset & ((1 << shift) - 1)) != 0)
  {
    print_error_align(asm_context, 1 << shift);
    return -1;
  }

  opcode |= (size << 30) |
            (v << 26) |
            (opc << 22) |
          (((offset >> shift) & 0xfff) << 10) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_ld_literal(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr,
  char reg_type)
{
  if (operand_count != 2)
  {
    return -2;
  }

  if (operands[1].type != OPERAND_ADDRESS) { return -2; }

  if (reg_type == 'x' && operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64 &&
      operands[0].type != OPERAND_REG_SCALAR)
  {
    return -2;
  }

  int offset = operands[1].value - asm_context->address;
  int v = 0;
  int size = 0;

  if (operands[0].type == OPERAND_REG_SCALAR)
  {
    v = 1;

    switch (operands[0].attribute)
    {
      case 2: size = 0; break;
      case 3: size = 1; break;
      case 4: size = 2; break;
      default: return -2;
    }
  }
    else
  {
    if (operands[0].type == OPERAND_REG_64) { size = 1; }
  }

  const int min = -(1 << 20);
  const int max = (1 << 20) - 1;

  if (check_range(asm_context, "offset", offset, min, max) != 0)
  {
    return -1;
  }

  if ((offset & 3) != 0)
  {
    print_error_align(asm_context, 4);
    return -1;
  }

  opcode |= (size << 30) |
            (v << 26) |
          (((offset >> 2) & 0x7ffff) << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_alias_reg_imm(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operands[1].type != OPERAND_NUMBER) { return -2; }

  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  int shift = 0;
  int imm = operands[1].value;

  if (operand_count == 2)
  {
    if (imm >= 0 && imm <= 0xfff)
    {
    }
      else
    if (imm >= 0x1000 && imm <= 0xfff000 && (imm & 0xfff) == 0)
    {
      imm = imm >> 12;
      shift = 1;
    }
      else
    {
      print_error(asm_context, "Illegal immediate value");
    }
  }
    else
  if (operand_count == 3)
  {
    if (operands[2].type != OPERAND_OPTION)
    {
      return -2;
    }

    if (check_range(asm_context, "immediate", imm, 0, 0xfff) != 0)
    {
      return -1;
    }

    if (operands[2].attribute != OPTION_LSL ||
        (operands[2].value != 12 && operands[2].value != 0))
    {
      print_error(asm_context, "Invalid shift");
      return -1;
    }

    if (operands[2].value == 12) { shift = 1; }
  }
    else
  {
    return -2;
  }

  int sf = operands[0].type == OPERAND_REG_32 ? 0 : 1;

  opcode |= (sf << 31) |
            (shift << 22) |
            (imm << 10) |
            (operands[0].value << 5);

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_scalar_shift_imm(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operand_count != 3) { return -2; }
  if (operands[2].type != OPERAND_NUMBER) { return -2; }

  if (operands[0].type != OPERAND_REG_SCALAR ||
      operands[1].type != OPERAND_REG_SCALAR ||
      operands[0].attribute != 2 ||
      operands[1].attribute != 2)
  {
    return -2;
  }

  if (check_range(asm_context, "immediate", operands[2].value, 0, 63) != 0)
  {
    return -1;
  }

  opcode |= (8 << 19) |
            (operands[2].value << 16) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_shift_imm(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr,
  char reg_type)
{
  if (operand_count != 3) { return -2; }
  if (operands[2].type != OPERAND_NUMBER) { return -2; }

  if (operands[0].type != OPERAND_REG_VECTOR ||
      operands[1].type != OPERAND_REG_VECTOR)
  {
    return -2;
  }

  if (operands[0].attribute != operands[1].attribute)
  {
    print_error_illegal_operands(asm_context, instr);
    return -1;
  }

  int q = operands[0].attribute & 1;
  int size = operands[0].attribute >> 1;
  int shift = operands[2].value;
  int imm = 0;

  switch (size)
  {
    case 0:
      if (check_range(asm_context, "immediate", shift, 0, 7) != 0)
      {
        return -1;
      }
      if (reg_type == 'r') { shift = 8 - shift; }
      imm = 0x8 | shift;
      break;
    case 1:
      if (check_range(asm_context, "immediate", shift, 0, 15) != 0)
      {
        return -1;
      }
      if (reg_type == 'r') { shift = 16 - shift; }
      imm = 0x10 | shift;
      break;
    case 2:
      if (check_range(asm_context, "immediate", shift, 0, 31) != 0)
      {
        return -1;
      }
      if (reg_type == 'r') { shift = 32 - shift; }
      imm = 0x20 | shift;
      break;
    case 3:
      if (check_range(asm_context, "immediate", shift, 0, 63) != 0)
      {
        return -1;
      }
      if (reg_type == 'r') { shift = 64 - shift; }
      imm = 0x40 | shift;
      break;
    default:
      print_error_illegal_operands(asm_context, instr);
      return -1;
  }

  opcode |= (q << 30) |
            (imm << 16) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_v_v_v_fpu(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operand_count != 3) { return -2; }

  if (operands[0].type != OPERAND_REG_VECTOR ||
      operands[1].type != OPERAND_REG_VECTOR ||
      operands[2].type != OPERAND_REG_VECTOR ||
      operands[0].attribute != operands[1].attribute ||
      operands[0].attribute != operands[2].attribute)
  {
    return -2;
  }

  int q = 0;
  int sz = 0;

  switch (operands[0].attribute)
  {
    case SIZE_2S: q = 0; sz = 0; break;
    case SIZE_4S: q = 1; sz = 0; break;
    case SIZE_2D: q = 1; sz = 1; break;
    default: return -2;
  }

  opcode |= (q << 30) |
            (sz << 22) |
            (operands[2].value << 16) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_v_v_fpu(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operand_count != 2) { return -2; }

  if (operands[0].type != OPERAND_REG_VECTOR ||
      operands[1].type != OPERAND_REG_VECTOR ||
      operands[0].attribute != operands[1].attribute)
  {
    return -2;
  }

  int q = 0;
  int sz = 0;

  switch (operands[0].attribute)
  {
    case SIZE_2S: q = 0; sz = 0; break;
    case SIZE_4S: q = 1; sz = 0; break;
    case SIZE_2D: q = 1; sz = 1; break;
    default: return -2;
  }

  opcode |= (q << 30) |
            (sz << 22) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_d_d_d_fpu(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operand_count != 3) { return -2; }

  if (operands[0].type != OPERAND_REG_SCALAR ||
      operands[1].type != OPERAND_REG_SCALAR ||
      operands[2].type != OPERAND_REG_SCALAR ||
      operands[0].attribute != operands[1].attribute ||
      operands[0].attribute != operands[2].attribute)
  {
    return -2;
  }

  int type = 0;

  switch (operands[0].attribute)
  {
    case 2: type = 0; break;
    case 3: type = 1; break;
    default: return -2;
  }

  opcode |= (type << 22) |
            (operands[2].value << 16) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_vector_d_d_fpu(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operand_count != 2) { return -2; }

  if (operands[0].type != OPERAND_REG_SCALAR ||
      operands[1].type != OPERAND_REG_SCALAR ||
      operands[0].attribute != operands[1].attribute)
  {
    return -2;
  }

  int type = 0;

  switch (operands[0].attribute)
  {
    case 2: type = 0; break;
    case 3: type = 1; break;
    default: return -2;
  }

  opcode |= (type << 22) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static int op_ld_st_reg_reg(
  AsmContext *asm_context,
  struct _operand *operands,
  int operand_count,
  uint32_t opcode,
  char *instr)
{
  if (operand_count != 2) { return -2; }

  if (operands[0].type != OPERAND_REG_32 &&
      operands[0].type != OPERAND_REG_64)
  {
    return -2;
  }

  if (operands[1].type != OPERAND_REG_REG_OFFSET)
  {
    return -2;
  }

  // FIXME: This should be able to be uxtw, lsl, sxtw, sxtx.
  int option = 3;
  int size = operands[0].type == OPERAND_REG_32 ? 0 : 1;
  int s = 0;

  if (operands[1].offset_shift != 0)
  {
    if ((size == 0 && operands[1].offset_shift != 2) ||
        (size == 1 && operands[1].offset_shift != 3))
    {
      print_error(asm_context, "Illegal shift value");
      return -1;
    }

    s = 1;
  }
    else
  if (operands[1].option == OPTION_UXTW)
  {
    option = 2;
  }
    else
  if (operands[1].option == OPTION_SXTW)
  {
    option = 6;
  }
    else
  if (operands[1].option == OPTION_SXTX)
  {
    option = 7;
  }

  opcode |= (size << 30) |
            (operands[1].offset_reg << 16) |
            (option << 13) |
            (s << 12) |
            (operands[1].value << 5) |
             operands[0].value;

  add_bin32(asm_context, opcode, IS_OPCODE);

  return 4;
}

static bool reg_matches(int operand, int wanted)
{
  switch (wanted)
  {
    case ARM64_REG_V_DOT:     return operand == OPERAND_REG_VECTOR;
    case ARM64_REG_V_SCALAR:  return operand == OPERAND_REG_SCALAR;
    case ARM64_REG_V_ELEMENT: return operand == OPERAND_REG_VECTOR_ELEMENT;
    case ARM64_REG_B:
      return operand == OPERAND_REG_32 || operand == OPERAND_REG_64;
    default:
      return false;
  }
}

static int get_register_size(_operand *operand)
{
  switch (operand->type)
  {
    case OPERAND_REG_32: return 32;
    case OPERAND_REG_64: return 64;
    case OPERAND_REG_VECTOR:
      switch (operand->attribute)
      {
        case SIZE_8B: return 8;
        case SIZE_16B: return 8;
        case SIZE_4H: return 16;
        case SIZE_8H: return 16;
        case SIZE_2S: return 32;
        case SIZE_4S: return 32;
        case SIZE_1D: return 64;
        case SIZE_2D: return 64;
        case SIZE_B: return 8;
        case SIZE_H: return 16;
        case SIZE_S: return 32;
        case SIZE_D: return 64;
        default: return -4;
      }
    case OPERAND_REG_VECTOR_ELEMENT:
      switch (operand->attribute)
      {
        case SIZE_8B: return 8;
        case SIZE_16B: return 8;
        case SIZE_4H: return 16;
        case SIZE_8H: return 16;
        case SIZE_2S: return 32;
        case SIZE_4S: return 32;
        case SIZE_1D: return 64;
        case SIZE_2D: return 64;
        case SIZE_B: return 8;
        case SIZE_H: return 16;
        case SIZE_S: return 32;
        case SIZE_D: return 64;
        default: return -4;
      }
    case OPERAND_REG_SCALAR:
      switch (operand->attribute)
      {
        case 0: return 8;
        case 1: return 16;
        case 2: return 32;
        case 3: return 64;
        default: return -3;
      }
    default:
      return -1;
  }
}

static bool match_vector_size(_operand *operands)
{
  int size_d = get_register_size(&operands[0]);
  int size_n = get_register_size(&operands[1]);

  return size_d == size_n;
}

static bool encode_vector_element(
  _operand *operand,
   int &imm,
   int &q,
   int attribute_d = -1)
{
  switch (operand->attribute)
  {
    case SIZE_B:
      imm = 1 | (operand->element << 1);
      break;
    case SIZE_H:
      imm = 2 | (operand->element << 2);
      break;
    case SIZE_S:
      imm = 4 | (operand->element << 3);
      break;
    case SIZE_D:
      imm = 8 | (operand->element << 4);
      break;
    default:
      imm = 0;
      return true;
  }

  switch (attribute_d)
  {
    case SIZE_16B:
    case SIZE_8H:
    case SIZE_4S:
    case SIZE_2D:
      q = 1;
      break;
    default:
      break;
  }

  return true;
}

static int get_vector_size(char *s)
{
  if (strcmp(s, "8b") == 0)
  {
    return SIZE_8B;
  }
    else
  if (strcmp(s, "16b") == 0)
  {
    return SIZE_16B;
  }
    else
  if (strcmp(s, "4h") == 0)
  {
    return SIZE_4H;
  }
    else
  if (strcmp(s, "8h") == 0)
  {
    return SIZE_8H;
  }
    else
  if (strcmp(s, "2s") == 0)
  {
    return SIZE_2S;
  }
    else
  if (strcmp(s, "4s") == 0)
  {
    return SIZE_4S;
  }
    else
  if (strcmp(s, "1d") == 0)
  {
    return SIZE_1D;
  }
    else
  if (strcmp(s, "2d") == 0)
  {
    return SIZE_2D;
  }

  return -1;
}

static void parse_vector_size(char *instr_case, int &vector_size)
{
  char *s = instr_case;

  while (*s != 0)
  {
    if (*s == '.')
    {
      vector_size = get_vector_size(s + 1);
      if (vector_size != -1) { *s = 0; }
      return;
    }

    s++;
  }
}

int parse_instruction_arm64(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int offset;
  int ret;
  uint32_t opcode;
  int found = 0;
  int vector_size = -1;
  bool is_reg_w;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  parse_vector_size(instr_case, vector_size);

  while (true)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(asm_context, token);
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
          print_error_illegal_expression(asm_context, instr);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = num;
    }
      else
    if (IS_TOKEN(token,'['))
    {
      // [x0]
      // [x0]!
      // [x0, #8]
      // [x0, #8]!
      // [x0], #8

      // [x0, x1]
      // [x0, x1, lsl #4]
      // [x0, w1, uxtw]
      // [x0, w1, sxtw]
      // [x0, x1, sxtx]

      token_type = tokens_get(asm_context, token, TOKENLEN);

      num = get_register_arm64(asm_context, &operands[operand_count], token);
      if (num == -2) { return -1; }
      if (num < 0 || operands[operand_count].type != OPERAND_REG_64)
      {
        print_error_unexp(asm_context, token);
        return -1;
      }

      operands[operand_count].type = OPERAND_REG_IMM_OFFSET;

      do
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (IS_TOKEN(token, ']'))
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, '!'))
          {
            operands[operand_count].index_type = INDEX_PRE;
            break;
          }

          if (IS_NOT_TOKEN(token, ','))
          {
            tokens_push(asm_context, token, token_type);
            break;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_NOT_TOKEN(token, '#'))
          {
            print_error_unexp(asm_context, token);
            return -1;
          }

          if (eval_expression(asm_context, &num) != 0)
          {
            if (asm_context->pass == 1)
            {
              ignore_operand(asm_context);
              num = 0;
            }
              else
            {
              print_error_illegal_expression(asm_context, instr);
              return -1;
            }
          }

          operands[operand_count].offset_imm = num;
          operands[operand_count].index_type = INDEX_POST;

          break;
        }

        if (IS_NOT_TOKEN(token, ','))
        {
          print_error_unexp(asm_context, token);
          return -1;
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        is_reg_w = false;

        num = get_register_arm64(token);

        if (num == -1)
        {
          num = get_register_arm32(token);

          if (num != -1) { is_reg_w = true; }
        }

        if (num == -1)
        {
          if (IS_NOT_TOKEN(token, '#'))
          {
            print_error_unexp(asm_context, token);
            return -1;
          }

          if (eval_expression(asm_context, &num) != 0)
          {
            if (asm_context->pass == 1)
            {
              ignore_operand(asm_context);
              num = 0;
            }
              else
            {
              print_error_illegal_expression(asm_context, instr);
              return -1;
            }
          }

          operands[operand_count].offset_imm = num;

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_NOT_TOKEN(token, ']'))
          {
            print_error_unexp(asm_context, token);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, '!'))
          {
            operands[operand_count].index_type = INDEX_PRE;
          }
            else
          {
            tokens_push(asm_context, token, token_type);
          }
        }
          else
        {
          operands[operand_count].offset_reg = num;
          operands[operand_count].type = OPERAND_REG_REG_OFFSET;

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_TOKEN(token, ','))
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);

            if (strcasecmp(token, "lsl") == 0)
            {
              if (is_reg_w)
              {
                print_error(asm_context,
                  "lsl cannot be used with 32 bit register");
                return -1;
              }

              if (expect_token(asm_context, '#') == -1) { return -1; }

              token_type = tokens_get(asm_context, token, TOKENLEN);
              num = atoi(token);

              operands[operand_count].offset_shift = num;
              operands[operand_count].option = OPTION_LSL;
            }
              else
            if (strcasecmp(token, "sxtx") == 0)
            {
              if (is_reg_w)
              {
                print_error(asm_context,
                  "sxtx cannot be used with 32 bit register");
                return -1;
              }

              operands[operand_count].option = OPTION_SXTX;
            }
              else
            if (strcasecmp(token, "uxtw") == 0)
            {
              if (!is_reg_w)
              {
                print_error(asm_context,
                  "uxtw cannot be used with 64 bit register");
                return -1;
              }

              operands[operand_count].option = OPTION_UXTW;
            }
              else
            if (strcasecmp(token, "sxtw") == 0)
            {
              if (!is_reg_w)
              {
                print_error(asm_context,
                  "sxtw cannot be used with 64 bit register");
                return -1;
              }

              operands[operand_count].option = OPTION_SXTW;
            }
              else
            {
              print_error_unexp(asm_context, token);
              return -1;
            }

            token_type = tokens_get(asm_context, token, TOKENLEN);

            if (IS_NOT_TOKEN(token, ']'))
            {
              print_error_unexp(asm_context, token);
              return -1;
            }
          }
            else
          {
            if (IS_NOT_TOKEN(token, ']'))
            {
              print_error_unexp(asm_context, token);
              return -1;
            }
          }
        }
      } while (false);
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
          print_error_illegal_expression(asm_context, instr);
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
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  // Accept better SIMD syntax.
  if (vector_size != -1)
  {
    for (n = 0; n < operand_count; n++)
    {
      if (operands[n].type == OPERAND_REG_V)
      {
        operands[n].type = OPERAND_REG_VECTOR;
        operands[n].attribute = vector_size;
      }
    }
  }

  if (operand_count == 2)
  {
    for (n = 0; table_arm64_simd_copy[n].instr != NULL; n++)
    {
      if (strcmp(table_arm64_simd_copy[n].instr, instr_case) != 0) { continue; }

      found = 1;

      if (!reg_matches(operands[0].type, table_arm64_simd_copy[n].reg_rd))
      {
        continue;
      }

      if (!reg_matches(operands[1].type, table_arm64_simd_copy[n].reg_rn))
      {
        continue;
      }

      if (table_arm64_simd_copy[n].reg_rd == ARM64_REG_V_DOT &&
          operands[0].attribute >= SIZE_B)
      {
        continue;
      }

      int imm5 = 0;
      int imm4 = table_arm64_simd_copy[n].imm4;
      int q = table_arm64_simd_copy[n].q;

      if (operands[0].type == OPERAND_REG_VECTOR_ELEMENT &&
          operands[1].type == OPERAND_REG_VECTOR_ELEMENT)
      {
        if (operands[0].attribute != operands[1].attribute)
        {
          print_error(asm_context, "Mismatched vector sizes.");
          return -1;
        }

        encode_vector_element(&operands[0], imm5, q);
        encode_vector_element(&operands[1], imm4, q);
      }
        else
      if (operands[0].type == OPERAND_REG_VECTOR_ELEMENT)
      {
        if (!match_vector_size(operands))
        {
          print_error(asm_context, "Mismatched register sizes");
          return -1;
        }

        encode_vector_element(&operands[0], imm5, q);
      }
        else
      if (operands[1].type == OPERAND_REG_VECTOR_ELEMENT)
      {
        if (!match_vector_size(operands))
        {
          print_error(asm_context, "Mismatched register sizes");
          return -1;
        }

        if (table_arm64_simd_copy[n].reg_rd == ARM64_REG_V_DOT)
        {
          encode_vector_element(&operands[1], imm5, q, operands[0].attribute);
        }
          else
        {
          encode_vector_element(&operands[1], imm5, q);
        }
      }
        else
      if (operands[1].type == OPERAND_REG_32 ||
          operands[1].type == OPERAND_REG_64)
      {
        if (!match_vector_size(operands))
        {
          print_error(asm_context, "Mismatched register sizes");
          return -1;
        }

        q = operands[0].attribute & 1;
        imm5 = 1 << (operands[0].attribute / 2);
      }

      opcode = 0x0e000400 |
        (q << 30) |
        (table_arm64_simd_copy[n].op << 28) |
        (imm5 << 16) |
        (imm4 << 11) |
        (operands[1].value << 5) |
         operands[0].value;

      add_bin32(asm_context, opcode, IS_OPCODE);

      return 4;
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
          ret = op_3_reg_option(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MATH_R_R_R_SHIFT:
        {
          ret = op_3_reg_shift(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MATH_R_R_IMM_SHIFT:
        {
          ret = op_add_sub_imm(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MATH_R_R_IMM6_IMM4:
        {
          ret = op_2_reg_imm6_imm4(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MOVE:
        {
          ret = op_move(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_REG_RELATIVE:
        {
          ret = op_reg_relative(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_REG_PAGE_RELATIVE:
        {
          ret = op_reg_page_relative(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_D_V:
        {
          ret = op_vector_d_v(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_V_V_TO_SCALAR:
        {
          ret = op_vector_v_v_to_scalar(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MATH_R_R_IMMR_S:
        {
          ret = op_logical_imm(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MATH_R_R_IMMR:
        {
          if (operands[0].type != operands[1].type) { continue; }
          if (operands[2].type != OPERAND_NUMBER) { continue; }

          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            continue;
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
          if (operands[0].type != OPERAND_AT) { continue; }
          if (operands[1].type != OPERAND_REG_64) { continue; }

          opcode = table_arm64[n].opcode | operands[1].value;
          opcode |= ((operands[0].value >> 4) & 0x7) << 16;
          opcode |= ((operands[0].value >> 8) & 0x1) << 8;
          opcode |= (operands[0].value & 0x7) << 5;

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RELATIVE19:
        {
          if (operands[0].type != OPERAND_ADDRESS) { continue; }

          //offset = operands[0].value - (asm_context->address + 4);
          offset = operands[0].value - asm_context->address;

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
          if (operands[0].type != OPERAND_ADDRESS) { continue; }

          //offset = operands[0].value - (asm_context->address + 4);
          offset = operands[0].value - asm_context->address;

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
        case OP_RET:
        {
          if (!(operand_count == 0 ||
               (operand_count == 1 && operands[0].type == OPERAND_REG_64)))
          {
            continue;
          }

          if (operand_count == 0) { operands[0].value = 30; }

          opcode = table_arm64[n].opcode | (operands[0].value << 5);

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case OP_LD_ST_IMM_P:
        {
          ret = op_ld_st_imm_p(asm_context, operands, operand_count, table_arm64[n].opcode, instr, table_arm64[n].reg_type);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_LD_ST_IMM:
        {
          ret = op_ld_st_imm(asm_context, operands, operand_count, table_arm64[n].opcode, instr, table_arm64[n].reg_type);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_LD_LITERAL:
        {
          ret = op_ld_literal(asm_context, operands, operand_count, table_arm64[n].opcode, instr, table_arm64[n].reg_type);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_ALIAS_REG_IMM:
        {
          ret = op_alias_reg_imm(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_SCALAR_SHIFT_IMM:
        {
          ret = op_scalar_shift_imm(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_SHIFT_IMM:
        {
          ret = op_vector_shift_imm(asm_context, operands, operand_count, table_arm64[n].opcode, instr, table_arm64[n].reg_type);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_V_V_V_FPU:
        {
          ret = op_vector_v_v_v_fpu(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_V_V_FPU:
        {
          ret = op_vector_v_v_fpu(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_D_D_D_FPU:
        {
          ret = op_vector_d_d_d_fpu(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_VECTOR_D_D_FPU:
        {
          ret = op_vector_d_d_fpu(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_LD_ST_REG_REG:
        {
          ret = op_ld_st_reg_reg(asm_context, operands, operand_count, table_arm64[n].opcode, instr);

          if (ret == -2) { continue; }
          return ret;
        }
        case OP_MUL_R_R_R_R:
        {
          if (operands[0].type != OPERAND_REG_32 &&
              operands[0].type != OPERAND_REG_64)
          {
            continue;
          }

          if (operands[0].type != operands[1].type &&
              operands[0].type != operands[2].type)
          {
            continue;
          }

          opcode = table_arm64[n].opcode |
            (operands[0].value << 0) |
            (operands[1].value << 5) |
            (operands[2].value << 16);

          if (operand_count == 4)
          {
            opcode |= operands[3].value << 10;

            if (operands[0].type != operands[3].type) { continue; }
          }

          if (operands[0].type == OPERAND_REG_64) { opcode |= (1 << 31); }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SMUL_R_R_R_R:
        {
          if (operands[0].type != OPERAND_REG_64 &&
              operands[1].type != OPERAND_REG_32 &&
              operands[2].type != OPERAND_REG_32)
          {
            continue;
          }

          opcode = table_arm64[n].opcode |
            (operands[0].value << 0) |
            (operands[1].value << 5) |
            (operands[2].value << 16);

          if (operand_count == 4)
          {
            opcode |= operands[3].value << 10;

            if (operands[3].type != OPERAND_REG_64) { continue; }
          }

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
              print_error(asm_context, "Invalid vector size");
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
                print_error(asm_context, "Invalid vector size");
                return -1;
              }
            }

            // Q field.
            if (((table_arm64[n].mask >> 30) & 0x1) == 0x1)
            {
              if ((vector_size & 0x1) != 0)
              {
                print_error(asm_context, "Invalid vector size");
                return -1;
              }
            }

            if (vector_size > SIZE_2D)
            {
              print_error(asm_context, "Unknown vector size");
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
    print_error_illegal_operands(asm_context, instr);
    return -1;
  }
    else
  {
    print_error_unknown_instr(asm_context, instr);
  }

  return -1;
}

