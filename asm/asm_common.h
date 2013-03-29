/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#ifndef _ASM_COMMON_H
#define _ASM_COMMON_H

#include "assembler.h"
#include "get_tokens.h"

#define IS_DATA 0
#define IS_OPCODE 1

// FIXME - change add_bin() to add_bin16() instead.
#define add_bin16 add_bin

void add_bin(struct _asm_context *asm_context, unsigned short int b, int flags);
void add_bin8(struct _asm_context *asm_context, unsigned char b, int flags);
void add_bin24(struct _asm_context *asm_context, unsigned int b, int flags);
void add_bin32(struct _asm_context *asm_context, unsigned int b, int flags);
int eat_operand(struct _asm_context *asm_context);
void lower_copy(char *d, const char *s);
int expect_token(struct _asm_context *asm_context, char ch);
int expect_token_s(struct _asm_context *asm_context, char *s);

#endif

