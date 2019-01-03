/**
 *  naken_asm assembler.
 *  Author: Michael Kohn
 *   Email: mike@mikekohn.net
 *     Web: http://www.mikekohn.net/
 * License: GPLv3
 *
 * Copyright 2010-2019 by Michael Kohn, Joe Davisson
 *
 * 6502 by Joe Davisson
 *
 */

#ifndef NAKEN_ASM_TABLE_6502_H
#define NAKEN_ASM_TABLE_6502_H

#include "common/assembler.h"

struct _table_6502
{
  char *name;
  uint8_t instr;
  uint8_t op;
};

struct _table_6502_opcodes
{
  uint8_t instr;
  uint8_t op;
  uint8_t cycles_min;
  uint8_t cycles_max;
};

enum
{
  M65XX_ADC,
  M65XX_AND,
  M65XX_ASL,
  M65XX_BBR0,
  M65XX_BBR1,
  M65XX_BBR2,
  M65XX_BBR3,
  M65XX_BBR4,
  M65XX_BBR5,
  M65XX_BBR6,
  M65XX_BBR7,
  M65XX_BBS0,
  M65XX_BBS1,
  M65XX_BBS2,
  M65XX_BBS3,
  M65XX_BBS4,
  M65XX_BBS5,
  M65XX_BBS6,
  M65XX_BBS7,
  M65XX_BCC,
  M65XX_BCS,
  M65XX_BEQ,
  M65XX_BIT,
  M65XX_BMI,
  M65XX_BNE,
  M65XX_BPL,
  M65XX_BRK,
  M65XX_BVC,
  M65XX_BVS,
  M65XX_BRA,
  M65XX_CLC,
  M65XX_CLD,
  M65XX_CLI,
  M65XX_CLV,
  M65XX_CMP,
  M65XX_CPX,
  M65XX_CPY,
  M65XX_DEC,
  M65XX_DEX,
  M65XX_DEY,
  M65XX_EOR,
  M65XX_INC,
  M65XX_INX,
  M65XX_INY,
  M65XX_JMP,
  M65XX_JSR,
  M65XX_LDA,
  M65XX_LDX,
  M65XX_LDY,
  M65XX_LSR,
  M65XX_NOP,
  M65XX_ORA,
  M65XX_PHA,
  M65XX_PHP,
  M65XX_PHX,
  M65XX_PHY,
  M65XX_PLA,
  M65XX_PLP,
  M65XX_PLX,
  M65XX_PLY,
  M65XX_RMB0,
  M65XX_RMB1,
  M65XX_RMB2,
  M65XX_RMB3,
  M65XX_RMB4,
  M65XX_RMB5,
  M65XX_RMB6,
  M65XX_RMB7,
  M65XX_ROL,
  M65XX_ROR,
  M65XX_RTI,
  M65XX_RTS,
  M65XX_SBC,
  M65XX_SEC,
  M65XX_SED,
  M65XX_SEI,
  M65XX_SMB0,
  M65XX_SMB1,
  M65XX_SMB2,
  M65XX_SMB3,
  M65XX_SMB4,
  M65XX_SMB5,
  M65XX_SMB6,
  M65XX_SMB7,
  M65XX_STA,
  M65XX_STP,
  M65XX_STX,
  M65XX_STY,
  M65XX_STZ,
  M65XX_TAX,
  M65XX_TAY,
  M65XX_TRB,
  M65XX_TSB,
  M65XX_TSX,
  M65XX_TXA,
  M65XX_TXS,
  M65XX_TYA,
  M65XX_WAI,
  M65XX_ERROR
};

enum
{
  OP_NONE,             //            A

  OP_IMMEDIATE,        // #$10       #

  OP_ADDRESS8,         // $10        zp
  OP_ADDRESS16,        // $1000      a

  OP_INDEXED8_X,       // $10,x      d,x
  OP_INDEXED8_Y,       // $10,y      d,y
  OP_INDEXED16_X,      // $1000,x    a,x
  OP_INDEXED16_Y,      // $1000,y    a,y

  OP_INDIRECT16,       // ($1000)    (a)
  OP_X_INDIRECT8,      // ($10,x)    (d,x)
  OP_INDIRECT8_Y,      // ($10),y    (d),y
  OP_INDIRECT8,        // ($10),y    (d)
  OP_X_INDIRECT16,     // ($1000,x)  (d,x)

  OP_RELATIVE,         // $10        r
  OP_ADDRESS8_RELATIVE // $10, LABEL
};

extern int table_6502_len;

#endif

