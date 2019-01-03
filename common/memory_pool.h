/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_MEMORY_POOL_H
#define NAKEN_ASM_MEMORY_POOL_H

struct _naken_heap
{
  struct _memory_pool *memory_pool;
};

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

struct _memory_pool
{
  struct _memory_pool *next;
  int len;
  int ptr;
  unsigned char buffer[];
};

struct _memory_pool *memory_pool_add(struct _naken_heap *heap, int heap_len);
void memory_pool_free(struct _memory_pool *memory_pool);

#endif

