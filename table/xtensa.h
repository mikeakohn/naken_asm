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

#ifndef NAKEN_ASM_TABLE_XTENSA_H
#define NAKEN_ASM_TABLE_XTENSA_H

#include "common/assembler.h"

enum
{
  XTENSA_OP_NONE,
  XTENSA_OP_AR_AT,
  XTENSA_OP_FR_FS,
  XTENSA_OP_FR_FS_FT,
  XTENSA_OP_AR_AS_AT,
  XTENSA_OP_AT_AS_I8,
  XTENSA_OP_N_AR_AS_AT,
  XTENSA_OP_N_AR_AS_I4,
};

struct _table_xtensa
{
  char *instr;
  uint32_t opcode_le;
  uint32_t mask_le;
  uint32_t opcode_be;
  uint32_t mask_be;
  uint8_t bits;
  uint8_t type;
};

extern struct _table_xtensa table_xtensa[];

#endif


