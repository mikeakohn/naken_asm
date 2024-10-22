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

#ifndef NAKEN_ASM_SIMULATE_Z80_H
#define NAKEN_ASM_SIMULATE_Z80_H

#include <unistd.h>

#include "simulate/Simulate.h"

#define REG_B   0
#define REG_C   1
#define REG_D   2
#define REG_E   3
#define REG_H   4
#define REG_L   5
#define REG_F   6
#define REG_A   7
#define REG_IV  8
#define REG_RR  9
#define REG_IFF 10
#define REG_IM  11
#define REG_IX  12
#define REG_IY  13
#define REG_SP  14
#define REG_PC  15

#define OP_REG_BC   0
#define OP_REG_DE   1
#define OP_REG_HL   2
#define OP_REG_IX   2
#define OP_REG_IY   2
#define OP_REG_SP   3
#define OP_REG_AF   3

// Conditionals
#define CC_NZ       0
#define CC_Z        1
#define CC_NC       2
#define CC_C        3
#define CC_PO       4
#define CC_PE       5
#define CC_P        6
#define CC_M        7

class SimulateZ80 : public Simulate
{
public:
  SimulateZ80(Memory *memory);
  virtual ~SimulateZ80();

  static Simulate * init(Memory * memory);

  virtual void reset();
  virtual void push(uint32_t value);
  virtual int set_reg(const char *reg_string, uint32_t value);
  virtual uint32_t get_reg(const char *reg_string);
  virtual void set_pc(uint32_t value);
  virtual void dump_registers();
  virtual int dump_ram(int start, int end);
  virtual int run(int max_cycles, int step);

  // simulated I/O space as read/write memory
  // (is public so is accessible for unit testing)
  uint8_t io_mem[256];

private:
  uint32_t pop();
  int get_reg_id(const char * reg_string);
  int get_q(int reg16);
  int get_p(int reg16);
  void set_p(int reg16, uint16_t value);
  void set_q(int reg16, uint16_t value);
  void set_xy(int xy, uint16_t value);
  uint16_t get_xy(int xy);
  void set_ir(int ir, uint8_t value);
  uint8_t get_ir(int ir);
  void set_parity(uint8_t a);
  void set_add_subtract(bool neg);
  void set_zero8(int val);
  void set_negative8(int val);
  void set_half_carry8(int _new, int old, int number, bool neg);
  void set_carry8(int _new, int old, int number, bool neg);
  void set_overflow8(int _new, int old, int number, bool neg);
  void set_zero16(int val);
  void set_negative16(int val);
  void set_half_carry16(int _new, int old, int number, bool neg);
  void set_carry16(int _new, int old, int number, bool neg);
  void set_overflow16(int _new, int old, int number, bool neg);
  void set_flags_a(int a, int number, int8_t vflag, uint16_t instr_enum);
  void set_flags8(int _new, int old, int number, int8_t vflag, uint16_t instr_enum);
  void set_flags16(int _new, int old, int number, int8_t vflag, uint8_t instr_enum);
  int daa(uint8_t instr_enum);
  int execute_op_none(struct _table_z80 * table_z80_item);
  int execute_op_cond_none(struct _table_z80 * table_z80_item, uint16_t opcode);
  int execute_op_a_reg8(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_reg8(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_reg8_cb(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_a_number8(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_number8(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_offset8(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_cond_offset8(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_reg8_v2(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_reg8_index_hl(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_index_hl_reg8(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_index_hl(struct _table_z80 * table_z80_item);
  int execute_op_a_index_hl(struct _table_z80 * table_z80_item);
  int execute_op_index_hl_cb(struct _table_z80 * table_z80_item);
  int execute_op_index_long(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_bit_reg8(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_bit_reg8_index_hl(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_bit_index(struct _table_z80 * table_z80_item, uint8_t opcode);
  int ldi_ldd(struct _table_z80 * table_z80_item);
  int cpi_cpd(struct _table_z80 * table_z80_item);
  int outi_outd(struct _table_z80 * table_z80_item);
  int ini_ind(struct _table_z80 * table_z80_item);
  int rld_rrd(struct _table_z80 * table_z80_item);
  int execute_op_none16(struct _table_z80 * table_z80_item);
  int execute_op_reg16(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_reg16p(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_hl_reg16_1(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_hl_reg16_2(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_index(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_a_index(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_xy(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_xy_reg16(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_index_xy(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_index_sp_xy(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_index_address_xy(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute_op_index_address_reg16(struct _table_z80 * table_z80_item, uint16_t opcode16);
  int execute_op_address(struct _table_z80 * table_z80_item);
  int execute_op_cond_address(struct _table_z80 * table_z80_item, uint8_t opcode);
  int execute();

  uint8_t reg[8];
  uint16_t ix;
  uint16_t iy;
  uint16_t sp;
  uint16_t pc;
  uint16_t af_tick;
  uint16_t bc_tick;
  uint16_t de_tick;
  uint16_t hl_tick;
  uint8_t iff1;
  uint8_t iff2;
  uint8_t im;           // interrupt mode
  uint8_t iv;           // interrupt vector register
  uint8_t rr;           // memory refresh register

  static const char * const flags[];
};

#endif

