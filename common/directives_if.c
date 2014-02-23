/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "directives_if.h"
#include "ifdef_expression.h"
#include "get_tokens.h"
#include "macros.h"
#include "print_error.h"

int ifdef_ignore(struct _asm_context *asm_context)
{
char token[TOKENLEN];
int token_type;

  while(1)
  {
    token_type = get_token(asm_context, token, TOKENLEN);

    if (token_type == TOKEN_EOF)
    {
      print_error("Missing endif", asm_context);
      return -1;
    }

    if (token_type == TOKEN_EOL)
    {
      asm_context->line++;
    }
      else
    if (token_type == TOKEN_POUND || IS_TOKEN(token,'.'))
    {
      token_type = get_token(asm_context, token, TOKENLEN);
      if (strcasecmp(token, "endif") == 0) return 0;
        else
      if (strcasecmp(token, "else") == 0) return 2;
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
  token_type = get_token(asm_context, token, TOKENLEN);
  asm_context->parsing_ifdef = 0;

  if (token_type != TOKEN_STRING)
  {
    print_error("#ifdef has no label", asm_context);
    return -1;
  }

  if (macros_lookup(&asm_context->macros, token, &param_count)!=NULL)
  {
    if (ifndef == 1) ignore_section = 1;
  }
    else
  {
    if (ifndef == 0) ignore_section = 1;
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

  if (num == -1) return -1;

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

