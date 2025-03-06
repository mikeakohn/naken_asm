/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "asm/pdp11.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/pdp11.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
  uint8_t flags;
};

int parse_instruction_pdp11(AsmContext *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
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
      print_error_unexp(asm_context, token);
      return -1;
    }

    operands[operand_count].value = num;
    operands[operand_count].type = OPERAND_NUMBER;

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(asm_context, token);
      return -1;
    }
  }

  for (n = 0; table_pdp11[n].instr != NULL; n++)
  {
    if (strcmp(table_pdp11[n].instr, instr_case) == 0)
    {
      switch (table_pdp11[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(asm_context, instr);
            return -1;
          }

          add_bin16(asm_context, table_pdp11[n].opcode, IS_OPCODE);

          return 2;
        }
        default:
        {
          break;
        }
      }
    }
  }

  print_error_unknown_instr(asm_context, instr);

  return -1;
}

