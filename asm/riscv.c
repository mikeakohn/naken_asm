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
#include "asm/riscv.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/riscv.h"

#define MAX_OPERANDS 5

enum
{
  OPERAND_NONE,
  OPERAND_X_REGISTER,
  OPERAND_F_REGISTER,
  OPERAND_NUMBER,
  OPERAND_RM,
  OPERAND_REGISTER_OFFSET,
};

#define RM_RNE 0
#define RM_RTZ 1
#define RM_RDN 2
#define RM_RUP 3
#define RM_RMM 4

struct _operand
{
  int value;
  int type;
  int16_t offset;
};

struct _modifiers
{
  int aq;
  int rl;
  int rm;
  int fence;
};

static const char *rm_string[] =
{
  "rne",
  "rtz",
  "rdn",
  "rup",
  "rmm",
};

static const char *fence_string[] =
{
  "sw",
  "sr",
  "so",
  "si",
  "pw",
  "pr",
  "po",
  "pi",
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

static int get_x_register_riscv(char *token)
{
  if (token[0] != 'x' && token[0] != 'X')
  {
    if (strcasecmp(token, "zero") == 0) { return 0; }
    if (strcasecmp(token, "ra") == 0) { return 1; }
    if (strcasecmp(token, "sp") == 0) { return 2; }
    if (strcasecmp(token, "gp") == 0) { return 3; }
    if (strcasecmp(token, "tp") == 0) { return 4; }
    if (strcasecmp(token, "fp") == 0) { return 8; }

    if (token[2] == 0)
    {
      if (token[0] == 't')
      {
        if (token[1] >= '0' && token[1] <= '2')
        {
          return (token[1] - '0') + 5;
        }
          else
        if (token[1] >= '3' && token[1] <= '6')
        {
          return (token[1] - '0') - 3 + 28;
        }

      }
      else if (token[0] == 'a' && token[1] >= '0' && token[1] <= '7')
      {
        return (token[1] - '0') + 10;
      }
      else if (token[0] == 's' && token[1] >= '0' && token[1] <= '1')
      {
        return (token[1] - '0') + 8;
      }
#if 0
      else if (token[0] == 'v' && token[1] >= '0' && token[1] <= '1')
      {
        return (token[1] - '0') + 16;
      }
#endif
    }

    if (token[0] == 's')
    {
      int n = get_register_number(token + 1);
      if (n >= 2 && n <= 11) { return n - 2 + 18; }
    }

    return -1;
  }

  return get_register_number(token + 1);
}

static int get_f_register_riscv(char *token)
{
  int n;

  if (token[0] != 'f') { return -1; }

  if (token[1] == 's')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 1) { return n + 8; }
    if (n >= 2 && n <= 11) { return n - 2 + 18; }
    return -1;
  }

#if 0
  if (token[1] == 'v')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 1) { return n + 16; }
    return -1;
  }
#endif

  if (token[1] == 'a')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 7) { return n + 10; }
    return -1;
  }

  if (token[1] == 't')
  {
    n = get_register_number(token + 2);
    if (n >= 0 && n <= 7) { return n + 0; }
    if (n >= 8 && n <= 11) { return n - 8 + 28; }
    return -1;
  }

  return get_register_number(token + 1);
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
      token_type = tokens_get(asm_context, token, TOKENLEN);
      n = 0;
      while (token[n] != 0) { token[n] = tolower(token[n]); n++; }

      if (strcasecmp(token, "aq") == 0) { modifiers->aq = 1; continue; }
      if (strcasecmp(token, "rl") == 0) { modifiers->rl = 1; continue; }

      strcat(instr_case, ".");
      strcat(instr_case, token);
      continue;
    }

    do
    {
      // Check for registers
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

      // Check for FPU rounding mode
      for (n = 0; n < 5; n++)
      {
        if (strcasecmp(token, rm_string[n]) == 0)
        {
          operands[operand_count].type = OPERAND_RM;
          modifiers->rm = n;
          break;
        }
      }

      if (n != 5) { break; }

      // Check for fence's operands
      for (n = 0; n < 8; n++)
      {
        if (strcasecmp(token, fence_string[n]) == 0)
        {
          modifiers->fence |= 1 << n;
          operand_count--;  // sorry :(
          break;
        }
      }

      if (n != 8) { break; }

      // Check if this is (reg)
      if (IS_TOKEN(token, '('))
      {
        char token[TOKENLEN];
        int token_type;

        token_type = tokens_get(asm_context, token, TOKENLEN);

        n = get_x_register_riscv(token);

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

          n = get_x_register_riscv(token);
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

int parse_instruction_riscv(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count;
  int matched = 0;
  uint32_t opcode;
  struct _modifiers modifiers;
  int n;

  memset(&modifiers, 0, sizeof(modifiers));
  modifiers.rm = -1;

  lower_copy(instr_case, instr);

  memset(&operands, 0, sizeof(operands));

  operand_count = get_operands(asm_context, operands, instr, instr_case, &modifiers);

  if (operand_count < 0) { return -1; }

  n = 0;
  while (table_riscv[n].instr != NULL)
  {
    if (strcmp(table_riscv[n].instr, instr_case) == 0)
    {
      matched = 1;

      // If aq or rl was set, make sure it was only done on the
      // right instructions.
      if (modifiers.aq != 0 || modifiers.rl != 0)
      {
        if (table_riscv[n].type != OP_LR && table_riscv[n].type != OP_STD_EXT)
        {
          n++;
          continue;
        }
      }

      if (modifiers.rm != -1)
      {
        if (operands[operand_count - 1].type != OPERAND_RM ||
            (table_riscv[n].type != OP_FP_FP_RM &&
             table_riscv[n].type != OP_R_FP_RM &&
             table_riscv[n].type != OP_FP_R_RM &&
             table_riscv[n].type != OP_FP_FP_FP_RM &&
             table_riscv[n].type != OP_FP_FP_FP_FP_RM))
        {
          n++;
          continue;
        }

        operand_count--;
      }

      if (modifiers.rm == -1) { modifiers.rm = 7; }

      // If the fence operands were used, make sure they only happened
      // with the fence instruction.

      if (modifiers.fence != 0 && table_riscv[n].type != OP_FENCE)
      {
        n++;
        continue;
      }

      switch (table_riscv[n].type)
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
        case OP_R_R:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
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

          if (operands[2].value < -2048 || operands[2].value > 0xfff)
          {
            print_error_range("Immediate", -2048, 0xfff, asm_context);
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
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            //offset = (uint32_t)operands[2].value - (asm_context->address + 4);
            offset = (uint32_t)operands[2].value - (asm_context->address);

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            if (offset < -4096 || offset >= 4095)
            {
              print_error_range("Offset", -4096, 4095, asm_context);
              return -1;
            }
          }

          immediate = ((offset >> 12) & 0x1) << 31;
          immediate |= ((offset >> 11) & 0x1) << 7;
          immediate |= ((offset >> 5) & 0x3f) << 25;
          immediate |= ((offset >> 1) & 0xf) << 8;

          opcode = table_riscv[n].opcode |
                  (immediate |
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

          int32_t offset = 0;
          uint32_t immediate;

          if (asm_context->pass == 2)
          {
            offset = (uint32_t)operands[1].value - (asm_context->address);
            //offset = (uint32_t)operands[1].value - (asm_context->address + 4);
            //address = operands[1].value;

            if ((offset & 0x1) != 0)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            const int low = -(1 << 20);
            const int high = (1 << 20) - 1;

            if (offset < low || offset > high)
            {
              print_error_range("Offset", low, high, asm_context);
              return -1;
            }
          }

          offset &= 0x1fffff;

          immediate = ((offset >> 20) & 0x1) << 31;
          immediate |= ((offset >> 12) & 0xff) << 12;
          immediate |= ((offset >> 11) & 0x1) << 20;
          immediate |= ((offset >> 1) & 0x3ff) << 21;

          opcode = table_riscv[n].opcode | immediate | (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_SHIFT:
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

          if (operands[2].value < 0 || operands[2].value > 31)
          {
            print_error_range("Immediate", 0, 31, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FENCE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }
          opcode = table_riscv[n].opcode | (modifiers.fence << 20);
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case OP_FFFF:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }
          opcode = table_riscv[n].opcode;
          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case OP_READ:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[0].value << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RD_INDEX_R:
        case OP_FD_INDEX_R:
        {
          int offset;
          int rd, rs1;

          if ((table_riscv[n].type == OP_RD_INDEX_R &&
               operands[0].type != OPERAND_X_REGISTER) ||
              (table_riscv[n].type == OP_FD_INDEX_R &&
               operands[0].type != OPERAND_F_REGISTER))
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operand_count == 2)
          {
            if (asm_context->pass == 1)
            {
              add_bin32(asm_context, table_riscv[n].opcode, IS_OPCODE);
              return 4;
            }

            if (operands[1].type != OPERAND_REGISTER_OFFSET)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            offset = operands[1].offset;
          }
            else
          if (operand_count == 3)
          {
            if (operands[1].type != OPERAND_X_REGISTER ||
                operands[2].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            offset = operands[2].value;
          }
            else
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          rd = operands[0].value;
          rs1 = operands[1].value;

          if (offset < -2048 || offset > 2047)
          {
            print_error_range("Offset", -2048, 2047, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  ((offset & 0xfff) << 20) |
                  (rs1 << 15) |
                  (rd << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_RS_INDEX_R:
        case OP_FS_INDEX_R:
        {
          int offset;
          int rs1, rs2;

          if ((table_riscv[n].type == OP_RS_INDEX_R &&
               operands[0].type != OPERAND_X_REGISTER) ||
              (table_riscv[n].type == OP_FS_INDEX_R &&
               operands[0].type != OPERAND_F_REGISTER))
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          if (operand_count == 2)
          {
            if (asm_context->pass == 1)
            {
              add_bin32(asm_context, table_riscv[n].opcode, IS_OPCODE);
              return 4;
            }

            if (operands[1].type != OPERAND_REGISTER_OFFSET)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            offset = operands[1].offset;
          }
            else
          if (operand_count == 3)
          {
            if (operands[1].type != OPERAND_X_REGISTER ||
                operands[2].type != OPERAND_NUMBER)
            {
              print_error_illegal_operands(instr, asm_context);
              return -1;
            }

            offset = operands[2].value;
          }
            else
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          rs2 = operands[0].value;
          rs1 = operands[1].value;

          if (offset < -2048 || offset > 2047)
          {
            print_error_range("Offset", -2048, 2047, asm_context);
            return -1;
          }

          offset = offset & 0xfff;

          opcode = table_riscv[n].opcode |
                (((offset >> 5) & 0x7f) << 25) |
                  (rs2 << 20) |
                  (rs1 << 15) |
                 ((offset & 0x1f) << 7);
          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_LR:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_REGISTER_OFFSET ||
              operands[1].offset != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (modifiers.aq == 1) { opcode |= (1 << 26); }
          if (modifiers.rl == 1) { opcode |= (1 << 25); }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_STD_EXT:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[2].type == OPERAND_X_REGISTER)
          {
            operands[2].type = OPERAND_REGISTER_OFFSET;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER ||
              operands[2].type != OPERAND_REGISTER_OFFSET ||
              operands[2].offset != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          // FIXME - The docs say rs2 and rs1 are reversed. gnu-as is like this.
          opcode = table_riscv[n].opcode |
                  (operands[1].value << 20) |
                  (operands[2].value << 15) |
                  (operands[0].value << 7);

          if (modifiers.aq == 1) { opcode |= (1 << 26); }
          if (modifiers.rl == 1) { opcode |= (1 << 25); }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_R_FP:
        case OP_R_FP_RM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_R_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_R_FP_FP:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_X_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER ||
              operands[2].type != OPERAND_F_REGISTER)
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
        case OP_FP:
        {
          if (operand_count != 1)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[0].value << 7);

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_FP:
        case OP_FP_FP_RM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_R:
        case OP_FP_R_RM:
        {
          if (operand_count != 2)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_X_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_R_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_FP_FP:
        case OP_FP_FP_FP_RM:
        {
          if (operand_count != 3)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER ||
              operands[2].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_FP_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
          }

          add_bin32(asm_context, opcode, IS_OPCODE);

          return 4;
        }
        case OP_FP_FP_FP_FP_RM:
        {
          if (operand_count != 4)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          if (operands[0].type != OPERAND_F_REGISTER ||
              operands[1].type != OPERAND_F_REGISTER ||
              operands[2].type != OPERAND_F_REGISTER ||
              operands[3].type != OPERAND_F_REGISTER)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_riscv[n].opcode |
                  (operands[3].value << 27) |
                  (operands[2].value << 20) |
                  (operands[1].value << 15) |
                  (operands[0].value << 7);

          if (table_riscv[n].type == OP_FP_FP_FP_FP_RM)
          {
            opcode |= (modifiers.rm << 12);
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

