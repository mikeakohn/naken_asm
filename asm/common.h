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

#ifndef NAKEN_ASM_ASM_COMMON_H
#define NAKEN_ASM_ASM_COMMON_H

#include <stdint.h>

#include "common/add_bin.h"
#include "common/assembler.h"
#include "common/tokens.h"

int ignore_operand(struct _asm_context *asm_context);
int ignore_paren_expression(struct _asm_context *asm_context);
int ignore_line(struct _asm_context *asm_context);
void lower_copy(char *d, const char *s);
int expect_token(struct _asm_context *asm_context, char ch);
int expect_token_s(struct _asm_context *asm_context, char *s);
int check_range(struct _asm_context *asm_context, char *type, int num, int min, int max);
int get_reg_number(char *token, int max);

#endif

