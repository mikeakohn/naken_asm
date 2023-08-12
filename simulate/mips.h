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

#ifndef NAKEN_ASM_SIMULATE_MIPS_H
#define NAKEN_ASM_SIMULATE_MIPS_H

#include <unistd.h>

#include "simulate/Simulate.h"

class SimulateMips : public Simulate
{
public:
  SimulateMips(Memory *memory);
  virtual ~SimulateMips();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int32_t get_offset16(uint32_t opcode);
  int delay_slot();
  int execute_shift(uint32_t opcode);
  int execute_mips_r(uint32_t opcode);
  int execute_mips_i(uint32_t opcode);

  int execute();

  int32_t reg[32];
  uint32_t pc;
  uint32_t hi;
  uint32_t lo;
  bool ra_was_set;
  bool force_break;
};

#endif

