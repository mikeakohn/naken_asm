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

typedef struct _imports
{
  struct _imports *next;
  int type;
  int size;
  uint8_t code[];
} Imports;

typedef struct _linker
{
  Imports *imports;
  uint8_t *symbol_list_buffer;
  uint32_t symbol_list_buffer_size;
  uint32_t symbol_list_buffer_end;
} Linker;

typedef struct _symbol_list
{
  Imports *imports;
  char name[];
} SymbolList;

int linker_add_file(Linker *linker, const char *filename);

int linker_search_code_from_symbol(
  Linker *linker,
  const char *symbol);

uint8_t *linker_get_code_from_symbol(
  Linker *linker,
  Imports **imports,
  const char *symbol,
  uint32_t *function_offset,
  uint32_t *function_size,
  uint8_t **obj_file,
  uint32_t *obj_size);

const char *linker_find_name_from_offset(
  Linker *linker,
  uint32_t offset);

int linker_get_symbol_count(Linker *linker);
const char *linker_get_symbol_at_index(Linker *linker, int index);
void linker_print_symbol_list(Linker *linker);
void linker_free(Linker *linker);

#endif

