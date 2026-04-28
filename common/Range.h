/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_RANGE_H
#define NAKEN_ASM_RANGE_H

#include "stdint.h"

struct Range
{
  Range() : start(0), end(0) { }
  uint32_t start;
  uint32_t end;
};

#endif

