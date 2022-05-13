/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
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

#define MAX_OPERANDS 5

enum
{
  OPERAND_INVALID,
  OPERAND_REGISTER,
  OPERAND_SPR,
  OPERAND_TBR,
  OPERAND_CR,
  OPERAND_NUMBER,
  OPERAND_REGISTER_OFFSET,
  OPERAND_VECTOR,
  OPERAND_FPU_REGISTER,
  OPERAND_BF,
};

struct _operand
{
  int value;
  int type;
  int16_t offset;
};

struct _modifiers
{
  int has_dot;
};

static int get_register_number(char *token)
{
  int num = 0;

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }

    num = (num * 10) + (*token - '0');
    token++;

    if (num > 31) { return -1; }
  }

  return num;
}

static int get_register_powerpc(char *token)
{
  if (tolower(token[0]) != 'r') { return -1; }

  return get_register_number(token + 1);
}

static int get_register_altivec(char *token)
{
  if (tolower(token[0]) != 'v') { return -1; }

  return get_register_number(token + 1);
}

static int get_register_fpu(char *token)
{
  if (tolower(token[0]) != 'f') { return -1; }
  if (tolower(token[1]) != 'p') { return -1; }

  return get_register_number(token + 2);
}

static int get_spr_powerpc(char *token)
{
  int n = 0;

  while (powerpc_spr[n].name != NULL)
  {
    if (strcasecmp(token, powerpc_spr[n].name) == 0)
    {
      return powerpc_spr[n].value;
    }

    n++;
  }

  return -1;
}

static int get_bf_powerpc(char *token)
{
  if (strcasecmp(token, "fl") == 0) { return 0; }
  if (strcasecmp(token, "fg") == 0) { return 1; }
  if (strcasecmp(token, "fe") == 0) { return 2; }
  if (strcasecmp(token, "fu") == 0) { return 3; }

  return -1;
}

static int get_operands(struct _asm_context *asm_context, struct _operand *operands, char *instr, char *instr_case, struct _modifiers *modifiers)
{
  char token[TOKENLEN];
  int token_type;
  int operand_count = 0;
  int n;

  while (1)
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
      // Check for PowerPC registers
      n = get_register_powerpc(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      // Check for Altivec registers
      n = get_register_altivec(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_VECTOR;
        operands[operand_count].value = n;
        break;
      }

      // Check for FPU registers
      n = get_register_fpu(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_FPU_REGISTER;
        operands[operand_count].value = n;
        break;
      }

      // Check for SPR
      n = get_spr_powerpc(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_SPR;
        operands[operand_count].value = n;
        break;
      }

      // Check for BF
      n = get_bf_powerpc(token);

      if (n != -1)
      {
        operands[operand_count].type = OPERAND_BF;
        operands[operand_count].value = n;
        break;
      }

      // Check for TBR
      if (strcasecmp(token, "tbl") == 0)
      {
        operands[operand_count].type = OPERAND_TBR;
        operands[operand_count].value = 268;
        break;
      }

      if (strcasecmp(token, "tbu") == 0)
      {
        operands[operand_count].type = OPERAND_TBR;
        operands[operand_count].value = 269;
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

      // Check if this is (reg)
      if (IS_TOKEN(token, '('))
      {
        char token[TOKENLEN];
        int token_type;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        n = get_register_powerpc(token);

        if (n != -1)
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, ')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].offset = 0;
          operands[operand_count].type = OPERAND_REGISTER_OFFSET;
          operands[operand_count].value = n;

          break;
        }

        tokens_push(asm_context, token, token_type);
      }

      // Assume this is just a number
      operands[operand_count].type = OPERAND_NUMBER;

      if (asm_context->pass == 1)
      {
        ignore_operand(asm_context);
        operands[operand_count].value = 0;
      }
      else
      {
        tokens_push(asm_context, token, token_type);
        if (eval_expression(asm_context, &n) != 0)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        operands[operand_count].value = n;

        token_type = tokens_get(asm_context, token, TOKENLEN);
        if (IS_TOKEN(token, '('))
        {
          if (operands[operand_count].value < -32768 ||
              operands[operand_count].value > 32767)
          {
            print_error_range("Offset", -32768, 32767, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          n = get_register_powerpc(token);
          if (n == -1)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          operands[operand_count].offset = (uint16_t)operands[operand_count].value;
          operands[operand_count].type = OPERAND_REGISTER_OFFSET;
          operands[operand_count].value = n;

          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (IS_NOT_TOKEN(token, ')'))
          {
            print_error_unexp(token, asm_context);
            return -1;
          }
        }
          else
        {
          tokens_push(asm_context, token, token_type);
        }
      }

      break;
    } while (0);

    operand_count++;

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }

    if (IS_NOT_TOKEN(token, ',') || operand_count == 5)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (operand_count == MAX_OPERANDS)
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
  while (table_powerpc[n].instr != NULL)
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

      switch (table_powerpc[n].type)
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
        case OP_RS_RA_RB:
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
        case OP_RA_RS_SH:
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

          if (operands[2].value < 0 || operands[2].value > 31)
          {
            print_error_range("Immediate", -32768, 65535, asm_context);
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
        case OP_RD:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[0].value << 21);
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
                  (operands[1].value << 21) |
                  (operands[0].value << 16) |
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
                   (offset & 0xfffc);

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
                   (operands[2].value & 0xfffc);

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
        case OP_BRANCH_COND_ALIAS:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            memcpy(&operands[1], &operands[0], sizeof(struct _operand));
            memset(&operands[0], 0, sizeof(struct _operand));
            operands[0].type = OPERAND_NUMBER;
            operand_count = 2;
          }

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

          offset = (uint32_t)operands[1].value - asm_context->address;

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

          if (operands[0].value < 0 || operands[0].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 16) |
                   (offset & 0xfffc);

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
        case OP_RD_OFFSET_RA:
        case OP_RS_OFFSET_RA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, table_powerpc[n].opcode, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER_OFFSET)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 21) |
                   (operands[1].value << 16) |
                   (operands[1].offset & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RD_RA_NB:
        case OP_RS_RA_NB:
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

          if (operands[2].value < 0 || operands[2].value > 31)
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
        case OP_CRD_CRS:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_CR || operands[1].type != OPERAND_CR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 23) |
                  (operands[1].value << 18);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_CRD:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_CR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[0].value << 23);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RD_SPR:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[1].type == OPERAND_NUMBER)
          {
            if (operands[1].value >= 0 && operands[1].value < 1024)
            {
              operands[1].type = OPERAND_SPR;
            }
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_SPR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                 ((operands[1].value & 0x1f) << 16) |
                 ((operands[1].value >> 5) << 11);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SPR_RS:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type == OPERAND_NUMBER)
          {
            if (operands[0].value >= 0 && operands[1].value < 1024)
            {
              operands[0].type = OPERAND_SPR;
            }
          }

          if (operands[0].type != OPERAND_SPR ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[1].value << 21) |
                 ((operands[0].value & 0x1f) << 16) |
                 ((operands[0].value >> 5) << 11);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RD_TBR:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[1].type == OPERAND_NUMBER)
          {
            if (operands[1].value >= 0 && operands[1].value < 1024)
            {
              operands[1].type = OPERAND_TBR;
            }
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_TBR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                 ((operands[1].value & 0x1f) << 16) |
                 ((operands[1].value >> 5) << 11);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_CRM_RS:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 0xff)
          {
            print_error_range("Constant", 0, 0xff, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[1].value << 21) |
                  (operands[0].value << 12);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RA_RS_SH_MB_ME:
        {
          if (operand_count != 5)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_REGISTER ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_NUMBER ||
              operands[3].type != OPERAND_NUMBER ||
              operands[4].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[2].value < 0 || operands[2].value > 31 ||
              operands[3].value < 0 || operands[3].value > 31 ||
              operands[4].value < 0 || operands[4].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[1].value << 21) |
                  (operands[0].value << 16) |
                  (operands[2].value << 11) |
                  (operands[3].value << 6) |
                  (operands[4].value << 1);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_STRM:
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

          if (operands[0].value < 0 || operands[0].value > 3)
          {
            print_error_range("Constant", 0, 3, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RA_RB_STRM:
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

          opcode = table_powerpc[n].opcode |
            (operands[1].value << 11) |
            (operands[0].value << 16) |
            (operands[2].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_RA_RB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_REGISTER ||
              operands[2].type != OPERAND_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
            (operands[2].value << 11) |
            (operands[1].value << 16) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD:
        case OP_VB:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (table_powerpc[n].type == OP_VD)
          {
            opcode = table_powerpc[n].opcode | (operands[0].value << 21);
          }
          else
          {
            opcode = table_powerpc[n].opcode | (operands[0].value << 11);
          }
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_VA_VB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_VECTOR ||
              operands[2].type != OPERAND_VECTOR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
            (operands[2].value << 11) |
            (operands[1].value << 16) |
            (operands[0].value << 21);

          if (modifiers.has_dot == 1)
          {
            opcode |= (1 << 10);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_VA_VB_VC:
        {
          if (operand_count != 4)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_VECTOR ||
              operands[2].type != OPERAND_VECTOR ||
              operands[3].type != OPERAND_VECTOR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
            (operands[3].value << 6) |
            (operands[2].value << 11) |
            (operands[1].value << 16) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_VA_VC_VB:
        {
          if (operand_count != 4)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_VECTOR ||
              operands[2].type != OPERAND_VECTOR ||
              operands[3].type != OPERAND_VECTOR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
            (operands[2].value << 6) |
            (operands[3].value << 11) |
            (operands[1].value << 16) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_VB_UIMM:
        case OP_VD_VB_SIMM:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_VECTOR ||
              operands[2].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          int lo = table_powerpc[n].type == OP_VD_VB_UIMM ? 0 : -16;
          int hi = table_powerpc[n].type == OP_VD_VB_UIMM ? 31 : 15;

          if (operands[2].value < lo || operands[2].value > hi)
          {
            print_error_range("Constant", lo, hi, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
            (operands[1].value << 11) |
           ((operands[2].value & 0x1f) << 16) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_SIMM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[1].value < -16 || operands[1].value > 15)
          {
            print_error_range("Constant", -16, 15, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
           ((operands[1].value & 0x1f) << 16) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_VB:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_VECTOR)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
            (operands[1].value << 11) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_VD_VA_VB_SH:
        {
          if (operand_count != 4)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_VECTOR ||
              operands[1].type != OPERAND_VECTOR ||
              operands[2].type != OPERAND_VECTOR ||
              operands[3].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[3].value < 0 || operands[3].value > 15)
          {
            print_error_range("Constant", -16, 15, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
           ((operands[3].value & 0xf) << 6) |
            (operands[2].value << 11) |
            (operands[1].value << 16) |
            (operands[0].value << 21);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FRT_OFFSET_RA:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (asm_context->pass == 1)
          {
            add_bin32(asm_context, table_powerpc[n].opcode, IS_OPCODE);
            return 4;
          }

          if (operands[0].type != OPERAND_FPU_REGISTER ||
              operands[1].type != OPERAND_REGISTER_OFFSET)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                   (operands[0].value << 21) |
                   (operands[1].value << 16) |
                   (operands[1].offset & 0xffff);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FRT_RA_RB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_FPU_REGISTER ||
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

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FRT_FRB:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_FPU_REGISTER ||
              operands[1].type != OPERAND_FPU_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 11);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FRT_FRA_FRB:
        case OP_FRT_FRA_FRC:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_FPU_REGISTER ||
              operands[1].type != OPERAND_FPU_REGISTER ||
              operands[2].type != OPERAND_FPU_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 16);

          if (table_powerpc[n].type == OP_FRT_FRA_FRB)
          {
            opcode |= operands[2].value << 11;
          }
          else
          {
            opcode |= operands[2].value << 6;
          }

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FRT_FRA_FRC_FRB:
        {
          if (operand_count != 4)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_FPU_REGISTER ||
              operands[1].type != OPERAND_FPU_REGISTER ||
              operands[2].type != OPERAND_FPU_REGISTER ||
              operands[3].type != OPERAND_FPU_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 21) |
                  (operands[1].value << 16) |
                  (operands[2].value << 6) |
                  (operands[3].value << 11);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BF_FRA_FRB:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if ((operands[0].type != OPERAND_BF &&
               operands[0].type != OPERAND_NUMBER) ||
               operands[1].type != OPERAND_FPU_REGISTER ||
               operands[2].type != OPERAND_FPU_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 23) |
                  (operands[1].value << 16) |
                  (operands[2].value << 11);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FRT:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_FPU_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[0].value << 21);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BF_BFA:
        case OP_BF_U:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if ((operands[0].type != OPERAND_BF &&
               operands[0].type != OPERAND_NUMBER) ||
               operands[1].type != OPERAND_NUMBER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 15)
          {
            print_error_range("Constant", 0, 15, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[0].value << 23);

          if (table_powerpc[n].type == OP_BF_BFA)
          {
            if (operands[1].value < 0 || operands[1].value > 7)
            {
              print_error_range("Constant", 0, 7, asm_context);
              return -1;
            }

            opcode |= operands[1].value << 18;
          }
          else
          {
            if (operands[1].value < 0 || operands[1].value > 15)
            {
              print_error_range("Constant", 0, 15, asm_context);
              return -1;
            }

            opcode |= operands[1].value << 12;
          }

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FLM_FRB:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_NUMBER ||
              operands[1].type != OPERAND_FPU_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 255)
          {
            print_error_range("Constant", 0, 255, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode |
                  (operands[0].value << 17) |
                  (operands[1].value << 11);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_BT:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].value < 0 || operands[0].value > 31)
          {
            print_error_range("Constant", 0, 31, asm_context);
            return -1;
          }

          opcode = table_powerpc[n].opcode | (operands[0].value << 21);

          if (modifiers.has_dot == 1)
          {
            opcode |= 1;
          }

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



