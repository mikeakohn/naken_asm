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

#define MAX_OPERANDS 2

enum
{
  OPERAND_REG,
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
  //int count;
  //uint16_t opcode;
  //int min, max;

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

    // Remove indent?
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

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  for (n = 0; table_agc[n].instr != NULL; n++)
  {
    if (strcmp(table_agc[n].instr, instr_case) == 0)
    {
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
          if (operand_count != 1 || operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "Address", operands[1].value, 0, 0x3ff) == -1) { return -1; }

          return 2;
        }
        case AGC_OP_K12:
        {
          if (operand_count != 1 || operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "Address", operands[1].value, 0, 0xfff) == -1) { return -1; }

          return 2;
        }
        case AGC_OP_IO:
        {
          if (operand_count != 1 || operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(asm_context, instr);
            return -1;
          }

          if (check_range(asm_context, "IO", operands[1].value, 0, 0x1ff) == -1) { return -1; }

          return 2;
        }
        default:
          break;
      }
    }
  }

  print_error_unknown_instr(asm_context, instr);

  return -1;
}

