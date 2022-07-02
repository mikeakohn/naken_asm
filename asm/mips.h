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

#ifndef NAKEN_ASM_ASM_MIPS_H
#define NAKEN_ASM_ASM_MIPS_H

#include "common/assembler.h"

int parse_instruction_mips(struct _asm_context *asm_context, char *instr);

int link_function_mips(
  struct _asm_context *asm_context,
  struct _imports *imports,
  const uint8_t *code,
  uint32_t function_offset,
  int size,
  uint8_t *obj_file,
  uint32_t obj_size);

#endif

