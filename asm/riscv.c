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
#include "asm/riscv.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/riscv.h"

enum
{
  OPERAND_NONE,
  OPERAND_X_REGISTER,
  OPERAND_F_REGISTER,
  OPERAND_NUMBER,
};

struct _operand
{
  int value;
  int type;
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

static int get_x_register_riscv(char *token)
{
  if (token[0] != 'x') { return -1; }

  return get_register_number(token + 1);
}

static int get_f_register_riscv(char *token)
{
  if (token[0] != 'f') { return -1; }

  return get_register_number(token + 1);
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case)
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

    // FIXME - FILL IN
#if 0
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL) break;
    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
#endif

    if (operand_count == 0 && IS_TOKEN(token, '.'))
    {
      strcat(instr_case, ".");
      token_type = tokens_get(asm_context, token, TOKENLEN);
      n = 0;
      while(token[n] != 0) { token[n] = tolower(token[n]); n++; }
      strcat(instr_case, token);
      continue;
    }

    do
    {
      n = get_x_register_riscv(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_X_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      n = get_f_register_riscv(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_F_REGISTER;
        operands[operand_count].value = n;
        break;
      }

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

int parse_instruction_riscv(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[5];
  int operand_count;
  int matched = 0;
  uint32_t opcode;
  int n;

  lower_copy(instr_case, instr);

  memset(&operands, 0, sizeof(operands));

  operand_count = get_operands(asm_context, operands, instr, instr_case);

  if (operand_count < 0) { return -1; }


  n = 0;
  while(table_riscv[n].instr != NULL)
  {
    if (strcmp(table_riscv[n].instr,instr_case) == 0)
    {
      matched = 1;

      switch(table_riscv[n].type)
      {
        case OP_R_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_I_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < -2048 || operands[2].value >= 2047)
          {
            print_error_range("Immediate", -2048, 2047, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_UI_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value >= 4096)
          {
            print_error_range("Immediate", 0, 4095, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_S_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < -2048 || operands[2].value >= 2048)
          {
            print_error_range("Immediate", -2048, 2047, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                (((operands[2].value >> 5) & 0x7f) << 25) |
                  (operands[1].value << 20) |
                  (operands[0].value << 15) |
                 ((operands[2].value & 0x1f) << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SB_TYPE:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int32_t offset = 0;

          if (asm_context->pass == 2)
          {
            offset = (uint32_t)operands[2].value - (asm_context->address + 4);

            if ((offset & 0x3) != 0)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (offset < -4096 || offset >= 4095)
            {
              print_error_range("Offset", -4096, 4095, asm_context);
              return -1;
            }

            offset = ((offset >> 12) & 0x1) << 11;
            offset |= (offset >> 11) & 0x1;
            offset |= ((offset >> 5) & 0x3f) << 25;
            offset |= ((offset >> 1) & 0xf) << 8;
          }


          opcode = table_riscv[n].opcode |
                  (offset |
                  (operands[1].value << 20) |
                  (operands[0].value << 15));
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_U_TYPE:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < -(1 << 19) || operands[1].value >= (1 << 20))
          {
            print_error_range("Immediate", -(1 << 19), (1 << 20) - 1, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 12) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_UJ_TYPE:
        case OP_SHIFT:
        case OP_FENCE:
        case OP_FFFF:
        case OP_READ:
        case OP_LR:
        case OP_STD_EXT:
        case OP_R_FP1:
        case OP_R_FP2:
        case OP_R_FP3:
        case OP_R_FP2_RM:
        case OP_R_FP3_RM:
        case OP_R_FP4_RM:
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



