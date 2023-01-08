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

#ifndef NAKEN_ASM_SYMBOLS_H
#define NAKEN_ASM_SYMBOLS_H

#include <stdint.h>

#define SYMBOLS_HEAP_SIZE 32768

struct _symbols_data
{
  uint8_t len;             // length of name[]
  uint8_t flag_rw : 1;     // can write to this
  uint8_t flag_export : 1; // ELF will export symbol
  uint16_t scope;          // Up to 65535 local scopes.  0 = global.
  uint32_t address;        // address for this name
  char name[];             // null terminated name of label:
};

// FIXME: Might be nicer to switch this to a memory pool.  Currently
// this is just for the linker to figure out what symbols need to be
// searched for in the .o and .a files.
struct _unfound_list
{
  char *buffer;
  uint32_t size;
};

struct _symbols
{
  struct _memory_pool *memory_pool;
  struct _unfound_list *unfound_list;
  uint8_t locked : 1;
  uint8_t in_scope : 1;
  uint8_t debug : 1;
  uint32_t current_scope;
};

struct _symbols_iter
{
  struct _memory_pool *memory_pool;
  char *name;
  uint32_t address;
  int ptr;
  int count;
  int end_flag;
  uint32_t scope;
  uint8_t flag_export : 1;
};

int symbols_init(struct _symbols *symbols);
void symbols_free(struct _symbols *symbols);
struct _symbols_data *symbols_find(struct _symbols *symbols, const char *name);
int symbols_append(struct _symbols *symbols, const char *name, uint32_t address);
int symbols_set(struct _symbols *symbols, char *name, uint32_t address);
int symbols_export(struct _symbols *symbols, char *name);
void symbols_lock(struct _symbols *symbols);
int symbols_lookup(struct _symbols *symbols, const char *name, uint32_t *address);
int symbols_iterate(struct _symbols *symbols, struct _symbols_iter *iter);
int symbols_print(struct _symbols *symbols, FILE *out);
int symbols_count(struct _symbols *symbols);
int symbols_export_count(struct _symbols *symbols);
int symbols_scope_start(struct _symbols *symbols);
int symbols_scope_reset(struct _symbols *symbols);
int symbols_scope_end(struct _symbols *symbols);

#endif

