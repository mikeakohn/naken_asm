/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2022 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "simulate/8008.h"


#define flag_c 0
#define flag_p 1
#define flag_z 2
#define flag_s 3

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

struct _simulate *simulate_init_8008(struct _memory *memory)
{
  struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(
     sizeof(struct _simulate_8008) + sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_8008;
  simulate->simulate_free = simulate_free_8008;
  simulate->simulate_dumpram = simulate_dumpram_8008;
  simulate->simulate_push = simulate_push_8008;
  simulate->simulate_set_reg = simulate_set_reg_8008;
  simulate->simulate_get_reg = simulate_get_reg_8008;
  simulate->simulate_set_pc = simulate_set_pc_8008;
  simulate->simulate_reset = simulate_reset_8008;
  simulate->simulate_dump_registers = simulate_dump_registers_8008;
  simulate->simulate_run = simulate_run_8008;

  simulate->memory = memory;
  simulate_reset_8008(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_8008(struct _simulate *simulate, uint32_t value)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_8008 *)simulate->context;

}

int simulate_set_reg_8008(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_8008 *)simulate->context;

  return 0;
}

uint32_t simulate_get_reg_8008(struct _simulate *simulate, char *reg_string)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_8008 *)simulate->context;

  return 0;
}

void simulate_set_pc_8008(struct _simulate *simulate, uint32_t value)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_8008 *)simulate->context;

}

void simulate_reset_8008(struct _simulate *simulate)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_8008 *)simulate->context;

}

void simulate_free_8008(struct _simulate *simulate)
{
  free(simulate);
}

int simulate_dumpram_8008(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_8008(struct _simulate *simulate)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_msp430 *)simulate->context;

}

int simulate_run_8008(struct _simulate *simulate, int max_cycles, int step)
{
  //struct _simulate_8008 *simulate_8008 =
  //  (struct _simulate_8008 *)simulate->context;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  while (stop_running == 0)
  {
    printf("CPU not supported.\n");
    break;
  } 

  signal(SIGINT, SIG_DFL);

  return 0;
}

