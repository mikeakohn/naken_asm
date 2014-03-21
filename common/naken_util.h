/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#ifndef NAKEN_430_UTIL_H
#define NAKEN_430_UTIL_H

#include "cpu_list.h"
#include "memory.h"
#include "simulate_msp430.h"

//typedef void (*disasm_range_t)(struct _memory *, int, int);

struct _util_context
{
  struct _memory memory;
  struct _simulate *simulate;
  struct _symbols symbols;
  long *debug_line_offset;
  FILE *src_fp;
  int fd;
  disasm_range_t disasm_range;
};

#endif

