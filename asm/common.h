/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2017 by Michael Kohn
 *
 */

#ifndef _ASM_COMMON_H
#define _ASM_COMMON_H

#include <stdint.h>

#include "common/assembler.h"
#include "common/tokens.h"

#define IS_DATA 0
#define IS_OPCODE 1

void add_bin8(struct _asm_context *asm_context, uint8_t b, int flags);
void add_bin16(struct _asm_context *asm_context, uint16_t b, int flags);
void add_bin24(struct _asm_context *asm_context, uint32_t b, int flags);
void add_bin32(struct _asm_context *asm_context, uint32_t b, int flags);
int eat_operand(struct _asm_context *asm_context);
int ignore_paren_expression(struct _asm_context *asm_context);
int ignore_line(struct _asm_context *asm_context);
void lower_copy(char *d, const char *s);
int expect_token(struct _asm_context *asm_context, char ch);
int expect_token_s(struct _asm_context *asm_context, char *s);
int check_range(struct _asm_context *asm_context, char *type, int num, int min, int max);

#endif

