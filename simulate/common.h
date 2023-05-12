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

#ifndef NAKEN_ASM_SIMULATE_COMMON_H
#define NAKEN_ASM_SIMULATE_COMMON_H

#include <unistd.h>

#include "common/memory.h"

typedef struct _simulate *(*simulate_init_t)(struct _memory *);
typedef void (*simulate_free_t)(struct _simulate *);
typedef int (*simulate_dumpram_t)(struct _simulate *, int start, int end);
typedef void (*simulate_push_t)(struct _simulate *, uint32_t value);
typedef int (*simulate_set_reg_t)(struct _simulate *, char *reg_string, uint32_t value);
typedef uint32_t (*simulate_get_reg_t)(struct _simulate *, char *reg_string);
typedef void (*simulate_set_pc_t)(struct _simulate *, uint32_t value);
typedef void (*simulate_reset_t)(struct _simulate *);
typedef void (*simulate_dump_registers_t)(struct _simulate *);
typedef int (*simulate_run_t)(struct _simulate *, int max_cycles, int step);

struct _simulate
{
  struct _memory *memory;
  int cycle_count;
  int nested_call_count;
  useconds_t usec;
  int break_point;
  int break_io;
  uint8_t step_mode : 1;
  uint8_t show : 1;
  uint8_t auto_run : 1;

  simulate_init_t simulate_init;
  simulate_free_t simulate_free;
  simulate_dumpram_t simulate_dumpram;
  simulate_push_t simulate_push;
  simulate_set_reg_t simulate_set_reg;
  simulate_get_reg_t simulate_get_reg;
  simulate_set_pc_t simulate_set_pc;
  simulate_reset_t simulate_reset;
  simulate_dump_registers_t simulate_dump_registers;
  simulate_run_t simulate_run;

  uint8_t context[];
};

#endif

