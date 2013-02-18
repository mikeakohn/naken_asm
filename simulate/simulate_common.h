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

#ifndef _SIMULATE_COMMON_H
#define _SIMULATE_COMMON_H

#include <unistd.h>

#include "memory.h"

typedef struct _simulate *(*simulate_init_t)(struct _memory *);
typedef void (*simulate_free_t)(struct _simulate *);
typedef void (*simulate_push_t)(struct _simulate *, unsigned int value);
typedef int (*simulate_set_reg_t)(struct _simulate *, char *reg_string, unsigned int value);
typedef unsigned int (*simulate_get_reg_t)(struct _simulate *, char *reg_string);
typedef void (*simulate_reset_t)(struct _simulate *);
typedef void (*simulate_dump_registers_t)(struct _simulate *);
typedef int (*simulate_run_t)(struct _simulate *, int max_cycles, int step);

struct _simulate
{
  struct _memory *memory;
  int cycle_count;
  int ret_count;
  useconds_t usec;
  int step_mode;
  int break_point;
  int show;

  simulate_init_t simulate_init;
  simulate_free_t simulate_free;
  simulate_push_t simulate_push;
  simulate_set_reg_t simulate_set_reg;
  simulate_get_reg_t simulate_get_reg;
  simulate_reset_t simulate_reset;
  simulate_dump_registers_t simulate_dump_registers;
  simulate_run_t simulate_run;

  //unsigned short int reg[16];
  unsigned char context[];
};


#endif

