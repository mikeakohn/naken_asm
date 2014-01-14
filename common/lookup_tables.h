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

#ifndef _LOOKUP_TABLES_H
#define _LOOKUP_TABLES_H

#define ADDRESS_HEAP_SIZE 32768

struct _address_heap
{
  struct _memory_pool *memory_pool;
  int locked;
};

struct _address_heap_iter
{
  struct _memory_pool *memory_pool;
  unsigned char *name;
  int address;
  int ptr;
  int count;
  int end_flag;
};

int address_heap_init(struct _address_heap *address_heap);
void address_heap_free(struct _address_heap *address_heap);
int address_heap_append(struct _asm_context *asm_context, char *name, int address);
void address_heap_lock(struct _address_heap *address_heap);
int address_heap_lookup(struct _address_heap *address_heap, char *name);
int address_heap_iterate(struct _address_heap *address_heap, struct _address_heap_iter *iter);
int address_heap_print(struct _address_heap *address_heap);
int address_heap_count_symbols(struct _address_heap *address_heap);

#endif

