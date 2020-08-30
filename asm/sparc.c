/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <ctype.h>

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

  while(*token != 0)
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
  struct _operand operands[3];
  int operand_count = 0;
  int token_type;
  int matched = 0;
  uint32_t opcode;
  int num, n;

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

    n = get_register_sparc(token);

    if (n != -1)
    {
      operands[operand_count].type = OPERAND_REGISTER;
      operands[operand_count].value = n;
    }
      else
    {
      operands[operand_count].type = OPERAND_NUMBER;

      if (asm_context->pass == 1)
      {
        eat_operand(asm_context);
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

  while (table_sparc[n].instr != NULL)
  {
    if (strcmp(table_sparc[n].instr, instr_case) == 0)
    {
      matched = 1;

      switch (table_sparc[n].type)
      {
        case OP_NONE:
        {
          if (operand_count == 0)
          {
            add_bin32(asm_context, table_sparc[n].opcode, IS_OPCODE);

            return 1;
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

            return 2;
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

