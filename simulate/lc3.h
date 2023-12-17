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

#ifndef NAKEN_ASM_SIMULATE_LC3_H
#define NAKEN_ASM_SIMULATE_LC3_H

#include <unistd.h>

#include "simulate/Simulate.h"

class SimulateLc3 : public Simulate
{
public:
  SimulateLc3(Memory *memory);
  virtual ~SimulateLc3();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int execute(uint16_t opcode);
  int get_reg_index(const char *reg_string);

  enum Flags
  {
    FLAG_PRIV = 0x8000,
    FLAG_N = 0x0004,
    FLAG_Z = 0x0002,
    FLAG_P = 0x0001
  };

  uint16_t reg[8];
  uint16_t pc;
  uint16_t psr;
};

#endif

