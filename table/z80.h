/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn
 *
 */

#ifndef NAKEN_ASM_TABLE_Z80_H
#define NAKEN_ASM_TABLE_Z80_H

#include "common/assembler.h"

enum
{
  OP_NONE,
  OP_NONE16,
  OP_NONE24,
  OP_A_REG8,
  OP_REG8,
  OP_A_REG_IHALF,
  OP_B_REG_IHALF,
  OP_C_REG_IHALF,
  OP_D_REG_IHALF,
  OP_E_REG_IHALF,
  OP_A_INDEX,
  OP_A_NUMBER8,
  OP_HL_REG16_1,
  OP_HL_REG16_2,
  OP_XY_REG16,
  OP_A_INDEX_HL,
  OP_INDEX_HL,
  OP_NUMBER8,
  OP_REG_IHALF,
  OP_INDEX,
  OP_INDEX_LONG,
  OP_BIT_REG8,
  OP_BIT_INDEX_HL,
  OP_BIT_INDEX,
  OP_ADDRESS,
  OP_COND_ADDRESS,
  OP_REG8_V2,
  OP_REG_IHALF_V2,
  OP_REG16,
  OP_XY,
  OP_INDEX_SP_HL,
  OP_INDEX_SP_XY,
  OP_AF_AF_TICK,
  OP_DE_HL,
  OP_IM_NUM,
  OP_A_INDEX_N,
  OP_REG8_INDEX_C,
  OP_F_INDEX_C,
  OP_INDEX_XY,
  OP_OFFSET8,
  OP_JR_COND_ADDRESS,
  OP_REG8_REG8,
  OP_REG8_REG_IHALF,
  OP_REG_IHALF_REG8,
  OP_REG_IHALF_REG_IHALF,
  OP_REG8_NUMBER8,
  OP_REG8_INDEX_HL,
  OP_REG8_INDEX,
  OP_INDEX_HL_REG8,
  OP_INDEX_HL_NUMBER8,
  OP_INDEX_REG8,
  OP_INDEX_NUMBER8,
  OP_A_INDEX_BC,
  OP_A_INDEX_DE,
  OP_A_INDEX_ADDRESS,
  OP_INDEX_BC_A,
  OP_INDEX_DE_A,
  OP_INDEX_ADDRESS_A,
  OP_IR_A,
  OP_A_IR,
  OP_REG16_ADDRESS,
  OP_XY_ADDRESS,
  OP_REG16_INDEX_ADDRESS,
  OP_HL_INDEX_ADDRESS,
  OP_XY_INDEX_ADDRESS,
  OP_INDEX_ADDRESS_REG16,
  OP_INDEX_ADDRESS_HL,
  OP_INDEX_ADDRESS_XY,
  OP_SP_HL,
  OP_SP_XY,
  OP_INDEX_ADDRESS8_A,
  OP_INDEX_C_REG8,
  OP_INDEX_C_ZERO,
  OP_REG16P,
  OP_BIT_INDEX_V2,
  OP_BIT_INDEX_REG8,
  OP_COND,
  OP_REG8_CB,
  OP_INDEX_HL_CB,
  OP_RESTART_ADDRESS,
};

enum
{
  Z80_NONE,
  Z80_ADC,
  Z80_ADD,
  Z80_AND,
  Z80_BIT,
  Z80_CALL,
  Z80_CCF,
  Z80_CP,
  Z80_CPD,
  Z80_CPDR,
  Z80_CPI,
  Z80_CPIR,
  Z80_CPL,
  Z80_DAA,
  Z80_DEC,
  Z80_DI,
  Z80_DJNZ,
  Z80_EI,
  Z80_EX,
  Z80_EXX,
  Z80_HALT,
  Z80_IM,
  Z80_IN,
  Z80_INC,
  Z80_IND,
  Z80_INDR,
  Z80_INI,
  Z80_INIR,
  Z80_JP,
  Z80_JR,
  Z80_LD,
  Z80_LDD,
  Z80_LDDR,
  Z80_LDI,
  Z80_LDIR,
  Z80_NEG,
  Z80_NOP,
  Z80_OR,
  Z80_OTDR,
  Z80_OTIR,
  Z80_OUT,
  Z80_OUTD,
  Z80_OUTI,
  Z80_POP,
  Z80_PUSH,
  Z80_RES,
  Z80_RET,
  Z80_RETI,
  Z80_RETN,
  Z80_RL,
  Z80_RLA,
  Z80_RLC,
  Z80_RLCA,
  Z80_RLD,
  Z80_RR,
  Z80_RRA,
  Z80_RRC,
  Z80_RRCA,
  Z80_RRD,
  Z80_RST,
  Z80_SBC,
  Z80_SCF,
  Z80_SET,
  Z80_SLA,
  Z80_SLL,
  Z80_SRA,
  Z80_SRL,
  Z80_STOP,
  Z80_SUB,
  Z80_XOR,
};

struct _table_instr_z80
{
  char *instr;
  uint8_t instr_enum;
};

struct _table_z80
{
  uint8_t instr_enum;
  uint8_t type;
  uint16_t opcode;
  uint16_t mask;
  uint8_t extra_opcode;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

extern struct _table_instr_z80 table_instr_z80[];
extern struct _table_z80 table_z80[];
extern struct _table_z80 table_z80_4_byte[];

#endif

