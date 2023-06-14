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

#ifndef NAKEN_ASM_DIRECTIVES_DATA_H
#define NAKEN_ASM_DIRECTIVES_DATA_H

#include "common/assembler.h"

int parse_db(AsmContext *asm_context, int null_term_flag);
int parse_dc16(AsmContext *asm_context);
int parse_dc32(AsmContext *asm_context);
int parse_dc64(AsmContext *asm_context);
int parse_dc(AsmContext *asm_context);
//int parse_dq(AsmContext *asm_context);
//int parse_ds(AsmContext *asm_context, int size);
int parse_data_fill(AsmContext *asm_context);
int parse_varuint(AsmContext *asm_context, int fixed_size);
int parse_resb(AsmContext *asm_context, int size);
int parse_align_bits(AsmContext *asm_context);
int parse_align_bytes(AsmContext *asm_context);

#endif

