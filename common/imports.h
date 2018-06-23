/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2018 by Michael Kohn
 *
 */

#ifndef _IMPORTS_H
#define _IMPORTS_H

#include <stdint.h>

enum
{
  IMPORT_TYPE_AR,
  IMPORT_TYPE_OBJ,
};

struct _imports
{
  struct _imports *next;
  int type;
  int size;
  uint8_t code[];
};

struct _imports *imports_add_file(const char *file_name);
int imports_verify(struct _imports *linker);
struct _imports *imports_find_code_for_symbol(const char *symbol, uint32_t *code, uint32_t *size);
const char *imports_find_name_from_offset(struct _imports *linker, uint32_t offset);
void imports_free(struct _imports *linker);

#endif

