/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TOKENS_H
#define NAKEN_ASM_TOKENS_H

//#include "assembler.h"

#define TOKENLEN 512

#define IS_TOKEN(t,a) (t[0]==a && t[1]==0)
#define IS_NOT_TOKEN(t,a) (t[0]!=a || t[1]!=0)

//struct _asm_context;
//typedef struct _asm_context AsmContext;

typedef struct _token_buffer
{
  const char *code;
  int ptr;
} TokenBuffer;

typedef struct _tokens
{
  FILE *in;
  int line;
  const char *filename;
  TokenBuffer token_buffer;
  int pushback_type;
  int pushback2_type;
  int unget_ptr;
  int unget_stack_ptr;
  int unget_stack[MAX_NESTED_MACROS + 1];
  char unget[512];
  char pushback[TOKENLEN];
  char pushback2[TOKENLEN];
} Tokens;

int tokens_open_file(AsmContext *asm_context, const char *filename);
void tokens_open_buffer(AsmContext *asm_context, const char *buffer);
void tokens_close(AsmContext *asm_context);
void tokens_reset(AsmContext *asm_context);
int tokens_get_char(AsmContext *asm_context);
int tokens_unget_char(AsmContext *asm_context, int ch);
int tokens_get(AsmContext *asm_context, char *token, int len);
void tokens_push(AsmContext *asm_context, const char *token, int token_type);
int tokens_escape_char(AsmContext *asm_context, uint8_t *s);

enum
{
  TOKEN_EOF = -1,
  TOKEN_EOL,
  TOKEN_NUMBER,
  TOKEN_FLOAT,
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

