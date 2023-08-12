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

#ifndef NAKEN_ASM_MEMORY_POOL_H
#define NAKEN_ASM_MEMORY_POOL_H

#include <stdint.h>

/*
  address_heap buffer looks like this:
  struct
  {
    char name[];
    int address;
  };
*/

struct MemoryPool
{
  MemoryPool *next;
  int len;
  int ptr;
  uint8_t buffer[];
};

struct NakenHeap
{
  MemoryPool *memory_pool;
};

MemoryPool *memory_pool_add(NakenHeap *heap, int heap_len);
void memory_pool_free(MemoryPool *memory_pool);

#endif

