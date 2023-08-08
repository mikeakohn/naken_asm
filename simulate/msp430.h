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

#ifndef NAKEN_ASM_SIMULATE_MSP430_H
#define NAKEN_ASM_SIMULATE_MSP430_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

class SimulateMsp430 : public Simulate
{
public:
  SimulateMsp430(Memory *memory);
  virtual ~SimulateMsp430();

  static Simulate *init(Memory *memory);

  virtual int dumpram(int start, int end);
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void reset();
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  void sp_inc(int *sp);
  uint16_t get_data(int reg_index, int As, int bw);
  void update_reg(int reg_index, int mode, int bw);
  int put_data(int PC, int reg_index, int mode, int bw, uint32_t data);
  int one_operand_exe(uint16_t opcode);
  int relative_jump_exe(uint16_t opcode);
  int two_operand_exe(uint16_t opcode);

  uint16_t reg[16];
};

#endif

