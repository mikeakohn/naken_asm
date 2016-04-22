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

static const char *reg_string[32] =
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

  //memory_init(&simulate->memory, 65536, 0);
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
//struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;


  return 0;
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
#if 0
  printf(" PC: 0x%04x,  SP: 0x%04x, SREG: I T H S V N Z C = 0x%02x\n"
         "                                %d %d %d %d %d %d %d %d\n",
         simulate_mips->pc,
         simulate_mips->sp,
         simulate_mips->sreg,
         GET_SREG(SREG_I),
         GET_SREG(SREG_T),
         GET_SREG(SREG_H),
         GET_SREG(SREG_S),
         GET_SREG(SREG_V),
         GET_SREG(SREG_N),
         GET_SREG(SREG_Z),
         GET_SREG(SREG_C));
#endif

  for (n = 0; n < 32; n++)
  {
    printf("%c%3s: 0x%02x", (n & 0x7) == 0 ? '\n' : ' ',
                            reg_string[n],
                            simulate_mips->reg[n]);
  }

  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_mips(struct _simulate *simulate, int max_cycles, int step)
{
//struct _simulate_mips *simulate_mips = (struct _simulate_mips *)simulate->context;

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


