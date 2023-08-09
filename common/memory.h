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

typedef struct _memory
{
  MemoryPage *pages;
  uint32_t low_address;
  uint32_t high_address;
  uint32_t entry_point;
  int endian;
  uint32_t size;
} Memory;

struct _asm_context;
typedef struct _asm_context AsmContext;

void memory_init(Memory *memory, uint32_t size);
void memory_free(Memory *memory);
void memory_clear(Memory *memory);
int memory_in_use(Memory *memory, uint32_t address);
int memory_get_page_address_min(Memory *memory, uint32_t address);
int memory_get_page_address_max(Memory *memory, uint32_t address);
int memory_page_size(Memory *memory);
uint8_t memory_read(AsmContext *asm_context, uint32_t address);
uint8_t memory_read_inc(AsmContext *asm_context);
void memory_write(AsmContext *asm_context, uint32_t address, uint8_t data, int line);
void memory_write_inc(AsmContext *asm_context, uint8_t data, int line);
int memory_debug_line(AsmContext *asm_context, uint32_t address);
void memory_debug_line_set(AsmContext *asm_context, uint32_t address, int value);
int memory_debug_line_m(Memory *memory, uint32_t address);
void memory_debug_line_set_m(Memory *memory, uint32_t address, int value);
void memory_dump(Memory *memory);

uint8_t memory_read_m(Memory *memory, uint32_t address);
uint16_t memory_read16_m(Memory *memory, uint32_t address);
uint32_t memory_read32_m(Memory *memory, uint32_t address);
void memory_write_m(Memory *memory, uint32_t address, uint8_t data);
void memory_write16_m(Memory *memory, uint32_t address, uint16_t data);
void memory_write32_m(Memory *memory, uint32_t address, uint32_t data);

#endif

