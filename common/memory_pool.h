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

typedef struct _naken_heap
{
  struct _memory_pool *memory_pool;
} NakenHeap;

/*
  address_heap buffer looks like this:
  struct
  {
    char name[];
    int address;
  };
*/

/*
  address_heap buffer looks like this:
  struct
  {
    char name[];
    int address;
  };
*/

typedef struct _memory_pool
{
  struct _memory_pool *next;
  int len;
  int ptr;
  unsigned char buffer[];
} MemoryPool;

MemoryPool *memory_pool_add(NakenHeap *heap, int heap_len);
void memory_pool_free(MemoryPool *memory_pool);

#endif

