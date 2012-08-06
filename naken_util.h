/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#ifndef NAKEN_430_UTIL_H
#define NAKEN_430_UTIL_H

#include "memory.h"
#include "simulate_msp430.h"

//#define ARCH_MSP430
//#define ARCH_DSPIC
//#define ARCH_ARM

typedef void (*disasm_range_t)(struct _memory *, int, int);

struct _util_context
{
  struct _memory memory;
  struct _simulate *simulate;
  long *debug_line_offset;
  FILE *src_fp;
  int fd;
  //int instr_bytes;
  disasm_range_t disasm_range;
};

#endif

