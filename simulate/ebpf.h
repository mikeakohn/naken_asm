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

#ifndef NAKEN_ASM_SIMULATE_EBPF_H
#define NAKEN_ASM_SIMULATE_EBPF_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

class SimulateEbpf : public Simulate
{
public:
  SimulateEbpf(Memory *memory);
  virtual ~SimulateEbpf();

  static Simulate *init(Memory *memory);

  virtual int dump_ram(int start, int end);
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void reset();
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int get_register(const char *s);

  int64_t reg[16];
  uint32_t pc;

};

#endif

