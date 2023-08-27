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
#include <stdint.h>

#include "common/MemoryPool.h"
#include "common/Symbols.h"

Symbols::Symbols() :
  memory_pool   (NULL),
  locked        (false),
  in_scope      (false),
  debug         (false),
  current_scope (0)
{
}

Symbols::~Symbols()
{
  memory_pool_free(memory_pool);
}

Symbols::Entry *Symbols::find(const char *name)
{
  MemoryPool *memory_pool = this->memory_pool;
  int ptr;

  // Check local scope.
  if (in_scope)
  {
    while (memory_pool != NULL)
    {
      ptr = 0;

      while (ptr < memory_pool->ptr)
      {
        Entry *entry = (Entry *)(memory_pool->buffer + ptr);

        if (current_scope == entry->scope &&
            strcmp(entry->name, name) == 0)
        {
          return entry;
        }

        ptr += entry->len + sizeof(Entry);
      }

      memory_pool = memory_pool->next;
    }

    memory_pool = this->memory_pool;
  }

  // Check global scope.
  while (memory_pool != NULL)
  {
    ptr = 0;

    while (ptr < memory_pool->ptr)
    {
      Entry *entry = (Entry *)(memory_pool->buffer + ptr);

      if (entry->scope == 0 && strcmp(entry->name, name) == 0)
      {
        return entry;
      }

      ptr += entry->len + sizeof(Entry);
    }

    memory_pool = memory_pool->next;
  }

  return NULL;
}

int Symbols::append(const char *name, uint32_t address)
{
  int token_len;
  MemoryPool *memory_pool = this->memory_pool;
  Entry *entry;

#ifdef DEBUG
//printf("Symbols::append(%s, %d);\n", name, address);
#endif

  if (locked) { return 0; }

  entry = find(name);

  if (entry != NULL)
  {
    // For unit test.  Probably a better way to do this.
    if (debug)
    {
      entry->address = address;
      return 0;
    }

    if (in_scope == false || entry->scope == current_scope)
    {
      printf("Error: Label '%s' already defined.\n", name);
      return -1;
    }
  }

  token_len = strlen(name) + 1;

  // Check if size of new label is bigger than 255.
  if (token_len > 255)
  {
    printf("Error: Label '%s' is too big.\n", name);
    return -1;
  }

  // If there is no pool, add one.
  if (memory_pool == NULL)
  {
    memory_pool = memory_pool_add((NakenHeap *)this, SYMBOLS_HEAP_SIZE);
  }

  // Find a pool that has enough area at the end to add this address.
  // If none can be found, alloc a new one.
  while (true)
  {
     if (memory_pool->ptr + token_len + (int)sizeof(Entry) < memory_pool->len)
     {
       break;
     }

     if (memory_pool->next == NULL)
     {
       memory_pool->next = memory_pool_add((NakenHeap *)this, SYMBOLS_HEAP_SIZE);
     }

     memory_pool = memory_pool->next;
  }

  // Divide by bytes_per_address (for AVR8 and dsPIC).
  //address = address / asm_context->bytes_per_address;

  // Set the new label/address entry.
  entry = (Entry *)(memory_pool->buffer + memory_pool->ptr);

  memcpy(entry->name, name, token_len);
  entry->len = token_len;
  entry->flag_rw = false;
  entry->flag_export = false;
  entry->address = address;
  entry->scope = in_scope ? current_scope : 0;

  memory_pool->ptr += token_len + sizeof(Entry);

  return 0;
}

int Symbols::set(const char *name, uint32_t address)
{
  Entry *entry = NULL;

  entry = find(name);

  if (entry == NULL)
  {
    if (append(name, address) != 0)
    {
      return -1;
    }

    entry = find(name);
    entry->scope = 0;
    entry->flag_rw = true;
  }
    else
  if (entry->flag_rw)
  {
    entry->address = address;
  }
    else
  {
    return -1;
  }

  return 0;
}

int Symbols::export_symbol(const char *name)
{
  Entry *entry = find(name);

  if (entry == NULL) { return -1; }

  if (entry->scope != 0)
  {
    printf("Error: Cannot export local variable '%s'\n", name);
    return -1;
  }

  entry->flag_export = true;

  return 0;
}

int Symbols::lookup(const char *name, uint32_t *address)
{
  Entry *entry = find(name);

  if (entry == NULL)
  {
    *address = 0;
    return -1;
  }

  *address = entry->address;

  return 0;
}

int Symbols::iterate(SymbolsIter *iter)
{
  MemoryPool *memory_pool = this->memory_pool;

  if (iter->end_flag == 1) { return -1; }
  if (iter->memory_pool == NULL)
  {
    iter->memory_pool = this->memory_pool;
    iter->ptr = 0;
  }

  while (memory_pool != NULL)
  {
    if (iter->ptr < memory_pool->ptr)
    {
      Entry * entry = (Entry *)(memory_pool->buffer + iter->ptr);

      iter->address = entry->address;
      iter->name = entry->name;
      iter->ptr = iter->ptr + entry->len + sizeof(Entry);
      iter->flag_export = entry->flag_export;
      iter->scope = entry->scope;
      iter->count++;

      return 0;
    }

    memory_pool = memory_pool->next;
  }

  iter->end_flag = 1;

  return -1;
}

int Symbols::print(FILE *out)
{
  SymbolsIter iter;

  memset(&iter, 0, sizeof(iter));

  fprintf(out, "%30s ADDRESS  SCOPE\n", "LABEL");

  while (iterate(&iter) != -1)
  {
    fprintf(out, "%30s %08x %d%s\n",
      iter.name,
      iter.address,
      iter.scope,
      iter.flag_export ? " EXPORTED" : "");
  }

  fprintf(out, " -> Total symbols: %d\n\n", iter.count);

  return 0;
}

int Symbols::count()
{
  MemoryPool *memory_pool = this->memory_pool;
  int ptr;
  int count = 0;

  while (memory_pool != NULL)
  {
    ptr = 0;
    while (ptr < memory_pool->ptr)
    {
      Entry *entry = (Entry *)(memory_pool->buffer + ptr);
      ptr += entry->len + sizeof(Entry);
      count++;
    }

    memory_pool = memory_pool->next;
  }

  return count;
}

int Symbols::export_count()
{
  MemoryPool *memory_pool = this->memory_pool;
  int ptr;
  int count = 0;

  while (memory_pool != NULL)
  {
    ptr = 0;
    while (ptr < memory_pool->ptr)
    {
      Entry *entry = (Entry *)(memory_pool->buffer + ptr);
      ptr += entry->len + sizeof(Entry);
      if (entry->flag_export) { count++; }
    }

    memory_pool = memory_pool->next;
  }

  return count;
}

int Symbols::scope_start()
{
  if (in_scope)
  {
    return -1;
  }

  in_scope = true;
  current_scope++;

  return 0;
}

