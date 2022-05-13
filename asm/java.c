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

#include "asm/common.h"
#include "asm/java.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
#include "table/java.h"

static const char *array_types[] =
{
  "boolean",
  "char",
  "float",
  "double",
  "byte",
  "short",
  "int",
  "long",
};

static int parse_num8(
  struct _asm_context *asm_context,
  const char *instr,
  int low,
  int high)
{
  int num;

  if (asm_context->pass == 1)
  {
    add_bin8(asm_context, 0, IS_OPCODE);
    ignore_line(asm_context);
    return 3;
  }

  if (eval_expression(asm_context, &num) != 0)
  {
    print_error_illegal_expression(instr, asm_context);
    return -1;
  }

  if (num < low && num > high)
  {
    print_error_range("Operand", low, high, asm_context);
    return -1;
  }

  add_bin8(asm_context, num & 0xff, IS_OPCODE);

  return 3;
}

static int parse_num16(
  struct _asm_context *asm_context,
  const char *instr,
  int low,
  int high)
{
  int num;

  if (asm_context->pass == 1)
  {
    add_bin8(asm_context, 0, IS_OPCODE);
    add_bin8(asm_context, 0, IS_OPCODE);
    ignore_line(asm_context);
    return 3;
  }

  if (eval_expression(asm_context, &num) != 0)
  {
    print_error_illegal_expression(instr, asm_context);
    return -1;
  }

  if (num < low && num > high)
  {
    print_error_range("Operand", low, high, asm_context);
    return -1;
  }

  add_bin8(asm_context, (num >> 8) & 0xff, IS_OPCODE);
  add_bin8(asm_context, num & 0xff, IS_OPCODE);

  return 3;
}

static int parse_index_w(
  struct _asm_context *asm_context,
  const char *instr,
  int opcode,
  int has_const)
{
  int index, length;
  int wide = 0;

  if (eval_expression(asm_context, &index) != 0)
  {
    if (asm_context->pass == 1)
    {
      add_bin8(asm_context, 0xc4, IS_OPCODE);
      add_bin8(asm_context, opcode, IS_OPCODE);
      add_bin8(asm_context, 0, IS_OPCODE);
      add_bin8(asm_context, 0, IS_OPCODE);

      ignore_line(asm_context);

      if (has_const == 1)
      {
        add_bin8(asm_context, 0, IS_OPCODE);
        return 5;
      }

      return 4;
    }

    print_error_illegal_expression(instr, asm_context);
    return -1;
  }

  if (memory_read_m(&asm_context->memory, asm_context->address) == 0xc4)
  {
    wide = 1;
  }

  if (index < 0 || index > 0xffff)
  {
    print_error_range("Offset", 0, 0xffff, asm_context);
    return -1;
  }

  if (index > 0xff || wide == 1)
  {
    add_bin8(asm_context, 0xc4, IS_OPCODE);
    add_bin8(asm_context, opcode, IS_OPCODE);
    add_bin8(asm_context, (index >> 8) & 0xff, IS_OPCODE);
    add_bin8(asm_context, index & 0xff, IS_OPCODE);

    length = 4;
  }
    else
  {
    add_bin8(asm_context, opcode, IS_OPCODE);
    add_bin8(asm_context, index & 0xff, IS_OPCODE);

    length = 2;
  }

  if (has_const == 1)
  {
    int num;

    if (expect_token(asm_context, ',') == -1) { return -1; }

    if (eval_expression(asm_context, &num) != 0)
    {
      if (asm_context->pass == 1)
      {
        ignore_line(asm_context);
        num = 0;
      }
        else
      {
        print_error_illegal_expression(instr, asm_context);
        return -1;
      }
    }

    if (index < -128 || index > 127)
    {
      print_error_range("Constant", -128, 127, asm_context);
      return -1;
    }

    add_bin8(asm_context, num & 0xff, IS_OPCODE);

    length++;
  }

  return length;
}

static int parse_array_type(
  struct _asm_context *asm_context,
  const char *instr)
{
  int n;
  char token[TOKENLEN];
  int token_type;

  if (asm_context->pass == 1)
  {
    add_bin8(asm_context, 0, IS_OPCODE);
    ignore_line(asm_context);
    return 2;
  }

  const int count = sizeof(array_types) / sizeof(char *);

  token_type = tokens_get(asm_context, token, TOKENLEN);

  for (n = 0; n < count; n++)
  {
    if (strcasecmp(array_types[n], token) == 0) { break; }
  }

  if (n == count)
  {
    tokens_push(asm_context, token, token_type);

    if (eval_expression(asm_context, &n) != 0)
    {
      print_error_illegal_expression(instr, asm_context);
      return -1;
    }
  }
    else
  {
    n += 4;
  }

  if (n < 0 || n > 0xff)
  {
    print_error_range("Type", 0, 0xff, asm_context);
    return -1;
  }

  add_bin8(asm_context, n, IS_OPCODE);

  return 2;
}

static int parse_offset(
  struct _asm_context *asm_context,
  const char *instr,
  int is_long)
{
  int offset;
  int address;

  if (asm_context->pass == 1)
  {
    ignore_line(asm_context);

    add_bin8(asm_context, 0, IS_OPCODE);
    add_bin8(asm_context, 0, IS_OPCODE);

    if (is_long == 1)
    {
      add_bin8(asm_context, 0, IS_OPCODE);
      add_bin8(asm_context, 0, IS_OPCODE);
      return 5;
    }

    return 3;
  }

  if (eval_expression(asm_context, &address) != 0)
  {
    print_error_illegal_expression(instr, asm_context);
    return -1;
  }

  offset = address  - (asm_context->address - 1);

  if (is_long == 0)
  {
    if (offset < -128 || offset > 127)
    {
      print_error_range("Offset", -128, 127, asm_context);
      return -1;
    }

    add_bin8(asm_context, (offset >> 8) & 0xff, IS_OPCODE);
    add_bin8(asm_context, offset & 0xff, IS_OPCODE);

    return 3;
  }
    else
  {
    if (offset < -32768 || offset > 32767)
    {
      print_error_range("Offset", -32768, 32767, asm_context);
      return -1;
    }

    add_bin8(asm_context, (offset >> 24) & 0xff, IS_OPCODE);
    add_bin8(asm_context, (offset >> 16) & 0xff, IS_OPCODE);
    add_bin8(asm_context, (offset >> 8) & 0xff, IS_OPCODE);
    add_bin8(asm_context, offset & 0xff, IS_OPCODE);

    return 5;
  }
}

int parse_instruction_java(struct _asm_context *asm_context, char *instr)
{
  char token[TOKENLEN];
  int token_type;
  char instr_case[TOKENLEN];
  int n;

  lower_copy(instr_case, instr);

  for (n = 0; n < 255; n++)
  {
    if (table_java[n].op_type == JAVA_OP_ILLEGAL) { continue; }

    if (strcmp(table_java[n].instr, instr_case) == 0)
    {
      switch (table_java[n].op_type)
      {
        case JAVA_OP_NONE:
          add_bin8(asm_context, n, IS_OPCODE);

          token_type = tokens_get(asm_context, token, TOKENLEN);

          if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
          {
            print_error_unexp(token, asm_context);
            return -1;
          }

          return 1;
        case JAVA_OP_CONSTANT_INDEX:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_num8(asm_context, instr, 0, 0xff);
        case JAVA_OP_FIELD_INDEX:
        case JAVA_OP_INTERFACE_INDEX:
        case JAVA_OP_METHOD_INDEX:
        case JAVA_OP_CLASS_INDEX:
        case JAVA_OP_SPECIAL_INDEX:
        case JAVA_OP_STATIC_INDEX:
        case JAVA_OP_VIRTUAL_INDEX:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_num16(asm_context, instr, 0, 0xffff);
        case JAVA_OP_LOCAL_INDEX:
          return parse_index_w(asm_context, instr, n, 0);
        case JAVA_OP_LOCAL_INDEX_CONST:
          return parse_index_w(asm_context, instr, n, 1);
        case JAVA_OP_ARRAY_TYPE:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_array_type(asm_context, instr);
        case JAVA_OP_CONSTANT16:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_num16(asm_context, instr, -32768, 32767);
        case JAVA_OP_CONSTANT8:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_num8(asm_context, instr, -128, 127);
        case JAVA_OP_OFFSET16:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_offset(asm_context, instr, 0);
        case JAVA_OP_OFFSET32:
          add_bin8(asm_context, n, IS_OPCODE);
          return parse_offset(asm_context, instr, 1);
        case JAVA_OP_WARN:
          printf("Warning: %s is reserved  %s:%d.\n",
            instr, asm_context->tokens.filename, asm_context->tokens.line);
          add_bin8(asm_context, n, IS_OPCODE);
          return 1;
      }
    }
  }

  print_error_unknown_instr(instr, asm_context);

  return -1;
}

