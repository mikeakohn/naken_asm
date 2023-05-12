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
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "simulate/null.h"

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

struct _simulate *simulate_init_null(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_null) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_null;
  simulate->simulate_free = simulate_free_null;
  simulate->simulate_dumpram = simulate_dumpram_null;
  simulate->simulate_push = simulate_push_null;
  simulate->simulate_set_reg = simulate_set_reg_null;
  simulate->simulate_get_reg = simulate_get_reg_null;
  simulate->simulate_set_pc = simulate_set_pc_null;
  simulate->simulate_reset = simulate_reset_null;
  simulate->simulate_dump_registers = simulate_dump_registers_null;
  simulate->simulate_run = simulate_run_null;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_null(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_null(struct _simulate *simulate, uint32_t value)
{
  //struct _simulate_null *simulate_null = (struct _simulate_null *)simulate->context;

}

int simulate_set_reg_null(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  //struct _simulate_null *simulate_null = (struct _simulate_null *)simulate->context;

  return 0;
}

uint32_t simulate_get_reg_null(struct _simulate *simulate, char *reg_string)
{
  //struct _simulate_null *simulate_null = (struct _simulate_null *)simulate->context;

  return 0;
}

void simulate_set_pc_null(struct _simulate *simulate, uint32_t value)
{
  //struct _simulate_null *simulate_null = (struct _simulate_null *)simulate->context;

}

void simulate_reset_null(struct _simulate *simulate)
{
  //struct _simulate_null *simulate_null = (struct _simulate_null *)simulate->context;

}

void simulate_free_null(struct _simulate *simulate)
{
  free(simulate);
}

int simulate_dumpram_null(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_null(struct _simulate *simulate)
{
  //struct _simulate_null *simulate_null = (struct _simulate_msp430 *)simulate->context;
}

int simulate_run_null(struct _simulate *simulate, int max_cycles, int step)
{
  //struct _simulate_null *simulate_null = (struct _simulate_null *)simulate->context;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  while(stop_running == 0)
  {
    printf("CPU not supported.\n");
    break;
  } 

  signal(SIGINT, SIG_DFL);

  return 0;
}

