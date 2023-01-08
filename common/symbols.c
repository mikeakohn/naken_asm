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

#include "common/symbols.h"
#include "common/memory_pool.h"

int symbols_init(struct _symbols *symbols)
{
  symbols->memory_pool = NULL;
  symbols->locked = 0;
  symbols->in_scope = 0;
  symbols->debug = 0;
  symbols->current_scope = 0;

  return 0;
}

void symbols_free(struct _symbols *symbols)
{
  memory_pool_free(symbols->memory_pool);
  symbols->memory_pool = NULL;
}

struct _symbols_data *symbols_find(struct _symbols *symbols, const char *name)
{
  struct _memory_pool *memory_pool = symbols->memory_pool;
  int ptr;

  // Check local scope.
  if (symbols->in_scope != 0)
  {
    while (memory_pool != NULL)
    {
      ptr = 0;

      while (ptr < memory_pool->ptr)
      {
        struct _symbols_data *symbols_data =
          (struct _symbols_data *)(memory_pool->buffer + ptr);

        if (symbols->current_scope == symbols_data->scope &&
            strcmp(symbols_data->name, name) == 0)
        {
          return symbols_data;
        }

        ptr += symbols_data->len + sizeof(struct _symbols_data);
      }

      memory_pool = memory_pool->next;
    }

    memory_pool = symbols->memory_pool;
  }

  // Check global scope.
  while (memory_pool != NULL)
  {
    ptr = 0;

    while (ptr < memory_pool->ptr)
    {
      struct _symbols_data *symbols_data =
        (struct _symbols_data *)(memory_pool->buffer + ptr);

      if (symbols_data->scope == 0 && strcmp(symbols_data->name, name) == 0)
      {
        return symbols_data;
      }

      ptr += symbols_data->len + sizeof(struct _symbols_data);
    }

    memory_pool = memory_pool->next;
  }

  return NULL;
}

int symbols_append(struct _symbols *symbols, const char *name, uint32_t address)
{
  int token_len;
  struct _memory_pool *memory_pool = symbols->memory_pool;
  struct _symbols_data *symbols_data;

#ifdef DEBUG
//printf("symbols_append(%s, %d);\n", name, address);
#endif

  if (symbols->locked == 1) { return 0; }

  symbols_data = symbols_find(symbols, name);

  if (symbols_data != NULL)
  {
    // For unit test.  Probably a better way to do this.
    if (symbols->debug == 1)
    {
      symbols_data->address = address;
      return 0;
    }

    if (symbols->in_scope == 0 || symbols_data->scope == symbols->current_scope)
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

  // If we have no pool, add one.
  if (memory_pool == NULL)
  {
    memory_pool = memory_pool_add((struct _naken_heap *)symbols, SYMBOLS_HEAP_SIZE);
  }

  // Find a pool that has enough area at the end to add this address.
  // If none can be found, alloc a new one.
  while (1)
  {
     if (memory_pool->ptr + token_len + sizeof(struct _symbols_data) < memory_pool->len)
     {
       break;
     }

     if (memory_pool->next == NULL)
     {
       memory_pool->next = memory_pool_add((struct _naken_heap *)symbols, SYMBOLS_HEAP_SIZE);
     }

     memory_pool = memory_pool->next;
  }

  // Divide by bytes_per_address (for AVR8 and dsPIC).
  //address = address / asm_context->bytes_per_address;

  // Set the new label/address entry.
  symbols_data =
    (struct _symbols_data *)(memory_pool->buffer + memory_pool->ptr);

  memcpy(symbols_data->name, name, token_len);
  symbols_data->len = token_len;
  symbols_data->flag_rw = 0;
  symbols_data->flag_export = 0;
  symbols_data->address = address;
  symbols_data->scope = symbols->in_scope == 0 ? 0 : symbols->current_scope;

  memory_pool->ptr += token_len + sizeof(struct _symbols_data);

  return 0;
} 

int symbols_set(struct _symbols *symbols, char *name, uint32_t address)
{
  struct _symbols_data *symbols_data = NULL;

  symbols_data = symbols_find(symbols, name);

  if (symbols_data == NULL)
  {
    if (symbols_append(symbols, name, address) != 0)
    {
      return -1; 
    }

    symbols_data = symbols_find(symbols, name);
    symbols_data->scope = 0;
    symbols_data->flag_rw = 1;
  }
    else
  if (symbols_data->flag_rw == 1)
  {
    symbols_data->address = address;
  }
    else
  {
    return -1;
  }

  return 0;
}

int symbols_export(struct _symbols *symbols, char *name)
{
  struct _symbols_data *symbols_data = symbols_find(symbols, name);

  if (symbols_data == NULL) { return -1; }

  if (symbols_data->scope != 0)
  {
    printf("Error: Cannot export local variable '%s'\n", name);
    return -1;
  }

  symbols_data->flag_export = 1;

  return 0;
}

void symbols_lock(struct _symbols *symbols)
{
  symbols->locked = 1;
}

int symbols_lookup(struct _symbols *symbols, const char *name, uint32_t *address)
{
  struct _symbols_data *symbols_data = symbols_find(symbols, name);

  if (symbols_data == NULL)
  {
    *address = 0;
    return -1;
  }

  *address = symbols_data->address;

  return 0;
}

int symbols_iterate(struct _symbols *symbols, struct _symbols_iter *iter)
{
  struct _memory_pool *memory_pool = symbols->memory_pool;

  if (iter->end_flag == 1) { return -1; }
  if (iter->memory_pool == NULL)
  {
    iter->memory_pool = symbols->memory_pool;
    iter->ptr = 0;
  }

  while (memory_pool != NULL)
  {
    if (iter->ptr < memory_pool->ptr)
    {
      struct _symbols_data * symbols_data =
        (struct _symbols_data *)(memory_pool->buffer + iter->ptr);

      iter->address = symbols_data->address;
      iter->name = symbols_data->name;
      iter->ptr = iter->ptr + symbols_data->len + sizeof(struct _symbols_data);
      iter->flag_export = symbols_data->flag_export;
      iter->scope = symbols_data->scope;
      iter->count++;

      return 0;
    }

    memory_pool = memory_pool->next;
  }

  iter->end_flag = 1;

  return -1;
}

int symbols_print(struct _symbols *symbols, FILE *out)
{
  struct _symbols_iter iter;

  memset(&iter, 0, sizeof(iter));

  fprintf(out, "%30s ADDRESS  SCOPE\n", "LABEL");

  while (symbols_iterate(symbols, &iter) != -1)
  {
    fprintf(out, "%30s %08x %d%s\n", iter.name, iter.address, iter.scope, iter.flag_export == 1 ? " EXPORTED" : "");
  }

  fprintf(out, " -> Total symbols: %d\n\n", iter.count);

  return 0;
}

int symbols_count(struct _symbols *symbols)
{
  struct _memory_pool *memory_pool = symbols->memory_pool;
  int ptr;
  int count = 0;

  while (memory_pool != NULL)
  {
    ptr = 0;
    while (ptr < memory_pool->ptr)
    {
      struct _symbols_data *symbols_data =
        (struct _symbols_data *)(memory_pool->buffer + ptr);
      ptr += symbols_data->len + sizeof(struct _symbols_data);
      count++;
    }

    memory_pool = memory_pool->next;
  }

  return count;
}

int symbols_export_count(struct _symbols *symbols)
{
  struct _memory_pool *memory_pool = symbols->memory_pool;
  int ptr;
  int count = 0;

  while (memory_pool != NULL)
  {
    ptr = 0;
    while (ptr < memory_pool->ptr)
    {
      struct _symbols_data *symbols_data =
        (struct _symbols_data *)(memory_pool->buffer + ptr);
      ptr += symbols_data->len + sizeof(struct _symbols_data);
      if (symbols_data->flag_export == 1) { count++; }
    }

    memory_pool = memory_pool->next;
  }

  return count;
}

int symbols_scope_start(struct _symbols *symbols)
{
  if (symbols->in_scope == 1)
  {
    return -1;
  }

  symbols->in_scope = 1;
  symbols->current_scope++;

  return 0;
}

int symbols_scope_reset(struct _symbols *symbols)
{
  symbols->current_scope = 0;

  return 0;
}

int symbols_scope_end(struct _symbols *symbols)
{
  symbols->in_scope = 0;

  return 0;
}

#if 0
int symbols_add_to_unfound(struct _symbols *symbols, const char *name)
{
  int ptr;

  if (symbols->need_unfound_symbols == 0) { return 0; }

  struct _unfound_list *unfound_list = symbols->unfound_list;

  // Allocate buffer if needed.
  if (unfound_list->buffer == NULL)
  {
    unfound_list->size = 0x10000;
    unfound_list->buffer = (char *)malloc(unfound_list->size);
    unfound_list->buffer[0] = 0;
  }

  // Search to see if symbol is already in the list.
  char *buffer = unfound_list->buffer;
  ptr = 0;

  while (buffer[ptr] != 0)
  {
    if (strcmp(buffer + ptr, name) == 0) { return 0; }
    ptr += strlen(buffer + ptr) + 1;
  }

  // Check if buffer needs to be realloac'd.
  int len = strlen(name);

  if (ptr + len + 2 >= unfound_list->size)
  {
    unfound_list->size += 0x10000;
    unfound_list->buffer = (char *)realloc(unfound_list->buffer, unfound_list->size);
    buffer = unfound_list->buffer;
  }

  strcpy(buffer + ptr, name);

  ptr += len + 1;
  buffer[ptr] = 0;

  return 0;
}

int symbols_print_unfound(struct _symbols *symbols)
{
  int ptr, count;
  char *buffer = symbols->unfound_list->buffer;

  if (buffer == NULL) { return 0; }

  printf("Unfound list:\n");

  ptr = 0;
  count = 0;

  while (buffer[ptr] != 0)
  {
    printf("  %d) %s\n", count, buffer + ptr);
    ptr += strlen(buffer + ptr) + 1;
    count++;
  }

  return 0;
}
#endif

