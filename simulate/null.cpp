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

SimulateNull::SimulateNull(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateNull::~SimulateNull()
{
}

Simulate *SimulateNull::init(Memory *memory)
{
  return new SimulateNull(memory);
}

void SimulateNull::push(uint32_t value)
{
}

int SimulateNull::set_reg(const char *reg_string, uint32_t value)
{
  return 0;
}

uint32_t SimulateNull::get_reg(const char *reg_string)
{
  return 0;
}

void SimulateNull::set_pc(uint32_t value)
{
}

void SimulateNull::reset()
{
}

int SimulateNull::dumpram(int start, int end)
{
  return -1;
}

void SimulateNull::dump_registers()
{
}

int SimulateNull::run(int max_cycles, int step)
{
  while (stop_running == false)
  {
    printf("CPU not supported.\n");
    break;
  }

  //signal(SIGINT, SIG_DFL);

  return 0;
}

