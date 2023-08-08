/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, Joe Davisson
 *
 * 6502 file by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_SIMULATE_6502_H
#define NAKEN_ASM_SIMULATE_6502_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

class Simulate6502 : public Simulate
{
public:
  Simulate6502(Memory *memory);
  virtual ~Simulate6502();

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
  int calc_address(int address, int mode);
  int operand_exe(int opcode);

  // Define registers and anything 6502 specific here
  int reg_a, reg_x, reg_y, reg_sr, reg_pc, reg_sp;
};

#endif

