/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: https://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2026 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_SIMULATE_AVR8_H
#define NAKEN_ASM_SIMULATE_AVR8_H

#include <unistd.h>

#include "simulate/Simulate.h"

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
  int get_reg16(int r) { return (reg[r + 1] << 8) | reg[r]; }

  int get_x() { return get_reg16(26); }
  int get_y() { return get_reg16(28); }
  int get_z() { return get_reg16(30); }

  void inc_x();
  void inc_y();
  void inc_z();
  void dec_x();
  void dec_y();
  void dec_z();

  void set_sp(int value) { sp = value; }
  int get_sp()  { return sp; }
  void inc_sp() { sp += 1; }
  void dec_sp() { sp -= 1; }

  void push_stack(uint8_t value)
  {
    ram[get_sp()] = value;
    dec_sp();
  }

  uint8_t pop_stack()
  {
    inc_sp();
    return ram[get_sp()];
  }

  void push_stack16(int value)
  {
    push_stack(value >> 8);
    push_stack(value & 0xff);
  }

  int pop_stack16()
  {
    int value;
    value  = pop_stack();
    value |= pop_stack() << 8;

    return value;
  }

  void set_reg16(int index, int value)
  {
    reg[index + 0] = value & 0xff;
    reg[index + 1] = (value >> 8) & 0xff;
  }

  uint8_t read_sram(uint32_t address)
  {
    if (serial_in != nullptr && address == serial_address)
    {
      return serial_read8();
    }

    return ram[address & ram_mask];
  }

  void write_sram(uint32_t address, uint8_t value)
  {
    if (serial_in != nullptr && address == serial_address)
    {
      serial_write8(value);
    }

    ram[address & ram_mask] = value;
  }

  int word_count();
  int execute_op_none(struct _table_avr8 *table_avr8);
  void execute_set_sreg_arith(uint8_t rd_prev, uint8_t rd, int k);
  void execute_set_sreg_arith_sub(uint8_t rd_prev, uint8_t rd, int k);
  void execute_set_sreg_logic(uint8_t rd_prev, uint8_t rd, int k);
  void execute_set_reg16(int prev_value, int value, int rd);
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

  virtual void ram_write8(uint32_t address, uint8_t data);
  virtual void ram_write16(uint32_t address, uint16_t data);
  virtual void ram_write32(uint32_t address, uint32_t data);

  virtual uint8_t ram_read8(uint32_t address);
  virtual uint16_t ram_read16(uint32_t address);
  virtual uint32_t ram_read32(uint32_t address);

  uint8_t reg[32];
  uint8_t *ram;
  //uint8_t io[64];
  int pc;
  // SPL = 0x3d
  // SPH = 0x3e
  int sp;
  uint8_t sreg;

  int ram_mask;
  int ram_size;

  enum
  {
    SREG_C = 0,
    SREG_Z = 1,
    SREG_N = 2,
    SREG_V = 3,
    SREG_S = 4,
    SREG_H = 5,
    SREG_T = 6,
    SREG_I = 7
  };
};

#endif

