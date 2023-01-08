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

// PAGE_SIZE is a way for naken_asm to be able to "address" a large
// memory range, but use only the memory needed to hold code and data.
// For example if a microcontroller had 2MB of memory and code is only
// 1k in size, if the page size is only 8k, only 8k of RAM would be
// malloc()'d and it would be marked as starting at the address where
// the code page exists.  I found that 8k made my 2MB Playstation 2 Java
// demo take 24 seconds to assemble.  64k pages drops it to 3.3 seconds.
// a 2MB page drops it to 1.7 seconds but causes naken_asm to be slower
// on start up (make tests takes a lot longer to run).
//#define PAGE_SIZE 8192
#define PAGE_SIZE (64 * 1024)
//#define PAGE_SIZE 2097152

#define ENDIAN_LITTLE 0
#define ENDIAN_BIG 1

#define DL_EMPTY -1
#define DL_DATA -2
#define DL_NO_CG -3

// TODO - Use this instead later
struct _memory_page
{
  uint32_t address;
  uint32_t offset_min,offset_max;
  struct _memory_page *next;
  uint8_t bin[PAGE_SIZE];
  int debug_line[];
};

struct _memory
{
  struct _memory_page *pages;
  uint32_t low_address;
  uint32_t high_address;
  uint32_t entry_point;
  int endian;
  uint32_t size;
  int debug_flag;
};

struct _asm_context;

void memory_init(struct _memory *memory, uint32_t size, int debug_flag);
void memory_free(struct _memory *memory);
void memory_clear(struct _memory *memory);
int memory_in_use(struct _memory *memory, uint32_t address);
int memory_get_page_address_min(struct _memory *memory, uint32_t address);
int memory_get_page_address_max(struct _memory *memory, uint32_t address);
int memory_page_size(struct _memory *memory);
uint8_t memory_read(struct _asm_context *asm_context, uint32_t address);
uint8_t memory_read_inc(struct _asm_context *asm_context);
void memory_write(struct _asm_context *asm_context, uint32_t address, uint8_t data, int line);
void memory_write_inc(struct _asm_context *asm_context, uint8_t data, int line);
int memory_debug_line(struct _asm_context *asm_context, uint32_t address);
void memory_debug_line_set(struct _asm_context *asm_context, uint32_t address, int value);
int memory_debug_line_m(struct _memory *memory, uint32_t address);
void memory_debug_line_set_m(struct _memory *memory, uint32_t address, int value);
void memory_dump(struct _memory *memory);

uint8_t memory_read_m(struct _memory *memory, uint32_t address);
uint16_t memory_read16_m(struct _memory *memory, uint32_t address);
uint32_t memory_read32_m(struct _memory *memory, uint32_t address);
void memory_write_m(struct _memory *memory, uint32_t address, uint8_t data);
void memory_write16_m(struct _memory *memory, uint32_t address, uint16_t data);
void memory_write32_m(struct _memory *memory, uint32_t address, uint32_t data);

#endif

