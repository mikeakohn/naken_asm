/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPL
 *
 * Copyright 2010-2013 by Michael Kohn
 *
 * 65xx file by Joe Davisson
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>

#include "asm_65xx.h"
#include "disasm_65xx.h"
#include "simulate_65xx.h"

static int stop_running=0;

static void handle_signal(int sig)
{
  stop_running=1;
  signal(SIGINT, SIG_DFL);
}

struct _simulate *simulate_init_65xx(struct _memory *memory)
{
struct _simulate *simulate;

  simulate=(struct _simulate *)malloc(sizeof(struct _simulate_65xx)+sizeof(struct _simulate));

  simulate->simulate_init=simulate_init_65xx;
  simulate->simulate_free=simulate_free_65xx;
  simulate->simulate_push=simulate_push_65xx;
  simulate->simulate_set_reg=simulate_set_reg_65xx;
  simulate->simulate_get_reg=simulate_get_reg_65xx;
  simulate->simulate_reset=simulate_reset_65xx;
  simulate->simulate_dump_registers=simulate_dump_registers_65xx;
  simulate->simulate_run=simulate_run_65xx;

  //memory_init(&simulate->memory, 65536, 0);
  simulate_reset_65xx(simulate);
  simulate->usec=1000000; // 1Hz
  simulate->show=1; // Show simulation
  simulate->step_mode=0;
  simulate->memory=memory;

  return simulate;
}

void simulate_push_65xx(struct _simulate *simulate, unsigned int value)
{
struct _simulate_65xx *simulate_65xx=(struct _simulate_65xx *)simulate->context;

  printf("Get rid of warning %p\n", simulate_65xx);
  //simulate_65xx->reg[1]-=2;
  //WRITE_RAM(simulate_65xx->reg[1], value&0xff);
  //WRITE_RAM(simulate_65xx->reg[1]+1, value>>8);
}

int simulate_set_reg_65xx(struct _simulate *simulate, char *reg_string, unsigned int value)
{
struct _simulate_65xx *simulate_65xx=(struct _simulate_65xx *)simulate->context;

  printf("Get rid of warning %p\n", simulate_65xx);

  return -1;
}

unsigned int simulate_get_reg_65xx(struct _simulate *simulate, char *reg_string)
{
struct _simulate_65xx *simulate_65xx=(struct _simulate_65xx *)simulate->context;

  printf("Get rid of warning %p\n", simulate_65xx);

  return -1;
}

void simulate_reset_65xx(struct _simulate *simulate)
{
struct _simulate_65xx *simulate_65xx=(struct _simulate_65xx *)simulate->context;

  printf("Get rid of warning %p\n", simulate_65xx);

  simulate->cycle_count=0;
  simulate->ret_count=0;
  //memset(simulate_65xx->reg, 0, sizeof(simulate_65xx->reg));
  //memory_clear(simulate->memory);
  simulate->break_point=-1;
}

void simulate_free_65xx(struct _simulate *simulate)
{
  //memory_free(simulate->memory);
  free(simulate);
}

void simulate_dump_registers_65xx(struct _simulate *simulate)
{
struct _simulate_65xx *simulate_65xx=(struct _simulate_65xx *)simulate->context;

  printf("Get rid of warning %p\n", simulate_65xx);

  printf("\nSimulation Register Dump                                  Stack\n");
  printf("-------------------------------------------------------------------\n");

  printf("\n\n");
  printf("%d clock cycles have passed since last reset.\n\n", simulate->cycle_count);
}

int simulate_run_65xx(struct _simulate *simulate, int max_cycles, int step)
{
struct _simulate_65xx *simulate_65xx=(struct _simulate_65xx *)simulate->context;

  printf("Get rid of warning %p\n", simulate_65xx);

  stop_running=0;
  signal(SIGINT, handle_signal);

  printf("Running... Press Ctl-C to break.\n");

  while(stop_running==0)
  {

    usleep(simulate->usec);
  }

  signal(SIGINT, SIG_DFL);
  //printf("Stopped.  PC=0x%04x.\n", simulate_65xx->reg[0]);
  //printf("%d clock cycles have passed since last reset.\n", simulate->cycle_count);

  return 0;
}


