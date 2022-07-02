/**
 *  naken_asm MSP430 assembler.
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
#include "common/memory.h"

int ignore_operand(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  // Eat all tokens until an ',' or EOL
  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token,',') || token_type == TOKEN_EOL || token_type == TOKEN_EOF)
    {
      tokens_push(asm_context, token, token_type);
      return 0;
    }
  }

  return -1;
}

int ignore_paren_expression(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  int paren_count = 0;

  // Remove expression that starts with (.
  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (IS_TOKEN(token,'('))
    {
      paren_count++;
    }
      else
    if (IS_TOKEN(token,')'))
    {
      paren_count--;
    }
      else
    if (token_type == TOKEN_EOL)
    {
      print_error_unexp(token, asm_context);
      return -1;
    }

    if (paren_count == 0) { break; }
  }

  return 0;
}

int ignore_line(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;

  while (1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);
    if (token_type == TOKEN_EOL || token_type == TOKEN_EOF) { break; }
  }

  return 0;
}

void lower_copy(char *d, const char *s)
{
  while (1)
  {
    *d = tolower(*s);
    if (*s == 0) break;
    d++;
    s++;
  }
}

int expect_token(struct _asm_context *asm_context, char ch)
{
  char token[TOKENLEN];

  tokens_get(asm_context, token, TOKENLEN);

  if (IS_NOT_TOKEN(token, ch))
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  return 0;
}

int expect_token_s(struct _asm_context *asm_context, char *s)
{
  char token[TOKENLEN];

  tokens_get(asm_context, token, TOKENLEN);

  if (strcasecmp(token,s) != 0)
  {
    print_error_unexp(token, asm_context);
    return -1;
  }

  return 0;
}

int check_range(struct _asm_context *asm_context, char *type, int num, int min, int max)
{
  if (num < min || num > max)
  {
    print_error_range(type, min, max, asm_context);
    return -1;
  }

  return 0;
}

int get_reg_number(char *token, int max)
{
  int num = 0;

  while (*token != 0)
  {
    if (*token < '0' || *token > '9') { return -1; }
    num = (num * 10) + (*token - '0');
    token++;
  }

  if (num > max) { return -1; }

  return num;
}

