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

#ifndef _LINKER_H
#define _LINKER_H

#include <stdint.h>

enum
{
  LINKER_TYPE_AR,
  LINKER_TYPE_OBJ,
};

struct _linker
{
  struct _linker *next;
  int type;
  int size;
  uint8_t code[];
};

struct _linker *linker_add_file(const char *file_name);
int linker_verify(struct _linker *linker);
struct _linker *linker_find_code_for_symbol(const char *symbol, uint32_t *code, uint32_t *size);
const char *linker_find_name_from_offset(struct _linker *linker, uint32_t offset);
void linker_free(struct _linker *linker);

#endif

