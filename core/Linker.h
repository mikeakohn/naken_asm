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

#ifndef NAKEN_ASM_LINKER_H
#define NAKEN_ASM_LINKER_H

#include <stdint.h>

enum
{
  IMPORT_TYPE_AR,
  IMPORT_TYPE_OBJ,
};

struct Imports
{
  Imports *next;
  int type;
  int size;
  uint8_t code[];
};

class Linker
{
public:
  Linker();
  ~Linker();

  int get_symbol_count();
  int add_file(const char *filename);
  int search_code_from_symbol(const char *symbol);

  uint8_t *get_code_from_symbol(
    Imports **imports,
    const char *symbol,
    uint32_t *function_offset,
    uint32_t *function_size,
    uint8_t **obj_file,
    uint32_t *obj_size);

  const char *find_name_from_offset(uint32_t offset);
  const char *get_symbol_at_index(int index);
  void print_symbol_list();

private:
  int verify_import(Imports *imports);
  Imports *get_from_symbol_list(const char *name);
  void add_to_symbol_list(Imports *imports, const char *name);

  Imports *imports;
  uint8_t *symbol_list_buffer;
  uint32_t symbol_list_buffer_size;
  uint32_t symbol_list_buffer_end;
};

struct SymbolList
{
  Imports *imports;
  char name[];
};

#endif

