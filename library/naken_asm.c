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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "common/UtilContext.h"
#include "common/util_disasm.h"
#include "fileio/file.h"
#include "naken_asm.h"

#ifdef __cplusplus
extern "C"
{
#endif

// Assembler functions.

void *naken_asm_create()
{
  return NULL;
}

void naken_asm_destroy(void *context)
{
}

int naken_asm_assemble(void *context, const char *source)
{
  return -1;
}

void naken_asm_set_pass_2(void *context)
{
}

int naken_asm_write(void *context, const char *filename)
{
  return -1;
}

// Disassembler functions.

void *naken_util_create()
{
  UtilContext *util_context = (UtilContext *)malloc(sizeof(UtilContext));
  util_init(util_context);
  return util_context;
}

void naken_util_destroy(void *context)
{
  free(context);
}

int naken_util_set_cpu_type(void *context, const char *name)
{
  return util_set_cpu_by_name(context, name);
}

int naken_util_open(void *context, const char *filename)
{
  int file_type = FILE_TYPE_AUTO;
  char cpu_name[32];
  int start_address = 0;

  memset(cpu_name, 0, sizeof(cpu_name));

  return file_read(
    filename,
    context,
    &file_type,
    cpu_name,
    start_address);
}

int naken_util_disasm(void *context, const char *range)
{
  util_disasm(context, range);
  return 0;
}

int naken_util_disasm_range(void *context, uint32_t start, uint32_t end)
{
  util_disasm_range(context, start, end);
  return 0;
}

#if 0
int naken_util_disasm(
  void *context,
  uint32_t address,
  char *code,
  int *cycles_min,
  int *cycles_max)
{
  //UtilContext *util_context = (UtilContext *)context;

  //if (util_context->disasm == NULL) { return -1; }

  return -1;
}
#endif

#ifdef __cplusplus
}
#endif

