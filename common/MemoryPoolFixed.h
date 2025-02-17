/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2025 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_MEMORY_POOL_FIXED_H
#define NAKEN_ASM_MEMORY_POOL_FIXED_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <assert.h>

template<typename TYPE>
class MemoryPoolFixed
{
public:
  MemoryPoolFixed<TYPE>() :
    current_pool (nullptr),
    free_entries (nullptr),
    index        (1024),
    length       (1024)
  {
  }

  MemoryPoolFixed<TYPE>(int size) :
    current_pool (nullptr),
    free_entries (nullptr),
    index        (size),
    length       (size)
  {
  }

  ~MemoryPoolFixed<TYPE>()
  {
    while (current_pool != nullptr)
    {
      Pool *last_pool = current_pool;
      current_pool = current_pool->next;
      free(last_pool);
    }
  }

  TYPE *alloc()
  {
    if (free_entries != nullptr)
    {
      TYPE *entry = (TYPE *)free_entries;
      free_entries = free_entries->next;
      return entry;
    }

    if (index == length)
    {
      Pool *pool = (Pool *)malloc(sizeof(TYPE) * length);

      pool->next = current_pool;
      current_pool = pool;
      index = 0;
    }

    return &current_pool->entries[index++];
  }

  void release(TYPE *entry)
  {
    EntryList *entry_list = (EntryList *)entry;

    if (free_entries == nullptr)
    {
      entry_list->next = nullptr;
    }
      else
    {
      entry_list->next = free_entries;
    }

    free_entries = entry_list;
  }

#ifdef UNIT_TEST
  int get_index()          { return index;  }
  int get_length()         { return length; }
  void *get_current_pool() { return current_pool; }
#endif

private:
  struct Pool
  {
    Pool *next;
    TYPE entries[];
  };

  struct EntryList
  {
    EntryList *next;
  };

  Pool *current_pool;
  EntryList *free_entries;

  int index;
  int length;
};

#endif

