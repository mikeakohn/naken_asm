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

  void dump();

  MemoryPage *pages;
  uint32_t low_address;
  uint32_t high_address;
  uint32_t entry_point;
  int endian;
};

class AsmContext;

//uint8_t memory_read(AsmContext *asm_context, uint32_t address);
//uint8_t memory_read_inc(AsmContext *asm_context);
//void memory_write(AsmContext *asm_context, uint32_t address, uint8_t data, int line);
//void memory_write_inc(AsmContext *asm_context, uint8_t data, int line);

#if 0
uint8_t memory_read_m(Memory *memory, uint32_t address);
uint16_t memory_read16_m(Memory *memory, uint32_t address);
uint32_t memory_read32_m(Memory *memory, uint32_t address);
void memory_write_m(Memory *memory, uint32_t address, uint8_t data);
void memory_write16_m(Memory *memory, uint32_t address, uint16_t data);
void memory_write32_m(Memory *memory, uint32_t address, uint32_t data);
#endif

#endif

