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

#include "asm/4004.h"
#include "asm/common.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/4004.h"

#define MAX_OPERANDS 2

enum
{
  OPERAND_NUMBER,
  OPERAND_R,
  OPERAND_P,
};

struct _operand
{
  int value;
  int type;
};

int parse_instruction_4004(struct _asm_context *asm_context, char *instr)
{
  char instr_case_mem[TOKENLEN];
  char *instr_case = instr_case_mem;
  char token[TOKENLEN];
  struct _operand operands[MAX_OPERANDS];
  int operand_count = 0;
  int token_type;
  int num, n, r;

  lower_copy(instr_case, instr);
  memset(&operands, 0, sizeof(operands));

/*
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

    {
      tokens_push(asm_context, token, token_type);

      if (eval_expression(asm_context, &num) != 0)
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      operands[operand_count].value = num;
      operands[operand_count].type = OPERAND_NUMBER;
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
*/

  n = 0;
  while (table_4004[n].instr != NULL)
  {
    if (strcmp(table_4004[n].instr, instr_case) == 0)
    {
      switch (table_4004[n].type)
      {
        case OP_NONE:
        {
          if (operand_count != 0)
          {
            print_error_opcount(instr, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_4004[n].opcode, IS_OPCODE);

          return 1;
        }
        case OP_R:
        case OP_P:
        {
          token_type = tokens_get(asm_context, token, TOKENLEN);
          if (token_type != TOKEN_NUMBER)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          r = atoi(token);
          if (r < 0 || r > 15 || (table_4004[n].type == OP_P && ((r & 1) == 1)))
          {
            print_error_illegal_register(instr, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_4004[n].opcode | r, IS_OPCODE);

          return 1;
        }
        case OP_ADDR12:
        {
          if (asm_context->pass == 1)
          {
            ignore_line(asm_context);
            add_bin8(asm_context, 0, IS_OPCODE);
            add_bin8(asm_context, 0, IS_OPCODE);
            return 2;
          }

          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          if (num < 0 || num > 0xfff)
          {
            print_error_range("Address", 0, 0xfff, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_4004[n].opcode | (num >> 8), IS_OPCODE);
          add_bin8(asm_context, num & 0xff, IS_OPCODE);

          return 2;
        }
        case OP_DATA:
        {
          if (asm_context->pass == 1)
          {
            ignore_line(asm_context);
            add_bin8(asm_context, 0, IS_OPCODE);
            return 1;
          }

          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          if (num < -8 || num > 0xf)
          {
            print_error_range("Literal", -8, 0xf, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_4004[n].opcode | (num & 0xf), IS_OPCODE);

          return 1;
        }
        case OP_P_DATA:
        case OP_R_ADDR8:
        {
          if (asm_context->pass == 1)
          {
            ignore_line(asm_context);
            add_bin8(asm_context, 0, IS_OPCODE);
            add_bin8(asm_context, 0, IS_OPCODE);
            return 2;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (token_type != TOKEN_NUMBER)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          r = atoi(token);

          if (r < 0 || r > 15 ||
             (table_4004[n].type == OP_P_DATA && ((r & 1) == 1)))
          {
            print_error_illegal_register(instr, asm_context);
            return -1;
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_NOT_TOKEN(token, ','))
          {
            tokens_push(asm_context, token, token_type);
          }

          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          int lo = table_4004[n].type == OP_P_DATA ? -128 : 0;

          if (num < lo || num > 0xff)
          {
            print_error_range(lo == 0 ? "Address" : "Literal", lo, 0xff, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_4004[n].opcode | r, IS_OPCODE);
          add_bin8(asm_context, num & 0xff, IS_OPCODE);

          return 2;
        }
        case OP_COND:
        case OP_COND_ALIAS:
        {
          if (asm_context->pass == 1)
          {
            ignore_line(asm_context);
            add_bin8(asm_context, 0, IS_OPCODE);
            add_bin8(asm_context, 0, IS_OPCODE);
            return 2;
          }

          r = 0;

          if (table_4004[n].type == OP_COND)
          {
            token_type = tokens_get(asm_context, token, TOKENLEN);

            if (token_type == TOKEN_NUMBER)
            {
              r = atoi(token);
            }
              else
            {
              if (strcasecmp(token, "Z") == 0) { r = 0x4; }
              else if (strcasecmp(token, "NZ") == 0) { r = 0xc; }
              else if (strcasecmp(token, "C") == 0) { r = 0x2; }
              else if (strcasecmp(token, "NC") == 0) { r = 0xa; }
              else if (strcasecmp(token, "TN") == 0) { r = 0x1; }
              else if (strcasecmp(token, "T") == 0) { r = 0x9; }
              else
              {
                print_error_unexp(token, asm_context);
                return -1;
              }
            }

            if (r < 0 || r > 15)
            {
              print_error_range("Condition", 0, 0xfff, asm_context);
              return -1;
            }
          }

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (IS_NOT_TOKEN(token, ','))
          {
            tokens_push(asm_context, token, token_type);
          }

          if (eval_expression(asm_context, &num) != 0)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          if (num < 0 || num > 0xff)
          {
            print_error_range("Address", 0, 0xff, asm_context);
            return -1;
          }

          add_bin8(asm_context, table_4004[n].opcode | r, IS_OPCODE);
          add_bin8(asm_context, num & 0xff, IS_OPCODE);

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

