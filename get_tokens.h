/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#ifndef _GET_TOKEN_H
#define _GET_TOKEN_H

#include "assembler.h"

#define IS_TOKEN(t,a) (t[0]==a && t[1]==0)
#define IS_NOT_TOKEN(t,a) (t[0]!=a || t[1]!=0)

struct _asm_context;

int get_next_char(struct _asm_context *asm_context);
int unget_next_char(struct _asm_context *asm_context, int ch);
int get_token(struct _asm_context *asm_context, char *token, int len);
void pushback(struct _asm_context *asm_context, char *token, int token_type);
int escape_char(struct _asm_context *asm_context, unsigned char *s);

enum
{
  TOKEN_EOF = -1,
  TOKEN_EOL,
  TOKEN_NUMBER,
  TOKEN_POUND,
  TOKEN_LABEL,
  TOKEN_STRING,
  TOKEN_SYMBOL,
  TOKEN_QUOTED,
  TOKEN_EQUALITY,
  TOKEN_DOLLAR,
  TOKEN_UNKOWN
};

#endif

