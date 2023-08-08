/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn
 *
 * 1802 file by Malik Enes Safak
 *
 */

#ifndef NAKEN_ASM_SIMULATE_1802_H
#define NAKEN_ASM_SIMULATE_1802_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

class Simulate1802 : public Simulate
{
public:
  Simulate1802(Memory *memory);
  virtual ~Simulate1802();

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
  int operand_exe(int opcode);

  // Define registers and anything 1802 specific here
  uint8_t reg_d, reg_p, reg_x, reg_t, reg_n, reg_i, reg_b;
  uint16_t reg_r[16];
  uint8_t reg_cntr, reg_cn;
  uint8_t flag_df, flag_q, flag_mie, flag_cie, flag_xie, flag_cil, flag_etq;
};

#endif

