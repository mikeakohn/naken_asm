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
#include "asm/copper.h"
#include "common/assembler.h"
#include "common/tokens.h"
#include "common/eval_expression.h"
//#include "table/copper.h"

enum
{
  INSTRUCTION_WAIT,
  INSTRUCTION_SKIP,
  INSTRUCTION_MOVE,
};

static int parse_wait_or_skip(struct _asm_context *asm_context, const char *instr, int is_skip)
{
  char token[TOKENLEN];
  int token_type;
  int n;
  int bfd = 0;
  int x = 127, y = 255, compare_x = 0x7f, compare_y = 0x7f;
  int param_count = 0;

  if (asm_context->pass == 1)
  {
    add_bin16(asm_context, 0, IS_OPCODE);
    add_bin16(asm_context, 0, IS_OPCODE);
    ignore_line(asm_context);

    return 0;
  }

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      break;
    }

    if (param_count != 0)
    {
      if (IS_NOT_TOKEN(token, ','))
      {
        print_error_unexp(token, asm_context);
        return -1;
      }
    }

    if (param_count == 5)
    {
      print_error_opcount(instr, asm_context);
    }

    if (param_count == 0)
    {
      if (strcasecmp(token, "bfd") == 0)
      {
        bfd = 1;
        param_count++;
        continue;
      }
        else
      {
        tokens_push(asm_context, token, token_type);
      }

      param_count++;
    }

    if (eval_expression(asm_context, &n) != 0)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    switch (param_count)
    {
      case 1:
        if (n < 0 || n > 0x7f)
        {
          print_error_range("X", 0, 0x7f, asm_context);
          return -1;
        }

        x = n;
        break;
      case 2:
        if (n < 0 || n > 0xff)
        {
          print_error_range("Y", 0, 0xff, asm_context);
          return -1;
        }

        y = n;
        break;
      case 3:
        if (n < 0 || n > 0x7f)
        {
          print_error_range("Compare X", 0, 0x7f, asm_context);
          return -1;
        }

        compare_x = n;
        break;
      case 4:
        if (n < 0 || n > 0x7f)
        {
          print_error_range("Compare Y", 0, 0x7f, asm_context);
          return -1;
        }

        compare_y = n;
        break;
      default:
        print_error_internal(asm_context, __FILE__, __LINE__);
        return -1;
    }

    param_count++;
  }

  // If this is a "wait" instruction with no operands, then bfd should be 1.
  if (param_count == 0) { bfd = 1; }

  add_bin16(asm_context, (y << 8) | (x << 1) | 1, IS_OPCODE);
  add_bin16(asm_context, (bfd << 15) | (compare_x << 8) | (compare_y << 1) | is_skip, IS_OPCODE);

  return 0;
}

static int parse_move(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  int n;

  if (asm_context->pass == 1)
  {
    add_bin16(asm_context, 0, IS_OPCODE);
    add_bin16(asm_context, 0, IS_OPCODE);
    ignore_line(asm_context);

    return 0;
  }

  if (eval_expression(asm_context, &n) != 0)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if ((n & 0x1) != 0)
  {
    print_error_align(asm_context, 2);
    return -1;
  }

  if (n < 0 || n > 254)
  {
    print_error_range("Register", 0, 254, asm_context);
    return -1;
  }

  add_bin16(asm_context, n, IS_OPCODE);

  if (expect_token(asm_context, ',') == -1) { return -1; }

  if (eval_expression(asm_context, &n) != 0)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  if (n < -32768 || n > 0xffff)
  {
    print_error_range("Value", -32768, 0xffff, asm_context);
    return -1;
  }

  add_bin16(asm_context, (n & 0xffff), IS_OPCODE);

  token_type = tokens_get(asm_context, token, TOKENLEN);

  if (token_type != TOKEN_EOL && token_type != TOKEN_EOF)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  return 0;
}

int parse_instruction_copper(struct _asm_context *asm_context, char *instr)
{
  char instr_case[TOKENLEN];
  int instruction;
  int is_skip = 0;

  lower_copy(instr_case, instr);

  if (strcmp(instr_case, "wait") == 0)
  {
    instruction = INSTRUCTION_WAIT;
    is_skip = 0;
  }
    else
  if (strcmp(instr_case, "move") == 0)
  {
    instruction = INSTRUCTION_MOVE;
  }
    else
  if (strcmp(instr_case, "skip") == 0)
  {
    instruction = INSTRUCTION_SKIP;
    is_skip = 1;
  }
    else
  {
    print_error_unknown_instr(instr, asm_context);
    return -1;
  }

  switch (instruction)
  {
    case INSTRUCTION_WAIT:
    case INSTRUCTION_SKIP:
      return parse_wait_or_skip(asm_context, instr, is_skip);
      break;
    case INSTRUCTION_MOVE:
      return parse_move(asm_context);
      break;
  }

  return -1;
}

