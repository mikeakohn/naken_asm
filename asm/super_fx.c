/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

#include "asm/super_fx.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/super_fx.h"

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

static int get_register_super_fx(char *token)
{
  int reg = 0;

  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  token++;

  while(*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }
    reg = (reg * 10) + (*token) - '0';

    token++;
  }

  if (reg >= 16) { return -1; }

  return reg;
}

int parse_instruction_super_fx(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n;
  int count;
  uint16_t opcode;

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

    num = get_register_super_fx(token);

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
    if (IS_TOKEN(token,'('))
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

      if (expect_token(asm_context, ')') == -1) { return -1; }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_ADDRESS;
    }
      else
    {
      print_error_unexp(token, asm_context);
      return -1;
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
  while(table_super_fx[n].instr != NULL)
  {
    if (strcmp(table_super_fx[n].instr, instr_case) == 0)
    {
      if (table_super_fx[n].alt == 1)
      {
        add_bin8(asm_context, 0x3d, IS_OPCODE);
        count = 1;
      }
        else
      if (table_super_fx[n].alt == 2)
      {
        add_bin8(asm_context, 0x3e, IS_OPCODE);
        count = 1;
      }
        else
      if (table_super_fx[n].alt == 3)
      {
        add_bin8(asm_context, 0x3f, IS_OPCODE);
        count = 1;
      }

      switch(table_super_fx[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_super_fx[n].opcode, IS_OPCODE);

          return count + 1;
        }
        case OP_REG:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[0].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[0].value;

          add_bin8(asm_context, opcode, IS_OPCODE);

          return count + 1;
        }
        case OP_NUM:
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

          if (operands[0].value < -128 || operands[0].value > 255)
          {
            print_error_range("Literal", -128, 255, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_super_fx[n].opcode, IS_OPCODE);
          add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

          return count + 2;
        }
        case OP_OFFSET:
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

          int offset = operands[0].value - (asm_context->address + 2);

          if (offset < -128 || operands[0].value > 127)
          {
            print_error_range("Offset", -128, 127, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_super_fx[n].opcode, IS_OPCODE);
          add_bin8(asm_context, offset & 0xff, IS_OPCODE);

          return count + 2;
        }
        case OP_REG_NUM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[0].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
            return -1;
          }

          if (operands[1].value < -128 || operands[1].value > 255)
          {
            print_error_range("Literal", -128, 255, asm_context);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[0].value;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);

          return count + 1;
        }
        case OP_REG_WORD:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[0].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
            return -1;
          }

          if (operands[0].value < -32768 || operands[0].value > 0xffff)
          {
            print_error_range("Literal", -32768, 0xffff, asm_context);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[0].value;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
          add_bin8(asm_context, (operands[0].value >> 8) & 0xff, IS_OPCODE);

          return count + 3;
        }
        case OP_REG_MEM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_ADDRESS)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[0].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 0xffff)
          {
            print_error_range("Literal", 0, 0xffff, asm_context);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[0].value;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);
          add_bin8(asm_context, (operands[1].value >> 8) & 0xff, IS_OPCODE);

          return count + 1;
        }
        case OP_MEM_REG:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_ADDRESS ||
              operands[1].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[1].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[1].value);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 0xffff)
          {
            print_error_range("Literal", 0, 0xffff, asm_context);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[1].value;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);
          add_bin8(asm_context, (operands[0].value >> 8) & 0xff, IS_OPCODE);

          return count + 1;
        }
        case OP_REG_SMEM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REG ||
              operands[1].type != OPERAND_ADDRESS)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[0].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[0].value);
            return -1;
          }

          if (operands[1].value < 0 || operands[1].value > 255)
          {
            print_error_range("Literal", 0, 255, asm_context);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[0].value;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, operands[1].value & 0xff, IS_OPCODE);

          return count + 1;
        }
        case OP_SMEM_REG:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_ADDRESS ||
              operands[1].type != OPERAND_REG)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (((1 << operands[1].value) & table_super_fx[n].reg_mask) == 0)
          {
            printf("Error: Cannot use r%d with this instruction.\n", operands[1].value);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 255)
          {
            print_error_range("Literal", 0, 255, asm_context);
            return -1;
          }

          opcode = table_super_fx[n].opcode | operands[1].value;

          add_bin8(asm_context, opcode, IS_OPCODE);
          add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

          return count + 1;
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


