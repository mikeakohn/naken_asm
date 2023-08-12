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

#ifndef NAKEN_ASM_SIMULATE_AVR8_H
#define NAKEN_ASM_SIMULATE_AVR8_H

#include <unistd.h>

#include "simulate/Simulate.h"

#define SREG_C 0
#define SREG_Z 1
#define SREG_N 2
#define SREG_V 3
#define SREG_S 4
#define SREG_H 5
#define SREG_T 6
#define SREG_I 7

#define RAM_MASK 0x1fff
#define RAM_SIZE (RAM_MASK + 1)

class SimulateAvr8 : public Simulate
{
public:
  SimulateAvr8(Memory *memory);
  virtual ~SimulateAvr8();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  int dump_ram(int start, int end);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  int word_count();
  int execute_op_none(struct _table_avr8 *table_avr8);
  void execute_set_sreg_arith(uint8_t rd_prev, uint8_t rd, int k);
  void execute_set_sreg_arith_sub(uint8_t rd_prev, uint8_t rd, int k);
  void execute_set_sreg_logic(uint8_t rd_prev, uint8_t rd, int k);
  void execute_set_sreg_reg16(int rd_prev, int rd);
  void execute_set_sreg_common(uint8_t value);
  void execute_set_sreg_sign();
  int execute_op_branch_s_k(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_branch_k(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_two_reg(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_reg_imm(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_one_reg(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_reg_bit(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_reg_imm_word(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_ioreg_bit(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_sreg_bit(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_relative(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute_op_jump(struct _table_avr8 *table_avr8, uint16_t opcode);
  int execute();

  uint8_t reg[32];
  uint8_t ram[RAM_SIZE];
  uint8_t io[64];
  //int sp_start;
  int pc;
  int sp;
  uint8_t sreg;
};

#endif

