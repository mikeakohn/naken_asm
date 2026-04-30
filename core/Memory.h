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

#ifndef NAKEN_ASM_MEMORY_H
#define NAKEN_ASM_MEMORY_H

#include <stdint.h>

#include "MemoryPage.h"

#define ENDIAN_LITTLE 0
#define ENDIAN_BIG 1

#define DL_EMPTY -1
#define DL_DATA -2
#define DL_NO_CG -3

class Memory
{
public:
  Memory();
  ~Memory();

  int get_page_size() { return PAGE_SIZE; }
  void clear();
  bool in_use(uint32_t address);
  uint32_t get_page_address_min(uint32_t address);
  uint32_t get_page_address_max(uint32_t address);

  uint8_t read8(uint32_t address);
  uint16_t read16(uint32_t address);
  uint32_t read32(uint32_t address);
  void write8(uint32_t address, uint8_t data);
  void write16(uint32_t address, uint16_t data);
  void write32(uint32_t address, uint32_t data);

  int read_debug(uint32_t address);
  void write_debug(uint32_t address, int line);
  void write(uint32_t address, uint8_t data, int line);

  bool is_little_endian() { return endian == ENDIAN_LITTLE; }

  void dump();

  MemoryPage *pages;
  uint32_t low_address;
  uint32_t high_address;
  uint32_t entry_point;
  int endian;
};

class AsmContext;

#endif

