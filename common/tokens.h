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

#ifndef _TOKENS_H
#define _TOKENS_H

//#include "assembler.h"

#define IS_TOKEN(t,a) (t[0]==a && t[1]==0)
#define IS_NOT_TOKEN(t,a) (t[0]!=a || t[1]!=0)

struct _asm_context;

struct _token_buffer
{
  const char *code;
  int ptr;
};

int tokens_open_file(struct _asm_context *asm_context, char *filename);
void tokens_open_buffer(struct _asm_context *asm_context, const char *buffer);
void tokens_close(struct _asm_context *asm_context);
void tokens_reset(struct _asm_context *asm_context);
int tokens_get_char(struct _asm_context *asm_context);
int tokens_unget_char(struct _asm_context *asm_context, int ch);
int tokens_get(struct _asm_context *asm_context, char *token, int len);
void tokens_push(struct _asm_context *asm_context, char *token, int token_type);
int tokens_escape_char(struct _asm_context *asm_context, unsigned char *s);

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
  TOKEN_TICKED,
  TOKEN_EQUALITY,
  TOKEN_DOLLAR,
  TOKEN_UNKOWN
};

#endif

