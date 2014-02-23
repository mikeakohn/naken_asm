/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2014 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "address_list.h"
#include "macros.h"

static struct _address_data *address_list_find(struct _address_list *address_list, char *name)
{
struct _memory_pool *memory_pool = address_list->memory_pool;
int ptr;

  while(memory_pool != NULL)
  {
    ptr = 0;

    while(ptr < memory_pool->ptr)
    {
      struct _address_data *address_data =
        (struct _address_data *)(memory_pool->buffer + ptr);

      if (strcmp(address_data->name, name) == 0)
      {
        return address_data;
      }

      ptr += address_data->len + sizeof(struct _address_data);
    }

    memory_pool = memory_pool->next;
  }

  return NULL;
}

int address_list_init(struct _address_list *address_list)
{
  address_list->memory_pool = NULL;
  address_list->locked = 0;

  return 0;
}

void address_list_free(struct _address_list *address_list)
{
  memory_pool_free(address_list->memory_pool);
  address_list->memory_pool = NULL;
}

int address_list_append(struct _asm_context *asm_context, char *name, int address)
{
int token_len;
struct _address_list *address_list = &asm_context->address_list;
struct _memory_pool *memory_pool = address_list->memory_pool;
int param_count_temp;

  if (address_list->locked == 1) return 0;

  if (address_list_lookup(address_list, name) != -1 ||
      macros_lookup(&asm_context->macros, name, &param_count_temp) != NULL)
  {
    printf("Error: Label '%s' already defined.\n", name);
    return -1;
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
    memory_pool = memory_pool_add((struct _naken_heap *)address_list, ADDRESS_HEAP_SIZE);
  }

  // Find a pool that has enough area at the end to add this address.
  // If none can be found, alloc a new one.
  while(1)
  {
     if (memory_pool->ptr + token_len + sizeof(struct _address_data) < memory_pool->len)
     {
       break;
     }

     if (memory_pool->next == NULL)
     {
       memory_pool->next = memory_pool_add((struct _naken_heap *)address_list, ADDRESS_HEAP_SIZE);
     }

     memory_pool = memory_pool->next;
  }

  // Divide by bytes_per_address (for AVR8 and dsPIC).
  address = address / asm_context->bytes_per_address;

  // Set the new label/address entry.
  struct _address_data *address_data =
    (struct _address_data *)(memory_pool->buffer + memory_pool->ptr);

  memcpy(address_data->name, name, token_len);
  address_data->len = token_len;
  address_data->flag_rw = 0;
  address_data->flag_export = 0;
  address_data->address = address;

  memory_pool->ptr += token_len + sizeof(struct _address_data);

  return 0;
} 

int address_list_set(struct _asm_context *asm_context, char *name, int address)
{
  struct _address_list *address_list = &asm_context->address_list;
  struct _address_data *address_data = NULL;

  address_data = address_list_find(address_list, name);

  if (address_data == NULL)
  {
    if (address_list_append(asm_context, name, address) != 0)
    {
      return -1; 
    }

    address_data = address_list_find(address_list, name);
    address_data->flag_rw = 1;
  }
    else
  if (address_data->flag_rw == 1)
  {
    address_data->address = address;
  }
    else
  {
    return -1;
  }

  return 0;
}

int address_list_export(struct _address_list *address_list, char *name)
{
  struct _address_data *address_data = address_list_find(address_list, name);

  if (address_data == NULL) { return -1; }

  address_data->flag_export = 1;

  return 0;
}

void address_list_lock(struct _address_list *address_list)
{
  address_list->locked = 1;
}

int address_list_lookup(struct _address_list *address_list, char *name)
{
  struct _address_data *address_data = address_list_find(address_list, name);

  if (address_data == NULL) { return -1; }

  return address_data->address;
}

int address_list_iterate(struct _address_list *address_list, struct _address_list_iter *iter)
{
struct _memory_pool *memory_pool = address_list->memory_pool;

  if (iter->end_flag == 1) return -1;
  if (iter->memory_pool == NULL)
  {
    iter->memory_pool = address_list->memory_pool;
    iter->ptr = 0;
  }

  while(memory_pool != NULL)
  {
    if(iter->ptr < memory_pool->ptr)
    {
      struct _address_data * address_data =
        (struct _address_data *)(memory_pool->buffer + iter->ptr);

      iter->address = address_data->address;
      iter->name = address_data->name;
      iter->ptr = iter->ptr + address_data->len + sizeof(struct _address_data);
      iter->count++;

      return 0;
    }

    memory_pool = memory_pool->next;
  }

  iter->end_flag = 1;

  return -1;
}

int address_list_print(struct _address_list *address_list)
{
struct _address_list_iter iter;

  memset(&iter, 0, sizeof(iter));

  printf("%30s ADDRESS\n", "LABEL");

  while(address_list_iterate(address_list, &iter) != -1)
  {
    printf("%30s %08x (%d)\n", iter.name, iter.address, iter.address);
  }

  printf("Total %d.\n\n", iter.count);

  return 0;
}

int address_list_count_symbols(struct _address_list *address_list)
{
struct _memory_pool *memory_pool = address_list->memory_pool;
int ptr;
int count = 0;

  while(memory_pool != NULL)
  {
    ptr = 0;
    while(ptr < memory_pool->ptr)
    {
      struct _address_data *address_data =
        (struct _address_data *)(memory_pool->buffer + ptr);
      ptr += address_data->len + sizeof(struct _address_data);
      count++;
    }

    memory_pool = memory_pool->next;
  }

  return count;
}


