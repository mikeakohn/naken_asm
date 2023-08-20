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

#include "MemoryPool.h"

#define SYMBOLS_HEAP_SIZE 32768

struct SymbolsData
{
  uint8_t len;             // length of name[]
  bool flag_rw     : 1;    // can write to this
  bool flag_export : 1;    // ELF will export symbol
  uint16_t scope;          // Up to 65535 local scopes.  0 = global.
  uint32_t address;        // address for this name
  char name[];             // null terminated name of label:
};

struct Symbols
{
  MemoryPool *memory_pool;
  bool locked   : 1;
  bool in_scope : 1;
  bool debug    : 1;
  uint32_t current_scope;
};

struct SymbolsIter
{
  MemoryPool *memory_pool;
  char *name;
  uint32_t address;
  int ptr;
  int count;
  int end_flag;
  uint32_t scope;
  bool flag_export : 1;
};

int symbols_init(Symbols *symbols);
void symbols_free(Symbols *symbols);
SymbolsData *symbols_find(Symbols *symbols, const char *name);
int symbols_append(Symbols *symbols, const char *name, uint32_t address);
int symbols_set(Symbols *symbols, const char *name, uint32_t address);
int symbols_export(Symbols *symbols, const char *name);
void symbols_lock(Symbols *symbols);
int symbols_lookup(Symbols *symbols, const char *name, uint32_t *address);
int symbols_iterate(Symbols *symbols, SymbolsIter *iter);
int symbols_print(Symbols *symbols, FILE *out);
int symbols_count(Symbols *symbols);
int symbols_export_count(Symbols *symbols);
int symbols_scope_start(Symbols *symbols);
int symbols_scope_reset(Symbols *symbols);
int symbols_scope_end(Symbols *symbols);

#endif

