/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "asm_common.h"
#include "asm_epiphany.h"
#include "assembler.h"
#include "disasm_epiphany.h"
#include "tokens.h"
#include "eval_expression.h"
#include "table_epiphany.h"

struct _operand
{
  uint8_t type;
  uint8_t reg;
  int value;
};

int parse_instruction_epiphany(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  char token[TOKENLEN];
  int token_type;
  struct _operand operands[3];
  int operand_count;
  int n;

  lower_copy(instr_case, instr);
  operand_count = 0;
  memset(operands, 0, sizeof(operands));

  // Parse operands
  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    { 
      break;
    }

    if (operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count != 0)
    {
      if (IS_NOT_TOKEN(token,','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
    }

#if 0
    if (strcasecmp(token,"A") == 0)
    {
      operands[operand_count].type = OP_REG_A;
    }
      else
#endif
    if (IS_TOKEN(token,'#'))
    {
      operands[operand_count].value = 0;
    }
      else
    if (IS_TOKEN(token,'('))
    {
    }
      else
    if (IS_TOKEN(token,'['))
    {
    }
      else
    {
      operands[operand_count].value = 0;
    }

    operand_count++;
  }

  // Get opcodes
  n = 0;
  while(table_epiphany[n].instr != NULL)
  {
    if (strcmp(table_epiphany[n].instr, instr_case) == 0)
    {
      switch(table_epiphany[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
          }
          break;
        }
        default:
          printf("Internal error %s:%d\n", __FILE__, __LINE__);
          return -1;
          break;
      }
    }

    n++;
  }

  print_error_unknown_operand_combo(instr, asm_context);

  return -1;
}

