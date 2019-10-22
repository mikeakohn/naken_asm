/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "asm/sh4.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/sh4.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_REG,
  OPERAND_FREG,
  OPERAND_DREG,
  OPERAND_FVREG,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_AT_R0_GBR,
  OPERAND_SPECIAL_REG,
};

enum
{
  SPECIAL_REG_FPUL,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_sh4(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  token++;

  if (token[0] != 0 && token[1] == 0)
  {
    if (*token < '0' || *token > '9') { return -1; }

    return (*token) - '0';
  }

  if (token[0] == '1' && token[1] >= '0' && token[1] <= '5' && token[2] == 0)
  {
    if (*token < '0' || *token > '7') { return -1; }

    return (token[1]) - '0' + 10;
  }

  return -1;
}

static int get_f_register_sh4(char *token)
{
  if (token[0] != 'f' && token[0] != 'F') { return -1; }

  return get_register_sh4(token + 1);
}

static int get_d_register_sh4(char *token)
{
  if (token[0] != 'd' && token[0] != 'D') { return -1; }

  int num = get_register_sh4(token + 1);

  return num <= 7 ? num : -1;
}

static int get_fv_register_sh4(char *token)
{
  if (token[0] != 'f' && token[0] != 'F') { return -1; }
  if (token[1] != 'v' && token[1] != 'V') { return -1; }

  if (token[3] != 0) { return -1; }
  if (token[2] < '0' || token[2] > '3') { return -1; }

  return token[2] - '0';
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
    if (IS_TOKEN(token, '#'))
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
    if (IS_TOKEN(token, '@'))
    {
      int error = 0;

      if (expect_token(asm_context, '(') == -1) { return -1; }

      do
      {
        token_type = tokens_get(asm_context, token, TOKENLEN);

        num = get_register_sh4(token);

        if (num != 0)
        {
          error = 1;
          break;
        }

        if (expect_token(asm_context, ',') == -1) { return -1; }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (strcasecmp(token, "gbr") != 0)
        {
          error = 1;
          break;
        }

        if (expect_token(asm_context, ')') == -1) { return -1; }

      } while(0);

      if (error == 1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_AT_R0_GBR;
      operands[operand_count].value = 0;
    }
      else
    if (strcasecmp(token, "fpul") == 0)
    {
      operands[operand_count].type = OPERAND_SPECIAL_REG;
      operands[operand_count].value = SPECIAL_REG_FPUL;
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

      switch(table_sh4[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin16(asm_context, table_sh4[n].opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_REG)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FREG:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_FREG)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_DREG:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_DREG)
          {
            opcode = table_sh4[n].opcode | (operands[0].value << 9);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_REG_REG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REG &&
              operands[1].type == OPERAND_REG)
          {
            opcode = table_sh4[n].opcode |
                    (operands[0].value << 4) |
                    (operands[1].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FREG_FREG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_FREG &&
              operands[1].type == OPERAND_FREG)
          {
            opcode = table_sh4[n].opcode |
                    (operands[0].value << 4) |
                    (operands[1].value << 8);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_DREG_DREG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_DREG &&
              operands[1].type == OPERAND_DREG)
          {
            opcode = table_sh4[n].opcode |
                    (operands[0].value << 5) |
                    (operands[1].value << 9);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_FVREG_FVREG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_FVREG &&
              operands[1].type == OPERAND_FVREG)
          {
            opcode = table_sh4[n].opcode |
                    (operands[0].value << 8) |
                    (operands[1].value << 10);

            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }

          break;
        }
        case OP_IMM_REG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
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
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
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
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
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
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
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
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
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
          if (operand_count == 2 &&
              operands[0].type == OPERAND_DREG &&
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
          if (operand_count == 2 &&
              operands[0].type == OPERAND_FREG &&
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
          if (operand_count == 2 &&
              operands[0].type == OPERAND_SPECIAL_REG &&
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
          if (operand_count == 2 &&
              operands[0].type == OPERAND_SPECIAL_REG &&
              operands[0].value == SPECIAL_REG_FPUL &&
              operands[1].type == OPERAND_DREG)
          {
            opcode = table_sh4[n].opcode | (operands[1].value << 9);

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
    return -1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}

