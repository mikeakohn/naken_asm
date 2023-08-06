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

#ifndef NAKEN_UTIL_LIBRARY_H
#define NAKEN_UTIL_LIBRARY_H

#include <stdint.h>

#ifdef __cplusplus
extern "C"
{
#endif

void *naken_asm_create();
void naken_asm_destroy(void *context);
int naken_asm_assemble(void *context, const char *source);
void naken_asm_set_pass_2(void *context);
int naken_asm_write(void *context, const char *filename);

void *naken_util_create();
void naken_util_destroy(void *context);
int naken_util_set_cpu_type(void *context, const char *name);
int naken_util_open(void *context, const char *filename);
int naken_util_disasm(void *context, const char *range);
int naken_util_disasm_range(void *context, uint32_t start, uint32_t end);

#if 0
int naken_util_disasm(
  void *context,
  uint32_t address,
  char *code,
  int *cycles_min,
  int *cycles_max);
#endif

#ifdef __cplusplus
}
#endif

#endif

