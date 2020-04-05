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
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_SXTW,
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
  int num, size, type;

  if (s[0] != 'w' && s[0] != 'W')
  {
    type = OPERAND_REG_32;
  }
    else
  if (s[0] != 'x' && s[0] != 'X')
  {
    type = OPERAND_REG_64;
  }
    else
  if (s[0] != 'v' && s[0] != 'V')
  {
    type = OPERAND_REG_VECTOR;
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

int parse_instruction_arm64(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  //int offset, value;
  uint32_t opcode;
  int found = 0;

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
    if (strcasecmp(token, "sxtw") == 0)
    {
      operands[operand_count].type = OPERAND_SXTW;
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

  n = 0;
  while(table_arm64[n].instr != NULL)
  {
    if (strcmp(table_arm64[n].instr, instr_case) == 0)
    {
      found = 1;

      switch(table_arm64[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin32(asm_context, table_arm64[n].opcode, IS_OPCODE);
            return 4;
          }

          break;
        }
        case OP_MATH_R32_R32_R32:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG_32 &&
              operands[1].type == OPERAND_REG_32 &&
              operands[2].type == OPERAND_REG_32)
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
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REG_64 &&
              operands[1].type == OPERAND_REG_64 &&
              operands[2].type == OPERAND_REG_64)
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
        default:
        {
          break;
        }
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

