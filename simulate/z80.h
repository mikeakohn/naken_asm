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

#ifndef NAKEN_ASM_SIMULATE_Z80_H
#define NAKEN_ASM_SIMULATE_Z80_H

#include <unistd.h>

#include "simulate/Simulate.h"

#define REG_B 0
#define REG_C 1
#define REG_D 2
#define REG_E 3
#define REG_H 4
#define REG_L 5
#define REG_F 6
#define REG_A 7

class SimulateZ80 : public Simulate
{
public:
  SimulateZ80(Memory *memory);
  virtual ~SimulateZ80();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int get_q(int reg16);
  int get_p(int reg16);
  int set_p(int reg16, int value);
  void set_q(int reg16, int value);
  void set_xy(int xy, int value);
  uint16_t get_xy(int xy);
  void set_parity(uint8_t a);
  void set_flags_a(int a, int number, uint8_t vflag);
  void set_flags16(int _new, int old, int number, uint8_t vflag);
  void add_reg16(int xy, int reg16);

  int execute_op_none(struct _table_z80 *table_z80, uint16_t opcode);
  int execute_op_a_reg8(struct _table_z80 *table_z80, uint8_t opcode);
  int execute_op_reg8(struct _table_z80 *table_z80, uint8_t opcode);
  int execute_op_a_number8(struct _table_z80 *table_z80, uint16_t opcode16);
  int execute_op_number8(struct _table_z80 *table_z80, uint16_t opcode16);
  int execute_op_reg8_v2(struct _table_z80 *table_z80, uint8_t opcode);
  int execute_op_reg16(struct _table_z80 *table_z80, uint8_t opcode);
  int execute_op_reg16p(struct _table_z80 *table_z80, uint8_t opcode);
  int execute_op_hl_reg16_2(struct _table_z80 *table_z80, uint8_t opcode);
  int execute_op_xy(struct _table_z80 *table_z80, uint16_t opcode16);
  int execute();

  uint8_t reg[8];
  uint16_t ix;
  uint16_t iy;
  uint16_t sp;
  uint16_t pc;
  //uint8_t status;
  uint8_t iff1;
  uint8_t iff2;
};

#endif

