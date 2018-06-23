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
#include "linker_ar.h"
#include "linker_obj.h"

struct _linker *linker_add_file(const char *file_name)
{
  return NULL;
}

int linker_verify(struct _linker *linker)
{
  if (linker->type == LINKER_TYPE_AR)
  {
    if (linker_ar_verify(linker->code, linker->size) != 0)
    {
      return -1;
    }
  }
    else
  if (linker->type == LINKER_TYPE_AR)
  {
    if (linker_obj_verify(linker->code, linker->size) != 0)
    {
      return -1;
    }
  }

  return 0;
}

struct _linker *linker_find_code_for_symbol(
  const char *symbol,
  uint32_t *code,
  uint32_t *size)
{
  return NULL;
}

const char *linker_find_name_from_offset(
  struct _linker *linker,
  uint32_t offset)
{
  return NULL;
}

void linker_free(struct _linker *linker)
{
}

