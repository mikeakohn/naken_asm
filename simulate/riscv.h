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

#ifndef NAKEN_ASM_SIMULATE_RISCV_H
#define NAKEN_ASM_SIMULATE_RISCV_H

#include <unistd.h>

#include "simulate/Simulate.h"

class SimulateRiscv : public Simulate
{
public:
  SimulateRiscv(Memory *memory);
  virtual ~SimulateRiscv();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int get_register(const char *name);
  int execute(uint32_t opcode);
  int jal_address(uint32_t opcode);
  int jalr_address(uint32_t opcode);
  int branch(uint32_t opcode);
  int load(uint32_t opcode);
  int store(uint32_t opcode);
  int alu(uint32_t opcode);
  int alu_reg(uint32_t opcode);

  enum
  {
    OP_LUI   = 0x37,
    OP_AUIPC = 0x17,
    OP_JAL   = 0x6f,
    OP_JALR  = 0x67,
    OP_BRA   = 0x63,
    OP_LD    = 0x03,
    OP_ST    = 0x23,
    OP_ALU   = 0x13,
    OP_ALU_R = 0x33,
    OP_BREAK = 0x73,
  };

  int32_t reg[32];
  uint32_t pc;
};

#endif

