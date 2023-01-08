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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "common/assembler.h"
#include "common/memory.h"

void memory_init(struct _memory *memory, uint32_t size, int debug_flag)
{
  memory->low_address = size - 1;
  memory->high_address = 0;
  memory->entry_point = 0xffffffff;
  memory->endian = ENDIAN_LITTLE;
  memory->size = size;
  memory->debug_flag = debug_flag;
  memory->pages = NULL;
  //memset(memory->debug_line, 0xff, sizeof(int) * memory->size);
}

void memory_free(struct _memory *memory)
{
  struct _memory_page *page;
  struct _memory_page *next;

  page = memory->pages;
  while (page != NULL)
  {
    next = page->next;
    free(page);
    page = next;
  }

  memory->pages = NULL;
}

void memory_clear(struct _memory *memory)
{
  struct _memory_page *page;
  struct _memory_page *next;

  page = memory->pages;
  while (page != NULL)
  {
    next = page->next;
    memset(page->bin, 0, PAGE_SIZE);
    page->offset_min = PAGE_SIZE;
    page->offset_max = 0;
    page = next;
  }
}

int memory_in_use(struct _memory *memory, uint32_t address)
{
  struct _memory_page *page;

  page = memory->pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      return 1;
    }

    page = page->next;
  }

  return 0;
}

int memory_get_page_address_min(struct _memory *memory, uint32_t address)
{
  struct _memory_page *page;

  page = memory->pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      return page->address + page->offset_min;
    }

    page = page->next;
  }

  print_error_internal(NULL, __FILE__, __LINE__);

  return 0;
}

int memory_get_page_address_max(struct _memory *memory, uint32_t address)
{
  struct _memory_page *page;

  page = memory->pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      return page->address + page->offset_max;
    }

    page = page->next;
  }

  print_error_internal(NULL, __FILE__, __LINE__);

  return 0;
}

int memory_page_size(struct _memory *memory)
{
  return PAGE_SIZE;
}

static uint8_t read_byte(struct _memory *memory, uint32_t address)
{
  struct _memory_page *page;

  page = memory->pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      return page->bin[address-page->address];
    }
    page = page->next;
  }

  return 0;
}

static int read_debug(struct _memory *memory, uint32_t address)
{
  struct _memory_page *page;

  if (memory->debug_flag == 0) return -1;

  page = memory->pages;
  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      return page->debug_line[address-page->address];
    }
    page = page->next;
  }

  return -1;
}

static struct _memory_page *alloc_page(struct _memory *memory, uint32_t address)
{
  struct _memory_page *page;

//printf("allocating page %d\n", address);
  page = malloc(sizeof(struct _memory_page) + (memory->debug_flag == 1 ? PAGE_SIZE * sizeof(int) : 0));
  page->address = (address / PAGE_SIZE) * PAGE_SIZE;
  page->offset_min = PAGE_SIZE;
  page->offset_max = 0;
  page->next = 0;

  memset(page->bin, 0, PAGE_SIZE);

  if (memory->debug_flag==1)
  {
    memset(page->debug_line, 0xff, PAGE_SIZE * sizeof(int));
  }

  return page;
}

static void write_byte(struct _memory *memory, uint32_t address, uint8_t data)
{
  struct _memory_page *page;

  if (memory->pages == NULL) { memory->pages = alloc_page(memory, address); }

  page = memory->pages;
  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      break;
    }

    if (page->next == NULL)
    {
      page->next = alloc_page(memory, address);
    }

    page = page->next;
  }

  if (memory->low_address > address) memory->low_address = address;
  if (memory->high_address < address) memory->high_address = address;

  int offset = address-page->address;
  if (page->offset_min > offset) { page->offset_min = offset; }
  if (page->offset_max < offset) { page->offset_max = offset; }

  page->bin[offset] = data;
  //page->debug_line[offset] = 1;
}

static void write_debug(struct _memory *memory, uint32_t address, int data)
{
  struct _memory_page *page;

  if (memory->debug_flag == 0) { return; }
  if (memory->pages == NULL) { memory->pages = alloc_page(memory, address); }

  page = memory->pages;
  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      break;
    }

    if (page->next == NULL)
    {
      page->next = alloc_page(memory, address);
    }

    page=page->next;
  }

  int offset = address-page->address;
  if (page->offset_min > offset) { page->offset_min = offset; }
  if (page->offset_max < offset) { page->offset_max = offset; }

  page->debug_line[offset] = data;
}

uint8_t memory_read(struct _asm_context *asm_context, uint32_t address)
{
  if (address >= asm_context->memory.size)
  {
    printf("Warning: Data read address %d overran %d byte boundary at %s:%d\n", address, asm_context->memory.size, asm_context->tokens.filename, asm_context->tokens.line);
    return 0;
  }

  return read_byte(&asm_context->memory, address);
}

uint8_t memory_read_inc(struct _asm_context *asm_context)
{
  return read_byte(&asm_context->memory, asm_context->address++);
}

void memory_write(struct _asm_context *asm_context, uint32_t address, uint8_t data, int line)
{
  if (address >= asm_context->memory.size)
  {
    printf("Warning: Data write address %d overran %d byte boundary at %s:%d\n", address, asm_context->memory.size, asm_context->tokens.filename, asm_context->tokens.line);
    return;
  }

  write_byte(&asm_context->memory, address, data);
  write_debug(&asm_context->memory, address, line);
}

void memory_write_inc(struct _asm_context *asm_context, uint8_t data, int line)
{
  write_byte(&asm_context->memory, asm_context->address, data);
  write_debug(&asm_context->memory, asm_context->address, line);
  asm_context->address++;
}

int memory_debug_line(struct _asm_context *asm_context, uint32_t address)
{
  return read_debug(&asm_context->memory, address);
}

void memory_debug_line_set(struct _asm_context *asm_context, uint32_t address, int value)
{
  write_debug(&asm_context->memory, address, value);
}

int memory_debug_line_m(struct _memory *memory, uint32_t address)
{
  return read_debug(memory, address);
}

void memory_debug_line_set_m(struct _memory *memory, uint32_t address, int value)
{
  write_debug(memory, address, value);
}

void memory_dump(struct _memory *memory)
{
  struct _memory_page *page = memory->pages;

  printf("------ memory dump (debug) ---------\n");
  printf(" low_address: 0x%08x\n", memory->low_address);
  printf("high_address: 0x%08x\n", memory->high_address);
  printf("      endian: %d\n", memory->endian);
  printf("        size: 0x%x\n", memory->size);
  printf("  debug_flag: %d\n", memory->debug_flag);

  while (page != NULL)
  {
    printf("  page: %p next=%p address=0x%08x offset_min=%d offset_max=%d\n", page, page->next, page->address, page->offset_min, page->offset_max);
    page = page->next;
  }
}

uint8_t memory_read_m(struct _memory *memory, uint32_t address)
{
  return read_byte(memory, address);
}

uint16_t memory_read16_m(struct _memory *memory, uint32_t address)
{
  if (memory->endian == ENDIAN_LITTLE)
  {
    return read_byte(memory, address) |
          (read_byte(memory, address + 1) << 8);
  }
    else
  {
    return (read_byte(memory, address) << 8) |
           (read_byte(memory, address + 1));
  }
}

uint32_t memory_read32_m(struct _memory *memory, uint32_t address)
{
  if (memory->endian == ENDIAN_LITTLE)
  {
    return read_byte(memory, address) |
          (read_byte(memory, address + 1) << 8) |
          (read_byte(memory, address + 2) << 16) |
          (read_byte(memory, address + 3) << 24);
  }
    else
  {
    return (read_byte(memory, address) << 24) |
           (read_byte(memory, address + 1) << 16) |
           (read_byte(memory, address + 2) << 8) |
           (read_byte(memory, address + 3));
  }
}

void memory_write_m(struct _memory *memory, uint32_t address, uint8_t data)
{
  write_byte(memory, address, data);
}

void memory_write16_m(struct _memory *memory, uint32_t address, uint16_t data)
{
  if (memory->endian == ENDIAN_LITTLE)
  {
    write_byte(memory, address + 0, data & 0xff);
    write_byte(memory, address + 1, data >> 8);
  }
    else
  {
    write_byte(memory, address + 0, data >> 8);
    write_byte(memory, address + 1, data & 0xff);
  }
}

void memory_write32_m(struct _memory *memory, uint32_t address, uint32_t data)
{
  if (memory->endian == ENDIAN_LITTLE)
  {
    write_byte(memory, address + 0, data & 0xff);
    write_byte(memory, address + 1, (data >> 8) & 0xff);
    write_byte(memory, address + 2, (data >> 16) & 0xff);
    write_byte(memory, address + 3, (data >> 24) & 0xff);
  }
    else
  {
    write_byte(memory, address + 0, (data >> 24) & 0xff);
    write_byte(memory, address + 1, (data >> 16) & 0xff);
    write_byte(memory, address + 2, (data >> 8) & 0xff);
    write_byte(memory, address + 3, data & 0xff);
  }
}


