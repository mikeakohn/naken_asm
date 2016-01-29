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
#include "asm/propeller.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/propeller.h"

enum
{
  OPERAND_NUMBER,
  OPERAND_IMMEDIATE,
};

struct _operand
{
  int type;
  int value;
};

int parse_instruction_propeller(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  struct _operand operands[2];
  int operand_count;
  uint32_t opcode;
  int n, i;

  lower_copy(instr_case, instr);

  memset(operands, 0, sizeof(operands));
  operand_count = 0;

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (token_type == TOKEN_POUND)
    {
      operands[operand_count].type = OPERAND_IMMEDIATE;
    }
      else
    {
      operands[operand_count].type = OPERAND_NUMBER;
      tokens_push(asm_context, token, token_type);
    }

    if (eval_expression(asm_context, &operands[operand_count].value) != 0)
    {
      if (asm_context->pass == 2)
      {
        print_error_illegal_expression(instr, asm_context);
        return -1;
      }

      eat_operand(asm_context);
      operands[operand_count].value = 0;
    }

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (IS_NOT_TOKEN(token,','))
    {
      print_error_unexp(token, asm_context);
    }
  }

  n = 0;

  while(table_propeller[n].instr != NULL)
  {
    if (strcmp(instr_case, table_propeller[n].instr) == 0)
    {
      for (i = 0; i < operand_count; i++)
      {
        if (operands[i].type == OPERAND_NUMBER)
        {
          if (check_range(asm_context, "Address", operands[0].value, 0, 511) == -1) { return -1; }
        }
          else
        if (operands[i].type == OPERAND_IMMEDIATE)
        {
          if (check_range(asm_context, "Address", operands[0].value, -256, 511) == -1) { return -1; }
        }
      }

      switch(table_propeller[n].type)
      {
        case PROPELLER_OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          add_bin32(asm_context, table_propeller[n].opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_DS:
        case PROPELLER_OP_DS_15_1:
        case PROPELLER_OP_DS_15_2:
        {
          if (operand_count != 2 || operands[0].type == OPERAND_IMMEDIATE)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_propeller[n].type == PROPELLER_OP_DS_15_1 &&
              (operands[1].value & 0x1) != 0)
          {
            print_error_align(asm_context, 1);
            return -1;
          }

          if (table_propeller[n].type == PROPELLER_OP_DS_15_2 &&
              (operands[1].value & 0x3) != 0)
          {
            print_error_align(asm_context, 2);
            return -1;
          }

          opcode = table_propeller[n].opcode;

          if (operands[1].type == OPERAND_IMMEDIATE)
          {
            opcode |= 0x00400000;
          }

          opcode |= (operands[0].value | 0x1f) << 9;
          opcode |= operands[1].value | 0x1f;

          add_bin32(asm_context, table_propeller[n].opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_S:
        {
          if (operand_count != 1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode;

          if (operands[0].type == OPERAND_IMMEDIATE)
          {
            opcode |= 0x00400000;
          }

          opcode |= operands[0].value | 0x1f;

          add_bin32(asm_context, table_propeller[n].opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_D:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_IMMEDIATE)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode;
          opcode |= operands[0].value | 0x1f;

          add_bin32(asm_context, table_propeller[n].opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_IMMEDIATE:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_IMMEDIATE)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode;
          opcode |= operands[0].value | 0x1f;

          add_bin32(asm_context, table_propeller[n].opcode, IS_OPCODE);
          return 4;
        }
        default:
        { 
          print_error_internal(asm_context, __FILE__, __LINE__);
          return -1;
        }
      }
    }

    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}



