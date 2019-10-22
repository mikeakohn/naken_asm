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
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
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

int parse_instruction_sh4(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  //int offset;
  int value;
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
        case OP_IMM_REG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_REG)
          {
            if (asm_context->pass == 2)
            {
              if (operands[0].value < -128 || operands[0].value > 0xff)
              {
                print_error_range("Constant", -128, 0xff, asm_context);
                return -1;
              }

              value = operands[0].value & 0xff;
            }
              else
            {
              value = 0;
            }

            opcode = table_sh4[n].opcode | value | (operands[1].value << 8);

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
            if (asm_context->pass == 2)
            {
              if (operands[0].value < 0 || operands[0].value > 0xff)
              {
                print_error_range("Constant", 0, 0xff, asm_context);
                return -1;
              }

              value = operands[0].value & 0xff;
            }
              else
            {
              value = 0;
            }

            opcode = table_sh4[n].opcode | value;

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

