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
#include "asm/webasm.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression_ex.h"
#include "table/webasm.h"

static int get_uint(
  struct _asm_context *asm_context,
  uint64_t *value,
  int as_bin)
{
  struct _var var;

  if (eval_expression_ex(asm_context, &var) == -1)
  {
    if (asm_context->pass == 2)
    {
      print_error_illegal_expression("Constant", asm_context);
      return -1;
    }

    ignore_operand(asm_context);

    *value = 0;
  }
    else
  {
    if (as_bin == 0)
    {
      *value = var_get_int64(&var);
    }
      else
    {
      *value = var_get_bin64(&var);
    }
  }

  return 0;
}

static int get_type(const char *token)
{
  int n = 0;

  while (webasm_types[n].name != NULL)
  {
    if (strcmp(webasm_types[n].name, token) == 0)
    {
      return webasm_types[n].type;
    }

    n++;
  }

  return -1;
}

int parse_instruction_webasm(struct _asm_context *asm_context, char *instr)
{
  int token_type;
  char token[TOKENLEN];
  char instr_case[TOKENLEN];
  uint64_t value, count;
  int64_t i;
  int type;
  int length, n, j;

  lower_copy(instr_case, instr);

  n = 0;
  while (table_webasm[n].instr != NULL)
  {
    if (strcmp(table_webasm[n].instr, instr_case) != 0)
    {
      n++;
      continue;
    }

    switch (table_webasm[n].type)
    {
      case WEBASM_OP_NONE:
        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);

        length = 1;
        break;
      case WEBASM_OP_UINT32:
        if (get_uint(asm_context, &value, 1) != 0) { return -1; }

        if (value < 0 || value > 0xffffffff)
        {
          print_error_range("Constant", 0, 0xffffffff, asm_context);
          return -1;
        }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        add_bin32(asm_context, value, 0);

        length = 5;
        break;
      case WEBASM_OP_UINT64:
        if (get_uint(asm_context, &value, 1) != 0) { return -1; }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        add_bin32(asm_context, value & 0xffffffff, 0);
        add_bin32(asm_context, value >> 32, 0);

        length = 9;
        break;
      case WEBASM_OP_VARINT64:
        if (get_uint(asm_context, &value, 0) != 0) { return -1; }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        length = add_bin_varuint(asm_context, value, 0);

        length += 1;
        break;
      case WEBASM_OP_VARINT32:
        if (get_uint(asm_context, &value, 0) != 0) { return -1; }

        i = (int64_t)value;

        if (i < -0x80000000LL || i > 0x7fffffffLL)
        {
          print_error_range("Constant", -0x80000000, 0x7fffffff, asm_context);
          return -1;
        }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        length = add_bin_varint(asm_context, value & 0xffffffff, 0);

        length += 1;
        break;
      case WEBASM_OP_FUNCTION_INDEX:
      case WEBASM_OP_LOCAL_INDEX:
      case WEBASM_OP_GLOBAL_INDEX:
        if (get_uint(asm_context, &value, 0) != 0) { return -1; }

        if (value < 0 || value > 0xffffffff)
        {
          print_error_range("Constant", 0, 0xffffffff, asm_context);
          return -1;
        }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        length = add_bin_varuint(asm_context, value, 0);

        length += 1;
        break;
      case WEBASM_OP_BLOCK_TYPE:
        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (token_type == TOKEN_EOF || token_type == TOKEN_EOL)
        {
          type = 0x40;
        }
          else
        {
          type = get_type(token);
        }

        if (type == -1)
        {
          print_error_unexp(token, asm_context);
          return -1;
        }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        add_bin8(asm_context, type, IS_OPCODE);

        length = 2;
        break;
      case WEBASM_OP_RELATIVE_DEPTH:
        if (get_uint(asm_context, &value, 0) != 0) { return -1; }

        i = (int64_t)value;

        if (i < -0x80000000LL || i > 0x7fffffffLL)
        {
          print_error_range("Constant", -0x80000000, 0x7fffffff, asm_context);
          return -1;
        }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        length = add_bin_varint(asm_context, value & 0xffffffff, 0);

        length += 1;
        break;
      case WEBASM_OP_TABLE:
        if (get_uint(asm_context, &value, 0) != 0) { return -1; }

        add_bin8(asm_context, table_webasm[n].opcode, IS_OPCODE);
        length = add_bin_varint(asm_context, value & 0xffffffff, 0);

        count = value;

        for (j = 0; j < count; j++)
        {
          if (expect_token(asm_context, ',') == -1) { return -1; }
          if (get_uint(asm_context, &value, 0) != 0) { return -1; }
          length += add_bin_varint(asm_context, value & 0xffffffff, 0);
        }

        token_type = tokens_get(asm_context, token, TOKENLEN);

        if (token_type != TOKEN_EOF && token_type != TOKEN_EOL)
        {
          print_error_unexp(token, asm_context);
        }

        length += 1;
        break;
      case WEBASM_OP_INDIRECT:
      case WEBASM_OP_MEMORY_IMMEDIATE:
      default:
        n++;
        continue;
    }

    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type != TOKEN_EOF && token_type != TOKEN_EOL)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    return length;
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

