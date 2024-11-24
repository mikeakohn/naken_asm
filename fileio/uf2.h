/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_UF2_H
#define NAKEN_ASM_UF2_H

#include <stdint.h>

struct Uf2Block
{
  uint32_t magic_0;
  uint32_t magic_1;
  uint32_t flags;
  uint32_t address;
  uint32_t byte_count;
  uint32_t sequence_block_number;
  uint32_t total_blocks;
  uint32_t board_family;
  uint8_t data[476];
  uint32_t magic_2;
};

#endif

