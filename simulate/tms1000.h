/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2024 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_SIMULATE_TMS1000_H
#define NAKEN_ASM_SIMULATE_TMS1000_H

#include <unistd.h>

#include "simulate/Simulate.h"

class SimulateTms1000 : public Simulate
{
public:
  SimulateTms1000(Memory *memory);
  virtual ~SimulateTms1000();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int execute(uint8_t opcode, uint8_t &update_s);
  int increment_pc(int pc);
  void dump_ram();

  uint8_t pc;
  uint8_t pa;
  uint8_t pb;
  uint8_t cl;
  uint8_t sr;
  uint8_t s_flag;
  uint8_t reg_a;
  uint8_t reg_x;
  uint8_t reg_y;

  uint8_t ram[64];
  uint16_t r_pins;
  uint8_t o_pins;
  uint8_t k_pins;
};

#endif

