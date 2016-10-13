/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2016 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm/common.h"
#include "asm/powerpc.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/powerpc.h"

enum
{
  OPERAND_NONE,
};

struct _operand
{
  int value;
  int type;
};

int parse_instruction_powerpc(struct _asm_context *asm_context, char *instr)
{
char token[TOKENLEN];
int token_type;
char instr_case[TOKENLEN];
struct _operand operands[3];
int operand_count = 0;
int matched = 0;
int n;

  lower_copy(instr_case, instr);

  memset(&operands, 0, sizeof(operands));
  while(1)
  {
    token_type=tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    // FIXME - FILL IN

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token, ',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  n = 0;
  while(table_powerpc[n].instr != NULL)
  {
    if (strcmp(table_powerpc[n].instr,instr_case) == 0)
    {
      matched = 1;

      switch(table_powerpc[n].type)
      {
        case OP_BRANCH:
        default:
          break;
      }
    }

    n++;
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



