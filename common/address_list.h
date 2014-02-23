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

#ifndef _ADDRESS_LIST_H
#define _ADDRESS_LIST_H

#include <stdint.h>

#define ADDRESS_HEAP_SIZE 32768

struct _address_data
{
  uint8_t len;           // length of name[]
  uint8_t flag_rw:1;     // can write to this
  uint8_t flag_export:1; // ELF will export symbol
  uint32_t address;      // address for this name
  char name[];           // null terminated name of label:
};

struct _address_list
{
  struct _memory_pool *memory_pool;
  int locked;
};

struct _address_list_iter
{
  struct _memory_pool *memory_pool;
  char *name;
  int address;
  int ptr;
  int count;
  int end_flag;
};

int address_list_init(struct _address_list *address_heap);
void address_list_free(struct _address_list *address_heap);
struct _address_data *address_list_find(struct _address_list *address_list, char *name);
int address_list_append(struct _address_list *address_list, char *name, int address);
int address_list_set(struct _address_list *address_list, char *name, int address);
int address_list_export(struct _address_list *address_list, char *name);
void address_list_lock(struct _address_list *address_heap);
int address_list_lookup(struct _address_list *address_heap, char *name);
int address_list_iterate(struct _address_list *address_heap, struct _address_list_iter *iter);
int address_list_print(struct _address_list *address_heap);
int address_list_count_symbols(struct _address_list *address_heap);

#endif

