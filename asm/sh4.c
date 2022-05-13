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

#include "asm/sh4.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/sh4.h"

#define MAX_OPERANDS 3

// This needs to be in sync with the table/sh4.h SH4_ enums.
enum
{
  OPERAND_NA,
  OPERAND_REG,
  OPERAND_FREG,
  OPERAND_DREG,
  OPERAND_XDREG,
  OPERAND_FVREG,
  OPERAND_AT_REG,
  OPERAND_AT_MINUS_REG,
  OPERAND_AT_REG_PLUS,
  OPERAND_AT_R0_REG,
  OPERAND_SPECIAL_REG,
  OPERAND_REG_BANK,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_AT_R0_GBR,
  OPERAND_AT_DISP_GBR,
  OPERAND_AT_DISP_PC,
  OPERAND_AT_DISP_REG,
};

struct _operand
{
  int value;
  int type;
  uint8_t disp_reg;
};

static int get_num(char *token)
{
  int num = 0;

  if (token[0] == 0) { return -1; }

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }
    num = (num * 10) + (*token - '0');

    token++;
  }

  return num;
}

static int get_register_sh4(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }

  int num = get_num(token + 1);

  if (num < 0 || num > 15) { return -1; }

  return num;
}

static int get_f_register_sh4(char *token)
{
  if (token[0] != 'f' && token[0] != 'F') { return -1; }
  if (token[1] != 'r' && token[1] != 'R') { return -1; }

  int num = get_num(token + 2);

  if (num < 0 || num > 15) { return -1; }

  return num;
}

static int get_d_register_sh4(char *token)
{
  if (token[0] != 'd' && token[0] != 'D') { return -1; }
  if (token[1] != 'r' && token[1] != 'R') { return -1; }

  int num = get_num(token + 2);

  if (num < 0 || num > 15 || (num & 1) != 0) { return -1; }

  return num / 2;
}

static int get_fv_register_sh4(char *token)
{
  if (token[0] != 'f' && token[0] != 'F') { return -1; }
  if (token[1] != 'v' && token[1] != 'V') { return -1; }

  int num = get_num(token + 2);

  if (num < 0 || num > 15 || (num & 0x3) != 0) { return -1; }

  return num / 4;
}

static int get_xd_register_sh4(char *token)
{
  if (token[0] != 'x' && token[0] != 'X') { return -1; }
  if (token[1] != 'd' && token[1] != 'D') { return -1; }

  int num = get_num(token + 2);

  if (num < 0 || num > 15 || (num & 1) != 0) { return -1; }

  return num / 2;
}

int get_special_reg(const char *token)
{
  int n;

  for (n = 1; sh4_specials[n] != NULL; n++)
  {
    if (strcasecmp(token, sh4_specials[n]) == 0) { return n; }
  }

  return -1;
}

static int get_register_bank_sh4(char *token)
{
  int num = 0, count = 0;

  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  token++;

  while (*token >= '0' && *token <= '9')
  {
    num = (num * 10)  + (*token - '0');
    count++;
    token++;
  }

  if (count == 0 || num > 7) { return -1; }

  if (strcasecmp(token, "_bank") == 0) { return num; }

  return -1;
}

static int get_displacement(
  struct _asm_context *asm_context,
  struct _operand *operands,
  int align,
  int mask,
  int operand)
{
  int value = (asm_context->pass == 2) ? operands[operand].value : 4;

  if ((value % align) != 0)
  {
    print_error_align(asm_context, align);
    return -1;
  }

  const int upper_value = mask * align;

  if (value < 0 || value > upper_value)
  {
    print_error_range("Displacement", 0, upper_value, asm_context);
    return -1;
  }

  return value;
}

static int calc_displacement(
  struct _asm_context *asm_context,
  struct _operand *operands,
  int align)
{
  int address = asm_context->address + 4;
  int value;

  if (asm_context->pass == 1) { return 0; }

  if (align == 2)
  {
    value = operands[0].value - address;

    if ((value & 1) != 0)
    {
      print_error_align(asm_context, 2);
      return -1;
    }
  }
    else
  {
    value = operands[0].value - (address & 0xfffffffc);

    if ((value & 3) != 0)
    {
      print_error_align(asm_context, 4);
      return -1;
    }
  }

  const int upper_value = 0xff * align;

  if (value < 0 || value > upper_value)
  {
    print_error_range("Displacement", 0, upper_value, asm_context);
    return -1;
  }

  return value;
}

static int parse_at(struct _asm_context *asm_context, struct _operand *operand, const char *instr)
{
  char token[TOKENLEN];
  int token_type;
  int num;

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (IS_TOKEN(token, '('))
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    num = get_register_sh4(token);

    if (num == -1)
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

      operand->value = num;

      if (expect_token(asm_context, ',') == -1) { return -1; }

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (strcasecmp(token, "gbr") == 0)
      {
        operand->type = OPERAND_AT_DISP_GBR;

        if (expect_token(asm_context, ')') == -1) { return -1; }

        return 0;
      }
        else
      if (strcasecmp(token, "pc") == 0)
      {
        operand->type = OPERAND_AT_DISP_PC;

        if (expect_token(asm_context, ')') == -1) { return -1; }

        return 0;
      }
        else
      if ((num = get_register_sh4(token)) == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operand->type = OPERAND_AT_DISP_REG;
      operand->disp_reg = num;

      if (expect_token(asm_context, ')') == -1) { return -1; }

      return 0;
    }

    if (expect_token(asm_context, ',') == -1) { return -1; }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (strcasecmp(token, "gbr") == 0)
    {
      operand->type = OPERAND_AT_R0_GBR;
      operand->value = 0;
    }
      else
    if ((num = get_register_sh4(token)) != -1)
    {
      operand->type = OPERAND_AT_R0_REG;
      operand->value = num;
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (expect_token(asm_context, ')') == -1) { return -1; }

    return 0;
  }

  if (IS_TOKEN(token, '-'))
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    num = get_register_sh4(token);

    if (num == -1)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    operand->value = num;
    operand->type = OPERAND_AT_MINUS_REG;

    return 0;
  }

  num = get_register_sh4(token);

  if (num != -1)
  {
    operand->value = num;
    operand->type = OPERAND_AT_REG;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token, '+'))
    {
      operand->type = OPERAND_AT_REG_PLUS;
    }
      else
    {
      tokens_push(asm_context, token, token_type);
    }

    return 0;
  }

  print_error_unexp(token, asm_context);

  return -1;
}

int parse_instruction_sh4(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int value, offset;
  uint16_t opcode;
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

    num = get_register_sh4(token);

    if (num != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_REG;
    }
      else
    if ((num = get_f_register_sh4(token)) != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_FREG;
    }
      else
    if ((num = get_d_register_sh4(token)) != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_DREG;
    }
      else
    if ((num = get_fv_register_sh4(token)) != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_FVREG;
    }
      else
    if ((num = get_xd_register_sh4(token)) != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_XDREG;
    }
      else
    if ((num = get_register_bank_sh4(token)) != -1)
    {
      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_REG_BANK;
    }
      else
    if (IS_TOKEN(token, '#'))
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
    if (IS_TOKEN(token, '@'))
    {
      if (parse_at(asm_context, &operands[operand_count], instr) == -1)
      {
        return -1;
      }
    }
      else
    if ((num = get_special_reg(token)) != -1)
    {
      operands[operand_count].type = OPERAND_SPECIAL_REG;
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

#if 0
printf("%d %d %d\n",
  operands[0].type,
  operands[1].type,
  operands[1].value);
#endif

  while (table_sh4[n].instr != NULL)
  {
    if (strcmp(table_sh4[n].instr, instr_case) == 0)
    {
      found = 1;

      if (operand_count != operand_type_sh4[table_sh4[n].type].count)
      {
        n++;
        continue;
      }

      const int shift_0 = operand_type_sh4[table_sh4[n].type].shift_0;
      const int shift_1 = operand_type_sh4[table_sh4[n].type].shift_1;
      const int type_0 = operand_type_sh4[table_sh4[n].type].type_0;
      const int type_1 = operand_type_sh4[table_sh4[n].type].type_1;

      switch (table_sh4[n].type)
      {
        case OP_NONE:
        {
          add_bin16(asm_context, table_sh4[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_REG:
        case OP_FREG:
        case OP_DREG:
        case OP_AT_REG:
        {
          if (operands[0].type == type_0)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << shift_0);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG_REG:
        case OP_FREG_FREG:
        case OP_DREG_DREG:
        case OP_DREG_XDREG:
        case OP_XDREG_DREG:
        case OP_XDREG_XDREG:
        case OP_FVREG_FVREG:
        case OP_FREG_AT_REG:
        case OP_DREG_AT_REG:
        case OP_FREG_AT_MINUS_REG:
        case OP_DREG_AT_MINUS_REG:
        case OP_FREG_AT_R0_REG:
        case OP_DREG_AT_R0_REG:
        case OP_XDREG_AT_REG:
        case OP_XDREG_AT_MINUS_REG:
        case OP_XDREG_AT_R0_REG:
        case OP_AT_REG_DREG:
        case OP_AT_REG_PLUS_DREG:
        case OP_AT_R0_REG_DREG:
        case OP_AT_REG_FREG:
        case OP_AT_REG_PLUS_FREG:
        case OP_AT_R0_REG_FREG:
        case OP_AT_REG_XDREG:
        case OP_AT_REG_PLUS_XDREG:
        case OP_AT_R0_REG_XDREG:
        case OP_AT_REG_PLUS_AT_REG_PLUS:
        case OP_REG_AT_REG:
        case OP_REG_AT_MINUS_REG:
        case OP_REG_AT_R0_REG:
        case OP_AT_REG_REG:
        case OP_AT_REG_PLUS_REG:
        case OP_AT_R0_REG_REG:
        case OP_REG_BANK_REG:
        case OP_REG_BANK_AT_MINUS_REG:
        {
          if (operands[0].type == type_0 && operands[1].type == type_1)
          {
            opcode = table_sh4[n].opcode |
                    (operands[0].value << shift_0) |
                    (operands[1].value << shift_1);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_IMM_REG:
        {
          if (operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_REG)
          {
            value = (asm_context->pass == 2) ? operands[0].value : 0;

            if (operands[0].value < -128 || operands[0].value > 0xff)
            {
              print_error_range("Constant", -128, 0xff, asm_context);
              return -1;
            }

            opcode = table_sh4[n].opcode |
                    (value & 0xff) |
                    (operands[1].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_IMM_R0:
        {
          if (operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_REG &&
              operands[1].value == 0)
          {
            value = (asm_context->pass == 2) ? operands[0].value : 0;

            if (value < 0 || value > 0xff)
            {
              print_error_range("Constant", 0, 0xff, asm_context);
              return -1;
            }

            opcode = table_sh4[n].opcode | (value & 0xff);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_IMM_AT_R0_GBR:
        {
          if (operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_AT_R0_GBR)
          {
            value = (asm_context->pass == 2) ? operands[0].value : 0;

            if (value < 0 || value > 0xff)
            {
              print_error_range("Constant", 0, 0xff, asm_context);
              return -1;
            }

            opcode = table_sh4[n].opcode | (value & 0xff);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_BRANCH_S9:
        {
          if (operands[0].type == OPERAND_ADDRESS)
          {
            offset = (asm_context->pass == 2) ?
              operands[0].value - (asm_context->address + 4) : 0;

            if (offset < -256 || offset > 255)
            {
              print_error_range("Offset", -256, 255, asm_context);
              return -1;
            }

            if ((offset & 1) != 0)
            {
              print_error_align(asm_context, 2);
              return -1;
            }

            offset = offset >> 1;

            opcode = table_sh4[n].opcode | (offset & 0xff);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_BRANCH_S13:
        {
          if (operands[0].type == OPERAND_ADDRESS)
          {
            offset = (asm_context->pass == 2) ?
              operands[0].value - (asm_context->address + 4) : 0;

            if (offset < -4096 || offset > 4095)
            {
              print_error_range("Offset", -4096, 4095, asm_context);
              return -1;
            }

            if ((offset & 1) != 0)
            {
              print_error_align(asm_context, 2);
              return -1;
            }

            offset = offset >> 1;

            opcode = table_sh4[n].opcode | (offset & 0xfff);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_DREG_FPUL:
        {
          if (operands[0].type == OPERAND_DREG &&
              operands[1].type == OPERAND_SPECIAL_REG &&
              operands[1].value == SPECIAL_REG_FPUL)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << 9);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FREG_FPUL:
        {
          if (operands[0].type == OPERAND_FREG &&
              operands[1].type == OPERAND_SPECIAL_REG &&
              operands[1].value == SPECIAL_REG_FPUL)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FPUL_FREG:
        {
          if (operands[0].type == OPERAND_SPECIAL_REG &&
              operands[0].value == SPECIAL_REG_FPUL &&
              operands[1].type == OPERAND_FREG)
          {
            opcode = table_sh4[n].opcode | (operands[1].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FPUL_DREG:
        {
          if (operands[0].type == OPERAND_SPECIAL_REG &&
              operands[0].value == SPECIAL_REG_FPUL &&
              operands[1].type == OPERAND_DREG)
          {
            opcode = table_sh4[n].opcode | (operands[1].value << 9);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FR0_FREG_FREG:
        {
          if (operands[0].type == OPERAND_FREG &&
              operands[0].value == 0 &&
              operands[1].type == OPERAND_FREG &&
              operands[2].type == OPERAND_FREG)
          {
            opcode = table_sh4[n].opcode |
                    (operands[1].value << 4) |
                    (operands[2].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_XMTRX_FVREG:
        {
          if (operands[0].type == OPERAND_SPECIAL_REG &&
              operands[0].value == SPECIAL_REG_XMTRX &&
              operands[1].type == OPERAND_FVREG)
          {
            opcode = table_sh4[n].opcode | (operands[1].value << 10);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG_SPECIAL:
        case OP_AT_REG_PLUS_SPECIAL:
        {
          if (operands[0].type == type_0 &&
              operands[1].type == type_1 &&
              operands[1].value == table_sh4[n].special)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << shift_0);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG_REG_BANK:
        case OP_AT_REG_PLUS_REG_BANK:
        {
          if (operands[0].type == type_0 &&
              operands[1].type == OPERAND_REG_BANK)
          {
            opcode = table_sh4[n].opcode |
                    (operands[0].value << shift_0) |
                    (operands[1].value << 4);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_R0_AT_DISP_GBR:
        {
          if (operands[0].type == OPERAND_REG &&
              operands[0].value == 0 &&
              operands[1].type == OPERAND_AT_DISP_GBR)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xff, 1);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_R0_AT_DISP_REG:
        {
          if (operands[0].type == OPERAND_REG &&
              operands[0].value == 0 &&
              operands[1].type == OPERAND_AT_DISP_REG)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xf, 1);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | (operands[1].disp_reg << 4) | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_AT_DISP_GBR_R0:
        {
          if (operands[0].type == OPERAND_AT_DISP_GBR &&
              operands[1].type == OPERAND_REG &&
              operands[1].value == 0)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xff, 0);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_AT_DISP_REG_R0:
        {
          if (operands[0].type == OPERAND_AT_DISP_REG &&
              operands[1].type == OPERAND_REG &&
              operands[1].value == 0)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xf, 0);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | (operands[0].disp_reg << 4) | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_AT_DISP_PC_REG:
        {
          if (operands[0].type == OPERAND_AT_DISP_PC &&
              operands[1].type == OPERAND_REG)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xff, 0);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | (operands[1].value << 8) | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          if (operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_REG)
          {
            value = calc_displacement(asm_context, operands, table_sh4[n].special);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | (operands[1].value << 8) | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_AT_DISP_REG_REG:
        {
          if (operands[0].type == OPERAND_AT_DISP_REG &&
              operands[1].type == OPERAND_REG)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xf, 0);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode |
                    (operands[0].disp_reg << 4) |
                    (operands[1].value << 8) |
                     value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_AT_DISP_PC_R0:
        {
          if (operands[0].type == OPERAND_AT_DISP_PC &&
              operands[1].type == OPERAND_REG &&
              operands[1].value == 0)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xff, 0);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          if (operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_REG &&
              operands[1].value == 0)
          {
            value = calc_displacement(asm_context, operands, table_sh4[n].special);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode | value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_R0_AT_REG:
        {
          if (operands[0].type == OPERAND_REG &&
              operands[0].value == 0 &&
              operands[1].type == OPERAND_AT_REG)
          {
            opcode = table_sh4[n].opcode | (operands[1].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_SPECIAL_REG:
        case OP_SPECIAL_AT_MINUS_REG:
        {
          if (operands[0].type == type_0 &&
              operands[0].value == table_sh4[n].special &&
              operands[1].type == type_1)
          {
            opcode = table_sh4[n].opcode | (operands[1].value << shift_1);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_IMM:
        {
          if (operands[0].type == OPERAND_NUMBER)
          {
            value = (asm_context->pass == 2) ? operands[0].value : 0;

            if (value < 0 || value > 0xff)
            {
              print_error_range("Constant", 0, 0xff, asm_context);
              return -1;
            }

            opcode = table_sh4[n].opcode | (value & 0xff);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG_AT_DISP_REG:
        {
          if (operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_AT_DISP_REG)
          {
            value = get_displacement(asm_context, operands, table_sh4[n].special, 0xf, 1);

            if (value == -1) { return - 1; }

            value = value / table_sh4[n].special;

            opcode = table_sh4[n].opcode |
                    (operands[0].value << 4) |
                    (operands[1].disp_reg << 8) |
                     value;

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
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
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

