/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "assembler.h"
#include "lookup_tables.h"
#include "macros.h"

int address_heap_init(struct _address_heap *address_heap)
{
  address_heap->memory_pool=NULL;
  address_heap->locked=0;

  return 0;
}

void address_heap_free(struct _address_heap *address_heap)
{
  free_pools(address_heap->memory_pool);
  address_heap->memory_pool=NULL;
}

int address_heap_append(struct _asm_context *asm_context, char *name, int address)
{
int token_len;
struct _address_heap *address_heap=&asm_context->address_heap;
struct _memory_pool *memory_pool=address_heap->memory_pool;

  if (address_heap->locked==1) return 0;

  int param_count_temp;
  if (address_heap_lookup(address_heap, name)!=-1 || defines_heap_lookup(&asm_context->defines_heap, name, &param_count_temp)!=NULL)
  {
    printf("Error: Label '%s' already defined.\n", name);
    return -1;
  }

  token_len=strlen(name)+1;

  if (token_len+4>ADDRESS_HEAP_SIZE)
  {
    printf("Error: Label '%s' is too big.\n", name);
    return -1;
  }

  if (memory_pool==NULL)
  {
    memory_pool=add_pool((struct _naken_heap *)address_heap, ADDRESS_HEAP_SIZE);
  }

  while(1)
  {
     if (memory_pool->ptr+token_len+sizeof(int)<memory_pool->len)
     {
       break;
     }

     if (memory_pool->next==NULL)
     {
       memory_pool->next=add_pool((struct _naken_heap *)address_heap, ADDRESS_HEAP_SIZE);
     }

     memory_pool=memory_pool->next;
  }

  memcpy(memory_pool->buffer+memory_pool->ptr, name, token_len);
  memcpy(memory_pool->buffer+memory_pool->ptr+token_len, &address, sizeof(int));
  memory_pool->ptr+=token_len+sizeof(int);

  return 0;
} 

void address_heap_lock(struct _address_heap *address_heap)
{
  address_heap->locked=1;
}

int address_heap_lookup(struct _address_heap *address_heap, char *name)
{
struct _memory_pool *memory_pool=address_heap->memory_pool;
int token_len;
int address;
int ptr;

  while(memory_pool!=NULL)
  {
    ptr=0;
    while(ptr<memory_pool->ptr)
    {
      token_len=strlen((char *)memory_pool->buffer+ptr)+1;
      if (strcmp((char *)memory_pool->buffer+ptr, name)==0)
      {
        memcpy(&address, memory_pool->buffer+ptr+token_len, sizeof(int));
        return address;
      }
      ptr+=token_len+sizeof(int);
    }

    memory_pool=memory_pool->next;
  }

  return -1;
}

int address_heap_iterate(struct _address_heap *address_heap, struct _address_heap_iter *iter)
{
struct _memory_pool *memory_pool=address_heap->memory_pool;
int token_len;

  if (iter->end_flag==1) return -1;
  if (iter->memory_pool==NULL)
  {
    iter->memory_pool=address_heap->memory_pool;
    iter->ptr=0;
  }

  while(memory_pool!=NULL)
  {
    if(iter->ptr<memory_pool->ptr)
    {
      token_len=strlen((char *)memory_pool->buffer+iter->ptr)+1;
      memcpy(&iter->address, memory_pool->buffer+iter->ptr+token_len, sizeof(int));
      iter->name=memory_pool->buffer+iter->ptr;
      iter->ptr=iter->ptr+token_len+sizeof(int);
      iter->count++;
      return 0;
    }

    memory_pool=memory_pool->next;
  }

  iter->end_flag=1;

  return -1;
}

int address_heap_print(struct _address_heap *address_heap)
{
struct _address_heap_iter iter;

  memset(&iter, 0, sizeof(iter));

  printf("%30s ADDRESS\n", "LABEL");

  while(address_heap_iterate(address_heap, &iter)!=-1)
  {
    printf("%30s %08x (%d)\n", iter.name, iter.address, iter.address);
  }

  printf("Total %d.\n\n", iter.count);

  return 0;
}

int address_heap_count_symbols(struct _address_heap *address_heap)
{
struct _memory_pool *memory_pool=address_heap->memory_pool;
int token_len;
int ptr;
int count=0;

  while(memory_pool!=NULL)
  {
    ptr=0;
    while(ptr<memory_pool->ptr)
    {
      token_len=strlen((char *)memory_pool->buffer+ptr)+1;
      ptr=ptr+token_len+sizeof(int);
      count++;
    }

    memory_pool=memory_pool->next;
  }

  return count;
}

