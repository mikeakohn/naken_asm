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
#include <ctype.h>

#include "asm/unsp.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/unsp.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NONE = 0,
  OPERAND_REGISTER,
  OPERAND_NUMBER,
  OPERAND_INDIRECT_BP_IMM6,
  OPERAND_IMM6,
  OPERAND_INDIRECT_RS,
  OPERAND_INDIRECT_RS_DEC,
  OPERAND_INDIRECT_RS_INC,
  OPERAND_INDIRECT_INC_RS,
  OPERAND_D_INDIRECT_RS,
  OPERAND_D_INDIRECT_RS_DEC,
  OPERAND_D_INDIRECT_RS_INC,
  OPERAND_D_INDIRECT_INC_RS,
  OPERAND_RS,
  OPERAND_IMM16,
  OPERAND_FROM_INDIRECT_ADDR16,
  OPERAND_TO_INDIRECT_ADDR16,
  OPERAND_RS_ASR_SHIFT,
  OPERAND_RS_LSL_SHIFT,
  OPERAND_RS_LSR_SHIFT,
  OPERAND_RS_ROL_SHIFT,
  OPERAND_RS_ROR_SHIFT,
  OPERAND_INDIRECT_ADDR6,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_unsp(const char *token)
{
  if ((token[0] == 'r' || token[0] == 'R') &&
      (token[1] >= '0' && token[1] <= '7') &&
      (token[2] == 0))
  {
    return token[1] - '0';
  }

  if (strcasecmp(token, "sp") == 0) { return 0; }
  if (strcasecmp(token, "bp") == 0) { return 5; }
  if (strcasecmp(token, "sr") == 0) { return 6; }
  if (strcasecmp(token, "pc") == 0) { return 7; }

  return -1;
}

int parse_instruction_unsp(struct _asm_context *asm_context, char *instr)
{
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  int token_type;
  int opcode;
  int matched = 0;
  int n;

  lower_copy(instr_case, instr);

  if (strcmp(instr_case, "int") == 0 ||
      strcmp(instr_case, "fir_mov") == 0 ||
      strcmp(instr_case, "irq") == 0 ||
      strcmp(instr_case, "fiq") == 0)
  {
    int count;

    strcat(instr_case, " ");

    for (count = 0; count < 3; count++)
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
      {
        tokens_push(asm_context, token, token_type);
        break;
      }

      if (strlen(token) > 10)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      lower_copy(instr_case + strlen(instr_case), token);
    }
  }

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    do
    {
      // Check for registers
      n = get_register_unsp(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      operands[operand_count].type = OPERAND_NUMBER;

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        operands[operand_count].value = 0;
      }
      else
      {
        tokens_push(asm_context, token, token_type);

        if (eval_expression(asm_context, &n) != 0)
        {
          return -1;
        }

        operands[operand_count].value = n;
      }
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

  for (n = 0; table_unsp[n].instr != NULL; n++)
  {
    if (strcmp(table_unsp[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_unsp[n].type)
      {
        case UNSP_OP_NONE:
        {
          add_bin16(asm_context, table_unsp[n].opcode, IS_OPCODE);
          return 2;
        }
        case UNSP_OP_GOTO:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (operands[0].value < 0 || operands[0].value > 0x3fffff)
            {
              print_error_range("Constant", 0, 0x3fffff, asm_context);
              return -1;
            }

            opcode =
              table_unsp[n].opcode |
            ((operands[0].value >> 8) & 0x3fffff);

            add_bin16(asm_context, opcode, IS_OPCODE);
            add_bin16(asm_context, operands[0].value & 0xffff, IS_OPCODE);

            return 4;
          }

          break;
        }
        case UNSP_OP_MUL:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER)
          {
            opcode =
              table_unsp[n].opcode |
             (operands[0].value << 9) |
              operands[1].value;

            add_bin16(asm_context, opcode, IS_OPCODE);

            return 2;
          }

          break;
        }
        case UNSP_OP_JMP:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            int offset = operands[0].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (offset < -0x3f || offset > -0x3f)
            {
              print_error_range("Offset", -0x3f, 0x3f, asm_context);
              return -1;
            }

            if (offset < 0)
            {
              offset = -offset;
            }
              else
            {
              offset |= 0x40;
            }

            add_bin16(asm_context, table_unsp[n].opcode | offset, IS_OPCODE);

            return 2;
          }

          break;
        }
        case UNSP_OP_ALU:
        {
        }
        case UNSP_OP_STACK:
        {
        }
        default:
        {
          break;
        }
      }

      break;
    }
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

