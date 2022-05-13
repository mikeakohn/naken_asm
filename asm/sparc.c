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
#include <stdint.h>

#include "asm/sparc.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/sparc.h"

enum
{
  OPERAND_INVALID,
  OPERAND_REGISTER,
  OPERAND_NUMBER,
  OPERAND_CC,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_sparc(char *token)
{
  int num = 0;

  if (token[0] != 'r' && token[0] != 'R') { return -1; }

  token++;

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }

    num = (num * 10) + (*token - '0');
    token++;

    if (num > 31) { return -1; }
  }

  return num;
}

int parse_instruction_sparc(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  char token[TOKENLEN];
  struct _operand operands[4];
  int operand_count = 0;
  int token_type;
  int matched = 0;
  uint32_t opcode;
  int32_t offset;
  int num, n;
  int annul = 0;
  int pt = -1;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (operand_count == 0 && IS_TOKEN(token, ','))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);

      if (strcasecmp(token, "a") == 0)
      {
        annul = 1;
      }
        else
      if (strcasecmp(token, "pn") == 0)
      {
        pt = 0;
      }
        else
      if (strcasecmp(token, "pt") == 0)
      {
        pt = 1;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      continue;
    }

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      if (operand_count != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
      break;
    }

    n = get_register_sparc(token);

    if (n != -1)
    {
      operands[operand_count].type = OPERAND_REGISTER;
      operands[operand_count].value = n;
    }
      else
    if (strcasecmp(token, "icc") == 0)
    {
      operands[operand_count].type = OPERAND_CC;
      operands[operand_count].value = 0;
    }
      else
    if (strcasecmp(token, "xcc") == 0)
    {
      operands[operand_count].type = OPERAND_CC;
      operands[operand_count].value = 2;
    }
      else
    {
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
          return -1;
        }

        operands[operand_count].value = n;
      }
    }

    operand_count++;
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL) { break; }
    if (IS_NOT_TOKEN(token, ',') || operand_count == 3)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }
  }

#ifdef DEBUG
printf("----- %d -----\n", operand_count);
for (n = 0; n < operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  n = 0;

  while (table_sparc[n].instr != NULL)
  {
    if (strcmp(table_sparc[n].instr, instr_case) == 0)
    {
      matched = 1;

      if (annul == 1 || pt != -1)
      {
        switch (table_sparc[n].type)
        {
          case OP_BRANCH:
          case OP_BRANCH_P:
          case OP_BRANCH_P_REG:
            break;
          default:
            print_error_unexp(",a/pt", asm_context);
            return -1;
        }
      }

      switch (table_sparc[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin32(asm_context, table_sparc[n].opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_REG_REG_REG:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_REGISTER &&
              operands[2].type == OPERAND_REGISTER)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0x1ff) != 0)
            {
              return -1;
            }

            opcode = table_sparc[n].opcode |
                    (operands[0].value << 25) |
                    (operands[2].value << 14) |
                     operands[1].value;

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_REG_SIMM13_REG:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_NUMBER &&
              operands[2].type == OPERAND_REGISTER)
          {
            num = operands[1].value;

            if (check_range(asm_context, "Immediate", num, -4096, 4095) != 0)
            {
              return -1;
            }

            opcode = table_sparc[n].opcode |
                    (operands[0].value << 25) |
                    (operands[2].value << 14) |
                    (operands[1].value & 0x1fff);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_FREG_FREG_FREG_FREG:
        {
          break;
        }
        case OP_FREG_FREG_IMM5_FREG:
        {
          break;
        }
        case OP_FREG_FREG_FREG:
        {
          break;
        }
        case OP_BRANCH:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            if (pt != -1)
            {
              print_error_unexp(token, asm_context);
              return -1;
            }

            offset = operands[0].value - asm_context->address;

            if (asm_context->pass == 1) { offset = 0; }

            if ((offset & 0x3) != 0)
            {
              print_error_align(asm_context, 2);
              return -1;
            }

            const int min = -(1 << 23);
            const int max = (1 << 23) - 1;
            if (pt == -1) { pt = 1; }

            if (offset < min || offset > max)
            {
              print_error_range("Displacement", min, max, asm_context);
            }

            offset = offset >> 2;

            opcode = table_sparc[n].opcode |
                    (annul << 29) |
                    (offset & 0x003fffff);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_BRANCH_P:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_CC &&
              operands[1].type == OPERAND_NUMBER)
          {
            offset = operands[1].value - asm_context->address;

            if (asm_context->pass == 1) { offset = 0; }

            if ((offset & 0x3) != 0)
            {
              print_error_align(asm_context, 2);
              return -1;
            }

            const int min = -(1 << 20);
            const int max = (1 << 20) - 1;
            if (pt == -1) { pt = 1; }

            if (offset < min || offset > max)
            {
              print_error_range("Displacement", min, max, asm_context);
            }

            offset = offset >> 2;

            opcode = table_sparc[n].opcode |
                    (annul << 29) |
                    (operands[0].value << 20) |
                    (pt << 19) |
                    (offset & 0x0007ffff);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_BRANCH_P_REG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_REGISTER &&
              operands[1].type == OPERAND_NUMBER)
          {
            offset = operands[1].value - asm_context->address;

            if (asm_context->pass == 1) { offset = 0; }

            if ((offset & 0x3) != 0)
            {
              print_error_align(asm_context, 2);
              return -1;
            }

            const int min = -(1 << 15);
            const int max = (1 << 15) - 1;
            if (pt == -1) { pt = 1; }

            if (offset < min || offset > max)
            {
              print_error_range("Displacement", min, max, asm_context);
            }

            offset = offset >> 2;

            opcode = table_sparc[n].opcode |
                    (annul << 29) |
                    (operands[0].value << 14) |
                    (pt << 19) |
                    (((offset >> 14) & 0x3) << 20) |
                    (offset & 0x00003fff);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        case OP_CALL:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            offset = operands[0].value - asm_context->address;

            if (asm_context->pass == 1) { offset = 0; }

            if ((offset & 0x3) != 0)
            {
              print_error_align(asm_context, 2);
              return -1;
            }

#if 0
            const int min = -(1 << 31);
            const int max = (1 << 31) - 1;

            if (offset < min || offset > max)
            {
              print_error_range("Displacement", min, max, asm_context);
            }
#endif

            offset = offset >> 2;

            opcode = table_sparc[n].opcode | (offset & 0x3fffffff);

            add_bin32(asm_context, opcode, IS_OPCODE);

            return 4;
          }

          break;
        }
        default:
        {
          break;
        }
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

