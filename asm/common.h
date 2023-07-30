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

#ifndef NAKEN_ASM_ASM_COMMON_H
#define NAKEN_ASM_ASM_COMMON_H

#include <stdint.h>

#include "common/add_bin.h"
#include "common/assembler.h"
#include "common/tokens.h"

int ignore_operand(AsmContext *asm_context);
int ignore_paren_expression(AsmContext *asm_context);
int ignore_line(AsmContext *asm_context);
void lower_copy(char *d, const char *s);
int expect_token(AsmContext *asm_context, char ch);
int expect_token_s(AsmContext *asm_context, const char *s);
int check_range(AsmContext *asm_context, const char *type, int num, int min, int max);
int get_reg_number(const char *token, int max);

#endif

