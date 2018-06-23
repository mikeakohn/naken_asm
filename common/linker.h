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

#include "imports.h"

struct _linker
{
  struct _imports *imports;
};

int linker_add_file(struct _linker *linker, const char *filename);
void linker_free(struct _linker *linker);

//int symbols_add_to_unfound(struct _symbols *symbols, const char *name);
//int symbols_print_unfound(struct _symbols *symbols);

#endif

