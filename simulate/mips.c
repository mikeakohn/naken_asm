/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2015 by Michael Kohn
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>
#include <signal.h>

#include "simulate/mips.h"

static int stop_running = 0;

static const char *reg_names[32] =
{
  "$0", "$at", "$v0", "$v1", "$a0", "$a1", "$a2", "$a3",
  "$t0", "$t1", "$t2", "$t3", "$t4", "$t5", "$t6", "$t7",
  "$s0", "$s1", "$s2", "$s3", "$s4", "$s5", "$s6", "$s7",
  "$t8", "$t9", "$k0", "$k1", "$gp", "$sp", "$fp", "$ra"
};


static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

struct _simulate *simulate_init_mips(struct _memory *memory)
{
struct _simulate *simulate;

  simulate = (struct _simulate *)malloc(sizeof(struct _simulate_mips) +
                                        sizeof(struct _simulate));

  simulate->simulate_init = simulate_init_mips;
  simulate->simulate_free = simulate_free_mips;
  simulate->simulate_dumpram = simulate_dumpram_mips;
  simulate->simulate_push = simulate_push_mips;
  simulate->simulate_set_reg = simulate_set_reg_mips;
  simulate->simulate_get_reg = simulate_get_reg_mips;
  simulate->simulate_reset = simulate_reset_mips;
  simulate->simulate_dump_registers = simulate_dump_registers_mips;
  simulate->simulate_run = simulate_run_mips;

  simulate->memory = memory;
  simulate_reset_mips(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_mips(struct _simulate *simulate, uint32_t value)
{
//struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;

}

int simulate_set_reg_mips(struct _simulate *simulate, char *reg_string, uint32_t value)
{
  struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;
  int reg, n;

  if (reg_string[0] != '$') { return -1; }

  if (reg_string[1] >= '0' && reg_string[1] <= '9' &&
      reg_string[2] >= '0' && reg_string[2] <= '9' &&
      reg_string[3] == 0)
  {
    reg = atoi(reg_string + 1);
    if (reg < 0 || reg > 31) { return -1; }

    simulate_mips->reg[reg] = value;
    return 0;
  }

  for (n = 0; n < 32; n++)
  {
    if (strcmp(reg_string, reg_names[n]) == 0)
    {
      simulate_mips->reg[n] = value;
      return 0;
    }
  }

  return -1;
}

uint32_t simulate_get_reg_mips(struct _simulate *simulate, char *reg_string)
{
//struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;

  return 0;
}

void simulate_reset_mips(struct _simulate *simulate)
{
//struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;

}

void simulate_free_mips(struct _simulate *simulate)
{
  free(simulate);
}

int simulate_dumpram_mips(struct _simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_mips(struct _simulate *simulate)
{
  struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;
  int n;

  printf("\nSimulation Register Dump\n");
  printf("-------------------------------------------------------------------\n");
  printf(" PC: 0x%04x\n", simulate_mips->pc);

  for (n = 0; n < 32; n++)
  {
    printf("%c%3s: 0x%08x", (n & 0x3) == 0 ? '\n' : ' ',
                            reg_names[n],
                            simulate_mips->reg[n]);
  }

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_mips(struct _simulate *simulate, int max_cycles, int step)
{
  struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;

  stop_running = 0;
  signal(SIGINT, handle_signal);

  while(stop_running == 0)
  {
    printf("CPU not supported.\n");
    break;
  } 

  signal(SIGINT, SIG_DFL);

  printf("Stopped.  PC=0x%04x.\n", simulate_mips->pc);
  printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}


