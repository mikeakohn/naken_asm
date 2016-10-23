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

#define MAX_OPERANDS 3

enum
{
  OPERAND_INVALID,
  OPERAND_REGISTER,
  OPERAND_LR,
  OPERAND_CR,
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
};

struct _modifiers
{
  int has_dot;
};

static int get_register_number(char *token)
{ 
  int num;
  
  if (token[0] < '0' || token[0] > '9') { return -1; }
  if (token[1] == 0) { return token[0] - '0'; }
  if (token[0] == '0' || token[2] != 0) { return -1; }
  if (token[1] < '0' || token[1] > '9') { return -1; }
  
  num = ((token[0] - '0') << 10) + (token[1] - '0');
  
  return (num < 32) ? num : -1;
}

static int get_register_powerpc(char *token)
{ 
  if (token[0] != 'r') { return -1; }
  
  return get_register_number(token + 1);
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case, struct _modifiers *modifiers)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int n;

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (operand_count == 0 && IS_TOKEN(token, '.'))
    {
      //token_type = tokens_get(asm_context, token, TOKENLEN);

      //strcat(instr_case, ".");
      //strcat(instr_case, token);

      modifiers->has_dot = 1;
      continue;
    }

    do
    {
      // Check for registers
      n = get_register_powerpc(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      if (strcasecmp(token, "lr") == 0)
      {
        operands[operand_count].type = OPERAND_LR;
        break;
      }

      if (strncasecmp(token, "cr", 2) == 0 && token[2] != 0)
      {
        if (token[3] == 0 && token[2] >= '0' && token[2] <= '7')
        {
          operands[operand_count].type = OPERAND_CR;
          operands[operand_count].value = token[2] - '0';
          break;
        }
      }

      // Assume this is just a number
      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
        n = 0;
      }
      else
      {
         tokens_push(asm_context, token, token_type);
         if (eval_expression(asm_context, &n) != 0)
         {
           print_error_unexp(token, asm_context);
           return -1;
         }
      }

      operands[operand_count].type = OPERAND_NUMBER;
      operands[operand_count].value = n;

      break;
    } while(0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

  return operand_count;
}

int parse_instruction_powerpc(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  int matched = 0;
  uint32_t opcode;
  int32_t offset;
  struct _modifiers modifiers;
  int temp;
  int n;

  memset(&modifiers, 0, sizeof(modifiers));
  memset(&operands, 0, sizeof(operands));

  lower_copy(instr_case, instr);

  operand_count = get_operands(asm_context, operands, instr, instr_case, &modifiers);

  if (operand_count < 0) { return -1; }

  n = 0;
  while(table_powerpc[n].instr != NULL)
  {
    if (strcmp(table_powerpc[n].instr, instr_case) == 0)
    {
      matched = 1;

      if (modifiers.has_dot == 1 && !(table_powerpc[n].flags & FLAG_DOT))
      {
        n++;
        continue;
      }

      if (modifiers.has_dot == 0 &&
         ((table_powerpc[n].flags & FLAG_REQUIRE_DOT) == FLAG_REQUIRE_DOT))
      {
        n++;
        continue;
      }

      switch(table_powerpc[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin32(asm_context, table_powerpc[n].opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RD_RA_RB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 16) |
                  (operands[2].value << 11);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RA_RS_RB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[1].value << 21) |
                  (operands[0].value << 16) |
                  (operands[2].value << 11);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RD_RA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 16);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RA_RS:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[1].value << 21) |
                  (operands[0].value << 16);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RD_RA_SIMM:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < -32768 || operands[2].value > 0xffff)
          {
            print_error_range("Immediate", -32768, 65535, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 16) |
                  (operands[2].value & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RD_SIMM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < -32768 || operands[1].value > 0xffff)
          {
            print_error_range("Immediate", -32768, 65535, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_RA_RS_UIMM:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value > 0xffff)
          {
            print_error_range("Immediate", 0, 65535, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 16) |
                  (operands[2].value & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);
          
          return 4;
        }
        case OP_BRANCH:
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

          offset = (uint32_t)operands[0].value - asm_context->address;

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          temp = (offset >> 26) & 0x3f;

          if (temp != 0 && temp != 0x3f)
          {
            print_error_range("Offset", -(1 << 25), (1 << 25) - 1, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (offset & ((1 << 26) - 1));

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_JUMP:
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

          if ((operands[0].value & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          temp = (operands[2].value >> 26) & 0x3f;

          if (temp != 0 && temp != 0x3f)
          {
            print_error_range("Address", -(1 << 25), (1 << 25) - 1, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[2].value & ((1 << 26) - 1));

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BRANCH_COND_BD:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          offset = (uint32_t)operands[2].value - asm_context->address;

          if ((offset & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          temp = offset & 0xffff0000;

          if (temp != 0 && temp != 0xffff0000)
          {
            print_error_range("Offset", -(1 << 15), (1 << 15) - 1, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 31 ||
              operands[1].value < 0 || operands[1].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 21) |
                   (operands[1].value << 16) |
                   (offset & ((1 << 26) - 1));

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_JUMP_COND_BD:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if ((operands[2].value & 0x3) != 0)
          {
            print_error_align(asm_context, 4);
            return -1;
          }

          temp = operands[2].value & 0xffff0000;

          if (temp != 0 && temp != 0xffff0000)
          {
            print_error_range("Address", -(1 << 15), (1 << 15) - 1, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 31 ||
              operands[1].value < 0 || operands[1].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 21) |
                   (operands[1].value << 16) |
                   (operands[2].value & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BRANCH_COND:
        {
          if (operand_count != 2)
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

          if (operands[0].value < 0 || operands[0].value > 31 ||
              operands[1].value < 0 || operands[1].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 21) |
                   (operands[1].value << 16);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_CMP:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_CR ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 23) |
                   (operands[1].value << 16) |
                   (operands[2].value << 11);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_CMPI:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_CR ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < -32768 || operands[2].value > 0xffff)
          {
            print_error_range("Immediate", -32768, 65535, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 23) |
                   (operands[1].value << 16) |
                   (operands[2].value & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_CRB_CRB_CRB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_NUMBER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 31 ||
              operands[1].value < 0 || operands[1].value > 31 ||
              operands[2].value < 0 || operands[2].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 21) |
                   (operands[1].value << 16) |
                   (operands[2].value << 11);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
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



