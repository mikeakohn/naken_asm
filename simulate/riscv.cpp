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

#include "simulate/riscv.h"

SimulateRiscv::SimulateRiscv(Memory *memory) : Simulate(memory)
{
  reset();
}

SimulateRiscv::~SimulateRiscv()
{
}

Simulate *SimulateRiscv::init(Memory *memory)
{
  return new SimulateRiscv(memory);
}

void SimulateRiscv::reset()
{
  memset(reg, 0, sizeof(reg));
}

void SimulateRiscv::push(uint32_t value)
{
}

int SimulateRiscv::set_reg(const char *reg_string, uint32_t value)
{
  return 0;
}

uint32_t SimulateRiscv::get_reg(const char *reg_string)
{
  return 0;
}

void SimulateRiscv::set_pc(uint32_t value)
{
}

void SimulateRiscv::dump_registers()
{
}

int SimulateRiscv::run(int max_cycles, int step)
{
  while (stop_running == false)
  {
    printf("CPU not supported.\n");
    break;
  }

  //signal(SIGINT, SIG_DFL);

  return 0;
}

