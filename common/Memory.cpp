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
#include "common/Memory.h"

Memory::Memory() :
  pages        (NULL),
  low_address  (0xffffffff),
  high_address (0),
  entry_point  (0xfffffff),
  endian       (ENDIAN_LITTLE)
{
}

Memory::~Memory()
{
  MemoryPage *page = pages;

  while (page != NULL)
  {
    MemoryPage *next = page->next;
    delete page;
    page = next;
  }

  pages = NULL;
}

void Memory::clear()
{
  MemoryPage *page = pages;

  while (page != NULL)
  {
    MemoryPage *next = page->next;
    memset(page->bin, 0, PAGE_SIZE);
    page->offset_min = PAGE_SIZE;
    page->offset_max = 0;
    page = next;
  }
}

bool Memory::in_use(uint32_t address)
{
  MemoryPage *page = pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      return true;
    }

    page = page->next;
  }

  return false;
}

uint32_t Memory::get_page_address_min(uint32_t address)
{
  MemoryPage *page = pages;

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

uint32_t Memory::get_page_address_max(uint32_t address)
{
  MemoryPage *page = pages;

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

uint8_t Memory::read8(uint32_t address)
{
  MemoryPage *page = pages;

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

uint16_t Memory::read16(uint32_t address)
{
  if (endian == ENDIAN_LITTLE)
  {
    return read8(address) | (read8(address + 1) << 8);
  }
    else
  {
    return (read8(address) << 8) | (read8(address + 1));
  }
}

uint32_t Memory::read32(uint32_t address)
{
  if (endian == ENDIAN_LITTLE)
  {
    return read8(address) |
          (read8(address + 1) << 8) |
          (read8(address + 2) << 16) |
          (read8(address + 3) << 24);
  }
    else
  {
    return (read8(address) << 24) |
           (read8(address + 1) << 16) |
           (read8(address + 2) << 8) |
           (read8(address + 3));
  }
}

void Memory::write8(uint32_t address, uint8_t data)
{
  if (pages == NULL)
  {
    pages = new MemoryPage(address);
  }

  MemoryPage *page = pages;

  while (page != NULL)
  {
    if (address >= page->address &&
        address < (uint64_t)page->address + PAGE_SIZE)
    {
      break;
    }

    if (page->next == NULL)
    {
      page->next = new MemoryPage(address);
    }

    page = page->next;
  }

  if (low_address  > address) { low_address  = address; }
  if (high_address < address) { high_address = address; }

  page->set_data(address, data);
}

void Memory::write16(uint32_t address, uint16_t data)
{
  if (endian == ENDIAN_LITTLE)
  {
    write8(address + 0, data & 0xff);
    write8(address + 1, data >> 8);
  }
    else
  {
    write8(address + 0, data >> 8);
    write8(address + 1, data & 0xff);
  }
}

void Memory::write32(uint32_t address, uint32_t data)
{
  if (endian == ENDIAN_LITTLE)
  {
    write8(address + 0, data & 0xff);
    write8(address + 1, (data >> 8) & 0xff);
    write8(address + 2, (data >> 16) & 0xff);
    write8(address + 3, (data >> 24) & 0xff);
  }
    else
  {
    write8(address + 0, (data >> 24) & 0xff);
    write8(address + 1, (data >> 16) & 0xff);
    write8(address + 2, (data >> 8) & 0xff);
    write8(address + 3, data & 0xff);
  }
}

int Memory::read_debug(uint32_t address)
{
  MemoryPage *page = pages;

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

void Memory::write_debug(uint32_t address, int line)
{
  if (pages == NULL)
  {
    pages = new MemoryPage(address);
  }

  MemoryPage *page = pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      break;
    }

    if (page->next == NULL)
    {
      page->next = new MemoryPage(address);
    }

    page = page->next;
  }

  page->set_debug(address, line);
}

void Memory::write(uint32_t address, uint8_t data, int line)
{
  if (pages == NULL)
  {
    pages = new MemoryPage(address);
  }

  MemoryPage *page = pages;

  while (page != NULL)
  {
    if (address >= page->address && address < page->address + PAGE_SIZE)
    {
      break;
    }

    if (page->next == NULL)
    {
      page->next = new MemoryPage(address);
    }

    page = page->next;
  }

  if (low_address  > address) { low_address  = address; }
  if (high_address < address) { high_address = address; }

  page->set_data(address, data);
  page->set_debug(address, line);
}

#if 0
uint8_t memory_read_m(Memory *memory, uint32_t address)
{
  return memory->read8(address);
}

void memory_write_m(Memory *memory, uint32_t address, uint8_t data)
{
  memory->write8(address, data);
}
#endif

void Memory::dump()
{
  MemoryPage *page = pages;

  printf("------ memory dump (debug) ---------\n");
  printf(" low_address: 0x%08x\n", low_address);
  printf("high_address: 0x%08x\n", high_address);
  printf("      endian: %d\n", endian);

  while (page != NULL)
  {
    printf("  page: %p next=%p address=0x%08x offset_min=%d offset_max=%d\n",
      page,
      page->next,
      page->address,
      page->offset_min,
      page->offset_max);

    page = page->next;
  }
}

