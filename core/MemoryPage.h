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

#ifndef NAKEN_ASM_MEMORY_PAGE_H
#define NAKEN_ASM_MEMORY_PAGE_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

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

class MemoryPage
{
public:
  MemoryPage(uint32_t address) :
    address    (address),
    offset_min (PAGE_SIZE),
    offset_max (0),
    next       (NULL)
  {
    memset(bin, 0, sizeof(bin));
    memset(debug_line, -1, sizeof(debug_line));

    this->address = (this->address / PAGE_SIZE) * PAGE_SIZE;
  }

  ~MemoryPage()
  {
  }

  void set_data(uint32_t address, uint8_t data)
  {
    uint32_t offset = address - this->address;

    if (offset < offset_min) { offset_min = offset; }
    if (offset > offset_max) { offset_max = offset; }

    bin[offset] = data;
  }

  void set_debug(uint32_t address, int value)
  {
    uint32_t offset = address - this->address;

    if (offset < offset_min) { offset_min = offset; }
    if (offset > offset_max) { offset_max = offset; }

    debug_line[offset] = value;
  }

  void dump()
  {
    printf("-- MemoryPage --\n");
    printf("     address: 0x%08x\n", address);
    printf("  offset_min: 0x%08x\n", offset_min);
    printf("  offset_max: 0x%08x\n", offset_max);
    printf("        next: %p\n", next);
  }

  uint32_t address;
  uint32_t offset_min, offset_max;
  MemoryPage *next;
  uint8_t bin[PAGE_SIZE];

  // debug_line was used to associate a line of code with an address.
  // It's also used to know which memory locations have been written to
  // so the hexfiles only save data for memory locations that are full.
  int debug_line[PAGE_SIZE];
};

#endif

