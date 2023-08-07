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
#include <inttypes.h>
#include <string.h>
#include <signal.h>

#include "simulate/ebpf.h"

static int stop_running = 0;

static void handle_signal(int sig)
{
  stop_running = 1;
  signal(SIGINT, SIG_DFL);
}

static int get_register(const char *s)
{
  while (*s == ' ') { s++; }

  if (*s != 'r' && *s != 'R') { return -1; }
  s++;

  if (s[1] == 0)
  {
    if (s[0] >= '0' && s[0] <= '9') { return s[0] - '0'; }
    return -1;
  }

  if (s[0] == '1' && s[1] == '0' && s[2] == 0)
  {
    return 10;
  }

  return -1;
}

Simulate *simulate_init_ebpf(Memory *memory)
{
  Simulate *simulate;

  simulate = (Simulate *)malloc(sizeof(SimulateEbpf) + sizeof(Simulate));

  simulate->simulate_init = simulate_init_ebpf;
  simulate->simulate_free = simulate_free_ebpf;
  simulate->simulate_dumpram = simulate_dumpram_ebpf;
  simulate->simulate_push = simulate_push_ebpf;
  simulate->simulate_set_reg = simulate_set_reg_ebpf;
  simulate->simulate_get_reg = simulate_get_reg_ebpf;
  simulate->simulate_set_pc = simulate_set_pc_ebpf;
  simulate->simulate_reset = simulate_reset_ebpf;
  simulate->simulate_dump_registers = simulate_dump_registers_ebpf;
  simulate->simulate_run = simulate_run_ebpf;

  //memory_init(&simulate->memory, 65536, 0);
  simulate->memory = memory;
  simulate_reset_ebpf(simulate);
  simulate->usec = 1000000; // 1Hz
  simulate->step_mode = 0;
  simulate->show = 1;       // Show simulation
  simulate->auto_run = 0;   // Will this program stop on a ret from main
  return simulate;
}

void simulate_push_ebpf(Simulate *simulate, uint32_t value)
{
  //SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;

}

int simulate_set_reg_ebpf(
  Simulate *simulate,
  const char *reg_string,
  uint32_t value)
{
  SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;

  int r = get_register(reg_string);

  if (r == -1) { return -1; }
  simulate_ebpf->reg[r] = value;

  return 0;
}

uint32_t simulate_get_reg_ebpf(Simulate *simulate, const char *reg_string)
{
  SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;

  int r = get_register(reg_string);

  if (r == -1) { return -1; }

  printf(" r%d: 0x%08" PRIx64 "\n", r, simulate_ebpf->reg[r]);

  return 0;
}

void simulate_set_pc_ebpf(Simulate *simulate, uint32_t value)
{
  SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;

  simulate_ebpf->pc = value;
}

void simulate_reset_ebpf(Simulate *simulate)
{
  SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;

  memset(simulate_ebpf->reg, 0, sizeof(simulate_ebpf->reg));
  simulate_ebpf->pc = 0;
}

void simulate_free_ebpf(Simulate *simulate)
{
  free(simulate);
}

int simulate_dumpram_ebpf(Simulate *simulate, int start, int end)
{
  return -1;
}

void simulate_dump_registers_ebpf(Simulate *simulate)
{
  SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;
  int n;

  for (n = 0; n < 11; n++)
  {
    printf(" r%d: %08" PRIx64 "\n", n, simulate_ebpf->reg[n]);
  }
}

int simulate_run_ebpf(Simulate *simulate, int max_cycles, int step)
{
  //SimulateEbpf *simulate_ebpf = (SimulateEbpf *)simulate->context;

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

