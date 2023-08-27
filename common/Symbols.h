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

struct SymbolsIter
{
  MemoryPool *memory_pool;
  const char *name;
  uint32_t address;
  int ptr;
  int count;
  int end_flag;
  uint32_t scope;
  bool flag_export : 1;
};

class Symbols
{
public:
  Symbols();
  ~Symbols();

  struct Entry
  {
    uint8_t len;             // length of name[]
    bool flag_rw     : 1;    // can write to this
    bool flag_export : 1;    // ELF will export symbol
    uint16_t scope;          // Up to 65535 local scopes.  0 = global.
    uint32_t address;        // address for this name
    char name[];             // null terminated name of label:
  };

  Entry *find(const char *name);
  int append(const char *name, uint32_t address);
  int set(const char *name, uint32_t address);
  int export_symbol(const char *name);
  int lookup(const char *name, uint32_t *address);
  int iterate(SymbolsIter *iter);
  int print(FILE *out);
  int count();
  int export_count();
  int scope_start();
  void scope_reset() { current_scope = 0; }
  void scope_end()   { in_scope = false; }
  void lock()        { locked = true; }
  bool is_locked()   { return locked; }
  void set_debug()   { debug = true; }

private:
  MemoryPool *memory_pool;
  bool locked   : 1;
  bool in_scope : 1;
  bool debug    : 1;
  uint32_t current_scope;
};

#endif

