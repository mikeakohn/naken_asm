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

#ifndef _SYMBOLS_H
#define _SYMBOLS_H

#include <stdint.h>

#define SYMBOLS_HEAP_SIZE 32768

struct _symbols_data
{
  uint8_t len;           // length of name[]
  uint8_t flag_rw:1;     // can write to this
  uint8_t flag_export:1; // ELF will export symbol
  uint32_t address;      // address for this name
  char name[];           // null terminated name of label:
};

struct _symbols
{
  struct _memory_pool *memory_pool;
  int locked;
};

struct _symbols_iter
{
  struct _memory_pool *memory_pool;
  char *name;
  int address;
  int ptr;
  int count;
  int end_flag;
};

int symbols_init(struct _symbols *symbols);
void symbols_free(struct _symbols *symbols);
struct _symbols_data *symbols_find(struct _symbols *symbols, char *name);
int symbols_append(struct _symbols *symbols, char *name, int address);
int symbols_set(struct _symbols *symbols, char *name, int address);
int symbols_export(struct _symbols *symbols, char *name);
void symbols_lock(struct _symbols *symbols);
int symbols_lookup(struct _symbols *symbols, char *name);
int symbols_iterate(struct _symbols *symbols, struct _symbols_iter *iter);
int symbols_print(struct _symbols *symbols);
int symbols_count(struct _symbols *symbols);

#endif

