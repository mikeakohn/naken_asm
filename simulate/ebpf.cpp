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

SimulateEbpf::SimulateEbpf(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateEbpf::~SimulateEbpf()
{
}

Simulate *SimulateEbpf::init(Memory *memory)
{
  return new SimulateEbpf(memory);
}

void SimulateEbpf::push(uint32_t value)
{
}

int SimulateEbpf::set_reg(const char *reg_string, uint32_t value)
{
  int r = get_register(reg_string);

  if (r == -1) { return -1; }
  reg[r] = value;

  return 0;
}

uint32_t SimulateEbpf::get_reg(const char *reg_string)
{
  int r = get_register(reg_string);

  if (r == -1) { return -1; }

  printf(" r%d: 0x%08" PRIx64 "\n", r, reg[r]);

  return 0;
}

void SimulateEbpf::set_pc(uint32_t value)
{
  pc = value;
}

void SimulateEbpf::reset()
{
  memset(reg, 0, sizeof(reg));
  pc = 0;
}

int SimulateEbpf::dump_ram(int start, int end)
{
  return -1;
}

void SimulateEbpf::dump_registers()
{
  int n;

  for (n = 0; n < 11; n++)
  {
    printf(" r%d: %08" PRIx64 "\n", n, reg[n]);
  }
}

int SimulateEbpf::run(int max_cycles, int step)
{
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

int SimulateEbpf::get_register(const char *s)
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

