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

#include <stdio.h>
#include <stdlib.h>

#include "linker.h"
#include "imports_ar.h"
#include "imports_obj.h"

struct _linker *imports_add_file(const char *file_name)
{
  return NULL;
}

int imports_verify(struct _linker *linker)
{
  if (linker->type == IMPORT_TYPE_AR)
  {
    if (imports_ar_verify(linker->code, linker->size) != 0)
    {
      return -1;
    }
  }
    else
  if (linker->type == IMPORT_TYPE_AR)
  {
    if (imports_obj_verify(linker->code, linker->size) != 0)
    {
      return -1;
    }
  }

  return 0;
}

struct _linker *imports_find_code_for_symbol(
  const char *symbol,
  uint32_t *code,
  uint32_t *size)
{
  return NULL;
}

const char *imports_find_name_from_offset(
  struct _linker *linker,
  uint32_t offset)
{
  return NULL;
}

void imports_free(struct _linker *linker)
{
}

