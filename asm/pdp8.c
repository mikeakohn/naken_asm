/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn, Lars Brinkhoff
 *
 * PDP-8 by Lars Brinkhoff
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

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
  uint8_t flags;
};

static const char *group_1[] =
{
  "iac",
  "bsw",
  "ral",
  "rar",
  "cml",
  "cma",
  "cll",
  "cla",
};

static const char *group_2[] =
{
  NULL,
  "hlt",
  "osr",
  NULL,
  "snl",
  "sza",
  "sma",
  "cla",
};

static const char *group_3[] =
{
  NULL,
  "hlt",
  "osr",
  NULL,
  "szl",
  "sna",
  "spa",
  "cla",
};

static int get_group(struct _asm_context *asm_context, char *token)
{
  int token_type;
  int opcode = 0;
  int n = 0;
  int group = 0;

  while (1)
  {
    if (strcasecmp(token, "cla") == 0)
    {
      opcode |= 0x0080;
    }
      else
    {
      if (group == 0 || group == 1)
      {
        n = -1;

        if (strcasecmp(token, "rtr") == 0)
        {
          opcode |= 07012;
          group = 1;
        }
          else
        if (strcasecmp(token, "rtl") == 0)
        {
          opcode |= 07006;
          group = 1;
        }
          else
        {
          for (n = 0; n < 7; n++)
          {
            if (strcmp(token, group_1[n]) == 0)
            {
              group = 1;
              opcode |= (1 << n);
              break;
            }
          }
        }
      }

      if (group == 0 || group == 2)
      {
        for (n = 0; n < 7; n++)
        {
          if (group_2[n] == 0) { continue; }
          if (strcmp(token, group_2[n]) == 0)
          {
            group = 2;
            opcode |= (1 << n);
            break;
          }
        }
      }

      if (group == 0 || group == 3)
      {
        n = -1;

        if (strcasecmp(token, "skp") == 0)
        {
          opcode |= 07410;
          group = 3;
        }
          else
        {
          for (n = 0; n < 7; n++)
          {
            if (group_3[n] == 0) { continue; }
            if (strcmp(token, group_3[n]) == 0)
            {
              group = 3;
              opcode |= (1 << n);
              break;
            }
          }
        }
      }
    }

    if (n == 7)
    {
      if (opcode == 0) { return -1; }

      print_error_unexp(token, asm_context);
      return -2;
    }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      switch (group)
      {
        case 0:
        case 1: opcode |= 0x0e00; break;
        case 2: opcode |= 0x0f00; break;
        case 3: opcode |= 0x0f08; break;
      }

      add_bin16(asm_context, opcode, IS_OPCODE);
      return 2;
    }
  }
}

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

  strcpy(token, instr);

  n = get_group(asm_context, token);

  if (n == -2) { return -1; }
  if (n != -1) { return n; }

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (1)
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

    if (IS_TOKEN(token, 'i') || IS_TOKEN(token, 'I'))
    {
      operands[operand_count].flags |= 0x100;
      continue;
    }

    if (IS_TOKEN(token, 'z') || IS_TOKEN(token, 'Z'))
    {
      operands[operand_count].flags |= 0x080;
      continue;
    }

    tokens_push(asm_context, token, token_type);

    if (eval_expression(asm_context, &num) != 0)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    operands[operand_count].value = num;
    operands[operand_count].type = OPERAND_NUMBER;

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

  while (table_pdp8[n].instr != NULL)
  {
    if (strcmp(table_pdp8[n].instr, instr_case) == 0)
    {
      switch (table_pdp8[n].type)
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

          // REVIEW: Isn't this an address that should be 0 to 127?
          if (operands[0].value < -64 || operands[0].value > 127)
          {
            print_error_range("Offset", 0, 127, asm_context);
            return -1;
          }

          opcode = table_pdp8[n].opcode |
                   operands[0].flags |
                  (operands[0].value & 0x7f);

          add_bin16(asm_context, opcode, IS_OPCODE);

          return 2;
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

