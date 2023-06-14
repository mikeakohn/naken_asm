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

#ifndef UTIL_CONTEXT_H
#define UTIL_CONTEXT_H

#include "common/cpu_list.h"
#include "common/memory.h"
#include "common/symbols.h"
#include "simulate/msp430.h"

typedef struct _util_context
{
  struct _memory memory;
  Symbols symbols;
  Simulate *simulate;
  uint32_t flags;
  uint8_t bytes_per_address;
  uint8_t alignment;
  disasm_range_t disasm_range;
} UtilContext;

void util_init(UtilContext *util_context);

// Converts text pass in on the command line to a start / end int.
int util_get_range(
  UtilContext *util_context,
  char *token,
  uint32_t *start,
  uint32_t *end);

int util_set_cpu_by_name(UtilContext *util_context, const char *name);

char *util_get_num(char *token, uint32_t *num);

char *util_get_address(
  UtilContext *util_context,
  char *token,
  uint32_t *address);

void util_print8(UtilContext *util_context, char *token);
void util_print16(UtilContext *util_context, char *token);
void util_print32(UtilContext *util_context, char *token);
void util_write8(UtilContext *util_context, char *token);
void util_write16(UtilContext *util_context, char *token);
void util_write32(UtilContext *util_context, char *token);

#endif

