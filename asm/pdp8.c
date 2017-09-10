/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn, Lars Brinkhoff
 *
 * PDP-8 by Lars Brinkhoff
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "asm/pdp8.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/pdp8.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
};

int parse_instruction_pdp8(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  uint16_t opcode;

  if (asm_context->pass == 1)
  {
    ignore_line(asm_context);

    add_bin16(asm_context, 0, IS_OPCODE);
    return 2;
  }

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

    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_NUMBER;
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 2)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  n = 0;
  while(table_pdp8[n].instr != NULL)
  {
    if (strcmp(table_pdp8[n].instr, instr_case) == 0)
    {
      switch(table_pdp8[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin16(asm_context, table_pdp8[n].opcode, IS_OPCODE);

          return 2;
        }
        case OP_M:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < -64 || operands[0].value > 127)
          {
            print_error_range("Literal", -64, 127, asm_context);
            return -1;
          }

          opcode = table_pdp8[n].opcode | (operands[0].value & 0x7f);

          print_error_illegal_operands(instr, asm_context);
          return -1;
        }
        case OP_IOT:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < -64 || operands[0].value > 127)
          {
            print_error_range("Literal", -64, 127, asm_context);
            return -1;
          }

          opcode = table_pdp8[n].opcode | (operands[0].value & 0x7f);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        case OP_OPR:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < -64 || operands[0].value > 127)
          {
            print_error_range("Literal", -64, 127, asm_context);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 7)
          {
            print_error_range("Bit", 0, 7, asm_context);
            return -1;
          }

          opcode = table_pdp8[n].opcode |
                  (operands[0].value & 0x7f) |
                  (operands[1].value << 7);
          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
        }
        default:
          break; 
      }
    }

    n++;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1; 
}


