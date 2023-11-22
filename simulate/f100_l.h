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

#ifndef NAKEN_ASM_SIMULATE_F100_L_H
#define NAKEN_ASM_SIMULATE_F100_L_H

#include <unistd.h>

#include "simulate/Simulate.h"

class SimulateF100L : public Simulate
{
public:
  SimulateF100L(Memory *memory);
  virtual ~SimulateF100L();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int get_pc() { return pc / 2; }
  int execute_instruction(uint16_t opcode);
  void bit_ops(uint16_t opcode);
  void shift_right(int &data, int bits, int j);
  void shift_left(int &data, int bits, int j);
  void alu(uint16_t opcode);

  uint16_t pc;
  uint16_t accum;

  struct
  {
    void set_i() { value |= 1; }
    void set_z() { value |= 2; }
    void set_v() { value |= 4; }
    void set_s() { value |= 8; }
    void set_c() { value |= 16; }
    void set_m() { value |= 32; }
    void set_f() { value |= 64; }

    int get_i() { return (value & 1) != 0 ? 1 : 0; }
    int get_z() { return (value & 2) != 0 ? 1 : 0; }
    int get_v() { return (value & 4) != 0 ? 1 : 0; }
    int get_s() { return (value & 8) != 0 ? 1 : 0; }
    int get_c() { return (value & 16) != 0 ? 1 : 0; }
    int get_m() { return (value & 32) != 0 ? 1 : 0; }
    int get_f() { return (value & 64) != 0 ? 1 : 0; }

    uint16_t value;
  } cr;

  enum
  {
    OP_ADD = 0x9,
    OP_ADS = 0x5,
    OP_AND = 0xc,
    OP_CAL = 0x2,
    OP_CMP = 0xb,
    OP_ICZ = 0x7,
    OP_JMP = 0xf,
    OP_LDA = 0x8,
    OP_NEQ = 0xd,
    OP_SBS = 0x6,
    OP_STO = 0x4,
    OP_SUB = 0xa
  };

  enum
  {
    DEST_NONE = 0,
    DEST_A,
    DEST_PC,
    DEST_EA,
    DEST_CALL,
    DEST_ICZ
  };
};

#endif

