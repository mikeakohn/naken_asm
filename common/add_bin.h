/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_ADD_BIN_H
#define NAKEN_ASM_ADD_BIN_H

#include <stdint.h>

#include "assembler.h"

#define IS_DATA 0
#define IS_OPCODE 1

void add_bin8(struct _asm_context *asm_context, uint8_t b, int flags);
void add_bin16(struct _asm_context *asm_context, uint16_t b, int flags);
//void add_bin24(struct _asm_context *asm_context, uint32_t b, int flags);
void add_bin32(struct _asm_context *asm_context, uint32_t b, int flags);
int add_bin_varuint(struct _asm_context *asm_context, uint64_t b, int fixed_size);
int add_bin_varint(struct _asm_context *asm_context, uint64_t b, int fixed_size);

#endif

