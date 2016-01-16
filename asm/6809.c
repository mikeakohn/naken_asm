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
#include "asm/6809.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/6809.h"

enum
{
  OPERAND_NONE,
  OPERAND_NUMBER,
  OPERAND_ADDRESS,
  OPERAND_REG,
};

enum
{
  REG_D,
  REG_X,
  REG_Y,
  REG_U,
  REG_S,
  REG_PC,
  REG_A,
  REG_B,
  REG_CC,
  REG_DP,
};

struct _aliases
{
  const char *name;
  const char *value;
};

static struct _aliases aliases[] =
{
  { "asl", "lsl" },
  { "bcc", "bhs" },
  { "bcs", "blo" },
  { "asla", "lsla" },
  { "aslb", "lslb" },
  { "lbcc", "lbhs" },
  { "lbcs", "lblo" },
  { NULL, NULL }
};

static void check_alias(char *instr)
{
  int n = 0;

  while(1)
  {
    if (strcmp(instr, aliases[n].name) == 0)
    {
      strcpy(instr, aliases[n].value);
      break;
    }

    n++;
  }
}

int parse_instruction_6809(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int operand_type;
  int operand_value;
  int operand_count = 0;
  int address_size = 0;
  int opcode = -1;
  int matched = 0;
  int n;

  lower_copy(instr_case, instr);
  check_alias(instr_case);

  do
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

#if 0
    if (strcasecmp(token, "a") == 0)
    {
      strcat(instr_case, "a");
      token_type=tokens_get(asm_context, token, TOKENLEN);
    }
#endif

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      operand_type = OPERAND_NONE;
      break;
    }

    if (token_type == TOKEN_POUND)
    {
      operand_type = OPERAND_NUMBER;
      if (eval_expression(asm_context, &operand_value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        eat_operand(asm_context);
        operand_value = 0xffff;
      }
    }
      else
    {
      operand_type = OPERAND_ADDRESS;
      tokens_push(asm_context, token, token_type);
      if (eval_expression(asm_context, &operand_value) != 0)
      {
        if (asm_context->pass == 2)
        {
          print_error_illegal_expression(instr, asm_context);
          return -1;
        }

        eat_operand(asm_context);
        operand_value = 0xffff;
      }

      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }

      if (expect_token_s(asm_context, "x") != 0) { return -1; }
      operand_type = OPERAND_REG;
      operand_value = REG_X;
    }
  } while(0);

  n = 0;
  while(1)
  {
    if (m6809_table[n].instr == NULL) { break; }

    if (strcmp(instr_case, m6809_table[n].instr) == 0)
    {
      matched = 1;

      switch(m6809_table[n].operand_type)
      {
        case M6809_OP_INHERENT:
        {
          if (operand_count == 0)
          {
            add_bin8(asm_context, m6809_table[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case M6809_OP_IMMEDIATE:
        {
          break;
        }
        case M6809_OP_EXTENDED:
        {
          break;
        }
        case M6809_OP_RELATIVE:
        {
          break;
        }
        case M6809_OP_DIRECT:
        {
          break;
        }
        case M6809_OP_INDEXED:
        {
          break;
        }
        case M6809_OP_VARIANT:
        {
          break;
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

  n = 0;
  while(1)
  {
    if (m6809_table_16[n].instr == NULL) { break; }

    if (strcmp(instr_case, m6809_table[n].instr) == 0)
    {
      matched = 1;

      switch(m6809_table[n].operand_type)
      {
        case M6809_OP_INHERENT:
        {
          if (operand_count == 0)
          {
            add_bin16(asm_context, m6809_table[n].opcode, IS_OPCODE);
            return 1;
          }

          break;
        }
        case M6809_OP_IMMEDIATE:
        {
          break;
        }
        case M6809_OP_EXTENDED:
        {
          break;
        }
        case M6809_OP_RELATIVE:
        {
          break;
        }
        case M6809_OP_DIRECT:
        {
          break;
        }
        case M6809_OP_INDEXED:
        {
          break;
        }
        case M6809_OP_VARIANT:
        {
          break;
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

#if 0
  if (opcode != -1)
  {
    add_bin8(asm_context, address_size, IS_OPCODE);
    add_bin8(asm_context, address_size, IS_OPCODE);
    if (address_size == 2)
    {
      if (operand_value < 0 || operand_value > 0xffff)
      {
        print_error_range("Address", 0, 0xffff, asm_context);
        return -1;
      }
      add_bin8(asm_context, address_size, IS_OPCODE);
      return 3;
    }
      else
    {
      return 2;
    }
  }
#endif

  if (matched == 1)
  {
    printf("Error: Unknown operands combo for '%s' at %s:%d.\n", instr, asm_context->filename, asm_context->line);
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
  }

  return -1;
}



