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

#include "imports.h"
#include "imports_ar.h"
#include "imports_obj.h"

#if 0
struct _imports *imports_add_file(const char *file_name)
{
  return NULL;
}
#endif

int imports_verify(struct _imports *imports)
{
  if (imports->type == IMPORT_TYPE_AR)
  {
    if (imports_ar_verify(imports->code, imports->size) != 0)
    {
      return -1;
    }
  }
    else
  if (imports->type == IMPORT_TYPE_AR)
  {
    if (imports_obj_verify(imports->code, imports->size) != 0)
    {
      return -1;
    }
  }

  return 0;
}

struct _imports *imports_find_code_for_symbol(
  const char *symbol,
  uint32_t *code,
  uint32_t *size)
{
  return NULL;
}

const char *imports_find_name_from_offset(
  struct _imports *imports,
  uint32_t offset)
{
  return NULL;
}

void imports_free(struct _imports *imports)
{
}

