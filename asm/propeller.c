/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "asm/common.h"
#include "asm/propeller.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/propeller.h"

#define SET_EFFECTS() \
  if (wz == 1) { opcode |= 0x02000000; } \
  if (wc == 1) { opcode |= 0x01000000; } \
  if (wr == 1) { opcode |= 0x00800000; } \
  if (wr == 0) { opcode &= 0xff7fffff; }

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

struct _conditions
{
  const char *name;
  uint8_t value;
};

static struct _conditions conditions[] =
{
  { "if_always",   0xf },
  { "if_never",    0x0 },
  { "if_e",        0xa },
  { "if_ne",       0x5 },
  { "if_a",        0x1 },
  { "if_b",        0xc },
  { "if_ae",       0x3 },
  { "if_be",       0xe },
  { "if_c",        0xc },
  { "if_nc",       0x3 },
  { "if_z",        0xa },
  { "if_nz",       0x5 },
  { "if_c_eq_z",   0x9 },
  { "if_c_ne_z",   0x6 },
  { "if_c_and_z",  0x8 },
  { "if_c_and_nz", 0x4 },
  { "if_nc_and_z", 0x2 },
  { "if_nc_and_nz",0x1 },
  { "if_c_or_z",   0xe },
  { "if_c_or_nz",  0xd },
  { "if_nc_or_z",  0xb },
  { "if_nc_or_nz", 0x7 },
  { "if_z_eq_c",   0x9 },
  { "if_z_ne_c",   0x6 },
  { "if_z_and_c",  0x8 },
  { "if_z_and_nc", 0x2 },
  { "if_nz_and_c", 0x4 },
  { "if_nz_and_nc",0x1 },
  { "if_z_or_c",   0xe },
  { "if_z_or_nc",  0xb },
  { "if_nz_or_c",  0xd },
  { "if_nz_or_nc", 0x7 },
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
  int8_t cond = 0xf;
  int wr = -1, wc = -1, wz = -1;
  int has_effect = 0;

  lower_copy(instr_case, instr);

  memset(operands, 0, sizeof(operands));
  operand_count = 0;

  const int len = sizeof(conditions) / sizeof(struct _conditions);

  for (n = 0; n < len; n++)
  {
    if (strcmp(instr_case, conditions[n].name) == 0)
    {
      token_type = tokens_get(asm_context, instr, TOKENLEN);
      lower_copy(instr_case, instr);

      cond = conditions[n].value;
      break;
    }
  }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (strcmp(token, "wr") == 0)
    {
      if (wr != -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      has_effect = 1;
      wr = 1;
      continue;
    }
      else
    if (strcmp(token, "nr") == 0)
    {
      if (wr != -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      has_effect = 1;
      wr = 0;
      continue;
    }
      else
    if (strcmp(token, "wc") == 0)
    {
      if (wc != -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      has_effect = 1;
      wc = 1;
      continue;
    }
      else
    if (strcmp(token, "wz") == 0)
    {
      if (wz != -1)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      has_effect = 1;
      wz = 1;
      continue;
    }

    if (has_effect == 1 || operand_count >= 2)
    {
      print_error_unexp(token, asm_context);
      return -1;
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

      ignore_operand(asm_context);
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
      return -1;
    }
  }

  n = 0;

  while (table_propeller[n].instr != NULL)
  {
    if (strcmp(instr_case, table_propeller[n].instr) == 0)
    {
      for (i = 0; i < operand_count; i++)
      {
        if (operands[i].type == OPERAND_NUMBER)
        {
          if (check_range(asm_context, "Address", operands[i].value, 0, 511) == -1) { return -1; }
        }
          else
        if (operands[i].type == OPERAND_IMMEDIATE)
        {
          if (check_range(asm_context, "Immediate", operands[i].value, -256, 511) == -1) { return -1; }
        }
      }

      if (has_effect != 0)
      {
        if ((table_propeller[n].mask & 0x02000000) != 0 && wz != -1)
        {
          print_error("Error: Instruction doesn't take WZ effect\n", asm_context);
          return -1;
        }

        if ((table_propeller[n].mask & 0x01000000) != 0 && wc != -1)
        {
          print_error("Error: Instruction doesn't take WC effect\n", asm_context);
          return -1;
        }

        if ((table_propeller[n].mask & 0x00800000) != 0 && wr != -1)
        {
          print_error("Error: Instruction doesn't take WR effect\n", asm_context);
          return -1;
        }
      }

      switch (table_propeller[n].type)
      {
        case PROPELLER_OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode | (cond << 18);
          SET_EFFECTS();

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_NOP:
        {
          if (operand_count != 0 || cond != 0xf)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode;
          SET_EFFECTS();

          add_bin32(asm_context, opcode, IS_OPCODE);
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

#if 0
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
#endif

          opcode = table_propeller[n].opcode | (cond << 18);

          if (operands[1].type == OPERAND_IMMEDIATE)
          {
            opcode |= 0x00400000;
          }

          opcode |= (operands[0].value & 0x1ff) << 9;
          opcode |= operands[1].value & 0x1ff;
          SET_EFFECTS();

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_S:
        {
          if (operand_count != 1)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode | (cond << 18);

          if (operands[0].type == OPERAND_IMMEDIATE)
          {
            opcode |= 0x00400000;
          }

          opcode |= operands[0].value & 0x1ff;
          SET_EFFECTS();

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_D:
        {
          if (operand_count != 1 || operands[0].type == OPERAND_IMMEDIATE)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode | (cond << 18);
          opcode |= (operands[0].value & 0x1ff) << 9;
          SET_EFFECTS();

          add_bin32(asm_context, opcode, IS_OPCODE);
          return 4;
        }
        case PROPELLER_OP_IMMEDIATE:
        {
          if (operand_count != 1 || operands[0].type != OPERAND_IMMEDIATE)
          {
            print_error_illegal_operands(instr, asm_context);
            return -1;
          }

          opcode = table_propeller[n].opcode | (cond << 18);
          opcode |= operands[0].value & 0x1ff;
          SET_EFFECTS();

          add_bin32(asm_context, opcode, IS_OPCODE);
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

