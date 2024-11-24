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

#ifndef NAKEN_ASM_FILE_H
#define NAKEN_ASM_FILE_H

#include <stdlib.h>

#include "common/assembler.h"
#include "common/UtilContext.h"

enum
{
  FILE_TYPE_AUTO = -1,
  FILE_TYPE_HEX = 0,
  FILE_TYPE_BIN,
  FILE_TYPE_ELF,
  FILE_TYPE_SREC,
  FILE_TYPE_WDC,
  FILE_TYPE_AMIGA,
  FILE_TYPE_TI_TXT,
  FILE_TYPE_MACHO,
  FILE_TYPE_UF2,
};

int file_write(const char *filename, AsmContext *asm_context, int file_type);

const char *file_get_file_type_name(int file_type);

int file_read(
  const char *filename,
  UtilContext *util_context,
  int *file_type,
  const char *cpu_name,
  uint32_t start_address);

#endif

