/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2023 by Michael Kohn, D.L. Karmann
 *
 * stm8 file by D.L. Karmann
 *
 */

#ifndef NAKEN_ASM_SIMULATE_STM8_H
#define NAKEN_ASM_SIMULATE_STM8_H

#include <unistd.h>

#include "common/memory.h"
#include "simulate/Simulate.h"

class SimulateStm8 : public Simulate
{
public:
  SimulateStm8(Memory *memory);
  virtual ~SimulateStm8();

  static Simulate *init(Memory *memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int run(int max_cycles, int step);

private:
  void push16(uint32_t value);
  void push24(uint32_t value);
  uint16_t pop16();
  uint32_t pop24();

  void calculate_flags(
    uint8_t flag_bits,
    uint16_t op1, 
    uint16_t op2,
    uint16_t rslt,
    uint8_t bit_size);

  int execute_op_common(
    struct _table_stm8_opcodes *table_stm8, 
    uint32_t eff_addr);

  int execute_op_none(struct _table_stm8_opcodes *table_stm8);
  int execute_op_number8(struct _table_stm8_opcodes *table_stm8);
  int execute_op_number16(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address8(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address16(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address24(struct _table_stm8_opcodes *table_stm8);
  int execute_op_index_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset8_index_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset16_index_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset24_index_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_index_y(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset8_index_y(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset16_index_y(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset24_index_y(struct _table_stm8_opcodes *table_stm8);
  int execute_op_offset8_index_sp(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect8(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect16(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect16_e(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect8_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect16_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect16_e_x(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect8_y(struct _table_stm8_opcodes *table_stm8);
  int execute_op_indirect16_e_y(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address_bit(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address_bit_loop(struct _table_stm8_opcodes *table_stm8);
  int execute_op_relative(struct _table_stm8_opcodes *table_stm8);
  int execute_op_single_register(struct _table_stm8_opcodes *table_stm8);
  int execute_op_two_registers(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address16_number8(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address8_address8(struct _table_stm8_opcodes *table_stm8);
  int execute_op_address16_address16(struct _table_stm8_opcodes *table_stm8);
  int execute();

  int stm8_int_opcode;

  uint8_t reg_a;
  uint16_t reg_x;
  uint16_t reg_y;
  uint16_t reg_sp;
  uint32_t reg_pc;
  uint8_t reg_cc;
};

#endif

