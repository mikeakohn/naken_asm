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

#include "asm/86000.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/86000.h"

enum
{
  OPERAND_INVALID,
  OPERAND_AT_REG,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
};

struct _operand
{
  int value;
  int type;
};

static int get_register_86000(char *token)
{
  if (token[0] != 'r' && token[0] != 'R') { return -1; }
  if (token[1] >= '0' && token[1] <= '3' && token[2] == 0)
  {
    return token[1] - '0';
  }

  return -1;
}

int parse_instruction_86000(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  char token[TOKENLEN];
  struct _operand operands[3];
  int operand_count = 0;
  int token_type;
  int matched = 0;
  int opcode, num, offset, bit, n;

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

    if (IS_TOKEN(token,'@'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      num = get_register_86000(token);

      if (num != -1)
      {
        operands[operand_count].type = OPERAND_AT_REG;
        operands[operand_count].value = num;
      }
        else
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }
      else
    if (token_type == TOKEN_POUND)
    {
      if (eval_expression(asm_context, &num) != 0)
      {
        if (asm_context->pass == 1)
        {
          ignore_operand(asm_context);
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
    {
      operands[operand_count].type = OPERAND_ADDRESS;

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
printf("-----\n");
for (n = 0; n < operand_count; n++)
{
printf("[%d %d]", operands[n].type, operands[n].value);
}
printf("\n");
#endif

  n = 0;

  while (table_86000[n].name != NULL)
  {
    if (strcmp(table_86000[n].name, instr_case) == 0)
    {
      matched = 1;

      switch (table_86000[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin8(asm_context, table_86000[n].opcode, IS_OPCODE);

            return 1;
          }

          break;
        }
        case OP_ADDRESS:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0x1ff) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | (num >> 8);

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case OP_IMMEDIATE:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_NUMBER)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Immediate", num, -128, 0xff) != 0)
            {
              return -1;
            }

            add_bin8(asm_context, table_86000[n].opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case OP_AT_REG:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_AT_REG)
          {
            opcode = table_86000[n].opcode | operands[0].value;

            add_bin8(asm_context, opcode, IS_OPCODE);

            return 1;
          }

          break;
        }
        case OP_ADDRESS_RELATIVE8:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0x1ff) != 0)
            {
              return -1;
            }

            offset = operands[1].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -128, 127) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | (num >> 8);

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_IMMEDIATE_RELATIVE8:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Immediate", num, -128, 0xff) != 0)
            {
              return -1;
            }

            offset = operands[1].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -128, 127) != 0)
            {
              return -1;
            }

            add_bin8(asm_context, table_86000[n].opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_AT_REG_IMMEDIATE_RELATIVE8:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_AT_REG &&
              operands[1].type == OPERAND_NUMBER &&
              operands[2].type == OPERAND_ADDRESS)
          {
            num = operands[1].value;

            if (check_range(asm_context, "Immediate", num, -128, 0xff) != 0)
            {
              return -1;
            }

            offset = operands[2].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -128, 127) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | operands[0].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_ADDRESS_BIT_RELATIVE8:
        {
          if (operand_count == 3 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_ADDRESS &&
              operands[2].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0x1ff) != 0)
            {
              return -1;
            }

            bit = operands[1].value;

            if (check_range(asm_context, "Bit", bit, 0, 7) != 0)
            {
              return -1;
            }

            offset = operands[2].value - (asm_context->address + 3);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -128, 127) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | ((num & 0x100) << 4) | bit;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_RELATIVE8:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            offset = operands[0].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -128, 127) != 0)
            {
              return -1;
            }

            add_bin8(asm_context, table_86000[n].opcode, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case OP_RELATIVE16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            // FIXME: Look into this.
            //offset = operands[0].value - (asm_context->address + 3);
            offset = operands[0].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -32768, 32767) != 0)
            {
              return -1;
            }

            add_bin8(asm_context, table_86000[n].opcode, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);
            add_bin8(asm_context, (offset >> 8) & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_ADDRESS12:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0xfff) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode |
                     ((num & 0x800) >> 7) |
                     ((num & 0x700) >> 8);

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case OP_ADDRESS16:
        {
          if (operand_count == 1 && operands[0].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0xffff) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_ADDRESS_BIT:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_ADDRESS &&
              operands[1].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Address", num, 0, 0xfff) != 0)
            {
              return -1;
            }

            bit = operands[1].value;

            if (check_range(asm_context, "Bit", bit, 0, 7) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | ((num & 0x100) >> 4) | bit;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case OP_AT_REG_RELATIVE8:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_AT_REG &&
              operands[1].type == OPERAND_ADDRESS)
          {
            offset = operands[1].value - (asm_context->address + 2);

            if (asm_context->pass == 1) { offset = 0; }

            if (check_range(asm_context, "Offset", offset, -128, 127) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | operands[0].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, offset & 0xff, IS_OPCODE);

            return 2;
          }

          break;
        }
        case OP_IMMEDIATE_ADDRESS:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_ADDRESS)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Immediate", num, -128, 0xff) != 0)
            {
              return -1;
            }

            num = operands[1].value;

            if (check_range(asm_context, "Address", num, 0, 0x1ff) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | ((num & 0x1ff) >> 8);

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);
            add_bin8(asm_context, operands[0].value & 0xff, IS_OPCODE);

            return 3;
          }

          break;
        }
        case OP_IMMEDIATE_AT_REG:
        {
          if (operand_count == 2 &&
              operands[0].type == OPERAND_NUMBER &&
              operands[1].type == OPERAND_AT_REG)
          {
            num = operands[0].value;

            if (check_range(asm_context, "Immediate", num, -128, 0xff) != 0)
            {
              return -1;
            }

            opcode = table_86000[n].opcode | operands[1].value;

            add_bin8(asm_context, opcode, IS_OPCODE);
            add_bin8(asm_context, num & 0xff, IS_OPCODE);

            return 2;
          }

          break;
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

