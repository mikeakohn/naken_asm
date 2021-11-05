/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2021 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/directives_if.h"
#include "common/ifdef_expression.h"
#include "common/tokens.h"
#include "common/macros.h"
#include "common/print_error.h"

int ifdef_ignore(struct _asm_context *asm_context)
{
  char token[TOKENLEN];
  int token_type;
  int nested_if = 0;

  while(1)
  {
    token_type = tokens_get(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOF)
    {
      print_error("Missing endif", asm_context);
      return -1;
    }

    if (token_type == TOKEN_EOL)
    {
      asm_context->tokens.line++;
    }
      else
    if (token_type == TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      token_type = tokens_get(asm_context, token, TOKENLEN);
      if (strcasecmp(token, "endif") == 0)
      {
        if (nested_if == 0) { return 0; }
        nested_if--;
      }
        else
      if (strcasecmp(token, "else") == 0)
      {
        if (nested_if == 0) { return 2; }
      }
        else
      if (strcasecmp(token, "if") == 0 || strcasecmp(token, "ifdef") == 0)
      {
        nested_if++;
      }
    }
  }
}

int parse_ifdef_ignore(struct _asm_context *asm_context, int ignore_section)
{
  if (ignore_section == 1)
  {
    if (ifdef_ignore(asm_context) == 2)
    {
      assemble(asm_context);
    }
  }
    else
  {
    if (assemble(asm_context) == 2)
    {
      ifdef_ignore(asm_context);
    }
  }

  return 0;
}

int parse_ifdef(struct _asm_context *asm_context, int ifndef)
{
  char token[TOKENLEN];
  int token_type;
  int ignore_section = 0;
  int param_count; // throw away

  asm_context->ifdef_count++;

  asm_context->parsing_ifdef = 1;
  token_type = tokens_get(asm_context, token, TOKENLEN);
  asm_context->parsing_ifdef = 0;

  if (token_type != TOKEN_STRING)
  {
    print_error("#ifdef has no label", asm_context);
    return -1;
  }

  if (macros_lookup(&asm_context->macros, token, &param_count) != NULL ||
      symbols_find(&asm_context->symbols, token) != NULL)
  {
    if (ifndef == 1) { ignore_section = 1; }
  }
    else
  {
    if (ifndef == 0) { ignore_section = 1; }
  }

  parse_ifdef_ignore(asm_context, ignore_section);

  asm_context->ifdef_count--;

  return 0;
}

int parse_if(struct _asm_context *asm_context)
{
  int num;

  asm_context->ifdef_count++;

  asm_context->parsing_ifdef = 1;
  num = eval_ifdef_expression(asm_context);
  asm_context->parsing_ifdef = 0;

  if (num == -1) { return -1; }

  if (num != 0)
  {
    parse_ifdef_ignore(asm_context, 0);
  }
    else
  {
    parse_ifdef_ignore(asm_context, 1);
  }

  asm_context->ifdef_count--;

  return 0;
}

