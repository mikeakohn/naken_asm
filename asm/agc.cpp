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

// Apollo Guidance Computer.

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "asm/agc.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/agc.h"

#define MAX_OPERANDS 1

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
};

struct Operand
{
  int value;
  int type;
};

int parse_instruction_agc(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct Operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  uint16_t opcode;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

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

    operands[operand_count].type = OPERAND_NUMBER;
    operands[operand_count].value = num;

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == MAX_OPERANDS)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  for (n = 0; table_agc[n].instr != NULL; n++)
  {
    if (strcmp(table_agc[n].instr, instr_case) == 0)
    {
      if (table_agc[n].is_extra_code)
      {
        add_bin16(asm_context, 000006, IS_OPCODE);
      }

      switch (table_agc[n].type)
      {
        case AGC_OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          add_bin16(asm_context, table_agc[n].opcode, IS_OPCODE);

          return 2;
        }
        case AGC_OP_K10:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "Address", operands[0].value, 0, 0x3ff) == -1) { return -1; }

          opcode = table_agc[n].opcode | (operands[0].value & 0x3ff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case AGC_OP_K12:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "Address", operands[0].value, 0, 0xfff) == -1) { return -1; }

          opcode = table_agc[n].opcode | (operands[0].value & 0xfff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case AGC_OP_IO:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "IO", operands[0].value, 0, 0x1ff) == -1) { return -1; }

          opcode = table_agc[n].opcode | (operands[0].value & 0x1ff);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case AGC_OP_NDX:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "index", operands[0].value, 0, 0xfff) == -1) { return -1; }

          if ((operands[0].value & 0xc00) == 0)
          {
            opcode = table_agc[n].opcode | (operands[0].value & 0x3ff);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 2;
          }
            else
          {
            opcode = table_agc[n].opcode | (operands[0].value & 0xfff);
            add_bin16(asm_context, 0x0006, IS_OPCODE);
            add_bin16(asm_context, opcode, IS_OPCODE);
            return 4;
          }
        }
        default:
          break;
      }
    }
  }

  print_error_unknown_instr(asm_context, instr);

  return -1;
}

