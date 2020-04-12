/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
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
  OPTION_UXTB,
  OPTION_UXTH,
  OPTION_UXTW,
  OPTION_UXTX,
  OPTION_SXTB,
  OPTION_SXTH,
  OPTION_SXTW,
  OPTION_SXTX,
  OPTION_LSL,
};

struct _operand
{
  int value;
  int type;
  int attribute;
};

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

  num = get_reg_number(s + 1, 31);

  if (num < 0 || num > 31) { return -1; }

  if (type != OPERAND_REG_VECTOR)
  {
    operand->type = type;
    operand->value = num;
    operand->attribute = size;
    return 0;
  }

  if (expect_token(asm_context, '.') == -1) { return -2; }

  token_type = tokens_get(asm_context, token, TOKENLEN);

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

    if (size > SIZE_2D) { return -2; }

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
    eat_operand(asm_context);
    return 1;
  }

  if (eval_expression(asm_context, &num) != 0)
  {
    return -2;
  }

  if (num < 0 || num > 7)
  {
    print_error_range("Shift", 0, 7, asm_context);
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
  int num, n, i, r;
  //int offset, value;
  uint32_t opcode;
  int found = 0;
  int size = 0;

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

    num = get_register_arm64(asm_context, &operands[operand_count], token);

    if (num == -2)
    {
      return -1;
    }

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
    if (IS_TOKEN(token,'#'))
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          eat_operand(asm_context);
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
          eat_operand(asm_context);
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

      if (operand_type_arm64[type].operand_count != operand_count)
      {
        continue;
      }

      if (operand_type_arm64[type].size_match == 1)
      {
        r = operand_type_arm64[type].register_map;

        for (i = 0; i < operand_type_arm64[type].operand_count; i++)
        {
          if ((r & 0x8) != 0)
          {
            if (operands[0].type != operands[i].type) { break; }
            if (operands[0].attribute != operands[i].attribute) { break; }

            if (i == 0)
            {
              if (operands[i].type == OPERAND_REG_64)
              {
                size = 1;
              }
                else
              if (operands[i].type == OPERAND_REG_VECTOR ||
                  operands[i].type == OPERAND_REG_SCALAR)
              {
                size = operands[i].attribute;
              }
                else
              if (operands[i].type == OPERAND_REG_VECTOR_ELEMENT)
              {
                size = operands[i].attribute - SIZE_B;
              }
            }
          }

          r = r << 1;
        }

        if (i != operand_type_arm64[type].operand_count) { continue; }
      }

      switch (type)
      {
        case OP_NONE:
        {
          add_bin32(asm_context, table_arm64[n].opcode, IS_OPCODE);
          return 4;

          break;
        }
        case OP_MATH_R32_R32_R32:
        {
          if (operands[0].type == OPERAND_REG_32)
          {
            opcode = table_arm64[n].opcode |
                     operands[0].value |
                    (operands[1].value << 5) |
                    (operands[2].value << 16);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }

          break;
        }
        case OP_MATH_R64_R64_R64:
        {
          if (operands[0].type == OPERAND_REG_64)
          {
            opcode = table_arm64[n].opcode |
                     operands[0].value |
                    (operands[1].value << 5) |
                    (operands[2].value << 16);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }

          break;
        }
        case OP_SCALAR_R_R:
        {
          if (operands[0].type == OPERAND_REG_SCALAR)
          {
            opcode = table_arm64[n].opcode |
                     operands[0].value |
                    (operands[1].value << 5) |
                    (size << 22);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_SCALAR_D_D:
        {
          if (size != 3) { continue; }

          if (operands[0].type == OPERAND_REG_SCALAR)
          {
            opcode = table_arm64[n].opcode |
                     operands[0].value |
                    (operands[1].value << 5) |
                    (size << 22);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }
          break;
        }
        case OP_VECTOR_V_V:
        {
          if (size == 6) { continue; }

          if (operands[0].type == OPERAND_REG_VECTOR)
          {
            opcode = table_arm64[n].opcode |
                     operands[0].value |
                    (operands[1].value << 5) |
                    ((size >> 1) << 22) |
                    ((size & 1) << 30);
            add_bin32(asm_context, opcode, IS_OPCODE);
            return 4;
          }

          break;
        }
        case OP_MATH_R_R_R_OPTION:
        {
          return -1;
          break;
        }
        default:
        {
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

