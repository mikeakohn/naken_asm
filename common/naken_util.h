/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2020 by Michael Kohn
 *
 */

#ifndef NAKEN_430_UTIL_H
#define NAKEN_430_UTIL_H

#include "common/cpu_list.h"
#include "common/memory.h"
#include "simulate/msp430.h"

struct _util_context
{
  struct _memory memory;
  struct _simulate *simulate;
  struct _symbols symbols;
  long *debug_line_offset;
  FILE *src_fp;
  int fd;
  uint32_t flags;
  uint8_t bytes_per_address;
  uint8_t alignment;
  disasm_range_t disasm_range;
};

#endif

