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
#include <ctype.h>

#include "asm/common.h"
#include "asm/tms340.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/tms340.h"

enum
{
  OPERAND_REGISTER,
  OPERAND_REGISTER_INDIRECT,
  OPERAND_REGISTER_INDIRECT_INC,
  OPERAND_REGISTER_INDIRECT_DEC,
  OPERAND_NUMBER,
  OPERAND_SYMBOLIC,
};

struct _operand
{
  uint8_t type;
  uint8_t reg;
  int value;
  int r;
};

static int get_register_tms340(char *token, int *r)
{
  if (strcmp(token, "sp") == 0)
  {
    *r = 0;
    return 15;
  }

  if (token[0] == 'a' || token[0] == 'A')
  {
    *r = 0;
  }
    else
  if (token[0] == 'b' || token[0] == 'B')
  {
    *r = 1;
  }
    else
  {
    return -1;
  }

  if (token[2] == 0 && (token[1] >= '0' && token[1] <= '9'))
  {
    return token[1] - '0';
  }
    else
  if (token[3] == 0 && token[1] == '1' &&
      token[2] >= '0' && token[2] <= '5')
  {
    return 10 + (token[2] - '0');
  }

  return -1;
}

int parse_instruction_tms340(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  struct _operand operands[3];
  int operand_count;
  int matched = 0;
  //int count = 2;
  //int offset;
  //int opcode;
  int n, r;

  lower_copy(instr_case, instr);

  operand_count = 0;
  memset(operands, 0, sizeof(operands));

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count >= 3)
    {
      print_error_opcount(instr, asm_context);
      return -1;
    }

    if ((n = get_register_tms340(token, &r)) != -1)
    {
      operands[operand_count].type = OPERAND_REGISTER;
      operands[operand_count].reg = n;
      operands[operand_count].r = r;
    }
      else
    if (IS_TOKEN(token, '*'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      n = get_register_tms340(token, &r);

      if (n == -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].type = OPERAND_REGISTER_INDIRECT;
      operands[operand_count].reg = n;
      operands[operand_count].r = r;

      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (IS_TOKEN(token,'+'))
      {
        operands[operand_count].type = OPERAND_REGISTER_INDIRECT_INC;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }
    }
      else
    if (IS_TOKEN(token,'@'))
    {
      operands[operand_count].type = OPERAND_SYMBOLIC;

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        operands[operand_count].value = 0;
      }
        else
      {
        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        operands[operand_count].value = n;
      }
    }
      else
    {
      tokens_push(asm_context, token, token_type);

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        n = 0;
      }
        else
      {
        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = n;
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) break;

    if (IS_NOT_TOKEN(token,',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  for (n = 0; table_tms340[n].instr != NULL; n++)
  {
    if (strcmp(table_tms340[n].instr, instr_case) == 0)
    {
      matched = 1;

      if (table_tms340[n].operand_count != operand_count)
      {
        continue;
      }

      if (table_tms340[n].operand_count == 0 && operand_count == 0)
      {
        add_bin16(asm_context, table_tms340[n].opcode, IS_OPCODE);
        return 2;
      }

#if 0
      switch(table_tms340[n].type)
      {
        case OP_NONE:
        {
        }
      }
#endif
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

