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

#ifndef NAKEN_ASM_ADD_BIN_H
#define NAKEN_ASM_ADD_BIN_H

#include <stdint.h>

#include "assembler.h"

#define IS_DATA 0
#define IS_OPCODE 1

void add_bin8(AsmContext *asm_context, uint8_t b, int flags);
void add_bin16(AsmContext *asm_context, uint16_t b, int flags);
//void add_bin24(AsmContext *asm_context, uint32_t b, int flags);
void add_bin32(AsmContext *asm_context, uint32_t b, int flags);
int add_bin_varuint(AsmContext *asm_context, uint64_t b, int fixed_size);
int add_bin_varint(AsmContext *asm_context, uint64_t b, int fixed_size);

#endif

