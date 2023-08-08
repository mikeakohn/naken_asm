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

#ifndef NAKEN_ASM_SIMULATE_TMS9900_H
#define NAKEN_ASM_SIMULATE_TMS9900_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

class SimulateTms9900 : public Simulate
{
public:
  SimulateTms9900(Memory *memory);
  virtual ~SimulateTms9900();

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
  int get_register(const char *token);

  uint16_t pc;
  uint16_t wp;
  uint16_t st;
};

#endif

