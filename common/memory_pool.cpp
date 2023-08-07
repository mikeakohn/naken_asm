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

#include "common/memory_pool.h"

MemoryPool *memory_pool_add(NakenHeap *heap, int heap_len)
{
  MemoryPool *curr_pool;
  MemoryPool *memory_pool = (MemoryPool *)malloc(heap_len + sizeof(MemoryPool));

  memory_pool->len = heap_len;
  memory_pool->ptr = 0;
  memory_pool->next = NULL;

  if (heap->memory_pool == NULL)
  {
    heap->memory_pool = memory_pool;
  }
    else
  {
    curr_pool = heap->memory_pool;
    while (curr_pool->next != NULL) { curr_pool = curr_pool->next; }
    curr_pool->next = memory_pool;
  }

#ifdef DEBUG
printf("add_pool pool=%p\n", memory_pool);
#endif

  return memory_pool;
}

void memory_pool_free(MemoryPool *memory_pool)
{
  MemoryPool *curr_pool;
  MemoryPool *last_pool;

  curr_pool = memory_pool;

  while (curr_pool != NULL)
  {
#ifdef DEBUG
printf("memory_pool_free pool=%p\n", curr_pool);
#endif
    last_pool = curr_pool;
    curr_pool = curr_pool->next;
    free(last_pool);
  }
}

