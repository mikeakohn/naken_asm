/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2012 by Michael Kohn
 *
 */

#ifndef _PARSE_ELF_H
#define _PARSE_ELF_H

#include "memory.h"

int elf_parse(char *filename, struct _memory *memory);

#endif

